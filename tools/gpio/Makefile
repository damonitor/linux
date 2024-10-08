# SPDX-License-Identifier: GPL-2.0
include ../scripts/Makefile.include

bindir ?= /usr/bin

# This will work when gpio is built in tools env. where srctree
# isn't set and when invoked from selftests build, where srctree
# is set to ".". building_out_of_srctree is undefined for in srctree
# builds
ifndef building_out_of_srctree
srctree := $(patsubst %/,%,$(dir $(CURDIR)))
srctree := $(patsubst %/,%,$(dir $(srctree)))
endif

# Do not use make's built-in rules
# (this improves performance and avoids hard-to-debug behaviour);
MAKEFLAGS += -r

override CFLAGS += -O2 -Wall -g -D_GNU_SOURCE -I$(OUTPUT)include

ALL_TARGETS := lsgpio gpio-hammer gpio-event-mon gpio-watch
ALL_PROGRAMS := $(patsubst %,$(OUTPUT)%,$(ALL_TARGETS))

all: $(ALL_PROGRAMS)

export srctree OUTPUT CC LD CFLAGS
include $(srctree)/tools/build/Makefile.include

#
# We need the following to be outside of kernel tree
#
$(OUTPUT)include/linux/gpio.h: ../../include/uapi/linux/gpio.h
	mkdir -p $(OUTPUT)include/linux 2>&1 || true
	ln -sf $(CURDIR)/../../include/uapi/linux/gpio.h $@

prepare: $(OUTPUT)include/linux/gpio.h

GPIO_UTILS_IN := $(OUTPUT)gpio-utils-in.o
$(GPIO_UTILS_IN): prepare FORCE
	$(Q)$(MAKE) $(build)=gpio-utils

#
# lsgpio
#
LSGPIO_IN := $(OUTPUT)lsgpio-in.o
$(LSGPIO_IN): prepare FORCE $(OUTPUT)gpio-utils-in.o
	$(Q)$(MAKE) $(build)=lsgpio
$(OUTPUT)lsgpio: $(LSGPIO_IN)
	$(QUIET_LINK)$(CC) $(CFLAGS) $(LDFLAGS) $< -o $@

#
# gpio-hammer
#
GPIO_HAMMER_IN := $(OUTPUT)gpio-hammer-in.o
$(GPIO_HAMMER_IN): prepare FORCE $(OUTPUT)gpio-utils-in.o
	$(Q)$(MAKE) $(build)=gpio-hammer
$(OUTPUT)gpio-hammer: $(GPIO_HAMMER_IN)
	$(QUIET_LINK)$(CC) $(CFLAGS) $(LDFLAGS) $< -o $@

#
# gpio-event-mon
#
GPIO_EVENT_MON_IN := $(OUTPUT)gpio-event-mon-in.o
$(GPIO_EVENT_MON_IN): prepare FORCE $(OUTPUT)gpio-utils-in.o
	$(Q)$(MAKE) $(build)=gpio-event-mon
$(OUTPUT)gpio-event-mon: $(GPIO_EVENT_MON_IN)
	$(QUIET_LINK)$(CC) $(CFLAGS) $(LDFLAGS) $< -o $@

#
# gpio-watch
#
GPIO_WATCH_IN := $(OUTPUT)gpio-watch-in.o
$(GPIO_WATCH_IN): prepare FORCE
	$(Q)$(MAKE) $(build)=gpio-watch
$(OUTPUT)gpio-watch: $(GPIO_WATCH_IN)
	$(QUIET_LINK)$(CC) $(CFLAGS) $(LDFLAGS) $< -o $@

clean:
	rm -f $(ALL_PROGRAMS)
	rm -f $(OUTPUT)include/linux/gpio.h
	find $(or $(OUTPUT),.) -name '*.o' -delete -o -name '\.*.d' -delete -o -name '\.*.cmd' -delete

install: $(ALL_PROGRAMS)
	install -d -m 755 $(DESTDIR)$(bindir);		\
	for program in $(ALL_PROGRAMS); do		\
		install $$program $(DESTDIR)$(bindir);	\
	done

FORCE:

.PHONY: all install clean FORCE prepare
