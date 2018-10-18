import root;
import pad_layout;

include "../run_info.asy";

string topDir = "../../";

TH2_palette = Gradient(blue, heavygreen, yellow, red);

string datasets[];
datasets.push("DS-323893/Totem1");
datasets.push("DS-323899/Totem1");
datasets.push("DS-323907/Totem1");
datasets.push("DS-323919/Totem1");
datasets.push("DS-323932/Totem1");
datasets.push("DS-323933/Totem1");
datasets.push("DS-323934/Totem1");

string diagonals[], dgn_labels[];
diagonals.push("45b"); dgn_labels.push("45 bot -- 56 top");
diagonals.push("45t"); dgn_labels.push("45 top -- 56 bot");

string rps[], rp_labels[];
rps.push("L_2_F"); rp_labels.push("45-220-fr");
rps.push("L_1_F"); rp_labels.push("45-210-fr");
rps.push("R_1_F"); rp_labels.push("56-210-fr");
rps.push("R_2_F"); rp_labels.push("56-220-fr");

TGraph_errorBar = None;

//xTicksDef = LeftTicks(1., 0.5);
//yTicksDef = RightTicks(0.01, 0.005);

//----------------------------------------------------------------------------------------------------

NewPad(false);
NewPad(false);
for (int rpi : rps.keys)
{
	NewPadLabel(rp_labels[rpi]);
}

for (int dsi : datasets.keys)
{
	NewRow();

	NewPadLabel(datasets[dsi] + ":");

	string f = topDir + datasets[dsi] + "/pileup_combined.root";

	for (int dgni : diagonals.keys)
	{
		NewRow();

		NewPad(false);
		NewPadLabel(dgn_labels[dgni]);
		
		for (int rpi : rps.keys)
		{
			NewPad("time $\ung{h}$", "event fraction");

			DrawRunBands(datasets[dsi], 0., 1., true);

			draw(scale(1./3600, 1.), RootGetObject(f, diagonals[dgni] + "/" + rps[rpi] + "/pl_suff/rel"), "p", cyan+1pt, mCi+2pt+cyan, "pl\_suff");
			draw(scale(1./3600, 1.), RootGetObject(f, diagonals[dgni] + "/" + rps[rpi] + "/pat_suff/rel"), "p", blue+1pt, mCi+2pt+blue, "pat\_suff");
                                  
			draw(scale(1./3600, 1.), RootGetObject(f, diagonals[dgni] + "/" + rps[rpi] + "/tr/rel"), "p", heavygreen+1pt, mCi+2pt+heavygreen, "tr");
			
			draw(scale(1./3600, 1.), RootGetObject(f, diagonals[dgni] + "/" + rps[rpi] + "/pat_more/rel"), "p", red+1pt, mCi+2pt+red, "pat\_more");

			ylimits(0, 1., Crop);
		}
	}

	frame f_legend = BuildLegend();

	NewPad(false);
	attach(f_legend);
}

GShipout(vSkip=1mm);
