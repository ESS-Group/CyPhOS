###############################
# ARMv7 driver compile units #
###############################
EXYNOS4412_DRIVER_BASE_DIR=arch/armv7/exynos4412/driver/
EXYNOS4412_DRIVER_BASE_SOURCES_CC = ExynosClockManagement.cc ExynosMultiCoreTimer.cc ExynosUartDriver.cc
EXYNOS4412_DRIVER_BASE_SOURCES_ASM = 
EXYNOS4412_DRIVER_BASE_OBJECTS = $(patsubst %.o,$(OBJDIR)/$(EXYNOS4412_DRIVER_BASE_DIR)%.o,$(EXYNOS4412_DRIVER_BASE_SOURCES_CC:%.cc=%.o)) $(patsubst %.oS,$(OBJDIR)/$(EXYNOS4412_DRIVER_BASE_DIR)%.oS,$(EXYNOS4412_DRIVER_BASE_SOURCES_ASM:%.S=%.oS))

COMPILE_OBJECTS += $(EXYNOS4412_DRIVER_BASE_OBJECTS)
LINKER_OBJECTS += $(EXYNOS4412_DRIVER_BASE_OBJECTS)