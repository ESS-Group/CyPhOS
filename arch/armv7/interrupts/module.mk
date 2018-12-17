###############################
# ARMv7 driver compile units #
###############################
ARMV7_INTERRUPT_DIR=arch/armv7/interrupts/
ARMV7_INTERRUPT_SOURCES_CC = first_stage_handler.cc second_stage_handler_critical.cc
ARMV7_INTERRUPT_SOURCES_ASM = arm_jumptable.S first_stage_handler.S first_stage_handler_critical.S
ARMV7_INTERRUPT_OBJECTS = $(patsubst %.o,$(OBJDIR)/$(ARMV7_INTERRUPT_DIR)%.o,$(ARMV7_INTERRUPT_SOURCES_CC:%.cc=%.o)) $(patsubst %.oS,$(OBJDIR)/$(ARMV7_INTERRUPT_DIR)%.oS,$(ARMV7_INTERRUPT_SOURCES_ASM:%.S=%.oS))

COMPILE_OBJECTS += $(ARMV7_INTERRUPT_OBJECTS)
LINKER_OBJECTS += $(ARMV7_INTERRUPT_OBJECTS)