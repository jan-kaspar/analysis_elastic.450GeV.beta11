import root;
import pad_layout;
include "../common.asy";

string datasets[], fills[];
datasets.push("data/fill7301/Totem1"); fills.push("7301");
datasets.push("data/fill7302/Totem1"); fills.push("7302");

string diagonals[];
diagonals.push("45b_56t");
diagonals.push("45t_56b");

string rps[], rp_labels[];
rps.push("L_2_F"); rp_labels.push("45-220-fr");
rps.push("L_1_F"); rp_labels.push("45-210-fr");
rps.push("R_1_F"); rp_labels.push("56-210-fr");
rps.push("R_2_F"); rp_labels.push("56-220-fr");

xSizeDef = 6cm;
ySizeDef = 5cm;
//yTicksDef = RightTicks(5., 1.);
//xTicks=LeftTicks(format="$$", 20., 10.)
xTicksDef = LeftTicks(200., 100.);

int gx=0, gy=0;

TH2_palette = Gradient(magenta, magenta, blue, blue, heavygreen, yellow, red);
TH2_z_min = 0;
TH2_z_max = 1.;

//----------------------------------------------------------------------------------------------------

for (int dsi : datasets.keys)
{
	for (int dgi : diagonals.keys)
	{
		string f = topDir + datasets[dsi] + "/eff3outof4_" + diagonals[dgi] + ".root";
		string f_dist = topDir + datasets[dsi] + "/distributions_" + diagonals[dgi] + ".root";

		real sgn = (diagonals[dgi] == "45b_56t") ? +1 : -1;
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

		++gy; gx = 0;
		NewPad(false, -1, gy);	
		label("{\SetFontSizesXX efficiency}");

		for (int rpi : rps.keys)
		{
			string d = "excluded RPs " + rps[rpi] + "/n_si 1.0";

			TH2_z_min = 0.75; TH2_z_max = 1.0;
			TH2_paletteTicks = PaletteTicks(Step=0.02, step=0.01);

			++gx;
			NewPad("$\th_x^*\ung{\mu rad}$", "$\th_y^*\ung{\mu rad}$", gx, gy, axesAbove=true);
			draw(scale(1e6, 1e6), RootGetObject(f, d+"/th_x, th_y dependence (unif)/h_simple_ratio_vs_th_x_th_y"), "def");

			draw(scale(1e6, 1e6*sgn), RootGetObject(f_dist, "fiducial cuts/fc_G"), "l", black+1pt);

			limits((-800, 100), (800, 600), Crop);
		}
	}
}

GShipout("eff3outof4_2D", vSkip=0pt);
