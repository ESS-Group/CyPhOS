# OMAP 4460 make module
LD_SCRIPT := arch/armv7/omap4460/ldscript-omap4460
BASE_ADDRESS := 0x40008000

# compiler/linker flags
ASMFLAGS += -include arch/armv7/omap4460/omap4460.h
CFLAGS += -include arch/armv7/omap4460/omap4460.h

###############################
# OMAP 4460 specific code     #
###############################
OMAP4460_GENERIC_DIR=arch/armv7/omap4460/
OMAP4460_GENERIC_SOURCES_CC = omap4460_smp.cc OMAPDebug.cc
OMAP4460_GENERIC_SOURCES_ASM = 
OMAP4460_GENERIC_OBJECTS = $(patsubst %.o,$(OBJDIR)/$(OMAP4460_GENERIC_DIR)%.o,$(OMAP4460_GENERIC_SOURCES_CC:%.cc=%.o)) $(patsubst %.oS,$(OBJDIR)/$(OMAP4460_GENERIC_DIR)%.oS,$(OMAP4460_GENERIC_SOURCES_ASM:%.S=%.oS))

COMPILE_OBJECTS += $(OMAP4460_GENERIC_OBJECTS)
LINKER_OBJECTS += $(OMAP4460_GENERIC_OBJECTS)


# include exynos submodules
SUB_MODULES = driver boot

include $(patsubst %,\
$(OMAP4460_GENERIC_DIR)%/module.mk,$(SUB_MODULES))

#######################
# ARMv7 specific code #
#######################

BASE_MODULES += arch/armv7 arch/armv7/cortexa9/common arch/armv7/cortexa9
CONFIG_CORTEXA9_GLOBAL_TIMER=true

include $(patsubst %,\
%/module.mk,$(BASE_MODULES))
