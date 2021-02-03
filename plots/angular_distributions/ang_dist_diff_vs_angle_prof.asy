import root;
import pad_layout;
include "../common.asy";

string datasets[], dataset_fills[];
datasets.push("DS-fill7301/Totem1"); dataset_fills.push("7301");
//datasets.push("DS-fill7302/Totem1"); dataset_fills.push("7302");

string dgns[] = { "45b_56t", "45t_56b" };
string dgn_labs[] = { "45 bot -- 56 top", "45 top -- 56 bot" };

string plot[];
string lab_h[];
string lab_v[];
real y_min[];
real y_max[];

plot.push("p_th_x_diffLR_vs_th_x"); lab_h.push("$\th_x^{*}\ung{\mu rad}$"); lab_v.push("$\De^{R-L}\th_x^{*}\ung{\mu rad}$"); y_min.push(-100); y_max.push(100);
plot.push("p_th_x_L_diffNF_vs_th_x_L"); lab_h.push("$\th_x^{*L}\ung{\mu rad}$"); lab_v.push("$\De^{220F-210F}\th_x^{*L}\ung{\mu rad}$"); y_min.push(-50); y_max.push(50);
plot.push("p_th_x_R_diffNF_vs_th_x_R"); lab_h.push("$\th_x^{*R}\ung{\mu rad}$"); lab_v.push("$\De^{220F-210F}\th_x^{*R}\ung{\mu rad}$"); y_min.push(-50); y_max.push(50);

plot.push("p_th_y_diffLR_vs_th_y"); lab_h.push("$\th_y^{*}\ung{\mu rad}$"); lab_v.push("$\De^{R-L}\th_y^{*}\ung{\mu rad}$"); y_min.push(-10); y_max.push(+10);
plot.push("p_th_y_L_diffNF_vs_th_y_L"); lab_h.push("$\th_y^{*L}\ung{\mu rad}$"); lab_v.push("$\De^{220F-210F}\th_y^{*L}\ung{\mu rad}$"); y_min.push(-20); y_max.push(+10);
plot.push("p_th_y_R_diffNF_vs_th_y_R"); lab_h.push("$\th_y^{*R}\ung{\mu rad}$"); lab_v.push("$\De^{220F-210F}\th_y^{*R}\ung{\mu rad}$"); y_min.push(-20); y_max.push(+10);

for (int dsi : datasets.keys)
{
	for (int dgi : dgns.keys)
	{
		NewRow();

		NewPad(false);	
		label("\vbox{\SetFontSizesXX\hbox{"+datasets[dsi]+"}\hbox{" + dgn_labs[dgi]+"}}");
		
		string f = topDir + datasets[dsi] + "/distributions_" + dgns[dgi] + ".root";

		for (int pi : plot.keys)
		{
			NewPad(lab_h[pi], lab_v[pi]);

			string base = "selected - angles/"+plot[pi];

			draw(scale(1e6, 1e6), RootGetObject(f, base), "d0,eb", red);

			RootObject fit = RootGetObject(f, base + "|pol1", error=false);
			if (fit.valid)
			{
				real slope = fit.rExec("GetParameter", 1);
				real slope_unc = fit.rExec("GetParError", 1);
				draw(scale(1e6, 1e6), fit, "", blue+2pt, format("slope = $%#+.4f$", slope) + format("$\pm %#.4f$", slope_unc));
			}

			//if (plot[pi] == "p_th_x_diffLR_vs_th_x")
			//	xlimits(-100, +100, Crop);

			ylimits(y_min[pi], y_max[pi], Crop);
			AttachLegend(NW, NW);
		}
	}
}

GShipout(vSkip=1mm);
