/*
 * Copyright (c) 2025 Suren Baghdasaryan <surenb@google.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
/*
 * Fork a child that concurrently modifies address space while the main
 * process is reading /proc/$PID/maps and verifying the results. Address
 * space modifications include:
 *     VMA splitting and merging
 *
 */
#undef NDEBUG
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <linux/fs.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

static unsigned long test_duration_sec = 5UL;
static int page_size;
static bool verbose;

/* /proc/pid/maps parsing routines */
struct page_content {
	char *data;
	ssize_t size;
};

#define LINE_MAX_SIZE		256

struct line_content {
	char text[LINE_MAX_SIZE];
	unsigned long start_addr;
	unsigned long end_addr;
};

static void read_two_pages(int maps_fd, struct page_content *page1,
			   struct page_content *page2)
{
	ssize_t  bytes_read;

	assert(lseek(maps_fd, 0, SEEK_SET) >= 0);
	bytes_read = read(maps_fd, page1->data, page_size);
	assert(bytes_read > 0 && bytes_read < page_size);
	page1->size = bytes_read;

	bytes_read = read(maps_fd, page2->data, page_size);
	assert(bytes_read > 0 && bytes_read < page_size);
	page2->size = bytes_read;
}

static void copy_first_line(struct page_content *page, char *first_line)
{
	char *pos = strchr(page->data, '\n');

	strncpy(first_line, page->data, pos - page->data);
	first_line[pos - page->data] = '\0';
}

static void copy_last_line(struct page_content *page, char *last_line)
{
	/* Get the last line in the first page */
	const char *end = page->data + page->size - 1;
	/* skip last newline */
	const char *pos = end - 1;

	/* search previous newline */
	while (pos[-1] != '\n')
		pos--;
	strncpy(last_line, pos, end - pos);
	last_line[end - pos] = '\0';
}

/* Read the last line of the first page and the first line of the second page */
static void read_boundary_lines(int maps_fd, struct page_content *page1,
				struct page_content *page2,
				struct line_content *last_line,
				struct line_content *first_line)
{
	read_two_pages(maps_fd, page1, page2);

	copy_last_line(page1, last_line->text);
	copy_first_line(page2, first_line->text);

	assert(sscanf(last_line->text, "%lx-%lx", &last_line->start_addr,
		      &last_line->end_addr) == 2);
	assert(sscanf(first_line->text, "%lx-%lx", &first_line->start_addr,
		      &first_line->end_addr) == 2);
}

/* Thread synchronization routines */
enum test_state {
	INIT,
	CHILD_READY,
	PARENT_READY,
	SETUP_READY,
	SETUP_MODIFY_MAPS,
	SETUP_MAPS_MODIFIED,
	SETUP_RESTORE_MAPS,
	SETUP_MAPS_RESTORED,
	TEST_READY,
	TEST_DONE,
};

struct vma_modifier_info;

typedef void (*vma_modifier_op)(const struct vma_modifier_info *mod_info);
typedef void (*vma_mod_result_check_op)(struct line_content *mod_last_line,
					struct line_content *mod_first_line,
					struct line_content *restored_last_line,
					struct line_content *restored_first_line);

struct vma_modifier_info {
	int vma_count;
	void *addr;
	int prot;
	void *next_addr;
	vma_modifier_op vma_modify;
	vma_modifier_op vma_restore;
	vma_mod_result_check_op vma_mod_check;
	pthread_mutex_t sync_lock;
	pthread_cond_t sync_cond;
	enum test_state curr_state;
	bool exit;
	void *child_mapped_addr[];
};

static void wait_for_state(struct vma_modifier_info *mod_info, enum test_state state)
{
	pthread_mutex_lock(&mod_info->sync_lock);
	while (mod_info->curr_state != state)
		pthread_cond_wait(&mod_info->sync_cond, &mod_info->sync_lock);
	pthread_mutex_unlock(&mod_info->sync_lock);
}

static void signal_state(struct vma_modifier_info *mod_info, enum test_state state)
{
	pthread_mutex_lock(&mod_info->sync_lock);
	mod_info->curr_state = state;
	pthread_cond_signal(&mod_info->sync_cond);
	pthread_mutex_unlock(&mod_info->sync_lock);
}

/* VMA modification routines */
static void *child_vma_modifier(struct vma_modifier_info *mod_info)
{
	int prot = PROT_READ | PROT_WRITE;
	int i;

	for (i = 0; i < mod_info->vma_count; i++) {
		mod_info->child_mapped_addr[i] = mmap(NULL, page_size * 3, prot,
				MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
		assert(mod_info->child_mapped_addr[i] != MAP_FAILED);
		/* change protection in adjacent maps to prevent merging */
		prot ^= PROT_WRITE;
	}
	signal_state(mod_info, CHILD_READY);
	wait_for_state(mod_info, PARENT_READY);
	while (true) {
		signal_state(mod_info, SETUP_READY);
		wait_for_state(mod_info, SETUP_MODIFY_MAPS);
		if (mod_info->exit)
			break;

		mod_info->vma_modify(mod_info);
		signal_state(mod_info, SETUP_MAPS_MODIFIED);
		wait_for_state(mod_info, SETUP_RESTORE_MAPS);
		mod_info->vma_restore(mod_info);
		signal_state(mod_info, SETUP_MAPS_RESTORED);

		wait_for_state(mod_info, TEST_READY);
		while (mod_info->curr_state != TEST_DONE) {
			mod_info->vma_modify(mod_info);
			mod_info->vma_restore(mod_info);
		}
	}
	for (i = 0; i < mod_info->vma_count; i++)
		munmap(mod_info->child_mapped_addr[i], page_size * 3);

	return NULL;
}

static void stop_vma_modifier(struct vma_modifier_info *mod_info)
{
	wait_for_state(mod_info, SETUP_READY);
	mod_info->exit = true;
	signal_state(mod_info, SETUP_MODIFY_MAPS);
}

static void print_first_lines(char *text, int nr)
{
	const char *end = text;

	while (nr && (end = strchr(end, '\n')) != NULL) {
		nr--;
		end++;
	}

	if (end) {
		int offs = end - text;

		text[offs] = '\0';
		printf(text);
		text[offs] = '\n';
		printf("\n");
	} else {
		printf(text);
	}
}

static void print_last_lines(char *text, int nr)
{
	const char *start = text + strlen(text);

	nr++; /* to ignore the last newline */
	while (nr) {
		while (start > text && *start != '\n')
			start--;
		nr--;
		start--;
	}
	printf(start);
}

static void print_boundaries(const char *title,
			     struct page_content *page1,
			     struct page_content *page2)
{
	if (!verbose)
		return;

	printf("%s", title);
	/* Print 3 boundary lines from each page */
	print_last_lines(page1->data, 3);
	printf("-----------------page boundary-----------------\n");
	print_first_lines(page2->data, 3);
}

static bool print_boundaries_on(bool condition, const char *title,
				struct page_content *page1,
				struct page_content *page2)
{
	if (verbose && condition)
		print_boundaries(title, page1, page2);

	return condition;
}

static void report_test_start(const char *name)
{
	if (verbose)
		printf("==== %s ====\n", name);
}

static struct timespec print_ts;

static void start_test_loop(struct timespec *ts)
{
	if (verbose)
		print_ts.tv_sec = ts->tv_sec;
}

static void end_test_iteration(struct timespec *ts)
{
	if (!verbose)
		return;

	/* Update every second */
	if (print_ts.tv_sec == ts->tv_sec)
		return;

	printf(".");
	fflush(stdout);
	print_ts.tv_sec = ts->tv_sec;
}

static void end_test_loop(void)
{
	if (verbose)
		printf("\n");
}

static void capture_mod_pattern(int maps_fd,
				struct vma_modifier_info *mod_info,
				struct page_content *page1,
				struct page_content *page2,
				struct line_content *last_line,
				struct line_content *first_line,
				struct line_content *mod_last_line,
				struct line_content *mod_first_line,
				struct line_content *restored_last_line,
				struct line_content *restored_first_line)
{
	print_boundaries("Before modification", page1, page2);

	signal_state(mod_info, SETUP_MODIFY_MAPS);
	wait_for_state(mod_info, SETUP_MAPS_MODIFIED);

	/* Copy last line of the first page and first line of the last page */
	read_boundary_lines(maps_fd, page1, page2, mod_last_line, mod_first_line);

	print_boundaries("After modification", page1, page2);

	signal_state(mod_info, SETUP_RESTORE_MAPS);
	wait_for_state(mod_info, SETUP_MAPS_RESTORED);

	/* Copy last line of the first page and first line of the last page */
	read_boundary_lines(maps_fd, page1, page2, restored_last_line, restored_first_line);

	print_boundaries("After restore", page1, page2);

	mod_info->vma_mod_check(mod_last_line, mod_first_line,
				restored_last_line, restored_first_line);

	/*
	 * The content of these lines after modify+resore should be the same
	 * as the original.
	 */
	assert(strcmp(restored_last_line->text, last_line->text) == 0);
	assert(strcmp(restored_first_line->text, first_line->text) == 0);
}

static void query_addr_at(int maps_fd, void *addr,
			  unsigned long *vma_start, unsigned long *vma_end)
{
	struct procmap_query q;

	memset(&q, 0, sizeof(q));
	q.size = sizeof(q);
	/* Find the VMA at the split address */
	q.query_addr = (unsigned long long)addr;
	q.query_flags = 0;
	assert(!ioctl(maps_fd, PROCMAP_QUERY, &q));
	*vma_start = q.vma_start;
	*vma_end = q.vma_end;
}

static inline void split_vma(const struct vma_modifier_info *mod_info)
{
	assert(mmap(mod_info->addr, page_size, mod_info->prot | PROT_EXEC,
		    MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED,
		    -1, 0) != MAP_FAILED);
}

static inline void merge_vma(const struct vma_modifier_info *mod_info)
{
	assert(mmap(mod_info->addr, page_size, mod_info->prot,
		    MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED,
		    -1, 0) != MAP_FAILED);
}

static inline void check_split_result(struct line_content *mod_last_line,
				      struct line_content *mod_first_line,
				      struct line_content *restored_last_line,
				      struct line_content *restored_first_line)
{
	/* Make sure vmas at the boundaries are changing */
	assert(strcmp(mod_last_line->text, restored_last_line->text) != 0);
	assert(strcmp(mod_first_line->text, restored_first_line->text) != 0);
}

static void test_maps_tearing_from_split(int maps_fd,
					 struct vma_modifier_info *mod_info,
					 struct page_content *page1,
					 struct page_content *page2,
					 struct line_content *last_line,
					 struct line_content *first_line)
{
	struct line_content split_last_line;
	struct line_content split_first_line;
	struct line_content restored_last_line;
	struct line_content restored_first_line;

	wait_for_state(mod_info, SETUP_READY);

	/* re-read the file to avoid using stale data from previous test */
	read_boundary_lines(maps_fd, page1, page2, last_line, first_line);

	mod_info->vma_modify = split_vma;
	mod_info->vma_restore = merge_vma;
	mod_info->vma_mod_check = check_split_result;

	report_test_start("Tearing from split");
	capture_mod_pattern(maps_fd, mod_info, page1, page2, last_line, first_line,
			    &split_last_line, &split_first_line,
			    &restored_last_line, &restored_first_line);

	/* Now start concurrent modifications for test_duration_sec */
	signal_state(mod_info, TEST_READY);

	struct line_content new_last_line;
	struct line_content new_first_line;
	struct timespec start_ts, end_ts;

	clock_gettime(CLOCK_MONOTONIC_COARSE, &start_ts);
	start_test_loop(&start_ts);
	do {
		bool last_line_changed;
		bool first_line_changed;
		unsigned long vma_start;
		unsigned long vma_end;

		read_boundary_lines(maps_fd, page1, page2, &new_last_line, &new_first_line);

		/* Check if we read vmas after split */
		if (!strcmp(new_last_line.text, split_last_line.text)) {
			/*
			 * The vmas should be consistent with split results,
			 * however if vma was concurrently restored after a
			 * split, it can be reported twice (first the original
			 * split one, then the same vma but extended after the
			 * merge) because we found it as the next vma again.
			 * In that case new first line will be the same as the
			 * last restored line.
			 */
			assert(!print_boundaries_on(
					strcmp(new_first_line.text, split_first_line.text) &&
					strcmp(new_first_line.text, restored_last_line.text),
					"Split result invalid", page1, page2));
		} else {
			/* The vmas should be consistent with merge results */
			assert(!print_boundaries_on(
					strcmp(new_last_line.text, restored_last_line.text),
					"Merge result invalid", page1, page2));
			assert(!print_boundaries_on(
					strcmp(new_first_line.text, restored_first_line.text),
					"Merge result invalid", page1, page2));
		}
		/*
		 * First and last lines should change in unison. If the last
		 * line changed then the first line should change as well and
		 * vice versa.
		 */
		last_line_changed = strcmp(new_last_line.text, last_line->text) != 0;
		first_line_changed = strcmp(new_first_line.text, first_line->text) != 0;
		assert(last_line_changed == first_line_changed);

		/* Check if PROCMAP_QUERY ioclt() finds the right VMA */
		query_addr_at(maps_fd, mod_info->addr + page_size,
			      &vma_start, &vma_end);
		/*
		 * The vma at the split address can be either the same as
		 * original one (if read before the split) or the same as the
		 * first line in the second page (if read after the split).
		 */
		assert((vma_start == last_line->start_addr &&
			vma_end == last_line->end_addr) ||
		       (vma_start == split_first_line.start_addr &&
			vma_end == split_first_line.end_addr));

		clock_gettime(CLOCK_MONOTONIC_COARSE, &end_ts);
		end_test_iteration(&end_ts);
	} while (end_ts.tv_sec - start_ts.tv_sec < test_duration_sec);
	end_test_loop();

	/* Signal the modifyer thread to stop and wait until it exits */
	signal_state(mod_info, TEST_DONE);
}

static inline void shrink_vma(const struct vma_modifier_info *mod_info)
{
	assert(mremap(mod_info->addr, page_size * 3, page_size, 0) != MAP_FAILED);
}

static inline void expand_vma(const struct vma_modifier_info *mod_info)
{
	assert(mremap(mod_info->addr, page_size, page_size * 3, 0) != MAP_FAILED);
}

static inline void check_shrink_result(struct line_content *mod_last_line,
				       struct line_content *mod_first_line,
				       struct line_content *restored_last_line,
				       struct line_content *restored_first_line)
{
	/* Make sure only the last vma of the first page is changing */
	assert(strcmp(mod_last_line->text, restored_last_line->text) != 0);
	assert(strcmp(mod_first_line->text, restored_first_line->text) == 0);
}

static void test_maps_tearing_from_resize(int maps_fd,
					  struct vma_modifier_info *mod_info,
					  struct page_content *page1,
					  struct page_content *page2,
					  struct line_content *last_line,
					  struct line_content *first_line)
{
	struct line_content shrunk_last_line;
	struct line_content shrunk_first_line;
	struct line_content restored_last_line;
	struct line_content restored_first_line;

	wait_for_state(mod_info, SETUP_READY);

	/* re-read the file to avoid using stale data from previous test */
	read_boundary_lines(maps_fd, page1, page2, last_line, first_line);

	mod_info->vma_modify = shrink_vma;
	mod_info->vma_restore = expand_vma;
	mod_info->vma_mod_check = check_shrink_result;

	report_test_start("Tearing from resize");
	capture_mod_pattern(maps_fd, mod_info, page1, page2, last_line, first_line,
			    &shrunk_last_line, &shrunk_first_line,
			    &restored_last_line, &restored_first_line);

	/* Now start concurrent modifications for test_duration_sec */
	signal_state(mod_info, TEST_READY);

	struct line_content new_last_line;
	struct line_content new_first_line;
	struct timespec start_ts, end_ts;

	clock_gettime(CLOCK_MONOTONIC_COARSE, &start_ts);
	start_test_loop(&start_ts);
	do {
		unsigned long vma_start;
		unsigned long vma_end;

		read_boundary_lines(maps_fd, page1, page2, &new_last_line, &new_first_line);

		/* Check if we read vmas after shrinking it */
		if (!strcmp(new_last_line.text, shrunk_last_line.text)) {
			/*
			 * The vmas should be consistent with shrunk results,
			 * however if the vma was concurrently restored, it
			 * can be reported twice (first as shrunk one, then
			 * as restored one) because we found it as the next vma
			 * again. In that case new first line will be the same
			 * as the last restored line.
			 */
			assert(!print_boundaries_on(
					strcmp(new_first_line.text, shrunk_first_line.text) &&
					strcmp(new_first_line.text, restored_last_line.text),
					"Shrink result invalid", page1, page2));
		} else {
			/* The vmas should be consistent with the original/resored state */
			assert(!print_boundaries_on(
					strcmp(new_last_line.text, restored_last_line.text),
					"Expand result invalid", page1, page2));
			assert(!print_boundaries_on(
					strcmp(new_first_line.text, restored_first_line.text),
					"Expand result invalid", page1, page2));
		}

		/* Check if PROCMAP_QUERY ioclt() finds the right VMA */
		query_addr_at(maps_fd, mod_info->addr, &vma_start, &vma_end);
		/*
		 * The vma should stay at the same address and have either the
		 * original size of 3 pages or 1 page if read after shrinking.
		 */
		assert(vma_start == last_line->start_addr &&
		       (vma_end - vma_start == page_size * 3 ||
			vma_end - vma_start == page_size));

		clock_gettime(CLOCK_MONOTONIC_COARSE, &end_ts);
		end_test_iteration(&end_ts);
	} while (end_ts.tv_sec - start_ts.tv_sec < test_duration_sec);
	end_test_loop();

	/* Signal the modifyer thread to stop and wait until it exits */
	signal_state(mod_info, TEST_DONE);
}

static inline void remap_vma(const struct vma_modifier_info *mod_info)
{
	/*
	 * Remap the last page of the next vma into the middle of the vma.
	 * This splits the current vma and the first and middle parts (the
	 * parts at lower addresses) become the last vma objserved in the
	 * first page and the first vma observed in the last page.
	 */
	assert(mremap(mod_info->next_addr + page_size * 2, page_size,
		      page_size, MREMAP_FIXED | MREMAP_MAYMOVE | MREMAP_DONTUNMAP,
		      mod_info->addr + page_size) != MAP_FAILED);
}

static inline void patch_vma(const struct vma_modifier_info *mod_info)
{
	assert(!mprotect(mod_info->addr + page_size, page_size,
			 mod_info->prot));
}

static inline void check_remap_result(struct line_content *mod_last_line,
				      struct line_content *mod_first_line,
				      struct line_content *restored_last_line,
				      struct line_content *restored_first_line)
{
	/* Make sure vmas at the boundaries are changing */
	assert(strcmp(mod_last_line->text, restored_last_line->text) != 0);
	assert(strcmp(mod_first_line->text, restored_first_line->text) != 0);
}

static void test_maps_tearing_from_remap(int maps_fd,
				struct vma_modifier_info *mod_info,
				struct page_content *page1,
				struct page_content *page2,
				struct line_content *last_line,
				struct line_content *first_line)
{
	struct line_content remapped_last_line;
	struct line_content remapped_first_line;
	struct line_content restored_last_line;
	struct line_content restored_first_line;

	wait_for_state(mod_info, SETUP_READY);

	/* re-read the file to avoid using stale data from previous test */
	read_boundary_lines(maps_fd, page1, page2, last_line, first_line);

	mod_info->vma_modify = remap_vma;
	mod_info->vma_restore = patch_vma;
	mod_info->vma_mod_check = check_remap_result;

	report_test_start("Tearing from remap");
	capture_mod_pattern(maps_fd, mod_info, page1, page2, last_line, first_line,
			    &remapped_last_line, &remapped_first_line,
			    &restored_last_line, &restored_first_line);

	/* Now start concurrent modifications for test_duration_sec */
	signal_state(mod_info, TEST_READY);

	struct line_content new_last_line;
	struct line_content new_first_line;
	struct timespec start_ts, end_ts;

	clock_gettime(CLOCK_MONOTONIC_COARSE, &start_ts);
	start_test_loop(&start_ts);
	do {
		unsigned long vma_start;
		unsigned long vma_end;

		read_boundary_lines(maps_fd, page1, page2, &new_last_line, &new_first_line);

		/* Check if we read vmas after remapping it */
		if (!strcmp(new_last_line.text, remapped_last_line.text)) {
			/*
			 * The vmas should be consistent with remap results,
			 * however if the vma was concurrently restored, it
			 * can be reported twice (first as split one, then
			 * as restored one) because we found it as the next vma
			 * again. In that case new first line will be the same
			 * as the last restored line.
			 */
			assert(!print_boundaries_on(
					strcmp(new_first_line.text, remapped_first_line.text) &&
					strcmp(new_first_line.text, restored_last_line.text),
					"Remap result invalid", page1, page2));
		} else {
			/* The vmas should be consistent with the original/resored state */
			assert(!print_boundaries_on(
					strcmp(new_last_line.text, restored_last_line.text),
					"Remap restore result invalid", page1, page2));
			assert(!print_boundaries_on(
					strcmp(new_first_line.text, restored_first_line.text),
					"Remap restore result invalid", page1, page2));
		}

		/* Check if PROCMAP_QUERY ioclt() finds the right VMA */
		query_addr_at(maps_fd, mod_info->addr + page_size, &vma_start, &vma_end);
		/*
		 * The vma should either stay at the same address and have the
		 * original size of 3 pages or we should find the remapped vma
		 * at the remap destination address with size of 1 page.
		 */
		assert((vma_start == last_line->start_addr &&
			vma_end - vma_start == page_size * 3) ||
		       (vma_start == last_line->start_addr + page_size &&
			vma_end - vma_start == page_size));

		clock_gettime(CLOCK_MONOTONIC_COARSE, &end_ts);
		end_test_iteration(&end_ts);
	} while (end_ts.tv_sec - start_ts.tv_sec < test_duration_sec);
	end_test_loop();

	/* Signal the modifyer thread to stop and wait until it exits */
	signal_state(mod_info, TEST_DONE);
}

int usage(void)
{
	fprintf(stderr, "Userland /proc/pid/{s}maps race test cases\n");
	fprintf(stderr, "  -d: Duration for time-consuming tests\n");
	fprintf(stderr, "  -v: Verbose mode\n");
	fprintf(stderr, "  -h: Help screen\n");
	exit(-1);
}

int main(int argc, char **argv)
{
	struct vma_modifier_info *mod_info;
	pthread_mutexattr_t mutex_attr;
	pthread_condattr_t cond_attr;
	int shared_mem_size;
	char fname[32];
	int vma_count;
	int maps_fd;
	int status;
	pid_t pid;
	int opt;

	while ((opt = getopt(argc, argv, "d:vh")) != -1) {
		if (opt == 'd')
			test_duration_sec = strtoul(optarg, NULL, 0);
		else if (opt == 'v')
			verbose = true;
		else if (opt == 'h')
			usage();
	}

	page_size = sysconf(_SC_PAGESIZE);
	/*
	 * Have to map enough vmas for /proc/pid/maps to contain more than one
	 * page worth of vmas. Assume at least 32 bytes per line in maps output
	 */
	vma_count = page_size / 32 + 1;
	shared_mem_size = sizeof(struct vma_modifier_info) + vma_count * sizeof(void *);

	/* map shared memory for communication with the child process */
	mod_info = (struct vma_modifier_info *)mmap(NULL, shared_mem_size,
		    PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

	assert(mod_info != MAP_FAILED);

	/* Initialize shared members */
	pthread_mutexattr_init(&mutex_attr);
	pthread_mutexattr_setpshared(&mutex_attr, PTHREAD_PROCESS_SHARED);
	assert(!pthread_mutex_init(&mod_info->sync_lock, &mutex_attr));
	pthread_condattr_init(&cond_attr);
	pthread_condattr_setpshared(&cond_attr, PTHREAD_PROCESS_SHARED);
	assert(!pthread_cond_init(&mod_info->sync_cond, &cond_attr));
	mod_info->vma_count = vma_count;
	mod_info->curr_state = INIT;
	mod_info->exit = false;

	pid = fork();
	if (!pid) {
		/* Child process */
		child_vma_modifier(mod_info);
		return 0;
	}

	sprintf(fname, "/proc/%d/maps", pid);
	maps_fd = open(fname, O_RDONLY);
	assert(maps_fd != -1);

	/* Wait for the child to map the VMAs */
	wait_for_state(mod_info, CHILD_READY);

	/* Read first two pages */
	struct page_content page1;
	struct page_content page2;

	page1.data = malloc(page_size);
	assert(page1.data);
	page2.data = malloc(page_size);
	assert(page2.data);

	struct line_content last_line;
	struct line_content first_line;

	read_boundary_lines(maps_fd, &page1, &page2, &last_line, &first_line);

	/*
	 * Find the addresses corresponding to the last line in the first page
	 * and the first line in the last page.
	 */
	mod_info->addr = NULL;
	mod_info->next_addr = NULL;
	for (int i = 0; i < mod_info->vma_count; i++) {
		if (mod_info->child_mapped_addr[i] == (void *)last_line.start_addr) {
			mod_info->addr = mod_info->child_mapped_addr[i];
			mod_info->prot = PROT_READ;
			/* Even VMAs have write permission */
			if ((i % 2) == 0)
				mod_info->prot |= PROT_WRITE;
		} else if (mod_info->child_mapped_addr[i] == (void *)first_line.start_addr) {
			mod_info->next_addr = mod_info->child_mapped_addr[i];
		}

		if (mod_info->addr && mod_info->next_addr)
			break;
	}
	assert(mod_info->addr && mod_info->next_addr);

	signal_state(mod_info, PARENT_READY);

	test_maps_tearing_from_split(maps_fd, mod_info, &page1, &page2,
				     &last_line, &first_line);

	test_maps_tearing_from_resize(maps_fd, mod_info, &page1, &page2,
				      &last_line, &first_line);

	test_maps_tearing_from_remap(maps_fd, mod_info, &page1, &page2,
				     &last_line, &first_line);

	stop_vma_modifier(mod_info);

	free(page2.data);
	free(page1.data);

	for (int i = 0; i < vma_count; i++)
		munmap(mod_info->child_mapped_addr[i], page_size);
	close(maps_fd);
	waitpid(pid, &status, 0);
	munmap(mod_info, shared_mem_size);

	return 0;
}
