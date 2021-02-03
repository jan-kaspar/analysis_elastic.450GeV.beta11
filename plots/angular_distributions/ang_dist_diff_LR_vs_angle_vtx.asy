import root;
import pad_layout;
include "../common.asy";

string datasets[] = {
	"data/fill7301/Totem1",
	"data/fill7302/Totem1",
};

string dgns[] = { "45b_56t", "45t_56b" };
string dgn_labs[] = { "45 bot -- 56 top", "45 top -- 56 bot" };

string plot[];
real x_scale[];
string lab_h[];
string lab_v[];
real y_min[];
real y_max[];

plot.push("th_x_diffLR_vs_th_x"); x_scale.push(1e6); lab_h.push("$\th_x^{*}\ung{\mu rad}$"); lab_v.push("$\De^{R-L}\th_x^{*}\ung{\mu rad}$"); y_min.push(-150); y_max.push(+150);
plot.push("th_x_diffLR_vs_vtx_x"); x_scale.push(1e3); lab_h.push("$x^*\ung{\mu m}$"); lab_v.push("$\De^{R-L}\th_x^{*}\ung{\mu rad}$"); y_min.push(-150); y_max.push(+150);
plot.push("th_x_diffLR_vs_th_y"); x_scale.push(1e6); lab_h.push("$\th_y^{*}\ung{\mu rad}$"); lab_v.push("$\De^{R-L}\th_x^{*}\ung{\mu rad}$"); y_min.push(-150); y_max.push(+150);
plot.push("th_x_diffLR_vs_vtx_y"); x_scale.push(1e3); lab_h.push("$y^*\ung{\mu m}$"); lab_v.push("$\De^{R-L}\th_x^{*}\ung{\mu rad}$"); y_min.push(-150); y_max.push(+150);

plot.push("th_y_diffLR_vs_th_y"); x_scale.push(1e6); lab_h.push("$\th_y^{*}\ung{\mu rad}$"); lab_v.push("$\De^{R-L}\th_y^{*}\ung{\mu rad}$"); y_min.push(-100); y_max.push(+100);
plot.push("th_y_diffLR_vs_vtx_y"); x_scale.push(1e3); lab_h.push("$y^*\ung{\mu m}$"); lab_v.push("$\De^{R-L}\th_y^{*}\ung{\mu rad}$"); y_min.push(-100); y_max.push(+100);
plot.push("th_y_diffLR_vs_th_x"); x_scale.push(1e6); lab_h.push("$\th_x^{*}\ung{\mu rad}$"); lab_v.push("$\De^{R-L}\th_y^{*}\ung{\mu rad}$"); y_min.push(-100); y_max.push(+100);
plot.push("th_y_diffLR_vs_vtx_x"); x_scale.push(1e3); lab_h.push("$x^*\ung{\mu m}$"); lab_v.push("$\De^{R-L}\th_y^{*}\ung{\mu rad}$"); y_min.push(-100); y_max.push(+100);

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

			string base = "selected - angles/h2_"+plot[pi];
			RootObject h2 = RootGetObject(f, base);
			h2.vExec("Rebin2D", 2, 2);

			draw(scale(x_scale[pi], 1e6), h2, "def");

			string base = "selected - angles/p_"+plot[pi];
			RootObject prof = RootGetObject(f, base);

			draw(scale(x_scale[pi], 1e6), prof, "eb", black);

			RootObject fit = RootGetObject(f, base + "|pol1", error=false);
			if (fit.valid)
			{
				real slope = fit.rExec("GetParameter", 1);
				real slope_unc = fit.rExec("GetParError", 1);
				draw(scale(x_scale[pi], 1e6), fit, "", magenta+2pt, format("slope = $%#+.4f$", slope) + format("$\pm %#.4f$", slope_unc));
			}

			ylimits(y_min[pi], y_max[pi], Crop);
			AttachLegend(NW, NW);
		}
	}
}

GShipout(vSkip=1mm);
