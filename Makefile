# CyPhOS Makefile #

OBJDIR := ./build
DEPDIR := ./deps

# bootable image
SYSTEM_IMAGE := boot

# Filename for the aspectc++ repository
ASPECTC_REPO := aspect.acp

# Linker script created by OSC script
OSC_LD_SCRIPT=OSC.ld


############################
# Generic sourcecode files #
############################
SUB_MODULES := arch/$(CYPHOS_ARCH)


# Disabled modules: stresstest baremetal
SUB_MODULES += cachemanagement common eventhandling interrupts core sync debug testcomponents driver benchmarking

#SUB_MODULES += stresstest

# Baremetal test components to verify some behavior
# SUB_MODULES += baremetal

# CAN benchmarks
SUB_MODULES += canbenchmark-components

# Enable tacle benchmark suite (requires submodule)
#SUB_MODULES += taclebench-components

include $(patsubst %,\
%/module.mk,$(SUB_MODULES))


# AspectC++ compiler 
ACXX := ag++

# Generic compiler/linker flags
LDLIBS += -L $(shell dirname $(shell $(CXX) --print-file-name=libgcc.a)) -lgcc
#-mlong-calls
#-Wextra -pedantic 
#CFLAGS := -Wall -I. -march=armv7-a -mcpu=cortex-a9 -g $(CFLAGS_ADD) -mhard-float -mfpu=fpa -nostdlib -mlong-calls -ffreestanding -fno-builtin
# FIXME -Wpedantic disabled for now because of aspectc++ bug
CFLAGS += -O3 -fno-strict-aliasing -Wall -Werror -I. $(CFLAGS_ADD) -nostdlib -ffreestanding -fno-builtin -fpic -include common/config.h -include common/debug.h -Wpedantic
CXXFLAGS += $(CFLAGS) -fno-stack-protector -Wno-non-virtual-dtor -fno-rtti -fno-unwind-tables -fno-exceptions -fnon-call-exceptions -fno-use-cxa-atexit -Wno-write-strings $(CXXFLAGS_ADD) -fno-omit-frame-pointer -std=c++14
ACCFLAGS += --keep_woven --c_compiler $(CXX) -p . -r $(ASPECTC_REPO) $(CXXFLAGS)

# Include git commit as define
CFLAGS += -DGIT_COMMIT=$$(git rev-parse HEAD)


# Create dependencies
$(DEPDIR)/%.d : %.c
	@echo "DEP		$@"
	@if test \( ! \( -d $(@D) \) \) ;then mkdir -p $(@D);fi
	$(OUTPUT) $(CC) $(CFLAGS) -MM -MT $(OBJDIR)/$*.o -MF $@ $<

$(DEPDIR)/%.d : %.cc
	@echo "DEP		$@"
	@if test \( ! \( -d $(@D) \) \) ;then mkdir -p $(@D);fi
	$(OUTPUT) $(CXX) $(CXXFLAGS) -MM -MT $(OBJDIR)/$*.o -MF $@ $<


# create binary object files
$(OBJDIR)/%.o : %.c
	@echo "CC		$@"
	@if test \( ! \( -d $(@D) \) \) ;then mkdir -p $(@D);fi
	$(OUTPUT) $(CC) -c $(CFLAGS) -o $@ $<

#$(OBJDIR)/%.o : %.cc
#	@echo "ACXX		$@"
#	@if test \( ! \( -d $(@D) \) \) ;then mkdir -p $(@D);fi
#	$(OUTPUT) $(ACXX) -c $(ACCFLAGS) -o $@ $<

$(OBJDIR)/%.o : %.cc
	@echo "CXX		$@"
	@if test \( ! \( -d $(@D) \) \) ;then mkdir -p $(@D);fi
	$(OUTPUT) $(CXX) -c $(CXXFLAGS) -o $@ $<

$(OBJDIR)/%.oS : %.S
	@echo "ASM		$@"
	@if test \( ! \( -d $(@D) \) \) ;then mkdir -p $(@D);fi
	$(OUTPUT) $(ASM) -c $(ASMFLAGS) -o $@ $<
	


#:%.o=%.d
DEP_FILES = $(patsubst $(OBJDIR)/%.o,$(DEPDIR)/%.d,$(filter %.o,$(COMPILE_OBJECTS)))

all: $(OBJDIR)/$(SYSTEM_IMAGE).img
#	echo "objects to compile: " $(COMPILE_OBJECTS)
#	echo "objects to directly link: " $(LINKER_OBJECTS)
#	echo "Depfiles: " $(DEP_FILES) 
	
# Generate OSC linker script
$(OSC_LD_SCRIPT): $(COMPILE_OBJECTS)
	@echo "Generating OSC linker script"
	./scripts/createlinkerscript.sh $(OBJDIR) $(OSC_LD_SCRIPT) $(OBJDUMP) $(ALIGNMENT)
	
# Link final binary
$(OBJDIR)/$(SYSTEM_IMAGE): $(OBJECT) $(OSC_LD_SCRIPT) $(LD_SCRIPT)
	@echo "First stage linking"
	@echo "LD		$@"
	@if test \( ! \( -d $(@D) \) \) ;then mkdir -p $(@D);fi
	$(OUTPUT) $(LD) -static -T $(LD_SCRIPT) -o $(@) $(LDFLAGS) $(LINKER_OBJECTS) $(LDLIBS)
# Include following command to create object mappings that can be used to later identify trigger/events
	@if test \( ! \( -d temp_files \) \) ;then mkdir -p temp_files;fi
	$(OUTPUT) $(OBJDUMP) -t --demangle $(@) > temp_files/objmapping_$(shell date +%d-%m-%y_+%H-%M)
# Save config for later verifications
	$(CXX) $(CXXFLAGS) -x c++ -dM -E - < /dev/null > temp_files/config_$(shell date +%d-%m-%y_+%H-%M)

	
# '$(SYSTEM_FILE).img' binary to boot directly baremetal

$(OBJDIR)/$(SYSTEM_IMAGE).img : $(OBJDIR)/$(SYSTEM_IMAGE)
	@echo "OBJCOPY		$@"
	@if test \( ! \( -d $(@D) \) \) ;then mkdir -p $(@D);fi
	$(OUTPUT) $(OBJCOPY) -O binary --image-base $(BASE_ADDRESS) -R ".benchmark_results" $< $@
	
	
	
# clean target
clean:	
	@echo "RM		$(OBJDIR)"
	$(OUTPUT) rm -rf $(OBJDIR)
	@echo "RM		$(DEPDIR)"
	$(OUTPUT) rm -rf $(DEPDIR)
	@echo "RM		$(CONFIG_FILE)"
	$(OUTPUT) rm -rf $(CONFIG_FILE)
	$(OUTPUT) rm -f $(ASPECTC_REPO)
#	@echo "Remove generated preload data"
#	$(OUTPUT) rm -rf $(PRELOAD_DIR)
	

# include dependencies
ifneq ($(MAKECMDGOALS),clean)
-include $(DEP_FILES)
endif

.PHONY: clean generate_preloaddata
	
	