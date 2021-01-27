#ifndef _common_alignment_hh_
#define _common_alignment_hh_

namespace edm
{
	class ParameterSet;
}

class TGraph;

#include <string>
#include <vector>

using namespace std;

//----------------------------------------------------------------------------------------------------

struct AlignmentData
{
	//	a: xy coupling in rad
	//	b: x shift in mm
	//	c: y shift in mm
	double a_L_2_F, b_L_2_F, c_L_2_F;
	double a_L_1_F, b_L_1_F, c_L_1_F;

	double a_R_1_F, b_R_1_F, c_R_1_F;
	double a_R_2_F, b_R_2_F, c_R_2_F;

	AlignmentData()
	{
		a_L_2_F = b_L_2_F = c_L_2_F = 0.;
		a_L_1_F = b_L_1_F = c_L_1_F = 0.;

		a_R_1_F = b_R_1_F = c_R_1_F = 0.;
		a_R_2_F = b_R_2_F = c_R_2_F = 0.;
	}
};

//----------------------------------------------------------------------------------------------------

enum AlignmentType { atNone, atConstant, atTimeDependent };

//----------------------------------------------------------------------------------------------------

struct AlignmentSource
{
	struct GraphSet
	{
		TGraph *L_2_F, *L_1_F, *R_1_F, *R_2_F;
		GraphSet() : L_2_F(nullptr), L_1_F(nullptr), R_1_F(nullptr), R_2_F(nullptr)
		{
		}
	} gs_a, gs_b, gs_c;

	AlignmentData cnst;

	AlignmentType type_a, type_b, type_c;
	string src_a, src_b, src_c;

	AlignmentSource() : type_a(atNone), type_b(atNone), type_c(atNone)
	{
	}

	void SetAlignmentA(AlignmentType t, const string &fn = "")
	{
		type_a = t;
		src_a = fn;
	}

	void SetAlignmentB(AlignmentType t, const string &fn = "")
	{
		type_b = t;
		src_b = fn;
	}

	void SetAlignmentC(AlignmentType t, const string &fn = "")
	{
		type_c = t;
		src_c = fn;
	}

	void Load(const vector<edm::ParameterSet> &v);

	void InitOne(const string label, AlignmentType t, const string &fn, GraphSet &gs, const string &obj);

	void Init();

	AlignmentData Eval(double timestamp) const;
};

#endif
