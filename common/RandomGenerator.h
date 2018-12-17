/*
 * RandomGenerator.h
 *
 *  Created on: 26.04.2017
 *      Author: hendrik
 */

#ifndef COMMON_RANDOMGENERATOR_H_
#define COMMON_RANDOMGENERATOR_H_

#include <common/types.h>

class RandomGenerator {
public:
	RandomGenerator();

	uint8_t random8(void);
	uint32_t random32(void);
	void srandom32(uint32_t seed);

	static RandomGenerator mInstance[NR_CPUS];
private:
	uint32_t mState;

};

#endif /* COMMON_RANDOMGENERATOR_H_ */
