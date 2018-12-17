/*
 * bitmanipulation.h
 *
 *  Created on: 20.03.2015
 *      Author: hendrik
 */

#ifndef COMMON_BITMANIPULATION_H_
#define COMMON_BITMANIPULATION_H_

/**
 * Sets bit n of variable val to flag
 */
template <class T> inline T setbit(T val, unsigned n, bool flag)
{ return flag ? (val | (1 << n)) : (val & (~(1 << n))); }

/**
 * Returns true if bit n of variable val is set.
 * Otherwise returns false.
 */
template <class T> inline bool testbit(T val, unsigned n)
{ return (val & (1 << n)); }



#endif /* COMMON_BITMANIPULATION_H_ */
