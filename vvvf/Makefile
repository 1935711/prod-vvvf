
DIR_SRC:=src
DIR_INCLUDE:=include
DIR_BUILD:=build
DIR_DATA:=data
TOOLCHAIN:=arm-none-eabi
CC:=gcc
LD:=ld

MAIN_NAME:=kernel
MAIN_IMG:=$(MAIN_NAME).img
MAIN_EX:=$(MAIN_NAME).elf
MAIN_LIST:=$(MAIN_NAME).list
MAIN_MAP:=$(MAIN_NAME).map
MAIN_SRC:=$(wildcard $(DIR_SRC)/*.S) $(wildcard $(DIR_SRC)/*.c)
MAIN_OBJ:=$(MAIN_SRC:$(DIR_SRC)/%=$(DIR_BUILD)/%.o) $(DIR_BUILD)/$(DIR_DATA)/audio.o
MAIN_DEP:=$(MAIN_OBJ:%.o=%.d)
MAIN_FLAGS_CC:=\
	-W \
	-Wall \
	-Wextra \
	-Werror \
	-Wno-unused-parameter \
	-Wshadow \
	-static \
	-nostartfiles \
	-mcpu=arm1176jzf-s \
	-mfloat-abi=hard \
	-mfpu=vfpv2 \
	-Ofast \
	-fdata-sections \
	-ffunction-sections \
	-I$(DIR_INCLUDE) \
	-lgcc \
	-lm
MAIN_FLAGS_LD:=\
	-T linker.ld \
	-Xlinker --no-undefined \
	-Xlinker -Map=$(DIR_BUILD)/$(MAIN_MAP) \
	-Xlinker --gc-sections
MAIN_FLAGS_QEMU:=\
	-machine raspi1ap \
	-kernel $(DIR_BUILD)/$(MAIN_EX) \
	-serial stdio \
	-audiodev pa,id=0

all: main
.PHONY: all

main: $(DIR_BUILD) $(DIR_BUILD)/$(DIR_DATA) $(DIR_BUILD)/$(MAIN_IMG) $(DIR_BUILD)/$(MAIN_LIST)
.PHONY: main

# Build kernel.
$(DIR_BUILD)/$(MAIN_EX): $(MAIN_OBJ)
	$(TOOLCHAIN)-$(CC) -o $(@) $(^) $(MAIN_FLAGS_CC) $(MAIN_FLAGS_LD)
$(DIR_BUILD)/$(MAIN_LIST): $(DIR_BUILD)/$(MAIN_EX)
	$(TOOLCHAIN)-objdump -d $(^) > $(DIR_BUILD)/$(MAIN_LIST)
$(DIR_BUILD)/$(MAIN_IMG): $(DIR_BUILD)/$(MAIN_EX)
	$(TOOLCHAIN)-objcopy -O binary $(^) $(DIR_BUILD)/$(MAIN_IMG)

# Convert PCM audio to an object file to link it into the kernel.
$(DIR_BUILD)/$(DIR_DATA)/audio.o: $(DIR_DATA)/audio.bin
	$(TOOLCHAIN)-$(LD) -r -b binary -o $(@) $(^)

# Compile source files to object files.
$(DIR_BUILD)/%.S.o: $(DIR_SRC)/%.S
	$(TOOLCHAIN)-$(CC) $(<) -o $(@) $(MAIN_FLAGS_CC) -c -MMD
$(DIR_BUILD)/%.c.o: $(DIR_SRC)/%.c
	$(TOOLCHAIN)-$(CC) $(<) -o $(@) $(MAIN_FLAGS_CC) -c -MMD

# Recompile source files after a header they include changes.
-include $(MAIN_DEP)

qemu: $(DIR_BUILD)/$(MAIN_IMG)
	qemu-system-arm $(MAIN_FLAGS_QEMU)
.PHONY: qemu
qemu-asm: $(DIR_BUILD)/$(MAIN_IMG)
	qemu-system-arm $(MAIN_FLAGS_QEMU) -d in_asm
.PHONY: qemu-asm

install: $(DIR_BUILD)/$(MAIN_IMG)
	rm -rf /media/tomasz/boot/$(MAIN_IMG)
	cp $(DIR_BUILD)/$(MAIN_IMG) /media/tomasz/boot/$(MAIN_IMG)
.PHONY: install

$(DIR_BUILD) $(DIR_BUILD)/$(DIR_DATA):
	mkdir -p $(@)
clean:
	rm -rf $(DIR_BUILD)
.PHONY: clean
