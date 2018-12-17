# i.MX6 make module
LD_SCRIPT := arch/armv7/imx6/ldscript-imx6
BASE_ADDRESS := 0x12000000

# compiler/linker flags
ASMFLAGS += -include arch/armv7/imx6/imx6.h
CFLAGS += -include arch/armv7/imx6/imx6.h -Iarch/armv7/imx6


###############################
# i.MX6 specific code     #
###############################
IMX6_GENERIC_DIR=arch/armv7/imx6/
IMX6_GENERIC_SOURCES_CC = imx6_smp.cc IMX6Debug.cc
IMX6_GENERIC_SOURCES_ASM = 
IMX6_GENERIC_OBJECTS = $(patsubst %.o,$(OBJDIR)/$(IMX6_GENERIC_DIR)%.o,$(IMX6_GENERIC_SOURCES_CC:%.cc=%.o)) $(patsubst %.oS,$(OBJDIR)/$(IMX6_GENERIC_DIR)%.oS,$(IMX6_GENERIC_SOURCES_ASM:%.S=%.oS))

COMPILE_OBJECTS += $(IMX6_GENERIC_OBJECTS)
LINKER_OBJECTS += $(IMX6_GENERIC_OBJECTS)


# include exynos submodules
SUB_MODULES = driver boot

include $(patsubst %,\
$(IMX6_GENERIC_DIR)%/module.mk,$(SUB_MODULES))

#######################
# ARMv7 specific code #
#######################

BASE_MODULES += arch/armv7 arch/armv7/cortexa9/common arch/armv7/cortexa9

CONFIG_CAN=true
CONFIG_CORTEXA9_GLOBAL_TIMER=true

include $(patsubst %,\
%/module.mk,$(BASE_MODULES))
