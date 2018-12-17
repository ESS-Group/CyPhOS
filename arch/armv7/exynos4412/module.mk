# Exynos 4412 specific Makefile (only tested on Odroid-U3+)

LD_SCRIPT := arch/armv7/exynos4412/ldscript-exynos4412
BASE_ADDRESS := 0x40008000

# compiler/linker flags
CFLAGS += -include arch/armv7/exynos4412/exynos4412.h -Iarch/armv7/exynos4412
ASMFLAGS += -include arch/armv7/exynos4412/exynos4412.h


###############################
# Exynos 4412 specific code   #
###############################
EXYNOS4412_GENERIC_DIR=arch/armv7/exynos4412/
EXYNOS4412_GENERIC_SOURCES_CC = exynos_smp.cc ExynosDebug.cc
EXYNOS4412_GENERIC_SOURCES_ASM = uart_debug.S
EXYNOS4412_GENERIC_OBJECTS = $(patsubst %.o,$(OBJDIR)/$(EXYNOS4412_GENERIC_DIR)%.o,$(EXYNOS4412_GENERIC_SOURCES_CC:%.cc=%.o)) $(patsubst %.oS,$(OBJDIR)/$(EXYNOS4412_GENERIC_DIR)%.oS,$(EXYNOS4412_GENERIC_SOURCES_ASM:%.S=%.oS))

COMPILE_OBJECTS += $(EXYNOS4412_GENERIC_OBJECTS)
LINKER_OBJECTS += $(EXYNOS4412_GENERIC_OBJECTS)


# include exynos submodules
SUB_MODULES = driver boot

include $(patsubst %,\
$(EXYNOS4412_GENERIC_DIR)%/module.mk,$(SUB_MODULES))

#######################
# ARMv7 specific code #
#######################

BASE_MODULES += arch/armv7 arch/armv7/cortexa9/common arch/armv7/cortexa9


include $(patsubst %,\
%/module.mk,$(BASE_MODULES))
