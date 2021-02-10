import root;
import pad_layout;
include "../common.asy";

string datasets[] = {
	"data/fill7301/Totem1",
	"data/fill7302/Totem1",
};

string dgns[] = {
	"45b_56t",
	"45t_56b"
};
string dgn_labs[] = { "45 bot -- 56 top", "45 top -- 56 bot" };

string plot[];
string lab_h[];
string lab_v[];
real y_min[];
real y_max[];

TH2_palette = Gradient(blue, heavygreen, yellow, red);

//----------------------------------------------------------------------------------------------------

void ProcessOne(string tag)
{
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

				string base = "selected - angles/h2_"+plot[pi];
				RootObject h2 = RootGetObject(f, base);
				//h2.vExec("Rebin2D", 2, 2);

				draw(scale(1e6, 1e6), h2, "def");

				string base = "selected - angles/p_"+plot[pi];

				RootObject prof = RootGetObject(f, base);
				draw(scale(1e6, 1e6), prof, "eb,d0", black);

				RootObject fit = RootGetObject(f, base + "|pol1", error=false);
				if (fit.valid)
				{
					real intercept = fit.rExec("GetParameter", 0);
					real intercept_unc = fit.rExec("GetParError", 0);
					real slope = fit.rExec("GetParameter", 1);
					real slope_unc = fit.rExec("GetParError", 1);

					draw(scale(1e6, 1e6), fit, "", magenta+2pt, "linear fit of profile:");

					AddToLegend(format("<slope = $%#+.4f$", slope) + format("$\pm %#.4f$", slope_unc));
					AddToLegend(format("<intercept = $%#+.2f$", intercept*1e6) + format("$\pm %#.2f\un{\mu rad}$", intercept_unc*1e6));
				}

				//if (plot[pi] == "p_th_x_diffLR_vs_th_x")
				//	xlimits(-100, +100, Crop);

				ylimits(y_min[pi], y_max[pi], Crop);
				AttachLegend(S, N);
			}
		}
	}

	GShipout("ang_dist_single_arm_diff_vs_angle_2D_" + tag, vSkip=1mm);
	
	purge();
}

//----------------------------------------------------------------------------------------------------

plot.push("th_x_L_diffNA_vs_th_x_L"); lab_h.push("$\th_x^{*L}\ung{\mu rad}$"); lab_v.push("$\th_x^{*L, 210F} - \th_x^{*L}\ung{\mu rad}$"); y_min.push(-100); y_max.push(100);
plot.push("th_x_L_diffFA_vs_th_x_L"); lab_h.push("$\th_x^{*L}\ung{\mu rad}$"); lab_v.push("$\th_x^{*L, 220F} - \th_x^{*L}\ung{\mu rad}$"); y_min.push(-100); y_max.push(100);
plot.push("th_x_R_diffNA_vs_th_x_R"); lab_h.push("$\th_x^{*R}\ung{\mu rad}$"); lab_v.push("$\th_x^{*R, 210F} - \th_x^{*R}\ung{\mu rad}$"); y_min.push(-100); y_max.push(100);
plot.push("th_x_R_diffFA_vs_th_x_R"); lab_h.push("$\th_x^{*R}\ung{\mu rad}$"); lab_v.push("$\th_x^{*R, 220F} - \th_x^{*R}\ung{\mu rad}$"); y_min.push(-100); y_max.push(100);

ProcessOne("x");

//----------------------------------------------------------------------------------------------------

plot.delete();
lab_h.delete();
lab_v.delete();
y_min.delete();
y_max.delete();

plot.push("th_y_L_diffNA_vs_th_y_L"); lab_h.push("$\th_y^{*L}\ung{\mu rad}$"); lab_v.push("$\th_y^{*L, 210F} - \th_y^{*L}\ung{\mu rad}$"); y_min.push(-1); y_max.push(1);
plot.push("th_y_L_diffFA_vs_th_y_L"); lab_h.push("$\th_y^{*L}\ung{\mu rad}$"); lab_v.push("$\th_y^{*L, 220F} - \th_y^{*L}\ung{\mu rad}$"); y_min.push(-1); y_max.push(1);
plot.push("th_y_R_diffNA_vs_th_y_R"); lab_h.push("$\th_y^{*R}\ung{\mu rad}$"); lab_v.push("$\th_y^{*R, 210F} - \th_y^{*R}\ung{\mu rad}$"); y_min.push(-1); y_max.push(1);
plot.push("th_y_R_diffFA_vs_th_y_R"); lab_h.push("$\th_y^{*R}\ung{\mu rad}$"); lab_v.push("$\th_y^{*R, 220F} - \th_y^{*R}\ung{\mu rad}$"); y_min.push(-1); y_max.push(1);

ProcessOne("y");
