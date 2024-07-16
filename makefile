TARGET := run
# for dll or static
INCLUDE := simplewin
HEADER := src/header.h

TMPDIR := tmp
SRCDIR := src
BINDIR := bin
#for dll or static
LIBDIR := lib
INCDIR := inc

CC := gcc
CFLAGS :=
LFLAGS :=
BFLAGS := -m64 -Wall -Wextra -Wpedantic -Wconversion -Wundef
# extra flags -mx32


GOAL := $(firstword $(MAKECMDGOALS))
GOAL := $(if $(GOAL),$(GOAL),fast)
# should grab all paths relative to makefile.
SRCS := $(shell find $(SOURCEDIR) -name "*.c")
OBJS := $(SRCS:%.c=$(TMPDIR)/$(GOAL)/%.o)
DEPS := $(SRCS:%.c=$(TMPDIR)/$(GOAL)/%.d)
LINK_CMD = $(CC) $(BFLAGS) $(LFLAGS) -o $@ $^
BINTARG := $(BINDIR)/$(TARGET)
LIBTARG := $(LIBDIR)/$(TARGET)


all: fast
fast: _fast $(BINTARG).exe
debug: _debug $(BINTARG).exe
release: _release $(BINTARG).exe
static: _static $(LIBTARG).a
dll: _dll $(LIBTARG).dll


-include $(DEPS)


_fast:
	$(eval CFLAGS += -DDEBUG_MODE)
	$(eval BFLAGS += -g -O0)

_debug:
	$(eval CFLAGS += -DDEBUG_MODE)
	$(eval BFLAGS += -g -Og)

_release: _optimize
	$(eval CFLAGS += -mwindows)

_dll: _optimize
	$(eval CFLAGS += -DDLL_MODE)
	$(eval LFLAGS += -shared)

_static: _optimize

_optimize:
	$(eval LFLAGS += -Wl,--gc-sections -s -flto)
	$(eval BFLAGS += -Os -fno-ident -fno-asynchronous-unwind-tables)


$(BINTARG).exe: $(OBJS)
	$(info DEPS is $(DEPS))
	-@mkdir $(BINDIR) 2>NUL
	$(CC) $(BFLAGS) $(LFLAGS) -o $@ $^

$(LIBTARG).dll: $(OBJS)
	-@mkdir $(LIBDIR) 2>NUL
	-@mkdir $(INCDIR) 2>NUL
	-@cmd /E:ON /c copy /a /y $(subst /,\,$(HEADER)) $(subst /,\,$(INCDIR))\$(INCLUDE).h
	$(CC) $(BFLAGS) $(LFLAGS) -o $@ $^

$(LIBTARG).a: $(OBJS)
	-@mkdir $(LIBDIR) 2>NUL
	-@mkdir $(INCDIR) 2>NUL
	-@cmd /E:ON /c copy /a /y $(subst /,\,$(HEADER)) $(subst /,\,$(INCDIR))\$(INCLUDE).h
	ar rcs $@ $(OBJS)


$(TMPDIR)/$(GOAL)/%.o: %.c $(TMPDIR)/$(GOAL)/%.d
	-@cmd /E:ON /C mkdir $(subst /,\,$(dir $@))
	$(CC) $(BFLAGS) $(CFLAGS) -c $< -o $@

$(TMPDIR)/$(GOAL)/%.d: %.c
	-@cmd /E:ON /C mkdir $(subst /,\,$(dir $@))
	$(CC) -MM -MT $(patsubst %.d,%.o,$@) -MF $@ -c $<


clean:
	-cmd /c rmdir /s /q $(TMPDIR)
	-cmd /c rmdir /s /q $(BINDIR)
	-cmd /c rmdir /s /q $(LIBDIR)
	-cmd /c rmdir /s /q $(INCDIR)

	
.PHONY: all fast debug release static dll clean 
.PHONY: _build _fast _debug _release _dll _static _optimize