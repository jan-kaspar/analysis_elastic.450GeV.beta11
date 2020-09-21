#define USE_INIT_ADDITIONAL

#include "../../parameters.h"

void Init_additional()
{
	if (diagonal == d45b_56t)
	{
		anal.fc_G = FiducialCut({{-155E-6, 400E-6}, {-150E-6, 160E-6}, {+150E-6, 160E-6}, {+155E-6, 400E-6}, {+150E-6, 500E-6}, {-150E-6, 500E-6}});
	}

	if (diagonal == d45t_56b)
	{
		anal.fc_G = FiducialCut({{-155E-6, 400E-6}, {-150E-6, 180E-6}, {+150E-6, 180E-6}, {+155E-6, 400E-6}, {+150E-6, 500E-6}, {-150E-6, 500E-6}});
	}
}
