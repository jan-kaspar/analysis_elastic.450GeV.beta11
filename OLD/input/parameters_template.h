#include "parameters_global.h"

unsigned int timestamp_min = 56448;
unsigned int timestamp_max = 59057;

void Init_base()
{
	// load global settings
	Init_global();

	//env.UseMatchedOptics();
}

//----------------------------------------------------------------------------------------------------

void Init_45b_56t()
{
	Init_global_45b_56t();

	// normalisation settings
	anal.L_int = 1.;
}

//----------------------------------------------------------------------------------------------------

void Init_45t_56b()
{
	Init_global_45t_56b();

	// normalisation settings
	anal.L_int = 1.;
}
