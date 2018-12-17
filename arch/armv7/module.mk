# cross-compiler prefix
# ARM 32bit armv7-a compatible gcc prefix
CROSS_COMPILE := arm-none-eabi-

# cross-compiler binutils
ASM := $(CROSS_COMPILE)gcc
CC := $(CROSS_COMPILE)gcc
CXX := $(CROSS_COMPILE)g++
LD := $(CROSS_COMPILE)ld
OBJCOPY := $(CROSS_COMPILE)objcopy
OBJDUMP := $(CROSS_COMPILE)objdump
ALIGNMENT := 0x10000

# Armv7-a specific compiler/linker flags
ASMFLAGS += -I. -I$(ARMV7_GENERIC_DIR) -march=armv7-a -mcpu=cortex-a9 -mtune=cortex-a9 -D__ASSEMBLY__
LDFLAGS += -marmelf
#-mlong-calls
#-Wextra -pedantic 
#CFLAGS := -Wall -I. -march=armv7-a -mcpu=cortex-a9 -g $(CFLAGS_ADD) -mhard-float -mfpu=fpa -nostdlib -mlong-calls -ffreestanding -fno-builtin
# FIXME -Wpedantic disabled for now because of aspectc++ bug
CFLAGS += -march=armv7-a -mcpu=cortex-a9 -mtune=cortex-a9 $(CFLAGS_ADD) -mfloat-abi=softfp -I$(ARMV7_GENERIC_DIR) -include common/armv7.h -mlong-calls

###############################
# ARMv7 generic compile units #
###############################
ARMV7_GENERIC_DIR=arch/armv7/
ARMV7_GENERIC_SOURCES_CC = 
ARMV7_GENERIC_SOURCES_ASM = 
ARMV7_GENERIC_OBJECTS = $(patsubst %.o,$(OBJDIR)/$(ARMV7_GENERIC_DIR)%.o,$(ARMV7_GENERIC_SOURCES_CC:%.cc=%.o)) $(patsubst %.oS,$(OBJDIR)/$(ARMV7_GENERIC_DIR)%.oS,$(ARMV7_GENERIC_SOURCES_ASM:%.S=%.oS))

COMPILE_OBJECTS += $(ARMV7_GENERIC_OBJECTS)
LINKER_OBJECTS += $(ARMV7_GENERIC_OBJECTS)


SUB_MODULES = driver interrupts core eventhandling

# Configuration options

CONFIG_ARMV7 := true

include $(patsubst %,\
$(ARMV7_GENERIC_DIR)%/module.mk,$(SUB_MODULES))
