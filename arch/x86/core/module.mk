###############################
# X86 core compile units #
###############################
X86_CORE_DIR=arch/x86/core/
X86_CORE_SOURCES_CC = SystemMode.cc
X86_CORE_SOURCES_ASM = 
X86_CORE_OBJECTS = $(patsubst %.o,$(OBJDIR)/$(X86_CORE_DIR)%.o,$(X86_CORE_SOURCES_CC:%.cc=%.o)) $(patsubst %.oS,$(OBJDIR)/$(X86_CORE_DIR)%.oS,$(X86_CORE_SOURCES_ASM:%.S=%.oS))

COMPILE_OBJECTS += $(X86_CORE_OBJECTS)
LINKER_OBJECTS += $(X86_CORE_OBJECTS)