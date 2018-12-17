# Testcomponent code
TACLEBENCH_IIR_DIR=taclebench-components/iir/

TACLEBENCH_IIR_SOURCES_CC = iir.cc TacleiirComponent.cc
TACLEBENCH_IIR_SOURCES_ASM = 
TACLEBENCH_IIR_OBJECTS = $(patsubst %o,$(OBJDIR)/$(TACLEBENCH_IIR_DIR)%o,$(TACLEBENCH_IIR_SOURCES_CC:%.cc=%.o)) $(patsubst %.oS,$(OBJDIR)/$(TACLEBENCH_IIR_DIR)%.oS,$(TACLEBENCH_IIR_SOURCES_ASM:%.S=%.oS))


COMPILE_OBJECTS += $(TACLEBENCH_IIR_OBJECTS)
LINKER_OBJECTS += $(TACLEBENCH_IIR_OBJECTS)

CFLAGS += -DTACLE_IIR