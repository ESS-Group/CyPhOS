
#include <component/OSC.h>
MEMBER_OF_OSC_BEGIN(Tacle_BitcountComponent)

/*
 * This is ugly, I know. The reason behind it is to ensure that the
 * tacle-benchmarks can be used unmodified.
 */
#include <submodules/tacle-bench/bench/kernel/bitcount/bitcnt_4.c>

MEMBER_OF_OSC_END
