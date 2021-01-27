#define USE_INIT_ADDITIONAL

#include "../../parameters.h"

void Init_additional()
{
	if (diagonal == d45b_56t)
	{
		anal.fc_G = FiducialCut({{460E-6, 400E-6}, {+440E-6, 500E-6}, {-440E-6, 500E-6}, {-460E-6, 400E-6}});
	}

	if (diagonal == d45t_56b)
	{
		anal.fc_G = FiducialCut({{+450E-6, 400E-6}, {+420E-6, 500E-6}, {-420E-6, 500E-6}, {-450E-6, 400E-6}});
	}
}
