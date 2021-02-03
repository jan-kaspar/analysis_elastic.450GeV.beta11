import root;
import pad_layout;
include "../common.asy";
include "../run_info.asy";

string fills[], f_datasets[];
fills.push("7301"); f_datasets.push("data/fill7301/ZeroBias");
fills.push("7302"); f_datasets.push("data/fill7302/ZeroBias");

string diagonals[] = { "45b_56t", "45t_56b" };
string dgn_labels[] = { "45 bot -- 56 top", "45 top -- 56 bot" };

string template = "dgn/# && #, L || R";

string criteria[] = { "pat_suff_destr", "pl_suff_destr" };

xSizeDef = 10cm;

TGraph_errorBar = None;

//----------------------------------------------------------------------------------------------------

for (int di : diagonals.keys)
{
	NewRow();

	NewPadLabel(dgn_labels[di]);
	
	real y_max = 0.3;
		
	NewPad("time$\ung{h}$", "destructive pile-up probability");

	for (int dsi : f_datasets.keys)
	{
		string f = topDir + f_datasets[dsi]+"/pileup_combined.root";
		string dgn = diagonals[di];

		DrawBands(fills[dsi], bands="run", labels="ds", 0, y_max);
		
		for (int ci : criteria.keys)
		{
			string element = replace(template, "#", criteria[ci]);
			pen p = StdPen(ci);

			RootObject obj = RootGetObject(f, dgn+"/"+element+"/rel", search=false, error=false);
			if (obj.valid)
				draw(swToHours, obj, "p", p, mCi+2pt+p, replace(criteria[ci], "_", "\_"));
		}
		
	}
	
	ylimits(0, y_max, Crop);
}

NewPad(false);
for (int ci : criteria.keys)
	AddToLegend(replace(criteria[ci], "_", "\_"), StdPen(ci));

AttachLegend();

GShipout("pileup_summary", hSkip=1mm, vSkip=0mm);
