#include "common_alignment.hh"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "TFile.h"
#include "TGraph.h"

//----------------------------------------------------------------------------------------------------

void AlignmentSource::Load(const vector<edm::ParameterSet> &v)
{
	SetAlignmentA(atConstant);
	SetAlignmentB(atConstant);
	SetAlignmentC(atConstant);

	for (const auto &p : v)
	{
		const auto &unit = p.getParameter<string>("unit");
		const double a = p.getParameter<double>("a");
		const double b = p.getParameter<double>("b");
		const double c = p.getParameter<double>("c");

		if (unit == "L_2_F") { cnst.a_L_2_F = a; cnst.b_L_2_F = b; cnst.c_L_2_F = c; }
		if (unit == "L_1_F") { cnst.a_L_1_F = a; cnst.b_L_1_F = b; cnst.c_L_1_F = c; }
		if (unit == "R_1_F") { cnst.a_R_1_F = a; cnst.b_R_1_F = b; cnst.c_R_1_F = c; }
		if (unit == "R_2_F") { cnst.a_R_2_F = a; cnst.b_R_2_F = b; cnst.c_R_2_F = c; }
	}
}

//----------------------------------------------------------------------------------------------------

void AlignmentSource::InitOne(const string label, AlignmentType t, const string &fn, GraphSet &gs, const string &obj)
{
	printf(">> AlignmentSource::InitOne > alignment `%s': type %u\n", label.c_str(), t);

	if (t == atTimeDependent)
	{
		TFile *alF = new TFile(fn.c_str());

		if (alF->IsZombie())
		{
			printf("\tERROR: cannot open file with alignment graphs.\n");
			delete alF;
			return;
		}

		TGraph *g_L_2_F = (TGraph *) alF->Get(( string("L_2_F/") + obj).c_str() );
		TGraph *g_L_1_F = (TGraph *) alF->Get(( string("L_1_F/") + obj).c_str() );

		TGraph *g_R_1_F = (TGraph *) alF->Get(( string("R_1_F/") + obj).c_str() );
		TGraph *g_R_2_F = (TGraph *) alF->Get(( string("R_2_F/") + obj).c_str() );

		if (g_L_2_F && g_L_1_F && g_R_1_F && g_R_2_F)
		{
			printf("\talignment graphs successfully loaded\n");
		} else {
			printf("\tERROR: unable to load some alignment graphs\n");
			delete alF;
			return;
		}

		gs.L_2_F = new TGraph(*g_L_2_F);
		gs.L_1_F = new TGraph(*g_L_1_F);

		gs.R_1_F = new TGraph(*g_R_1_F);
		gs.R_2_F = new TGraph(*g_R_2_F);

		delete alF;
	}
}

//----------------------------------------------------------------------------------------------------

void AlignmentSource::Init()
{
	printf(">> AlignmentSource::Init\n");
	InitOne("a", type_a, src_a, gs_a, "a_fit");
	InitOne("b", type_b, src_b, gs_b, "b_fit");
	InitOne("c", type_c, src_c, gs_c, "c_fit");
}

//----------------------------------------------------------------------------------------------------

AlignmentData AlignmentSource::Eval(double timestamp) const
{
	AlignmentData d;

	// a
	if (type_a == atNone)
	{
		d.a_L_2_F = 0.;
		d.a_L_1_F = 0.;

		d.a_R_1_F = 0.;
		d.a_R_2_F = 0.;
	}

	if (type_a == atConstant)
	{
		d.a_L_2_F = cnst.a_L_2_F;
		d.a_L_1_F = cnst.a_L_1_F;

		d.a_R_1_F = cnst.a_R_1_F;
		d.a_R_2_F = cnst.a_R_2_F;
	}

	if (type_a == atTimeDependent)
	{
		d.a_L_2_F = gs_a.L_2_F->Eval(timestamp)*1E-3;
		d.a_L_1_F = gs_a.L_1_F->Eval(timestamp)*1E-3;

		d.a_R_1_F = gs_a.R_1_F->Eval(timestamp)*1E-3;
		d.a_R_2_F = gs_a.R_2_F->Eval(timestamp)*1E-3;
	}

	// b
	if (type_b == atNone)
	{
		d.b_L_2_F = 0.;
		d.b_L_1_F = 0.;

		d.b_R_1_F = 0.;
		d.b_R_2_F = 0.;
	}

	if (type_b == atConstant)
	{
		d.b_L_2_F = cnst.b_L_2_F;
		d.b_L_1_F = cnst.b_L_1_F;

		d.b_R_1_F = cnst.b_R_1_F;
		d.b_R_2_F = cnst.b_R_2_F;
	}

	if (type_b == atTimeDependent)
	{
		d.b_L_2_F = gs_b.L_2_F->Eval(timestamp)*1E-3;
		d.b_L_1_F = gs_b.L_1_F->Eval(timestamp)*1E-3;

		d.b_R_1_F = gs_b.R_1_F->Eval(timestamp)*1E-3;
		d.b_R_2_F = gs_b.R_2_F->Eval(timestamp)*1E-3;
	}

	// c
	if (type_c == atNone)
	{
		d.c_L_2_F = 0.;
		d.c_L_1_F = 0.;

		d.c_R_1_F = 0.;
		d.c_R_2_F = 0.;
	}

	if (type_c == atConstant)
	{
		d.c_L_2_F = cnst.c_L_2_F;
		d.c_L_1_F = cnst.c_L_1_F;

		d.c_R_1_F = cnst.c_R_1_F;
		d.c_R_2_F = cnst.c_R_2_F;
	}

	if (type_c == atTimeDependent)
	{
		d.c_L_2_F = gs_c.L_2_F->Eval(timestamp)*1E-3;
		d.c_L_1_F = gs_c.L_1_F->Eval(timestamp)*1E-3;

		d.c_R_1_F = gs_c.R_1_F->Eval(timestamp)*1E-3;
		d.c_R_2_F = gs_c.R_2_F->Eval(timestamp)*1E-3;
	}

	return d;
}
