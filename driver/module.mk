# Event handling code
DRIVER_DIR=driver/
DRIVER_SOURCES_CC = GenericTimer.cc ConsoleBuffer.cc GenericIO.cc
DRIVER_SOURCES_ASM = 
DRIVER_OBJECTS = $(patsubst %o,$(OBJDIR)/$(DRIVER_DIR)%o,$(DRIVER_SOURCES_CC:%.cc=%.o)) $(patsubst %.oS,$(OBJDIR)/$(DRIVER_DIR)%.oS,$(DRIVER_SOURCES_ASM:%.S=%.oS))

COMPILE_OBJECTS += $(DRIVER_OBJECTS)
LINKER_OBJECTS += $(DRIVER_OBJECTS)