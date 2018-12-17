##################################
# Cortex-A9 driver compile units #
##################################
CORTEXA9_COMMON_DIR=arch/armv7/cortexa9/common/
CORTEXA9_COMMON_SOURCES_CC = cortexa9_generics.cc
CORTEXA9_COMMON_SOURCES_ASM = 
CORTEXA9_COMMON_OBJECTS = $(patsubst %.o,$(OBJDIR)/$(CORTEXA9_COMMON_DIR)%.o,$(CORTEXA9_COMMON_SOURCES_CC:%.cc=%.o)) $(patsubst %.oS,$(OBJDIR)/$(CORTEXA9_COMMON_DIR)%.oS,$(CORTEXA9_COMMON_SOURCES_ASM:%.S=%.oS))

COMPILE_OBJECTS += $(CORTEXA9_COMMON_OBJECTS)
LINKER_OBJECTS += $(CORTEXA9_COMMON_OBJECTS)