
#include <component/OSC.h>
MEMBER_OF_OSC_BEGIN(Taclefir2dimComponent)

/*
 * This is ugly, I know. The reason behind it is to ensure that the
 * tacle-benchmarks can be used unmodified.
 */
#include <submodules/tacle-bench/bench/kernel/fir2dim/fir2dim.c>

MEMBER_OF_OSC_END
