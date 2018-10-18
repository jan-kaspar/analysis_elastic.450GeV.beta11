import root;
import pad_layout;

string topDir = "../../";

TH2_palette = Gradient(blue, heavygreen, yellow, red);

string datasets[], d_labels[];
datasets.push("DS-test1"); d_labels.push("\vbox{\hbox{22 Nov 2017}\hbox{fill 6410}\hbox{run 306982}}");

string dgns[] = {
	"45b_56t",
	"45t_56b"
};

int cuts[] = { 1, 2 };

real scale_x[] = { 1e6, 1e6, 1e6, 1e6, 1e0, 1e0, 1e6, 1e6 };
real scale_y[] = { 1e6, 1e6, 1e0, 1e0, 1e0, 1e0, 1e0, 1e0 };

string label_x[] = { "$\th_x^{*R}\ung{\mu rad}$", "$\th_y^{*R}\ung{\mu rad}$", "$\th_x^{*R}\ung{\mu rad}$", "$\th_x^{*L}\ung{\mu rad}$", "$y^{R,N}\ung{mm}$", "$y^{L,N}\ung{mm}$", "$\th_x^*\ung{\mu rad}$", "$\th_y^*\ung{\mu rad}$" };
string label_y[] = { "$\th_x^{*L}\ung{\mu rad}$", "$\th_y^{*L}\ung{\mu rad}$", "$x^{*R}\ung{mm}$", "$x^{*L}\ung{mm}$", "$y^{R,F} - y^{R,N}\ung{mm}$", "$y^{L,F} - y^{L,N}\ung{mm}$", "$\De^{R-L} x^*\ung{mm}$", "$\De^{R-L} y^*\ung{mm}$" };
string label_cut[] = { "$\De^{R-L} \th_x^{*}\ung{\mu rad}$", "$\De^{R-L} \th_y^{*}\ung{\mu rad}$", "$x^{*R}\ung{mm}$", "$x^{*L}\ung{mm}$", "$cq5$", "$cq6$", "$cq7$", "$cq8$" };

real lim_x_low[] = { -1000, +200, -1000, -1000, -15, -15, -1500, -600 };
real lim_x_high[] = { +1000, +500, +1000, +1000, +15, +15, +1500, +600 };

real lim_y_low[] = { -1000, +200, -0.8, -0.8, -0.5, -0.5, -2, -4 };
real lim_y_high[] = { +1000, +500, +0.8, +0.8, +0.5, +0.5, +2, +4 };

real lim_q[] = { 1500., 50, 10., 10., 0.15, 0.15, 2.5 };

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
		label("{\SetFontSizesXX " + d_labels[dsi] + "}");

		for (int dgni : dgns.keys)
		{
			string dgn = dgns[dgni];
			string f = topDir + dataset+"/distributions_" + dgn + ".root";

			real x_ll = lim_x_low[idx];
			real x_lh = lim_x_high[idx];
			real y_ll = lim_y_low[idx];
			real y_lh = lim_y_high[idx];

			if (cut == 2 && dgn == "45t_56b")
			{
				x_lh = -lim_x_low[idx];
				x_ll = -lim_x_high[idx];
				y_lh = -lim_y_low[idx];
				y_ll = -lim_y_high[idx];
			}

			NewPad(label_x[idx], label_y[idx]);
			scale(Linear, Linear, Log);
			string objC = format("elastic cuts/cut %i", cut) + format("/plot_before_cq%i", cut);
			draw(scale(scale_x[idx], scale_y[idx]), RootGetObject(f, objC+"#0"), "p,d0,bar");
			draw(scale(scale_x[idx], scale_y[idx]), RootGetObject(f, objC+"#1"));
			draw(scale(scale_x[idx], scale_y[idx]), RootGetObject(f, objC+"#2"));
			limits((x_ll, y_ll), (y_lh, x_lh), Crop);

			AttachLegend(replace(dgn, "_", " -- "), NW, NW);
		}
	}

	GShipout(format("collinearity_example_%i", cut), vSkip=1mm);
}
