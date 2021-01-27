#include "parameters_global.h"

unsigned int timestamp_min = 69013;
unsigned int timestamp_max = 76799;

string timberDir = "2018_10_14_fill7302";

void Init_base()
{
	// load global settings
	Init_global();

	anal.alignment_t0 = timestamp_min;
	anal.alignment_ts = (timestamp_max - timestamp_min) / 4.;

	// alignment
	AlignmentSource alSrc;
	alSrc.SetAlignmentA(atConstant);
	alSrc.SetAlignmentB(atConstant);
	alSrc.SetAlignmentC(atConstant);

	// first number: from alignment.cc, second number: TODO source
	// TODO: why the numbers
	alSrc.cnst.a_L_2_F = 0E-3; alSrc.cnst.b_L_2_F = +300E-3 - 10E-3; alSrc.cnst.c_L_2_F = +250E-3;
	alSrc.cnst.a_L_1_F = 0E-3; alSrc.cnst.b_L_1_F = +700E-3 + 10E-3; alSrc.cnst.c_L_1_F =  -50E-3;
                                                                   
	alSrc.cnst.a_R_1_F = 0E-3; alSrc.cnst.b_R_1_F = -200E-3 - 10E-3; alSrc.cnst.c_R_1_F = -300E-3;
	alSrc.cnst.a_R_2_F = 0E-3; alSrc.cnst.b_R_2_F = +600E-3 + 10E-3; alSrc.cnst.c_R_2_F = -200E-3;

	alignmentSources.push_back(alSrc);
}

//----------------------------------------------------------------------------------------------------

void Init_45b_56t()
{
	Init_global_45b_56t();

	// analysis settings
	anal.cut1_c = +45E-6;
	anal.cut2_c = +7E-6;

	anal.cut5_c = +0.02;
	anal.cut6_c = -0.009;

	anal.cut7_c = -0.08;
	anal.cut8_c = -0.24;

	anal.cut9_c = +0.10;
	anal.cut10_c = +0.02;

	// normalisation settings
	anal.L_int = 1.;
}

//----------------------------------------------------------------------------------------------------

void Init_45t_56b()
{
	Init_global_45t_56b();

	// analysis settings
	anal.cut1_c = +39E-6;
	anal.cut2_c = +0E-6;

	anal.cut5_c = -0.02;
	anal.cut6_c = +0.009;

	anal.cut7_c = -0.01;
	anal.cut8_c = +0.32;

	anal.cut9_c = +0.09;
	anal.cut10_c = +0.06;

	// normalisation settings
	anal.L_int = 1.;
}
