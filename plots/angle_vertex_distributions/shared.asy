import root;
import pad_layout;
include "../common.asy";

string datasets[];
datasets.push("data/fill7301/Totem1");
datasets.push("data/fill7302/Totem1");

string dgns[], d_labels[];
dgns.push("45b_56t"); d_labels.push("45 bot -- 56 top");
dgns.push("45t_56b"); d_labels.push("45 top -- 56 bot");

string f_tags[], f_labels[], f_shorts[];

TH2_palette = Gradient(blue, heavygreen, yellow, red);

string dir = "selected - angles";

//----------------------------------------------------------------------------------------------------

string plots[];
real x_scales[], y_scales[];
string lab_hs[], lab_vs[];
real y_mins[], y_maxs[];

void ResetPlots()
{
	plots.delete();
	x_scales.delete();
	y_scales.delete();
	lab_hs.delete();
	lab_vs.delete();
	y_mins.delete();
	y_maxs.delete();
}

//----------------------------------------------------------------------------------------------------

void PlotOne(string fn)
{
	for (int dsi : datasets.keys)
	{
		for (int dgi : dgns.keys)
		{
			NewRow();

			NewPadLabel("\vbox{\hbox{"+datasets[dsi]+"}\hbox{" + d_labels[dgi]+"}}");
			
			string f = topDir + datasets[dsi] + "/distributions_" + dgns[dgi] + ".root";

			for (int fli : f_tags.keys)
			{
				NewRow();

				NewPadLabel(f_labels[fli]);

				for (int pi : plots.keys)
				{
					string plot = replace(plots[pi], "<F>", f_tags[fli]);
					string lab_h = replace(lab_hs[pi], "<F>", f_tags[fli]);
					string lab_v = replace(lab_vs[pi], "<F>", f_tags[fli]);

					NewPad(lab_h, lab_v);

					string base = dir + "/h2_" + plot;
					RootObject h2 = RootGetObject(f, base);
					h2.vExec("Rebin2D", 4, 4);

					draw(scale(x_scales[pi], y_scales[pi]), h2, "def");

					string base = dir + "/p_" + plot;
					RootObject prof = RootGetObject(f, base);

					draw(scale(x_scales[pi], y_scales[pi]), prof, "eb", black);

					RootObject fit = RootGetObject(f, base + "|pol1", error=false);
					if (fit.valid)
					{
						real intercept = fit.rExec("GetParameter", 0) * y_scales[pi];
						real intercept_unc = fit.rExec("GetParError", 0) * y_scales[pi];

						real slope = fit.rExec("GetParameter", 1) * y_scales[pi] / x_scales[pi];
						real slope_unc = fit.rExec("GetParError", 1) * y_scales[pi] / x_scales[pi];

						real fit_x_min = fit.rExec("GetXmin");
						real fit_x_max = fit.rExec("GetXmax");
						real intercept_mid = fit.rExec("Eval", (fit_x_min + fit_x_max)/2.) * y_scales[pi];

						draw(scale(x_scales[pi], y_scales[pi]), fit, "l", cyan+2pt, "linear fit:");

						AddToLegend(format("slope = $%#+.4f$", slope) + format("$\pm %#.4f$", slope_unc));

						AddToLegend(format("intercept at 0 = $%#+.4f$", intercept) + format("$\pm %#.4f$", intercept_unc));
						AddToLegend(format("intercept at range mid = $%#+.4f$", intercept_mid));
					}

					//ylimits(y_mins[pi], y_maxs[pi], Crop);
					AttachLegend(S, N);
				}
			}
		}
	}

	GShipout(fn);
}
