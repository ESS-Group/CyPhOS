
###############################
# i.MX6 generic compile units #
###############################
IMX6_BOOT_GENERIC_DIR=arch/armv7/imx6/boot/
IMX6_BOOT_GENERIC_SOURCES_CC = initOS.cc
IMX6_BOOT_GENERIC_SOURCES_ASM =
IMX6_BOOT_GENERIC_OBJECTS = $(patsubst %.o,$(OBJDIR)/$(IMX6_BOOT_GENERIC_DIR)%.o,$(IMX6_BOOT_GENERIC_SOURCES_CC:%.cc=%.o)) $(patsubst %.oS,$(OBJDIR)/$(IMX6_BOOT_GENERIC_DIR)%.oS,$(IMX6_BOOT_GENERIC_SOURCES_ASM:%.S=%.oS))

COMPILE_OBJECTS += $(IMX6_BOOT_GENERIC_OBJECTS)
LINKER_OBJECTS += $(IMX6_BOOT_GENERIC_OBJECTS)


SUB_MODULES =

include $(patsubst %,\
$(IMX6_BOOT_GENERIC_DIR)%/module.mk,$(SUB_MODULES))
