
#include <component/OSC.h>
MEMBER_OF_OSC_BEGIN(Tacle_ComplexUpdatesComponent)

/*
 * This is ugly, I know. The reason behind it is to ensure that the
 * tacle-benchmarks can be used unmodified.
 */
#include <submodules/tacle-bench/bench/kernel/complex_updates/complex_updates.c>

MEMBER_OF_OSC_END
