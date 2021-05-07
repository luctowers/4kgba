SHELL     := /bin/bash
PATH      := $(DEVKITARM)/bin:$(DEVKITPRO)/tools/bin:$(PATH)

PROJ      := 2048
ELF       := $(PROJ).elf
ROM       := $(PROJ).gba
COBJS     := $(PROJ).o

INCLUDES  := -I../libadvance
LIBADV    := ../libadvance/libadvance.a
CFLAGS    := -O2 -mcpu=arm7tdmi -mthumb-interwork -mthumb
LDFLAGS   := -specs=gba.specs

build : $(ROM)
clean :
	rm -f $(COBJS) $(ELF) $(ROM)

$(COBJS) : %.o : %.c
	arm-none-eabi-gcc -c $< -o $@ $(INCLUDES) $(CFLAGS)

$(ELF) : $(COBJS) $(LIBADV)
	arm-none-eabi-gcc $^ -o $@ $(LDFLAGS)

$(ROM) : $(ELF)
	arm-none-eabi-objcopy -O binary $< $@
	gbafix $@ -t $(PROJ)

$(LIBADV) :
	$(MAKE) -C ../libadvance