# Event handling code
EVENTHANDLING_DIR=eventhandling/
EVENTHANDLING_SOURCES_CC = EventHandler.cc EventPriorityQueue.cc EventQueue.cc StackPool.cc MultiQueuePriorityStrategy.cc
# FIXME make SchedulingStrategy configurable
# FifoStrategy.cc MultiFifoStrategy.cc 
EVENTHANDLING_SOURCES_ASM = 
EVENTHANDLING_OBJECTS = $(patsubst %o,$(OBJDIR)/$(EVENTHANDLING_DIR)%o,$(EVENTHANDLING_SOURCES_CC:%.cc=%.o)) $(patsubst %.oS,$(OBJDIR)/$(EVENTHANDLING_DIR)%.oS,$(EVENTHANDLING_SOURCES_ASM:%.S=%.oS))

COMPILE_OBJECTS += $(EVENTHANDLING_OBJECTS)
LINKER_OBJECTS += $(EVENTHANDLING_OBJECTS)