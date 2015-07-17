#ifndef RNG_H
#define RNG_H

namespace genesis {

/* Period parameters */
#define RNG_N						17U			// Matrix Size
#define RNG_M						13U			// Matrix Period
#define RNG_MATRIX_A				0x9908b0df	// constant vector a
#define RNG_UPPER_MASK				0x80000000	// most significant w-r bits
#define RNG_LOWER_MASK				0x7fffffff	// least significant r bits

/* Tempering parameters */
#define RNG_TEMPERING_MASK_B		0x9d2c5680
#define RNG_TEMPERING_MASK_C		0xefc60000
#define RNG_TEMPERING_SHIFT_U(y)	(y >> 11)
#define RNG_TEMPERING_SHIFT_S(y)	(y << 7)
#define RNG_TEMPERING_SHIFT_T(y)	(y << 15)
#define RNG_TEMPERING_SHIFT_L(y)	(y >> 18)

//Mersenne Twister algorithm based on Game Coding Complete source code
class RNG
{
private:
	unsigned int		rseed;
	unsigned long		mt[RNG_N]; //the array for the state vector
	unsigned int		mti; //mti==N+1 means mt[N] is not initialized

public:
	RNG();

	unsigned int random( unsigned int n );
	float random();
	void setRandomSeed( unsigned int n );
	unsigned int getRandomSeed();
	void randomize();
};

}

#endif // RNG_H
