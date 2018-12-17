# Common code
COMMON_DIR=common/
COMMON_SOURCES_CC = baremetal.cc OutputStream.cc SyncOutput.cc Queue.cc greeting.cc RandomGenerator.cc
COMMON_SOURCES_ASM = 
COMMON_OBJECTS = $(patsubst %o,$(OBJDIR)/$(COMMON_DIR)%o,$(COMMON_SOURCES_CC:%.cc=%.o)) $(patsubst %.oS,$(OBJDIR)/$(COMMON_DIR)%.oS,$(COMMON_SOURCES_ASM:%.S=%.oS))

COMPILE_OBJECTS += $(COMMON_OBJECTS)
LINKER_OBJECTS += $(COMMON_OBJECTS)