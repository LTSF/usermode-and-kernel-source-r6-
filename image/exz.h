#pragma once
#include <cstddef>
#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h> 
#define EXP_2_MAGIC_USSS() UsefulFuntion(rand(), rand(), rand(), rand());

inline int UsefulFuntion(int x, int y, int z, int r)
{
	for (int i = 0; i < 4; i++)
	{
		x++;
		y--;
		z += 20;
		r -= 40;
	}
	for (int i = 0; i < 5; i++)
	{
		x++;
		y += 32;
		z -= 20;
		r += 40;
	}
	return x + y + z + r;
}