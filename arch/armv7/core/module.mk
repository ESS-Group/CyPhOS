###############################
# ARMv7 driver compile units #
###############################
ARMV7_CORE_DIR=arch/armv7/core/
ARMV7_CORE_SOURCES_CC = SystemMode.cc
ARMV7_CORE_SOURCES_ASM = 
ARMV7_CORE_OBJECTS = $(patsubst %.o,$(OBJDIR)/$(ARMV7_CORE_DIR)%.o,$(ARMV7_CORE_SOURCES_CC:%.cc=%.o)) $(patsubst %.oS,$(OBJDIR)/$(ARMV7_CORE_DIR)%.oS,$(ARMV7_CORE_SOURCES_ASM:%.S=%.oS))

COMPILE_OBJECTS += $(ARMV7_CORE_OBJECTS)
LINKER_OBJECTS += $(ARMV7_CORE_OBJECTS)