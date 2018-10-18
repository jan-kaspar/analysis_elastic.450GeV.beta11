import root;
import pad_layout;

include "../run_info.asy";

string topDir = "../../";

string datasets[];
//datasets.push("DS-323893/Totem1");
//datasets.push("DS-323899/Totem1");
datasets.push("DS-323907/Totem1");
//datasets.push("DS-323907-1/Totem1");
//datasets.push("DS-323907-2/Totem1");
//datasets.push("DS-323919/Totem1");
//datasets.push("DS-323932/Totem1");
//datasets.push("DS-323933/Totem1");
//datasets.push("DS-323934/Totem1");

string diagonals[] = { "45b", "45t" };
string dgn_labels[] = { "45 bot -- 56 top", "45 top -- 56 bot" };

string elements[][] = {
	{ "L_1_F/<Q>", "L_2_F/<Q>", "L_1_F, L_2_F/<Q> <OP> <Q>" },
	{ "R_1_F/<Q>", "R_2_F/<Q>", "R_1_F, R_2_F/<Q> <OP> <Q>" },
	{ "", "", "dgn/<Q> <OP> <Q>, L <OP> R" }
};

string row_captions[] = { "sector 45", "sector 56", "sectors 45 $\oplus$ 56" };

string col_captions[] = { "210-fr", "220-far", "210-fr $\oplus$ 220-fr" };

xSizeDef = 10cm;

//----------------------------------------------------------------------------------------------------

void DrawRatio(RootObject num, RootObject den, pen p, string label="")
{
	int n = den.iExec("GetN");
	for (int i = 0; i < n; ++i)
	{
		real ax[] = {0.};
		real ay[] = {0.};

		num.vExec("GetPoint", i, ax, ay); real num_x = ax[0], num_y = ay[0];
		den.vExec("GetPoint", i, ax, ay); real den_x = ax[0], den_y = ay[0];
		real x_unc = den.rExec("GetErrorX", i);

		real x = num_x;
		real r = num_y / den_y;
		real r_unc = 0.;

		draw(swToMinutes * (x, r), p, mCi+2pt + p);
		draw(swToMinutes * ((x-x_unc, r)--(x+x_unc, r)), p);
		draw(swToMinutes * ((x, r-r_unc)--(x, r+r_unc)), p);
	}

	AddToLegend(label, p, mCi+2pt+p);
}

//----------------------------------------------------------------------------------------------------

TGraph_errorBar = None;

for (int dsi : datasets.keys)
{
	string f = topDir + datasets[dsi]+"/pileup_combined.root";

	for (int di : diagonals.keys)
	{
		string dgn = diagonals[di];
		
		NewPage();

		NewPad(false, -1, -1);
		label("\vbox{\SetFontSizesXX\hbox{"+datasets[dsi]+"}\hbox{"+dgn_labels[di]+"}}");
	
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

				real y_min = 0.7;
				real y_max = 1.;
		
				NewPad("time$\ung{h}$", "probability", c, r);
				DrawRunBands(datasets[dsi], y_min, y_max);

				string on_den = dgn+"/"+ replace(replace(template, "<OP>", "&&"), "<Q>", "pat_suff") +"/rel";
				string on_num_tr = dgn+"/"+ replace(replace(template, "<OP>", "&&"), "<Q>", "tr") +"/rel";
				string on_num_pat_more = dgn+"/"+ replace(replace(template, "<OP>", "||"), "<Q>", "pat_more") +"/rel";

				RootObject o_den = RootGetObject(f, on_den, search=false, error=false);
				RootObject o_num_tr = RootGetObject(f, on_num_tr, search=false, error=false);
				RootObject o_num_pat_more = RootGetObject(f, on_num_pat_more, search=false, error=false);

				if (!o_den.valid || !o_num_tr.valid || !o_num_pat_more.valid)
				{
					write("* some input missing");
					continue;
				}

				DrawRatio(o_num_tr, o_den, blue, "tr");
				//DrawRatio(o_num_pat_more, o_den, red, "pat\_more");

				ylimits(y_min, y_max, Crop);
				//limits((2, y_min), (3.6, y_max), Crop);
				
				fr_leg = BuildLegend();
					
			}
		}

		NewPad(false, elements[0].length, elements.length - 1);
		attach(fr_leg);
	}
}

GShipout(hSkip=1mm, vSkip=0mm);
