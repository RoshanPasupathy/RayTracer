#ifndef _MORTON_CODE_H_
#define _MORTON_CODE_H_
#define MORT_MULT 1000
/*
TODO:
1. Need min and max values of coordinates
*/

#include <stdint.h>
#include <assert.h>
#include "Vector3.h"
typedef uint64_t mcode_t;

#define K_MORT 2
// most lsb bits we can encode
#define NBITS ((sizeof(mcode_t) * 8 / (K_MORT + 1)) - 1)

// right shift needed to be bring number down to nbits
mcode_t red_rshift[3] = { 0, 0, 0 }; // REDuction right shift
mcode_t logN = 1;
// fast computation of morton code. Actually need only logN bits 
mcode_t masks[NBITS] = { 0 };
mcode_t lshifts[NBITS];


void compute_encode_params()
{
	mcode_t N = 16;// 7;//NBITS;
	mcode_t i = 0;
	mcode_t n;
	// divide by 2 in every iteration
	for (n = N;; i++)
	{
		//assert(n == N >> i);
		mcode_t submask = ((mcode_t) 1 << n) - 1; // n 1s in LSB
		// 1 << i == N / n
		masks[i] = 0;
		for (mcode_t j = 0; j < (1 << i); j++)
		{
			masks[i] = (masks[i] << (n * (K_MORT + 1))) | submask;
		}
		lshifts[i] = n * K_MORT;
		if (n == 1) break;

		n = ((n & 1) == 0) ? n >> 1 : (n >> 1) + 1;
	}
	logN = i + 1;
}
#include <bitset>
#include <iostream>
// recursive divide and conquer approach
void compute_masks(mcode_t N, mcode_t offset, mcode_t level)
{
	// get your result from left and right child
	if (N == 1)
	{
		masks[level] |= ((mcode_t) 1 << offset);
		return;
	}
	mcode_t res = ((mcode_t) 1 << N) - (mcode_t) 1;
	masks[level] |= (res << offset);

	mcode_t leftn = N >> (mcode_t)1;
	mcode_t rightn = ((N & 1) == 0) ? leftn : leftn + 1;
	compute_masks(rightn, offset, level + 1); // right child
	compute_masks(rightn, offset + rightn * (K_MORT + 1), level + 1); // left child
}

void compute_lshifts(mcode_t N)
{
	mcode_t i = 0;
	mcode_t n;
	// divide by 2 in every iteration
	for (n = N;; i++)
	{
		// 1 << i == N / n
		lshifts[i] = n * K_MORT;
		if (n == 1) break;

		n = ((n & 1) == 0) ? n >> 1 : (n >> 1) + 1;
	}
	logN = i + 1;
}

//#ifndef log2
//mcode_t _log2(mcode_t x)
//{
//	mcode_t logx = 0;
//	while (x >> logx != 1) logx++;
//	return logx;
//}
//#endif // !log2

void compute_encode_rshift(Vector3 vec)
{
	assert(vec.x() >= 0 && vec.y() >= 0 && vec.z() >= 0);
	// convert to unsigned int
	mcode_t x = (mcode_t)(vec.x() * MORT_MULT);
	mcode_t y = (mcode_t)(vec.y() * MORT_MULT);
	mcode_t z = (mcode_t)(vec.z() * MORT_MULT);

	mcode_t nbits_val = log2(x) + 1;
	red_rshift[0] = (nbits_val > NBITS) ? nbits_val - NBITS : 0;

	nbits_val = log2(y) + 1;
	red_rshift[1] = (nbits_val > NBITS) ? nbits_val - NBITS : 0;

	nbits_val = log2(z) + 1;
	red_rshift[2] = (nbits_val > NBITS) ? nbits_val - NBITS : 0;

	//std::bitset<sizeof(mcode_t) * 8> rb(((mcode_t)1 << NBITS) - 1);
	//std::bitset<sizeof(mcode_t) * 8> xb(x);
	//std::bitset<sizeof(mcode_t) * 8> yb(y);
	//std::bitset<sizeof(mcode_t) * 8> zb(z);

	//std::cout << "Max Number: " << rb << std::endl;
	//std::cout << " X  Number: " << xb << " RSHIFT:" << red_rshift[0] << std::endl;
	//std::cout << " Y  Number: " << yb << " RSHIFT:" << red_rshift[1] << std::endl;
	//std::cout << " Z  Number: " << zb << " RSHIFT:" << red_rshift[2] << std::endl;
}

mcode_t interleave(mcode_t num)
{
	num &= masks[0];
	for (uint8_t i = 1; i < logN; i++)
	{
		num = (num ^ (num << lshifts[i])) & masks[i];
	}
	return num;
}

void _mortoncode_init_params(Vector3 range)
{
	compute_masks(NBITS, 0, 0);
	compute_lshifts(NBITS);
	compute_encode_rshift(range);
}

/*
PREREQ: update functions are called
compute_masks
compute_lshifts
compute_encode_rshift
*/
mcode_t _mortoncode(Vector3 vec)
{
	//assert(vec.x() >= 0 && vec.y() >= 0 && vec.z() >= 0);
	// coonvert to unsigned int
	mcode_t x = (mcode_t) (vec.x() * MORT_MULT);
	mcode_t y = (mcode_t) (vec.y() * MORT_MULT);
	mcode_t z = (mcode_t) (vec.z() * MORT_MULT);

	x = x >> red_rshift[0];
	y = y >> red_rshift[1];
	z = z >> red_rshift[2];
	return (interleave(x) << 2) | (interleave(y) << 1) | interleave(z);
}


void mcode_print_params()
{
	std::cout << std::endl;
	for (uint8_t i = 0; i < logN; i++)
	{
		std::bitset<sizeof(mcode_t) * 8> x(masks[i]);
		std::cout << "Mask: " << x << "  LSHIFT: " << lshifts[i] << std::endl;
	}
	

	//std::cout << "HEX" << std::endl;
	//for (uint8_t i = 0; i < logN; i++)
	//{
	//	std::cout << "Mask: " << std::hex << masks[i] << "  ,lshift: " << lshifts[i] << std::endl;
	//}
}
#endif
