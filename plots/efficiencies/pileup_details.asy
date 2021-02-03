import root;
import pad_layout;
include "../common.asy";
include "../run_info.asy";

string fills[], f_datasets[];
fills.push("7301"); f_datasets.push("DS-fill7301/ZeroBias");
fills.push("7302"); f_datasets.push("DS-fill7302/ZeroBias");

string diagonals[] = { "45b_56t", "45t_56b" };
string dgn_labels[] = { "45 bot -- 56 top", "45 top -- 56 bot" };

string elements[][] = {
	{ "L_1_F/#", "L_2_F/#", "L_1_F, L_2_F/# && #" },
	{ "R_1_F/#", "R_2_F/#", "R_1_F, R_2_F/# && #" },
	{ "", "", "dgn/# && #, L || R" }
};

string criteria[] = { "pat_suff_destr", "pl_suff_destr" };

string row_captions[] = { "left", "right", "left OR right" };

string col_captions[] = { "210-fr", "220-far", "210-fr AND 220-fr" };

xSizeDef = 15cm;

//----------------------------------------------------------------------------------------------------

TGraph_errorBar = None;

for (int dsi : f_datasets.keys)
{
	string f = topDir + f_datasets[dsi]+"/pileup_combined.root";

	for (int di : diagonals.keys)
	{
		string dgn = diagonals[di];
		
		NewPage();

		NewPad(false, -1, -1);
		label("\vbox{\SetFontSizesXX\hbox{"+f_datasets[dsi]+"}\hbox{"+dgn_labels[di]+"}}");
	
		for (int ri : row_captions.keys)
		{
			NewPad(false, -1, ri);
			label("{\SetFontSizesXX "+row_captions[ri]+"}");
		}
	
		for (int ci : col_captions.keys)
		{
			NewPad(false, ci, -1);
			label("{\SetFontSizesXX "+col_captions[ci]+"}");
		}

		frame fr_leg;
	
		for (int r = 0; r < elements.length; ++r)
		{
			for (int c = 0; c < elements[0].length; ++c)
			{
				string template = elements[r][c];
				if (template == "")
					continue;

				real y_max = 0.2;
		
				NewPad("time$\ung{h}$", "destructive pile-up probability", c, r);
				DrawBands(fills[dsi], bands="run", labels="ds", 0., y_max);
		
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

		NewPad(false, elements[0].length, elements.length - 1);
		attach(fr_leg);
	}
}

GShipout("pileup_details", hSkip=1mm, vSkip=0mm);
