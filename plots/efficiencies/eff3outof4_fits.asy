import root;
import pad_layout;
include "../common.asy";

string datasets[], fills[];
datasets.push("DS-fill7301/Totem1"); fills.push("7301");
datasets.push("DS-fill7302/Totem1"); fills.push("7302");

string diagonals[];
diagonals.push("45b_56t");
diagonals.push("45t_56b");

string rps[], rp_labels[];
rps.push("L_2_F"); rp_labels.push("45-220-fr");
rps.push("L_1_F"); rp_labels.push("45-210-fr");
rps.push("R_1_F"); rp_labels.push("56-210-fr");
rps.push("R_2_F"); rp_labels.push("56-220-fr" );

xSizeDef = 6cm;
ySizeDef = 5cm;
yTicksDef = RightTicks(5., 1.);

int gx=0, gy=0;

TF1_nPoints = 4;

//----------------------------------------------------------------------------------------------------

for (int dsi : datasets.keys)
{
	real rp_eff_cen[] = { 0.96, 0.98, 0.97, 0.95};
	//if (datasets[dsi] == "DS2a")
	//	rp_eff_cen = new real[] { 0.95, 0.98, 0.98, 0.96 };

	for (int dgi : diagonals.keys)
	{
		string f = topDir + datasets[dsi] + "/eff3outof4_fit.root";
		string opt = "vl,eb";
		
		++gy; gx = 0;
		for (int rpi : rps.keys)
		{
			++gx;
			NewPad(false, gx, gy);
			label("{\SetFontSizesXX " + rp_labels[rpi] + "}");
		}
		
		NewPad(false, -1, gy);
		label(replace("\vbox{\SetFontSizesXX\hbox{dataset: "+datasets[dsi]+"}\hbox{diagonal: "+diagonals[dgi]+"}}", "_", "\_"));

		frame fLegend;

		++gy; gx = 0;
		for (int rpi : rps.keys)
		{
			string d = diagonals[dgi] + "/" + rps[rpi];

			++gx;
			NewPad("$\th_y^*\ung{\mu rad}$", "\ung{\%}", gx, gy);
			currentpad.yTicks = RightTicks(1., 0.5);
			draw(scale(1e6, 100), RootGetObject(f, d+"/h_refined_ratio.th_y"), opt, blue, "efficiency histogram");

			/*
			RootObject fit = RootGetObject(f, d+"/pol0");
			TF1_x_min = -inf; TF1_x_max = +inf;
			draw(scale(1e6, 100), fit, heavygreen+2pt, "const fit");
			TF1_x_min = 0; TF1_x_max = 120e-6;
			draw(scale(1e6, 100), fit, heavygreen+dashed);
			*/

			RootObject fit = RootGetObject(f, d+"/pol1");
			TF1_x_min = -inf; TF1_x_max = +inf;
			draw(scale(1e6, 100), fit, red+2pt, "linear fit");
			TF1_x_min = 0; TF1_x_max = 120e-6;
			draw(scale(1e6, 100), fit, red+dashed);

			real y = 100*rp_eff_cen[rpi] - 1.5;

			string slope_label = format("slope = ($%#.1f$", fit.rExec("GetParameter", 1))
				+ format("$\pm %#.1f) \un{rad^{-1}}$", fit.rExec("GetParError", 1));
			label(slope_label, (400, y), red, Fill(white));

			limits((150, 100*rp_eff_cen[rpi] - 2), (650, 100*rp_eff_cen[rpi] + 2), Crop);
			fLegend = BuildLegend();
		}

		if (dgi == 0)
		{
			++gx;
			NewPad(false, gx, gy);
			add(fLegend);
		}
	}
}

GShipout("eff3outof4_fits", vSkip=0pt);
