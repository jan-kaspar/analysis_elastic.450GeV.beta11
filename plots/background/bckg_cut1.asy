import root;
import pad_layout;

string topDir = "../../";

TH2_palette = Gradient(blue, heavygreen, yellow, red);

string datasets[] = {
	"DS-test1",
	"DS-test2",
	"DS-test3",
	"DS-test3bis",
	"DS-test4",
	"DS-test3ter-1",
	"DS-test3ter-2"
};

string dgns[] = {
	"45b_56t",
	"45t_56b"
};

string parts[], p_labels[];
pen p_pens[];
parts.push(""); p_pens.push(red); p_labels.push("full");
parts.push("/ls_part1"); p_pens.push(blue); p_labels.push("first part");
parts.push("/ls_part2"); p_pens.push(heavygreen); p_labels.push("second part");

int cuts[] = { 1 };

real scale_x[] = { 1e6, 1e6, 1e6, 1e6, 1e0, 1e0, 1e6, 1e6 };
real scale_y[] = { 1e6, 1e6, 1e0, 1e0, 1e0, 1e0, 1e0, 1e0 };

string label_x[] = { "$\th_x^{*R}\ung{\mu rad}$", "$\th_y^{*R}\ung{\mu rad}$", "$\th_x^{*R}\ung{\mu rad}$", "$\th_x^{*L}\ung{\mu rad}$", "$y^{R,N}\ung{mm}$", "$y^{L,N}\ung{mm}$", "$\th_x^*\ung{\mu rad}$", "$\th_y^*\ung{\mu rad}$" };
string label_y[] = { "$\th_x^{*L}\ung{\mu rad}$", "$\th_y^{*L}\ung{\mu rad}$", "$x^{*R}\ung{mm}$", "$x^{*L}\ung{mm}$", "$y^{R,F} - y^{R,N}\ung{mm}$", "$y^{L,F} - y^{L,N}\ung{mm}$", "$\De^{R-L} x^*\ung{mm}$", "$\De^{R-L} y^*\ung{mm}$" };
string label_cut[] = { "$\De^{R-L} \th_x^{*}\ung{\mu rad}$", "$\De^{R-L} \th_y^{*}\ung{\mu rad}$", "$x^{*R}\ung{mm}$", "$x^{*L}\ung{mm}$", "$cq5$", "$cq6$", "$cq7$", "$cq8$" };

real lim_x_low[] = { -700, -220, -1000, -1000, -15, -15, -1500, -600 };
real lim_x_high[] = { +700, -100, +1000, +1000, +15, +15, +1500, +600 };

real lim_y_low[] = { -700, -220, -0.8, -0.8, -0.5, -0.5, -2, -4 };
real lim_y_high[] = { +700, -100, +0.8, +0.8, +0.5, +0.5, +2, +4 };

real lim_q[] = { 700., 50, 10., 10., 0.15, 0.15, 2.5 };

for (int ci : cuts.keys)
{
	int cut = cuts[ci];
	int idx = cut - 1;

	write(format("* cut %i", cut));

	for (int dsi : datasets.keys)
	{
		string dataset = datasets[dsi];

		NewRow();

		NewPad(false);
		label("{\SetFontSizesXX " + datasets[dsi] + "}");

		for (int dgi : dgns.keys)
		{
			string dgn = dgns[dgi];
			string f = topDir + dataset+"/distributions_" + dgn + ".root";

			string obj_name_par = format("elastic cuts/cut %i", cut) + format("/g_cut_parameters", cut);
			RootObject obj_par = RootGetObject(f, obj_name_par);
			real ax[] = {0}, ay[] = {0};
			obj_par.vExec("GetPoint", 0, ax, ay); real cca = ay[0];
			obj_par.vExec("GetPoint", 1, ax, ay); real ccb = ay[0];
			obj_par.vExec("GetPoint", 2, ax, ay); real ccc = ay[0];
			obj_par.vExec("GetPoint", 3, ax, ay); real csi = ay[0];
			obj_par.vExec("GetPoint", 4, ax, ay); real n_si = ay[0];

			NewPad(label_cut[idx]);

			real scale = scale_y[idx];

			for (int pti : parts.keys)
			{

				string f = topDir + dataset + parts[pti] +"/distributions_" + dgn + ".root";
				string obj_name_h = format("elastic cuts/cut %i", cut) + format("/h_cq%i", cut);
				RootObject obj_h = RootGetObject(f, obj_name_h);

				draw(scale(scale, 1.), obj_h, "vl,eb", p_pens[pti]+1pt);
			}

			xlimits(-lim_q[idx], +lim_q[idx], Crop);

			yaxis(XEquals(+n_si * csi * scale, false), dashed);
			yaxis(XEquals(-n_si * csi * scale, false), dashed);

			AttachLegend(replace(dgn, "_", " -- "), NW, NW);
		}

		if (dsi == 0)
		{
			NewPad(false);
			for (int pti : parts.keys)
				AddToLegend(p_labels[pti], p_pens[pti]);
			AttachLegend();
		}
	}
}

GShipout(vSkip=1mm);
