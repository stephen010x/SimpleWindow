TARGET := run

TMPDIR := temp
SRCDIR := src
BINDIR := bin
#LIBDIR := lib

CC := gcc
CFLAGS := 
LFLAGS :=
BFLAGS := -m64 -Wall -Wextra -Wpedantic -Wconversion -Wundef
# extra flags -mx32


# should grab all paths relative to makefile.
SRCS := $(shell find $(SOURCEDIR) -name "*.c")
OBJS := $(SRCS:%.c=$(TMPDIR)/%.o)
DEPS := $(SRCS:%.c=$(TMPDIR)/%.d)
LINK_CMD = $(CC) $(BFLAGS) $(LFLAGS) -o $@ $^
TARGET := $(BINDIR)/$(TARGET)


all: fast
fast: _fast $(TARGET).exe
debug: _debug $(TARGET).exe
release: _release $(TARGET).exe
static: _static $(TARGET).a
dll: _dll $(TARGET).dll


-include $(DEPS)


_fast:
	$(eval CFLAGS += -DDEBUG_MODE)
	$(eval BFLAGS += -g -O0)

_debug:
	$(eval CFLAGS += -DDEBUG_MODE)
	$(eval BFLAGS += -g -Og)

_release:
	$(eval CFLAGS += -mwindows)
	$(eval LFLAGS += -Wl,--gc-sections -s -flto)
	$(eval BFLAGS += -Os -fno-ident -fno-asynchronous-unwind-tables)

_dll:
	$(eval CFLAGS += -DDLL_MODE)
	$(eval LFLAGS += -shared)
	$(eval DFLAGS += -Os)

_static:
	$(eval CFLAGS += -Os)

_strip:
	strip $(TARGET).exe


$(TARGET).exe: $(OBJS)
	$(info DEPS is $(DEPS))
	-@mkdir $(BINDIR) 2>NUL
	$(CC) $(BFLAGS) $(LFLAGS) -o $@ $^

$(TARGET).dll: $(OBJS)
	-@mkdir $(BINDIR) 2>NUL
	$(CC) $(BFLAGS) $(LFLAGS) -o $@ $^
	
$(TARGET).a: $(OBJS)
	-@mkdir $(BINDIR) 2>NUL
	ar rcs $@ $(OBJS)


$(TMPDIR)/%.o: %.c $(TMPDIR)/%.d
	-@cmd /E:ON /C mkdir $(subst /,\,$(dir $@))
	$(CC) $(BFLAGS) $(CFLAGS) -c $< -o $@

$(TMPDIR)/%.d: %.c
	-@cmd /E:ON /C mkdir $(subst /,\,$(dir $@))
	$(CC) -MM -MT $@ -MF $@ -c $<

clean:
	cmd /c rmdir /s /q $(TMPDIR)
	cmd /c rmdir /s /q $(BINDIR)

	
.PHONY: all fast debug release static dll clean 
.PHONY: _build _fast _debug _release _dll _static _strip