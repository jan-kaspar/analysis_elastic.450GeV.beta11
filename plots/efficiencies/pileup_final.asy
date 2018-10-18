import root;
import pad_layout;

include "../run_info.asy";

string topDir = "../../";

string datasets[];
datasets.push("DS-323893/ZeroBias");
datasets.push("DS-323899/ZeroBias");
datasets.push("DS-323907/ZeroBias");
datasets.push("DS-323919/ZeroBias");
datasets.push("DS-323932/ZeroBias");
datasets.push("DS-323933/ZeroBias");
datasets.push("DS-323934/ZeroBias");

string diagonals[] = { "45b", "45t" };
string dgn_labels[] = { "45 bot -- 56 top", "45 top -- 56 bot" };

string template = "dgn/# && #, L || R";

string criteria[] = { "pat_suff_destr", "pl_suff_destr" };

xSizeDef = 10cm;

TGraph_errorBar = None;

//----------------------------------------------------------------------------------------------------

frame fr_leg;

NewPad(false);
for (int di : diagonals.keys)
	NewPadLabel(dgn_labels[di]);

for (int dsi : datasets.keys)
{
	NewRow();

	NewPadLabel(datasets[dsi]);

	string f = topDir + datasets[dsi]+"/pileup_combined.root";

	for (int di : diagonals.keys)
	{
		string dgn = diagonals[di];

		real y_max = 0.5;
		
		NewPad("time$\ung{h}$", "destructive pile-up probability");
		DrawRunBands(datasets[dsi], 0, y_max);
		
		for (int ci : criteria.keys)
		{
			string element = replace(template, "#", criteria[ci]);
			pen p = StdPen(ci);

			RootObject obj = RootGetObject(f, dgn+"/"+element+"/rel", search=false, error=false);
			if (obj.valid)
				draw(swToHours, obj, "p", p, mCi+2pt+p, replace(criteria[ci], "_", "\_"));
		}
		
		ylimits(0, y_max, Crop);
		//limits((2, 0), (3.6, y_max), Crop);
		
		fr_leg = BuildLegend();
	}
}

NewPad(false);
attach(fr_leg);

GShipout(hSkip=1mm, vSkip=0mm);
