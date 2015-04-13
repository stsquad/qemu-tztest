VPATH	= $(ARCH):../$(ARCH):../../common/$(ARCH):../

EL0_NS_ELF		= el0_nsec.elf
EL0_NS_IMAGE 	= el0_nsec.bin
EL0_NS_LOAD		= el0_nsec.lds
EL0_NS_OBJS		= tztest_nsec.o \
				  tztest.o \
				  el0.o \
                  builtins.o

libgcc := $(shell $(CC) $(CFLAGS) --print-libgcc-file-name)
FLATLIBS = ../../libcflat/libcflat.a $(libgcc) ../../libcflat/$(ARCH)/libeabi.a

-include .*.d

CFLAGS += -I$(ARCH) -I../$(ARCH) -I../ -I../$(ARCH)
CFLAGS += -I../../common/$(ARCH) -I../../common/

##################################################################

$(EL0_NS_ELF): $(EL0_NS_OBJS) $(EL0_NS_LOAD)
	$(LD) -o $@ $(EL0_NS_OBJS) $(FLATLIBS) --script=$(EL0_NS_LOAD)

$(EL0_NS_IMAGE): $(EL0_NS_ELF)
	$(OBJCOPY) -O binary $< $@

$(EL0_NS_LOAD): el0_nsec.lds.S Makefile ../../platform/$(PLAT)/
	$(CC) $(CFLAGS) -E -P -C -o $@ $<

%.o: %.S
	$(CC) $(CFLAGS) -c -nostdlib -o $(notdir $@) $<

all: $(EL0_NS_IMAGE)

clean:
	$(RM) $(EL0_NS_OBJS) $(EL0_NS_LOAD) $(EL0_NS_ELF) $(EL0_NS_IMAGE) .*.d