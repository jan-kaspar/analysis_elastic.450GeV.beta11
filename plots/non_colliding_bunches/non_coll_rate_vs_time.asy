import root;
import pad_layout;
include "../run_info.asy";

string topDir = "../../";

string datasets[], fills[];
pen d_pens[];
datasets.push("DS-fill7284/EmptyBX"); fills.push("7284"); d_pens.push(red);
datasets.push("DS-fill7289/EmptyBX"); fills.push("7289"); d_pens.push(blue);

string conditions[], c_labels[];
pen c_pens[];
conditions.push("tr"); c_labels.push("tr"); c_pens.push(blue);
conditions.push("pat_suff"); c_labels.push("pat-suff"); c_pens.push(red);

xSizeDef = 10cm;
ySizeDef = 6cm;

xTicksDef = LeftTicks(0.5, 0.1);

TGraph_errorBar = None;

//----------------------------------------------------------------------------------------------------

void Print(RootObject obj_tr, RootObject obj_all)
{
	int n = obj_tr.iExec("GetN");
	for (int i = 0; i < n; ++i)
	{
		real ax[] = {0.};
		real ay[] = {0.};

		real prescale = 59;

		obj_tr.vExec("GetPoint", i, ax, ay);
		real t = ax[0]/3600, v_tr = ay[0] * 59;

		obj_all.vExec("GetPoint", i, ax, ay);
		real v_all = ay[0] * 59;

		real t_w = obj_tr.rExec("GetErrorX", i) * 2. / 3600;

		write(format("%.5f", t) + format(",%.5f", t_w) + format(",%.3f", v_tr) + format(",%.3f", v_all));
	}
}

//----------------------------------------------------------------------------------------------------

void DrawOne(string name, string rps[], string rp_labels[])
{
	for (int dsi : datasets.keys)
	{
		NewRow();

		NewPadLabel("fill " + fills[dsi]);

		for (int rpi : rps.keys)
		{
			NewPad("time$\ung{hours}$", "rate$\ung{Hz}$");

			for (int ci : conditions.keys)
			{
				RootObject obj = RootGetObject(topDir + datasets[dsi] + "/pileup_combined.root", rps[rpi] + "/" + conditions[ci] + "/rate");
				real prescale = 59.;
				draw(scale(1./3600, prescale), obj, "l,p", c_pens[ci], mCi+3pt+c_pens[ci]);
			}

			real y_max = 100.;
			if (name == "45" && fills[dsi] == "7284")
				y_max = 1000;

			ylimits(0, y_max, Crop);

			AttachLegend(rp_labels[rpi]);

			// print out
			write("");
			write("# fill " + fills[dsi] + ", RP " + rp_labels[rpi]);
			RootObject obj_tr = RootGetObject(topDir + datasets[dsi] + "/pileup_combined.root", rps[rpi] + "/tr/rate");
			RootObject obj_pat_suff = RootGetObject(topDir + datasets[dsi] + "/pileup_combined.root", rps[rpi] + "/pat_suff/rate");
			Print(obj_tr, obj_pat_suff);
		}
	}

	GShipout("non_coll_rate_vs_time_" + name, hSkip=0mm, vSkip=0mm);
}

//----------------------------------------------------------------------------------------------------

string rps[], rp_labels[];
rps.push("45t_56b/L_1_F"); rp_labels.push("XRPV.D6L5.B2/top");
rps.push("45t_56b/L_2_F"); rp_labels.push("XRPV.B6L5.B2/top");
rps.push("45b_56t/L_1_F"); rp_labels.push("XRPV.D6L5.B2/bot");
rps.push("45b_56t/L_2_F"); rp_labels.push("XRPV.B6L5.B2/bot");

DrawOne("45", rps, rp_labels);

//----------------------------------------------------------------------------------------------------

string rps[], rp_labels[];
rps.push("45b_56t/R_1_F"); rp_labels.push("XRPV.D6R5.B1/top");
rps.push("45b_56t/R_2_F"); rp_labels.push("XRPV.B6R5.B1/top");
rps.push("45t_56b/R_1_F"); rp_labels.push("XRPV.D6R5.B1/bot");
rps.push("45t_56b/R_2_F"); rp_labels.push("XRPV.B6R5.B1/bot");

DrawOne("56", rps, rp_labels);

//----------------------------------------------------------------------------------------------------

/*
NewPad(false);
for (int ti : types.keys)
	AddToLegend(t_labels[ti], t_pens[ti]);
AttachLegend();
*/

//----------------------------------------------------------------------------------------------------

GShipout(vSkip=0mm);
