
#############################
# X86 generic compile units #
#############################
X86_BOOT_GENERIC_DIR=arch/x86/boot/
X86_BOOT_GENERIC_SOURCES_CC = initOS.cc
X86_BOOT_GENERIC_SOURCES_ASM = startup-64.S startup_ap-64.S paging.S
X86_BOOT_GENERIC_OBJECTS = $(patsubst %.o,$(OBJDIR)/$(X86_BOOT_GENERIC_DIR)%.o,$(X86_BOOT_GENERIC_SOURCES_CC:%.cc=%.o)) $(patsubst %.oS,$(OBJDIR)/$(X86_BOOT_GENERIC_DIR)%.oS,$(X86_BOOT_GENERIC_SOURCES_ASM:%.S=%.oS))

COMPILE_OBJECTS += $(X86_BOOT_GENERIC_OBJECTS)
LINKER_OBJECTS += $(X86_BOOT_GENERIC_OBJECTS)


