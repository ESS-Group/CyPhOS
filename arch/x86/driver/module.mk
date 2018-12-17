###############################
# X86 driver compile units    #
###############################
X86_DRIVER_DIR=arch/x86/driver/
X86_DRIVER_SOURCES_CC = CGAScreen.cc DebugStream.cc LocalAPIC.cc IOAPIC.cc APICTimer.cc ACPI.cc LegacyPIC.cc X86MMU.cc UART.cc LegacyKeyboard.cc InputOutputAggregator.cc SMPManager.cc CATManager.cc
X86_DRIVER_SOURCES_ASM = 
X86_DRIVER_OBJECTS = $(patsubst %.o,$(OBJDIR)/$(X86_DRIVER_DIR)%.o,$(X86_DRIVER_SOURCES_CC:%.cc=%.o)) $(patsubst %.oS,$(OBJDIR)/$(X86_DRIVER_DIR)%.oS,$(X86_DRIVER_SOURCES_ASM:%.S=%.oS))

COMPILE_OBJECTS += $(X86_DRIVER_OBJECTS)
LINKER_OBJECTS += $(X86_DRIVER_OBJECTS)