# Testcomponent code
TACLEBENCH_MATRIX1_DIR=taclebench-components/matrix1/

TACLEBENCH_MATRIX1_SOURCES_CC = matrix1.cc Taclematrix1Component.cc
TACLEBENCH_MATRIX1_SOURCES_ASM = 
TACLEBENCH_MATRIX1_OBJECTS = $(patsubst %o,$(OBJDIR)/$(TACLEBENCH_MATRIX1_DIR)%o,$(TACLEBENCH_MATRIX1_SOURCES_CC:%.cc=%.o)) $(patsubst %.oS,$(OBJDIR)/$(TACLEBENCH_MATRIX1_DIR)%.oS,$(TACLEBENCH_MATRIX1_SOURCES_ASM:%.S=%.oS))


COMPILE_OBJECTS += $(TACLEBENCH_MATRIX1_OBJECTS)
LINKER_OBJECTS += $(TACLEBENCH_MATRIX1_OBJECTS)

CFLAGS += -DTACLE_MATRIX1