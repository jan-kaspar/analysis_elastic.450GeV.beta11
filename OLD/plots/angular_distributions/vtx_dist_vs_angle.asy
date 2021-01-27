import root;
import pad_layout;

string topDir = "../../";

string datasets[] = {
	"DS-fill7301/Totem1",
	"DS-fill7302/Totem1",
};

string dgns[] = { "45b_56t", "45t_56b" };
string dgn_labs[] = { "45 bot -- 56 top", "45 top -- 56 bot" };

string plot[];
string lab_h[];
string lab_v[];
real x_min[], x_max[];
real y_min[], y_max[];

plot.push("vtx_x_L_vs_th_x_L"); lab_h.push("$\th_x^{*,L}\ung{\mu rad}$"); lab_v.push("$x^{*,L}\ung{\mu m}$"); x_min.push(-600); x_max.push(+600); y_min.push(-1000); y_max.push(+1000);
plot.push("vtx_x_R_vs_th_x_R"); lab_h.push("$\th_x^{*,R}\ung{\mu rad}$"); lab_v.push("$x^{*,R}\ung{\mu m}$"); x_min.push(-600); x_max.push(+600); y_min.push(-1000); y_max.push(+1000);

plot.push("vtx_y_L_vs_th_y_L"); lab_h.push("$\th_y^{*,L}\ung{\mu rad}$"); lab_v.push("$y^{*,L}\ung{\mu m}$"); x_min.push(-500); x_max.push(+500); y_min.push(-1000); y_max.push(+1000);
plot.push("vtx_y_R_vs_th_y_R"); lab_h.push("$\th_y^{*,R}\ung{\mu rad}$"); lab_v.push("$y^{*,R}\ung{\mu m}$"); x_min.push(-500); x_max.push(+500); y_min.push(-1000); y_max.push(+1000);

TH2_palette = Gradient(blue, heavygreen, yellow, red);

//----------------------------------------------------------------------------------------------------

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

			string base = "selected - vertex/h2_"+plot[pi];
			RootObject h2 = RootGetObject(f, base);
			h2.vExec("Rebin2D", 2, 2);

			draw(scale(1e6, 1e3), h2, "def");

			/*
			string base = "selected - angles/p_"+plot[pi];
			RootObject prof = RootGetObject(f, base, error=false);

			draw(scale(x_scale[pi], 1e6), prof, "eb", black);

			RootObject fit = RootGetObject(f, base + "|pol1", error=false);
			if (fit.valid)
			{
				real slope = fit.rExec("GetParameter", 1);
				real slope_unc = fit.rExec("GetParError", 1);
				draw(scale(x_scale[pi], 1e6), fit, "", magenta+2pt, format("slope = $%#+.4f$", slope) + format("$\pm %#.4f$", slope_unc));
			}
			*/

			limits((x_min[pi], y_min[pi]), (x_max[pi], y_max[pi]), Crop);
			AttachLegend(NW, NW);
		}
	}
}

GShipout(vSkip=1mm);
