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

TH2_palette = Gradient(blue, heavygreen, yellow, red);
TH2_z_min = 0;
TH2_z_max = 1.;

//----------------------------------------------------------------------------------------------------

for (int dsi : datasets.keys)
{
	for (int dgi : diagonals.keys)
	{
		string f = topDir + datasets[dsi] + "/eff3outof4_details.root";
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
			string d = diagonals[dgi] + "/" + rps[rpi];

			TH2_z_min = 0.7; TH2_z_max = 1.0;
			TH2_paletteTicks = PaletteTicks(Step=0.02, step=0.01);

			++gx;
			NewPad("$\th_x^*\ung{\mu rad}$", "$\th_y^*\ung{\mu rad}$", gx, gy, axesAbove=true);
			draw(scale(1., sgn), RootGetObject(f, d+"/track/th_x, th_y : rel"), "def");

			limits((-800, 150), (800, 650), Crop);
		}
		
		++gy; gx = 0;
		NewPad(false, -1, gy);	
		label("{\SetFontSizesXX more than 1 track}");

		for (int rpi : rps.keys)
		{
			string d = diagonals[dgi] + "/" + rps[rpi];

			TH2_z_min = 0.; TH2_z_max = 0.3;
			
			++gx;
			NewPad("$\th_x^*\ung{\mu rad}$", "$\th_y^*\ung{\mu rad}$", gx, gy);
			draw(scale(1., sgn), RootGetObject(f, d+"/pl_suff_no_track/th_x, th_y : rel"), "def");

			limits((-800, 150), (800, 650), Crop);
		}
	}
}

GShipout("eff3outof4_details_2D", vSkip=0pt);
