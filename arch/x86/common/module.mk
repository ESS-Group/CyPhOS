###############################
# X86 common compile units #
###############################
X86_COMMON_DIR=arch/x86/common/
X86_COMMON_SOURCES_CC = X86System.cc
X86_COMMON_SOURCES_ASM = 
X86_COMMON_OBJECTS = $(patsubst %.o,$(OBJDIR)/$(X86_COMMON_DIR)%.o,$(X86_COMMON_SOURCES_CC:%.cc=%.o)) $(patsubst %.oS,$(OBJDIR)/$(X86_COMMON_DIR)%.oS,$(X86_COMMON_SOURCES_ASM:%.S=%.oS))

COMPILE_OBJECTS += $(X86_COMMON_OBJECTS)
LINKER_OBJECTS += $(X86_COMMON_OBJECTS)