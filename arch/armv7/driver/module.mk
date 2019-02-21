###############################
# ARMv7 driver compile units #
###############################
ARMV7_DRIVER_DIR=arch/armv7/driver/
ARMV7_DRIVER_SOURCES_CC = ARMV7CacheControl.cc ARMGIC.cc OSCARMGIC.cc ARMv7PageTable.cc ARMMMU.cc ARMV7PerformanceCounter.cc ARMv7CacheColoring.cc
ARMV7_DRIVER_SOURCES_ASM = ARMV7CacheControl.S
ARMV7_DRIVER_OBJECTS = $(patsubst %.o,$(OBJDIR)/$(ARMV7_DRIVER_DIR)%.o,$(ARMV7_DRIVER_SOURCES_CC:%.cc=%.o)) $(patsubst %.oS,$(OBJDIR)/$(ARMV7_DRIVER_DIR)%.oS,$(ARMV7_DRIVER_SOURCES_ASM:%.S=%.oS))

COMPILE_OBJECTS += $(ARMV7_DRIVER_OBJECTS)
LINKER_OBJECTS += $(ARMV7_DRIVER_OBJECTS)