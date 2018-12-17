###############################
# X86 common compile units #
###############################
X86_INTERRUPTS_DIR=arch/x86/interrupts/
X86_INTERRUPTS_SOURCES_CC = interrupt_handler.cc IDTManager.cc
X86_INTERRUPTS_SOURCES_ASM = 
X86_INTERRUPTS_OBJECTS = $(patsubst %.o,$(OBJDIR)/$(X86_INTERRUPTS_DIR)%.o,$(X86_INTERRUPTS_SOURCES_CC:%.cc=%.o)) $(patsubst %.oS,$(OBJDIR)/$(X86_INTERRUPTS_DIR)%.oS,$(X86_INTERRUPTS_SOURCES_ASM:%.S=%.oS))

COMPILE_OBJECTS += $(X86_INTERRUPTS_OBJECTS)
LINKER_OBJECTS += $(X86_INTERRUPTS_OBJECTS)