import root;
import pad_layout;
include "../common.asy";
include common_code;

string dataset = "merged";

string diagonal = "combined";
string binning = "eb";

// TODO
string fit_file = "/afs/cern.ch/work/j/jkaspar/analyses/elastic/6500GeV/combined/coulomb_analysis_1/fits/2500-2rp-ob-2-10-0.05/exp3,t_max=0.15/fit.root";
string fit_obj = "g_fit_CH";

string unc_file = "/afs/cern.ch/work/j/jkaspar/analyses/elastic/6500GeV/beta2500/2rp/systematics/matrix.root";
string unc_types[], unc_labels[];
pen unc_pens[];
//unc_types.push("all"); unc_pens.push(blue+opacity(0.5)); unc_labels.push("all");
unc_types.push("all-but-norm"); unc_pens.push(green); unc_labels.push("all except normalisation");

xSizeDef = 8cm;
ySizeDef = 6cm;

A_ref = 632.;
B_ref = 20.4;
ref_str = MakeRefStr();


//----------------------------------------------------------------------------------------------------

real EvalRatio(RootObject f, real x)
{
	/*
	real y = f.rExec("Eval", x);
	real y_ref = A_ref * exp(- B_ref * x);
	return y / y_ref;
	*/

	int N = f.iExec("GetN");

	for (int i = 0; i < N-1; ++i)
	{
		real tca[] = {0.};
		real sca[] = {0.};
		f.vExec("GetPoint", i, tca, sca);

		real tna[] = {0.};
		real sna[] = {0.};
		f.vExec("GetPoint", i+1, tna, sna);

		if (tca[0] < x && x <= tna[0])
		{
			real ref_c = A_ref * exp(- B_ref * tca[0]);
			real ref_n = A_ref * exp(- B_ref * tna[0]);

			real rat_c = sca[0] / ref_c;
			real rat_n = sna[0] / ref_n;

			return rat_c + (rat_n - rat_c) / (tna[0] - tca[0]) * (x - tca[0]);
		}
	}

	return 0;
}

//----------------------------------------------------------------------------------------------------

void DrawUncRelBand(RootObject bc, RootObject relUnc, pen p)
{
	if (relUnc.InheritsFrom("TGraph"))
	{
		int N = bc.iExec("GetN");

		guide g;
		guide g_u, g_b;

		for (int i = 0; i < N; ++i)
		{
			real ta[] = {0.};
			real sa[] = {0.};

			bc.vExec("GetPoint", i, ta, sa);
			real ru = relUnc.rExec("Eval", ta);

			real y_ref = A_ref * exp(- B_ref * ta[0]);

			real c_rel = sa[0] / y_ref - 1.;
			real c_rel_pl = sa[0] * (1. + ru) / y_ref - 1.;
			real c_rel_mi = sa[0] * (1. - ru) / y_ref - 1.;

			g_u = g_u -- Scale((ta[0], c_rel_pl));
			g_b = g_b -- Scale((ta[0], c_rel_mi));
		}

		g_b = reverse(g_b);
		filldraw(g_u--g_b--cycle, p, nullpen);
	}

	if (relUnc.InheritsFrom("TH1"))
	{
		guide g_u, g_b;

		for (int bi = 1; bi < relUnc.iExec("GetNbinsX"); ++bi)
		{
			real c = relUnc.rExec("GetBinCenter", bi);
			real w = relUnc.rExec("GetBinWidth", bi);
			real ru = relUnc.rExec("GetBinContent", bi);

			real v_rat = EvalRatio(bc, c);

			real v_rel = v_rat - 1.;
			real v_rel_pl = v_rat * (1. + ru) - 1.;
			real v_rel_mi = v_rat * (1. - ru) - 1.;

			g_u = g_u -- Scale((c-w/2, v_rel_pl)) -- Scale((c+w/2, v_rel_pl));
			g_b = g_b -- Scale((c-w/2, v_rel_mi)) -- Scale((c+w/2, v_rel_mi));
		}

		g_b = reverse(g_b);
		filldraw(g_u--g_b--cycle, p, nullpen);
	}
}

//----------------------------------------------------------------------------------------------------

NewPad("$|t|\ung{GeV^2}$", "${\d\si/\d t - \hbox{ref} \over \hbox{ref}}\ ,\ \ \hbox{ref} = $ "+ref_str+"");
currentpad.xTicks = LeftTicks(0.05, 0.01);
currentpad.yTicks = RightTicks(0.02, 0.01);

AddToLegend("<systematic uncertainties:");
for (int ui : unc_types.keys)
{
	RootObject fit = RootGetObject(fit_file, fit_obj);
	RootObject relUnc = RootGetObject(unc_file, "matrices/" + unc_types[ui] + "/" + binning + "/h_stddev");
	DrawUncRelBand(fit, relUnc, unc_pens[ui]);
	AddToLegend(unc_labels[ui], mSq+6pt+unc_pens[ui]);
}

AddToLegend("<data with statistical uncertainties:");
DrawRelDiff(
	RootGetObject(topDir+"DS-merged/merged.root", binning+"/"+dataset+"/"+diagonal+"/h_dsdt"),
	red+0.8pt);
AddToLegend("data", mPl+4pt+(red+0.8pt));

limits((0, -0.03), (0.25, 0.07), Crop);
xaxis(YEquals(0, false), dashed);

AttachLegend();
