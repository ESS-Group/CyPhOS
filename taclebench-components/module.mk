# Testcomponent code
TACLEBENCH_DIR=taclebench-components/

TACLEBENCH_MODULES := binarysearch
TACLEBENCH_MODULES += bitcount
TACLEBENCH_MODULES += bitonic
TACLEBENCH_MODULES += bsort
TACLEBENCH_MODULES += complex_updates
TACLEBENCH_MODULES += countnegative
TACLEBENCH_MODULES += fac
TACLEBENCH_MODULES += fft

# Disabled because of crash
TACLEBENCH_MODULES += filterbank

TACLEBENCH_MODULES += fir2dim
TACLEBENCH_MODULES += iir
TACLEBENCH_MODULES += insertsort
TACLEBENCH_MODULES += jfdctint

# Disabled because of floating point constant
#TACLEBENCH_MODULES += lms
TACLEBENCH_MODULES += ludcmp
TACLEBENCH_MODULES += matrix1
TACLEBENCH_MODULES += md5
TACLEBENCH_MODULES += minver
TACLEBENCH_MODULES += pm
TACLEBENCH_MODULES += prime
TACLEBENCH_MODULES += quicksort
TACLEBENCH_MODULES += recursion
# Disabled because of null redefinition
#TACLEBENCH_MODULES += sha
TACLEBENCH_MODULES += st

# Disable unknown pragma warning for tacle-benchmark
CFLAGS += -Wno-error=unknown-pragmas -Wno-error=array-bounds -Wno-error=maybe-uninitialized

# Enable globale TACLEBENCH define
CFLAGS += -DCONFIG_TACLEBENCH

include $(patsubst %,\
$(TACLEBENCH_DIR)%/module.mk,$(TACLEBENCH_MODULES))

# Shared sources


TACLEBENCH_SOURCES_CC = TacleBenchInitiator.cc
TACLEBENCH_SOURCES_ASM = 
TACLEBENCH_OBJECTS = $(patsubst %o,$(OBJDIR)/$(TACLEBENCH_DIR)%o,$(TACLEBENCH_SOURCES_CC:%.cc=%.o)) $(patsubst %.oS,$(OBJDIR)/$(TACLEBENCH_DIR)%.oS,$(TACLEBENCH_SOURCES_ASM:%.S=%.oS))


COMPILE_OBJECTS += $(TACLEBENCH_OBJECTS)
LINKER_OBJECTS += $(TACLEBENCH_OBJECTS)