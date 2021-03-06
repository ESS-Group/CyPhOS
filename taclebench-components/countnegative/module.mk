# Testcomponent code
TACLEBENCH_COUNTNEGATIVE_DIR=taclebench-components/countnegative/

TACLEBENCH_COUNTNEGATIVE_SOURCES_CC = countnegative.cc TacleCountnegativeComponent.cc
TACLEBENCH_COUNTNEGATIVE_SOURCES_ASM = 
TACLEBENCH_COUNTNEGATIVE_OBJECTS = $(patsubst %o,$(OBJDIR)/$(TACLEBENCH_COUNTNEGATIVE_DIR)%o,$(TACLEBENCH_COUNTNEGATIVE_SOURCES_CC:%.cc=%.o)) $(patsubst %.oS,$(OBJDIR)/$(TACLEBENCH_COUNTNEGATIVE_DIR)%.oS,$(TACLEBENCH_COUNTNEGATIVE_SOURCES_ASM:%.S=%.oS))


COMPILE_OBJECTS += $(TACLEBENCH_COUNTNEGATIVE_OBJECTS)
LINKER_OBJECTS += $(TACLEBENCH_COUNTNEGATIVE_OBJECTS)

CFLAGS += -DTACLE_COUNTNEGATIVE