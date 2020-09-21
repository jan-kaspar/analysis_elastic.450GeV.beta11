#include "parameters_global.h"

unsigned int timestamp_min = 58711;
unsigned int timestamp_max = 76799;

string timberDir = "no_timber_dir";

void Init_base()
{
	// load global settings
	Init_global();

	// select input
	distilledNtuples.clear();
	distilledNtuples.push_back("../../../DS-fill7301/Totem1");
	//distilledNtuples.push_back("../../../DS-fill7302/Totem1"); // TODO: this requires a different alignment

	// update settings
	anal.alignment_t0 = timestamp_min;
	anal.alignment_ts = (timestamp_max - timestamp_min) / 2.;

	// crude alignment
	AlignmentSource alSrc;
	alSrc.SetAlignmentA(atConstant);
	alSrc.SetAlignmentB(atConstant);
	alSrc.SetAlignmentC(atConstant);

	alSrc.cnst.a_L_2_F = 0E-3; alSrc.cnst.b_L_2_F = +100E-3 - 40E-3; alSrc.cnst.c_L_2_F = +250E-3;
	alSrc.cnst.a_L_1_F = 0E-3; alSrc.cnst.b_L_1_F = +450E-3 + 40E-3; alSrc.cnst.c_L_1_F =  -50E-3;

	alSrc.cnst.a_R_1_F = 0E-3; alSrc.cnst.b_R_1_F = +150E-3 - 40E-3; alSrc.cnst.c_R_1_F = -300E-3;
	alSrc.cnst.a_R_2_F = 0E-3; alSrc.cnst.b_R_2_F = +850E-3 + 40E-3; alSrc.cnst.c_R_2_F = -200E-3;

	alignmentSources.push_back(alSrc);
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
