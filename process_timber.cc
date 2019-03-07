#include "common_definitions.h"
#include "common_algorithms.h"
#include "parameters.h"
#include "common.h"

#include "TFile.h"
#include "TGraphErrors.h"
#include "TH1D.h"

using namespace std;

//----------------------------------------------------------------------------------------------------

const double E = 450.;	// GeV
const double m = 0.938;	// GeV

const map<string, double> m_beta_st = {
	{ "h", 11. },
	{ "v", 11. },
};

//----------------------------------------------------------------------------------------------------

void ProcessOne(TGraph *g_em, const string &beam, const string &proj)
{
	const double beta_st = m_beta_st.find(proj)->second;

	const double ga = E / m;

	TGraph *g_emit = new TGraph();
	TGraph *g_bd = new TGraph();
	TGraph *g_vtx = new TGraph();

	for (int i = 0; i < g_em->GetN(); ++i)
	{
		double time = g_em->GetX()[i];
		const double emit = g_em->GetY()[i] * 1E-6;

		time -= timestamp0;

		const double bd = sqrt(emit / ga / beta_st);
		const double vtx = sqrt(emit / ga * beta_st);

		int idx = g_emit->GetN();
		g_emit->SetPoint(idx, time, emit);
		g_bd->SetPoint(idx, time, bd);
		g_vtx->SetPoint(idx, time, vtx);
	}

	g_emit->Write(("g_emit_" + beam + "_" + proj).c_str());
	g_bd->Write(("g_bd_" + beam + "_" + proj).c_str());
	g_vtx->Write(("g_vtx_" + beam + "_" + proj).c_str());
}

//----------------------------------------------------------------------------------------------------

void ProcessCombined(TGraph *g_em_b1, TGraph *g_em_b2, const string &beam, const string &proj)
{
	const double beta_st = m_beta_st.find(proj)->second;

	const double ga = E / m;

	//TGraph *g_emit = new TGraph();
	TGraph *g_bd = new TGraph();
	TGraph *g_vtx = new TGraph();

	for (int i = 0; i < g_em_b1->GetN(); ++i)
	{
		double time = g_em_b1->GetX()[i];
		const double emit1 = g_em_b1->GetY()[i] * 1E-6;
		const double emit2 = g_em_b2->Eval(time) * 1E-6;

		if (emit1 < 0. || emit2 < 0.)
			continue;

		time -= timestamp0;

		const double bd1 = sqrt(emit1 / ga / beta_st);
		const double bd2 = sqrt(emit2 / ga / beta_st);
		const double bd = sqrt(bd1*bd1 + bd2*bd2);

		const double vtx1 = sqrt(emit1 / ga * beta_st);
		const double vtx2 = sqrt(emit2 / ga * beta_st);
		const double vtx = sqrt(vtx1*vtx1 + vtx2*vtx2) / 2.;

		int idx = g_bd->GetN();
		//g_emit->SetPoint(idx, time, emit);
		g_bd->SetPoint(idx, time, bd);
		g_vtx->SetPoint(idx, time, vtx);
	}

	//g_emit->Write(("g_emit_" + beam + "_" + proj).c_str());
	g_bd->Write(("g_bd_" + beam + "_" + proj).c_str());
	g_vtx->Write(("g_vtx_" + beam + "_" + proj).c_str());
}

//----------------------------------------------------------------------------------------------------

int main(int argc, char **argv)
{
	if (argc != 2)
		return 1;

	// init diagonal
	Init(argv[1]);
	if (diagonal != dCombined)
		return rcIncompatibleDiagonal;

	// get input data
	TFile *f_in = TFile::Open(("/afs/cern.ch/exp/totem/scratch/data/RP/" + timberDir + "/conditions/emittance.root").c_str());

	TGraph *g_emit_B1_H = (TGraph *) f_in->Get("LHC.BSRT.5R4.B1:AVERAGE_WEIGHTED_EMITTANCE_H");
	TGraph *g_emit_B1_V = (TGraph *) f_in->Get("LHC.BSRT.5R4.B1:AVERAGE_WEIGHTED_EMITTANCE_V");

	TGraph *g_emit_B2_H = (TGraph *) f_in->Get("LHC.BSRT.5L4.B2:AVERAGE_WEIGHTED_EMITTANCE_H");
	TGraph *g_emit_B2_V = (TGraph *) f_in->Get("LHC.BSRT.5L4.B2:AVERAGE_WEIGHTED_EMITTANCE_V");
	
	// prepare output
	TFile *f_out = new TFile("process_timber.root", "recreate");

	// run processing
	ProcessOne(g_emit_B1_H, "b1", "h");
	ProcessOne(g_emit_B1_V, "b1", "v");

	ProcessOne(g_emit_B2_H, "b2", "h");
	ProcessOne(g_emit_B2_V, "b2", "v");

	ProcessCombined(g_emit_B1_H, g_emit_B2_H, "comb", "h");
	ProcessCombined(g_emit_B1_V, g_emit_B2_V, "comb", "v");

	// clean up
	delete f_in;
	delete f_out;

	return 0;
}
