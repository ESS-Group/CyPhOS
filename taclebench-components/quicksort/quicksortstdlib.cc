
#include <component/OSC.h>
MEMBER_OF_OSC_BEGIN(TaclequicksortComponent)

/*
 * This is ugly, I know. The reason behind it is to ensure that the
 * tacle-benchmarks can be used unmodified.
 */
#include <submodules/tacle-bench/bench/kernel/quicksort/quicksortstdlib.c>

MEMBER_OF_OSC_END
