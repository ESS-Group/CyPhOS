# Testcomponent code
TACLEBENCH_BINARYSEARCH_DIR=taclebench-components/binarysearch/

TACLEBENCH_BINARYSEARCH_SOURCES_CC = binarysearch.cc TaclebinarysearchComponent.cc
TACLEBENCH_BINARYSEARCH_SOURCES_ASM = 
TACLEBENCH_BINARYSEARCH_OBJECTS = $(patsubst %o,$(OBJDIR)/$(TACLEBENCH_BINARYSEARCH_DIR)%o,$(TACLEBENCH_BINARYSEARCH_SOURCES_CC:%.cc=%.o)) $(patsubst %.oS,$(OBJDIR)/$(TACLEBENCH_BINARYSEARCH_DIR)%.oS,$(TACLEBENCH_BINARYSEARCH_SOURCES_ASM:%.S=%.oS))


COMPILE_OBJECTS += $(TACLEBENCH_BINARYSEARCH_OBJECTS)
LINKER_OBJECTS += $(TACLEBENCH_BINARYSEARCH_OBJECTS)

CFLAGS += -DTACLE_BINARYSEARCH