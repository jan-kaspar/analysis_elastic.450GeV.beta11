#include "parameters_global.h"

unsigned int timestamp_min = 58711;
unsigned int timestamp_max = 62660;

string timberDir = "2018_10_14_fill7301";

void Init_base()
{
	// load global settings
	Init_global();

	anal.alignment_t0 = timestamp_min;
	anal.alignment_ts = (timestamp_max - timestamp_min) / 2.;

	// alignment
	AlignmentSource alSrc;
	alSrc.SetAlignmentA(atConstant);
	alSrc.SetAlignmentB(atConstant);
	alSrc.SetAlignmentC(atConstant);

	// first number: from alignment.cc, second number: TODO source
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

	// analysis settings
	anal.cut1_c = +28E-6;
	anal.cut2_c = +4E-6;

	anal.cut5_c = -0.00;
	anal.cut6_c = +0.00;

	anal.cut7_c = -0.09;
	anal.cut8_c = -0.12;

	anal.cut9_c = +0.09;
	anal.cut10_c = -0.02;


	// normalisation settings
	anal.L_int = 1.;
}

//----------------------------------------------------------------------------------------------------

void Init_45t_56b()
{
	Init_global_45t_56b();

	// analysis settings
	anal.cut1_c = +20E-6;
	anal.cut2_c = +1E-6;

	anal.cut5_c = -0.03;
	anal.cut6_c = +0.03;

	anal.cut7_c = -0.07;
	anal.cut8_c = +0.46;

	anal.cut9_c = +0.09;
	anal.cut10_c = +0.00;

	// normalisation settings
	anal.L_int = 1.;
}
