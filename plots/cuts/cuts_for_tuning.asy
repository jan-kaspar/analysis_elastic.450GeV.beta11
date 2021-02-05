import root;
import pad_layout;
include "../common.asy";

string dataset = "data/fill7302/Totem1";

string dgns[] = { "45b_56t", "45t_56b" };

int cuts[] = { 1, 2, 5, 6, 7, 8, 9, 10 };

real scale_x[] = { 1e6, 1e6, 1e6, 1e6, 1e0, 1e0, 1e6, 1e6, 1e0, 1e0 };
real scale_y[] = { 1e6, 1e6, 1e0, 1e0, 1e0, 1e0, 1e0, 1e0, 1e0, 1e0 };

string label_cut[] = { "$\De^{R-L} \th_x^{*}\ung{\mu rad}$", "$\De^{R-L} \th_y^{*}\ung{\mu rad}$", "$x^{*R}\ung{mm}$", "$x^{*L}\ung{mm}$", "$cq5$", "$cq6$", "$cq7$", "$cq8$", "$cq9$", "$cq10$" };

real lim_q[] = { 300, 300, 0., 0., 0.5, 0.5, 1.0, 3.0, 1.5, 1.5 };

//----------------------------------------------------------------------------------------------------

NewPad(false);
label(replace("{\SetFontSizesXX " + dataset + "}", "_", "\_"));

NewRow();

NewPad(false);
for (string dgn : dgns)
{
	NewPad(false);
	label("{\SetFontSizesXX " + replace(dgn, "_", "\_") + "}");
}

for (int ci : cuts.keys)
{
	int cut = cuts[ci];
	int idx = cut - 1;

	NewRow();

	NewPad(false);
	label(format("{\SetFontSizesXX cut %u}", cut));

	for (int dgi : dgns.keys)
	{
		string dgn = dgns[dgi];
		string f = topDir + dataset+"/distributions_" + dgn + ".root";
	
		// ---------- discriminator distribution ----------

		string obj_name_par = format("elastic cuts/cut %i", cut) + format("/g_cut_parameters", cut);
		RootObject obj_par = RootGetObject(f, obj_name_par);
		real ax[] = {0}, ay[] = {0};
		obj_par.vExec("GetPoint", 0, ax, ay); real cca = ay[0];
		obj_par.vExec("GetPoint", 1, ax, ay); real ccb = ay[0];
		obj_par.vExec("GetPoint", 2, ax, ay); real ccc = ay[0];
		obj_par.vExec("GetPoint", 3, ax, ay); real csi = ay[0];
		obj_par.vExec("GetPoint", 4, ax, ay); real n_si = ay[0];

		NewPad(label_cut[idx]);

		string obj_name_h = format("elastic cuts/cut %i", cut) + format("/h_cq%i", cut);
		RootObject obj_h = RootGetObject(f, obj_name_h);

		real scale = scale_y[idx];

		draw(scale(scale, 1.), obj_h, "vl,eb", red+1pt);

		xlimits(-lim_q[idx], +lim_q[idx], Crop);

		yaxis(XEquals(+n_si * csi * scale, false), blue+dashed);
		yaxis(XEquals(-n_si * csi * scale, false), blue+dashed);

		AddToLegend(format("<mean = $%#.3f$", obj_h.rExec("GetMean") * scale));
		AddToLegend(format("<RMS = $%#.3f$", obj_h.rExec("GetRMS") * scale));
		AddToLegend(format("<cut = $\pm%#.3f$", n_si * csi * scale));
		AttachLegend();
	}
}

GShipout(vSkip=1mm);
