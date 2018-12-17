###############################
# ARMv7 driver compile units #
###############################
IMX6_DRIVER_BASE_DIR=arch/armv7/imx6/driver/
IMX6_DRIVER_BASE_SOURCES_CC = IMX6UartDriver.cc IMX6ClockManagement.cc IMX6ECSPI.cc MCP2515SPI.cc IMX6GPIO.cc CanTXBuffer.cc CanRXBuffer.cc IMX6Watchdog.cc IMX6TempMonitor.cc
IMX6_DRIVER_BASE_SOURCES_ASM = 
IMX6_DRIVER_BASE_OBJECTS = $(patsubst %.o,$(OBJDIR)/$(IMX6_DRIVER_BASE_DIR)%.o,$(IMX6_DRIVER_BASE_SOURCES_CC:%.cc=%.o)) $(patsubst %.oS,$(OBJDIR)/$(IMX6_DRIVER_BASE_DIR)%.oS,$(IMX6_DRIVER_BASE_SOURCES_ASM:%.S=%.oS))

COMPILE_OBJECTS += $(IMX6_DRIVER_BASE_OBJECTS)
LINKER_OBJECTS += $(IMX6_DRIVER_BASE_OBJECTS)