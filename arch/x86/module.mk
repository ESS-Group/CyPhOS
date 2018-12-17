# X86 make module

LD_SCRIPT := arch/x86/ldscript
BASE_ADDRESS := 0x100000

GRUB_CONFIG := arch/x86/grub/grub.cfg
ISO_TEMP_DIR := $(OBJDIR)/iso

#####################
# X86 compile units #
#####################

# cross-compiler binutils
ASM := gcc
CC := gcc
CXX := g++
LD := ld
OBJCOPY := objcopy
OBJDUMP := objdump
ACXX := ag++
ALIGNMENT := 0x80000

#MKRESCUE for iso generation
ifneq ($(shell which grub-mkrescue 2> /dev/null),)
MKRESCUE = grub-mkrescue
endif
ifneq ($(shell which grub2-mkrescue 2> /dev/null),)
MKRESCUE = grub2-mkrescue
endif


X86_BASE_DIR = arch/x86/
X86_BASE_SOURCES_CC = 
X86_BASE_SOURCES_ASM = 
X86_BASE_OBJECTS = $(patsubst %.o,$(OBJDIR)/$(X86_BASE_DIR)%.o,$(X86_BASE_SOURCES_CC:%.cc=%.o)) $(patsubst %.oS,$(OBJDIR)/$(X86_BASE_DIR)%.oS,$(X86_BASE_SOURCES_ASM:%.S=%.oS))

COMPILE_OBJECTS += $(X86_BASE_OBJECTS)
LINKER_OBJECTS += $(X86_BASE_OBJECTS)

CFLAGS += -I$(X86_BASE_DIR) -include arch/x86/x86.h -mno-red-zone
ASMFLAGS += -I$(X86_BASE_DIR) -include arch/x86/x86.h -D__ASSEMBLY__ -Werror -Wall
LDFLAGS += -e _start -m elf_x86_64 -n
# Enable 64bit mode
CONFIG_AMD64 := true

SUB_MODULES = boot cachemanagement core eventhandling interrupts driver common


include $(patsubst %,\
$(X86_BASE_DIR)%/module.mk,$(SUB_MODULES))

# Create ELF multiboot image
$(OBJDIR)/$(SYSTEM_IMAGE).elf : $(OBJDIR)/$(SYSTEM_IMAGE)
	@echo "OBJCOPY		$@"
	@if test \( ! \( -d $(@D) \) \) ;then mkdir -p $(@D);fi
	$(OUTPUT) $(OBJCOPY) --image-base $(BASE_ADDRESS) -R ".benchmark_results" $< $@

# Create ISO-Bootimage
$(OBJDIR)/$(SYSTEM_IMAGE).iso : $(OBJDIR)/$(SYSTEM_IMAGE).elf
	@echo "MKRESCUE	$@"
	mkdir -p $(ISO_TEMP_DIR)/boot
	mkdir -p $(ISO_TEMP_DIR)/boot/grub
	cp $(GRUB_CONFIG) $(ISO_TEMP_DIR)/boot/grub/
	cp $(OBJDIR)/$(SYSTEM_IMAGE).elf $(ISO_TEMP_DIR)/boot/boot.elf
	$(OUTPUT) $(MKRESCUE) -o $@ $(ISO_TEMP_DIR)
	
iso: $(OBJDIR)/$(SYSTEM_IMAGE).iso 