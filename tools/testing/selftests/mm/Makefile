# SPDX-License-Identifier: GPL-2.0
# Makefile for mm selftests

LOCAL_HDRS += $(selfdir)/mm/local_config.h $(top_srcdir)/mm/gup_test.h
LOCAL_HDRS += $(selfdir)/mm/mseal_helpers.h

include local_config.mk

ifeq ($(ARCH),)

ifeq ($(CROSS_COMPILE),)
uname_M := $(shell uname -m 2>/dev/null || echo not)
else
uname_M := $(shell echo $(CROSS_COMPILE) | grep -o '^[a-z0-9]\+')
endif
ARCH ?= $(shell echo $(uname_M) | sed -e 's/aarch64.*/arm64/' -e 's/ppc64.*/powerpc/')
endif

# Without this, failed build products remain, with up-to-date timestamps,
# thus tricking Make (and you!) into believing that All Is Well, in subsequent
# make invocations:
.DELETE_ON_ERROR:

# Avoid accidental wrong builds, due to built-in rules working just a little
# bit too well--but not quite as well as required for our situation here.
#
# In other words, "make $SOME_TEST" is supposed to fail to build at all,
# because this Makefile only supports either "make" (all), or "make /full/path".
# However,  the built-in rules, if not suppressed, will pick up CFLAGS and the
# initial LDLIBS (but not the target-specific LDLIBS, because those are only
# set for the full path target!). This causes it to get pretty far into building
# things despite using incorrect values such as an *occasionally* incomplete
# LDLIBS.
MAKEFLAGS += --no-builtin-rules

CFLAGS = -Wall -O2 -I $(top_srcdir) $(EXTRA_CFLAGS) $(KHDR_INCLUDES) $(TOOLS_INCLUDES)
LDLIBS = -lrt -lpthread -lm

# Some distributions (such as Ubuntu) configure GCC so that _FORTIFY_SOURCE is
# automatically enabled at -O1 or above. This triggers various unused-result
# warnings where functions such as read() or write() are called and their
# return value is not checked. Disable _FORTIFY_SOURCE to silence those
# warnings.
CFLAGS += -U_FORTIFY_SOURCE

KDIR ?= /lib/modules/$(shell uname -r)/build
ifneq (,$(wildcard $(KDIR)/Module.symvers))
ifneq (,$(wildcard $(KDIR)/include/linux/page_frag_cache.h))
TEST_GEN_MODS_DIR := page_frag
else
PAGE_FRAG_WARNING = "missing page_frag_cache.h, please use a newer kernel"
endif
else
PAGE_FRAG_WARNING = "missing Module.symvers, please have the kernel built first"
endif

TEST_GEN_FILES = cow
TEST_GEN_FILES += compaction_test
TEST_GEN_FILES += gup_longterm
TEST_GEN_FILES += gup_test
TEST_GEN_FILES += hmm-tests
TEST_GEN_FILES += hugetlb-madvise
TEST_GEN_FILES += hugetlb-read-hwpoison
TEST_GEN_FILES += hugetlb-soft-offline
TEST_GEN_FILES += hugepage-mmap
TEST_GEN_FILES += hugepage-mremap
TEST_GEN_FILES += hugepage-shm
TEST_GEN_FILES += hugepage-vmemmap
TEST_GEN_FILES += khugepaged
TEST_GEN_FILES += madv_populate
TEST_GEN_FILES += map_fixed_noreplace
TEST_GEN_FILES += map_hugetlb
TEST_GEN_FILES += map_populate
ifneq (,$(filter $(ARCH),arm64 riscv riscv64 x86 x86_64))
TEST_GEN_FILES += memfd_secret
endif
TEST_GEN_FILES += migration
TEST_GEN_FILES += mkdirty
TEST_GEN_FILES += mlock-random-test
TEST_GEN_FILES += mlock2-tests
TEST_GEN_FILES += mrelease_test
TEST_GEN_FILES += mremap_dontunmap
TEST_GEN_FILES += mremap_test
TEST_GEN_FILES += mseal_test
TEST_GEN_FILES += on-fault-limit
TEST_GEN_FILES += pagemap_ioctl
TEST_GEN_FILES += pfnmap
TEST_GEN_FILES += process_madv
TEST_GEN_FILES += thuge-gen
TEST_GEN_FILES += transhuge-stress
TEST_GEN_FILES += uffd-stress
TEST_GEN_FILES += uffd-unit-tests
TEST_GEN_FILES += uffd-wp-mremap
TEST_GEN_FILES += split_huge_page_test
TEST_GEN_FILES += ksm_tests
TEST_GEN_FILES += ksm_functional_tests
TEST_GEN_FILES += mdwe_test
TEST_GEN_FILES += hugetlb_fault_after_madv
TEST_GEN_FILES += hugetlb_madv_vs_map
TEST_GEN_FILES += hugetlb_dio
TEST_GEN_FILES += droppable
TEST_GEN_FILES += guard-regions
TEST_GEN_FILES += merge

ifneq ($(ARCH),arm64)
TEST_GEN_FILES += soft-dirty
endif

ifeq ($(ARCH),x86_64)
CAN_BUILD_I386 := $(shell ./../x86/check_cc.sh "$(CC)" ../x86/trivial_32bit_program.c -m32)
CAN_BUILD_X86_64 := $(shell ./../x86/check_cc.sh "$(CC)" ../x86/trivial_64bit_program.c)
CAN_BUILD_WITH_NOPIE := $(shell ./../x86/check_cc.sh "$(CC)" ../x86/trivial_program.c -no-pie)

VMTARGETS := protection_keys
VMTARGETS += pkey_sighandler_tests
BINARIES_32 := $(VMTARGETS:%=%_32)
BINARIES_64 := $(VMTARGETS:%=%_64)

ifeq ($(CAN_BUILD_WITH_NOPIE),1)
CFLAGS += -no-pie
endif

ifeq ($(CAN_BUILD_I386),1)
TEST_GEN_FILES += $(BINARIES_32)
endif

ifeq ($(CAN_BUILD_X86_64),1)
TEST_GEN_FILES += $(BINARIES_64)
endif

else ifeq ($(ARCH),arm64)
TEST_GEN_FILES += protection_keys
TEST_GEN_FILES += pkey_sighandler_tests
else ifeq ($(ARCH),powerpc)
TEST_GEN_FILES += protection_keys
endif

ifneq (,$(filter $(ARCH),arm64 mips64 parisc64 powerpc riscv64 s390x sparc64 x86_64 s390))
TEST_GEN_FILES += va_high_addr_switch
ifneq ($(ARCH),riscv64)
TEST_GEN_FILES += virtual_address_range
endif
TEST_GEN_FILES += write_to_hugetlbfs
endif

TEST_PROGS := run_vmtests.sh

TEST_FILES := test_vmalloc.sh
TEST_FILES += test_hmm.sh
TEST_FILES += va_high_addr_switch.sh
TEST_FILES += charge_reserved_hugetlb.sh
TEST_FILES += hugetlb_reparenting_test.sh
TEST_FILES += test_page_frag.sh

# required by charge_reserved_hugetlb.sh
TEST_FILES += write_hugetlb_memory.sh

include ../lib.mk

$(TEST_GEN_PROGS): vm_util.c thp_settings.c
$(TEST_GEN_FILES): vm_util.c thp_settings.c

$(OUTPUT)/uffd-stress: uffd-common.c
$(OUTPUT)/uffd-unit-tests: uffd-common.c
$(OUTPUT)/uffd-wp-mremap: uffd-common.c
$(OUTPUT)/protection_keys: pkey_util.c
$(OUTPUT)/pkey_sighandler_tests: pkey_util.c

ifeq ($(ARCH),x86_64)
BINARIES_32 := $(patsubst %,$(OUTPUT)/%,$(BINARIES_32))
BINARIES_64 := $(patsubst %,$(OUTPUT)/%,$(BINARIES_64))

$(BINARIES_32) $(BINARIES_64): pkey_util.c

define gen-target-rule-32
$(1) $(1)_32: $(OUTPUT)/$(1)_32
.PHONY: $(1) $(1)_32
endef

define gen-target-rule-64
$(1) $(1)_64: $(OUTPUT)/$(1)_64
.PHONY: $(1) $(1)_64
endef

ifeq ($(CAN_BUILD_I386),1)
$(BINARIES_32): CFLAGS += -m32 -mxsave
$(BINARIES_32): LDLIBS += -lrt -ldl -lm
$(BINARIES_32): $(OUTPUT)/%_32: %.c
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) $(notdir $^) $(LDLIBS) -o $@
$(foreach t,$(VMTARGETS),$(eval $(call gen-target-rule-32,$(t))))
endif

ifeq ($(CAN_BUILD_X86_64),1)
$(BINARIES_64): CFLAGS += -m64 -mxsave
$(BINARIES_64): LDLIBS += -lrt -ldl
$(BINARIES_64): $(OUTPUT)/%_64: %.c
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) $(notdir $^) $(LDLIBS) -o $@
$(foreach t,$(VMTARGETS),$(eval $(call gen-target-rule-64,$(t))))
endif

# x86_64 users should be encouraged to install 32-bit libraries
ifeq ($(CAN_BUILD_I386)$(CAN_BUILD_X86_64),01)
all: warn_32bit_failure

warn_32bit_failure:
	@echo "Warning: you seem to have a broken 32-bit build" 2>&1;		\
	echo  "environment. This will reduce test coverage of 64-bit" 2>&1;	\
	echo  "kernels. If you are using a Debian-like distribution," 2>&1;	\
	echo  "try:"; 2>&1;							\
	echo  "";								\
	echo  "  apt-get install gcc-multilib libc6-i386 libc6-dev-i386";	\
	echo  "";								\
	echo  "If you are using a Fedora-like distribution, try:";		\
	echo  "";								\
	echo  "  yum install glibc-devel.*i686";				\
	exit 0;
endif
endif

# IOURING_EXTRA_LIBS may get set in local_config.mk, or it may be left empty.
$(OUTPUT)/cow: LDLIBS += $(IOURING_EXTRA_LIBS)

$(OUTPUT)/gup_longterm: LDLIBS += $(IOURING_EXTRA_LIBS)

$(OUTPUT)/mlock-random-test $(OUTPUT)/memfd_secret: LDLIBS += -lcap

$(OUTPUT)/ksm_tests: LDLIBS += -lnuma

$(OUTPUT)/migration: LDLIBS += -lnuma

local_config.mk local_config.h: check_config.sh
	/bin/sh ./check_config.sh $(CC)

EXTRA_CLEAN += local_config.mk local_config.h

ifeq ($(IOURING_EXTRA_LIBS),)
all: warn_missing_liburing

warn_missing_liburing:
	@echo ; \
	echo "Warning: missing liburing support. Some tests will be skipped." ; \
	echo
endif

ifneq ($(PAGE_FRAG_WARNING),)
all: warn_missing_page_frag

warn_missing_page_frag:
	@echo ; \
	echo "Warning: $(PAGE_FRAG_WARNING). page_frag test will be skipped." ; \
	echo
endif
