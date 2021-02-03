import root;
import pad_layout;
include "../common.asy";
include "../run_info.asy";

TH2_palette = Gradient(blue, heavygreen, yellow, red);

string datasets[], ds_labels[];
datasets.push("DS-fill7301/Totem1"); ds_labels.push("7301");
datasets.push("DS-fill7302/Totem1"); ds_labels.push("7302");

string rows[];
rows.push("top");
rows.push("bottom");

string rps[], rp_labels[], rp_lhc_labels[];
rps.push("L_2_F"); rp_labels.push("45-220-fr"); rp_lhc_labels.push("XRPV.B6L5.B2");
rps.push("L_1_F"); rp_labels.push("45-210-fr"); rp_lhc_labels.push("XRPV.D6L5.B2");
rps.push("R_1_F"); rp_labels.push("56-210-fr"); rp_lhc_labels.push("XRPV.D6R5.B1");
rps.push("R_2_F"); rp_labels.push("56-220-fr"); rp_lhc_labels.push("XRPV.B6R5.B1");

TGraph_errorBar = None;

//xTicksDef = LeftTicks(1., 0.5);
//yTicksDef = RightTicks(0.01, 0.005);

//----------------------------------------------------------------------------------------------------

void PlotRate(RootObject obj, pen p, string label)
{
	for (int i = 0; i < obj.iExec("GetN"); ++i)
	{
		real ax[] = {0.};
		real ay[] = {0.};
		obj.vExec("GetPoint", i, ax, ay);
		real x = ax[0];
		real y = ay[0];
		real x_unc = obj.rExec("GetErrorX", i);
		real y_unc = obj.rExec("GetErrorY", i);

		real l = 2. * x_unc;
		real r = y / l;
		real r_unc = y_unc / l;

		real sc = 1./3600;

		draw(((x-x_unc)*sc, r)--((x+x_unc)*sc, r), p);
		draw((x*sc, r-r_unc)--(x*sc, r+r_unc), p);
		draw((x*sc, r), mCi+2pt+p);

		//write(format("%#.3f", (x-x_unc)*sc) + format(" %#.3f", (x+x_unc)*sc) + format(" %#.3E", r));
	}

	AddToLegend(label, p, mCi+2pt+p);
}

//----------------------------------------------------------------------------------------------------

NewPad(false);
NewPad(false);
for (int rpi : rps.keys)
{
	NewPadLabel("\vbox{\hbox{" + rp_labels[rpi] + "}\hbox{" + rp_lhc_labels[rpi] + "}}");
}

for (int dsi : datasets.keys)
{
	NewRow();

	NewPadLabel(ds_labels[dsi] + ":");

	string f = topDir + datasets[dsi] + "/pileup_combined.root";

	for (int ri : rows.keys)
	{
		NewRow();

		NewPad(false);
		NewPadLabel(rows[ri]);

		for (int rpi : rps.keys)
		{
			NewPad("time from 14 Oct 2018$\ung{h}$", "prescaled rate$\ung{Hz}$");
			scale(Linear, Linear(true));

			real y_max = 1000;
			DrawBands(ds_labels[dsi], bands="run", labels="ds", 0., y_max);

			string row = rows[ri];
			string rp = rps[rpi];

			bool arm45 = (find(rp, "L") == 0);
			bool arm56 = (find(rp, "R") == 0);

			string diagonal = "";
			if ((arm45 && row == "bottom") || (arm56 && row == "top")) diagonal = "45b_56t";
			if ((arm45 && row == "top") || (arm56 && row == "bottom")) diagonal = "45t_56b";

			//write(row + ", " + rp + " -> " + diagonal);

			write("");
			write("* " + ds_labels[dsi] + ", " + rp_lhc_labels[rpi] + ", " + row);

			PlotRate(RootGetObject(f, diagonal + "/" + rps[rpi] + "/tr/val"), blue, "single track");
			PlotRate(RootGetObject(f, diagonal + "/" + rps[rpi] + "/pat_suff/val"), red, "track(s) + showers");

			ylimits(0, y_max, Crop);
		}
	}

	frame f_legend = BuildLegend();

	NewPad(false);
	attach(f_legend);
}

GShipout("event_category_rates", vSkip=1mm);
