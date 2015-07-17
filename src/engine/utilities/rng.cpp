#include <limits>
#include <time.h>

#include "rng.h"

namespace genesis {

RNG::RNG() : rseed(1), mti(RNG_N+1)
{
}//RNG::RNG

//returns a number from 0 - n excluding n
unsigned int RNG::random( unsigned int n ) {
	unsigned long y;
	static unsigned long mag01[2]={0x0, RNG_MATRIX_A};

	if( n == 0 )
		return 0;

	/* mag01[x] = x * MATRIX_A  for x=0,1 */

	if( mti >= RNG_N ) { /* generate N words at one time */
		unsigned int kk;

		if( mti == RNG_N+1 )   /* if sgenrand() has not been called, */
			setRandomSeed(4357); /* a default initial seed is used   */

		for( kk = 0; kk < RNG_N-RNG_M; kk++ ) {
			y = (mt[kk]&RNG_UPPER_MASK) | (mt[kk+1]&RNG_LOWER_MASK);
			mt[kk] = mt[kk+RNG_M] ^ (y >> 1) ^ mag01[y & 0x1];
		}
		for( ; kk < RNG_N-1; kk++ ) {
			y = (mt[kk]&RNG_UPPER_MASK) | (mt[kk+1]&RNG_LOWER_MASK);
			mt[kk] = mt[kk+(RNG_M-RNG_N)] ^ (y >> 1) ^ mag01[y & 0x1];
		}
		y = (mt[RNG_N-1]&RNG_UPPER_MASK) | (mt[0]&RNG_LOWER_MASK);
		mt[RNG_N-1] = mt[RNG_M-1] ^ (y >> 1) ^ mag01[y & 0x1];

		mti = 0;
	}

	y = mt[mti++];
	y ^= RNG_TEMPERING_SHIFT_U(y);
	y ^= RNG_TEMPERING_SHIFT_S(y) & RNG_TEMPERING_MASK_B;
	y ^= RNG_TEMPERING_SHIFT_T(y) & RNG_TEMPERING_MASK_C;
	y ^= RNG_TEMPERING_SHIFT_L(y);

	return (y%n);
}//RNG::random(unsigned int)

float RNG::random() {
	float r = (float)random(std::numeric_limits<unsigned int>::max());
	float divisor = (float)std::numeric_limits<unsigned int>::max();
	return (r / divisor);
}//RNG::random()

void RNG::setRandomSeed( unsigned int n ) {
	mt[0] = n & 0xffffffff;
	for( mti = 1; mti < RNG_N; mti++ )
		mt[mti] = (69069 * mt[mti-1]) & 0xffffffff;

	rseed = n;
}//RNG::setRandomSeed

unsigned int RNG::getRandomSeed() {
	return rseed;
}//RNG::getRandomSeed

void RNG::randomize() {
	setRandomSeed((unsigned int)time(0));
}//RNG::randomize


}
