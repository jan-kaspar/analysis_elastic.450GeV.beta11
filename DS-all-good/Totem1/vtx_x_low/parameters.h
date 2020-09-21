#define USE_INIT_ADDITIONAL

#include "../../parameters.h"

void Init_additional()
{
	anal.vtx_x_max = -0.1;

	if (diagonal == d45b_56t)
	{
		anal.fc_G = FiducialCut({{-400E-6, 160E-6}, {+200E-6, 160E-6}, {+540E-6, 220E-6}, {+440E-6, 500E-6}, {-440E-6, 500E-6}, {-540E-6, 220E-6}});
	}

	if (diagonal == d45t_56b)
	{
		anal.fc_G = FiducialCut({{-200E-6, 180E-6}, {+400E-6, 180E-6}, {+520E-6, 220E-6}, {+420E-6, 500E-6}, {-420E-6, 500E-6}, {-520E-6, 220E-6}});
	}
}
