# CyPhOS #

**CyPhOS** - Real-time operating system for cyber-physical systems

The system supports the ARMv7 (Cortex-A9) and AMD64 processor architecture.

## Prerequisites ##
To compile this operating system you need an arm (arm-none-eabi) toolchain. Currently supported is gcc 5.3 (newer/older versions might also work). Make sure your compiler is in your $PATH.

Also version 2 of [AspectC++](http://www.aspectc.org) is required to compile the operating system. Make sure that ag++ and ac++ is within you `$PATH` variable. 

### Supported GCC versions ###
 - 6.2 (or newer 6 series)
 - 7.3
 - 8.1, 8.2

Although older GCC versions might work CyPhOS is only developed and tested with the newest released GCC version (8.2).
 
#### Unsupported GCC versions ####
 - 6.1 Does not work because of a bug where volatile is ignored. [Bugreport](https://gcc.gnu.org/bugzilla/show_bug.cgi?id=70848)

### Compiling the crossdev-toolchain (necessary for ARMv7) ###

#### Setup the environment ####

First you need to configure the build environment with some variables.

```
export TARGET=arm-none-eabi
export PREFIX="$HOME/cross-compiler/$TARGET"
export PATH="$PREFIX/bin:$PATH"
```

$PREFIX is set to the installation directory of the cross-compiler toolchain.


#### Building binutils ####

 - Download the binutils sourcecode [here](ftp://ftp.gnu.org/gnu/binutils/) to some directory
 - Change into the directory of the sourcecode: `cd $somedirectory`
 - Extract the sourcecode: `tar xf binutils-$VERSION.tar.bz2`
 - Create the build directory: `mkdir binutils-build`
 - Change into the build directory: `cd binutils-build`
 - Configure the binutils sources: `../binutils-$VERSION/configure --target=$TARGET --prefix="$PREFIX" --with-sysroot --disable-nls --disable-werror`
 - Compile the sources: `make`
 - Install the compiled tools: `make install`
 
#### Building crossdev gcc ####
 - Download the gcc sourcecode [here](ftp://ftp.gnu.org/gnu/gcc/gcc-8.2.0) to some directory
 - Change into the directory of the sourcecode: `cd $somedirectory`
 - Extract the sourcecode: `tar xf gcc-$VERSION.tar.bz2`
 - Create the build directory: `mkdir gcc-build`
 - Change into the build directory: `cd gcc-build`
 - Configure the gcc sources: `../gcc-$VERSION/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --enable-languages=c,c++ --without-headers`
 - Compile the bootstrap gcc: `make all-gcc`
 - Compile the additional libgcc: `make all-target-libgcc`
 - Install the compiler and libgcc: `make install-gcc; make install-target-libgcc`

You must also include the path of your installed compiler to your $PATH variable.

## Building of the OS ##

To compile the OS the specific board needs to be set. This can be done by setting the  `CYPHOS_ARCH` environment variable.

 - `export CYPHOS_ARCH="BOARDVALUE"` where BOARDVALUE is one of the targets below.

After setting the variable you can just call make -jN (where N is the number of threads to compiler)

During the compilation a script is started to generate parts of the linker script which enables the operating system
to preload components during runtime.

- `make -j4` (for example)

This will result in an elf binary under `build/boot` and a raw binary under `build/boot.img`

To boot this image look under the board specific documentation.

## Supported Hardware ##

Currently there is only support for ARM systems with Cortex-A9 processors and AMD64 system.

**List of currently supported platforms:**

 - PandaBoard ES
 - Hardkernel ODroid U3+
 - Wandboard (www.wandboard.org)

## Platform-specific guides ##

### ODroid-U3 ###

#### Build ####

The board specific value for the architecture variable (BOARDVALUE above) is `armv7/exynos4412`

#### Booting ####

To boot the operating system you need a bootloader with support for baremetal booting and without the MMU enabled already.
You can use a modified version of u-boot for exynos from:

[Github](https://github.com/djselbeck/u-boot) which can boot baremetal images via fastboot

### PandaBoard ES ###

#### Build ####

The board specific value for the architecture variable (BOARDVALUE above) is `armv7/omap4460`

#### Booting ####

To boot the operating system from USB you need the usbboot bootloader. 

[Github](https://github.com/djselbeck/omap4boot) (Booting with usb3 ports is a bit unstable)

As an alternative I recommend setting up network based PXEboot with U-boot.

### Wandboard (Quad) ###

#### Build ####

The board specific value for the architecture variable (BOARDVALUE above) is `armv7/imx6`

#### Booting ####

The generated image file is best booted via u-boot direct loading. You can use loady to load the binary `build/boot.img`

To load this image use the following command sequence in u-boot:

 - `loady` - This will listen for a ymodem file transfer (can be initiated via minicom for example)
 - Send the build/boot.img to the serial console of u-boot.
 - `go 0x12000000` this needs to be the address of the file transfer. This instructs u-boot to branch(jump) the cpu to the startup code of CyPhOS.

As an alternative I recommend setting up network based PXEboot with U-boot.

### AMD64 architecture ###

You only need gcc with g++ and ag++ installed on your system.

Compilation can be done via:
 - `export CYPHOS_ARCH=x86`
 - `make iso`
 - This will result in an .iso file that you can dd on an USB stick or boot via qemu directly. Also the generated ELF image is multiboot compliant.
