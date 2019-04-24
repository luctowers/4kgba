SHELL     := /bin/bash
PATH      := $(DEVKITARM)/bin:$(DEVKITPRO)/tools/bin:$(PATH)

PROJ      := libadvance
LIB       := $(PROJ).a

ASMOBJS   := bios_functions.o

ARCH      := -mcpu=arm7tdmi -mthumb -mthumb-interwork

build : $(LIB)
clean :
	rm -f $(ASMOBJS) $(LIB)

$(ASMOBJS) : %.o : %.s
	arm-none-eabi-gcc -c $< -o $@ $(CFLAGS)

$(LIB) : $(ASMOBJS)
	arm-none-eabi-ar rcs $@ $^