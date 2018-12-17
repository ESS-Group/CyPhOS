# Testcomponent code
TACLEBENCH_RECURSION_DIR=taclebench-components/recursion/

TACLEBENCH_RECURSION_SOURCES_CC = recursion.cc TaclerecursionComponent.cc
TACLEBENCH_RECURSION_SOURCES_ASM = 
TACLEBENCH_RECURSION_OBJECTS = $(patsubst %o,$(OBJDIR)/$(TACLEBENCH_RECURSION_DIR)%o,$(TACLEBENCH_RECURSION_SOURCES_CC:%.cc=%.o)) $(patsubst %.oS,$(OBJDIR)/$(TACLEBENCH_RECURSION_DIR)%.oS,$(TACLEBENCH_RECURSION_SOURCES_ASM:%.S=%.oS))


COMPILE_OBJECTS += $(TACLEBENCH_RECURSION_OBJECTS)
LINKER_OBJECTS += $(TACLEBENCH_RECURSION_OBJECTS)

CFLAGS += -DTACLE_RECURSION