##################################
# Cortex-A9 driver compile units #
##################################


CORTEXA9_DRIVER_DIR=arch/armv7/cortexa9/
CORTEXA9_DRIVER_SOURCES_CC =
CORTEXA9_DRIVER_SOURCES_ASM = 
CORTEXA9_DRIVER_OBJECTS = $(patsubst %.o,$(OBJDIR)/$(CORTEXA9_DRIVER_DIR)%.o,$(CORTEXA9_DRIVER_SOURCES_CC:%.cc=%.o)) $(patsubst %.oS,$(OBJDIR)/$(CORTEXA9_DRIVER_DIR)%.oS,$(CORTEXA9_DRIVER_SOURCES_ASM:%.S=%.oS))

ifeq ($(CONFIG_CORTEXA9_GLOBAL_TIMER), true)
	CORTEXA9_DRIVER_SOURCES_CC += CortexA9GlobalTimer.cc
endif

COMPILE_OBJECTS += $(CORTEXA9_DRIVER_OBJECTS)
LINKER_OBJECTS += $(CORTEXA9_DRIVER_OBJECTS)

CFLAGS += -I$(CORTEXA9_DRIVER_DIR)
ASMFLAGS += -I$(CORTEXA9_DRIVER_DIR)

SUB_MODULES = boot cachemanagement

include $(patsubst %,\
$(CORTEXA9_DRIVER_DIR)%/module.mk,$(SUB_MODULES))
