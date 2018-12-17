# Event handling code
CORE_DIR=core/
CORE_SOURCES_CC = SystemMode.cc
CORE_SOURCES_ASM = 
CORE_OBJECTS = $(patsubst %o,$(OBJDIR)/$(CORE_DIR)%o,$(CORE_SOURCES_CC:%.cc=%.o)) $(patsubst %.oS,$(OBJDIR)/$(CORE_DIR)%.oS,$(CORE_SOURCES_ASM:%.S=%.oS))

COMPILE_OBJECTS += $(CORE_OBJECTS)
LINKER_OBJECTS += $(CORE_OBJECTS)