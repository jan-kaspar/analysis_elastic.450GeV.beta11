import root;
import pad_layout;
include "../common.asy";

string datasets[] = {
	"data/fill7301/Totem1",
	"data/fill7302/Totem1",
};

string diagonals[], diagonal_labels[];
diagonals.push("45b_56t"); diagonal_labels.push("45 bot -- 56 top");
diagonals.push("45t_56b"); diagonal_labels.push("45 top -- 56 bot");

TH2_palette = Gradient(blue, heavygreen, yellow, red);

xSizeDef = 8cm;

//----------------------------------------------------------------------------------------------------

for (int dsi : datasets.keys)
{
	for (int dgni : diagonals.keys)
	{
		NewRow();

		NewPad(false);
		label("\vbox{\SetFontSizesXX\hbox{" + datasets[dsi] + "}\hbox{" + diagonal_labels[dgni]+ "}}");

		string f = topDir+"/"+datasets[dsi]+"/distributions_" + diagonals[dgni] + ".root";

		TH2_x_min = -800e-6;
		TH2_x_max = +800e-6;

		if (diagonals[dgni] == "45b_56t")
		{
			TH2_y_min = +30e-6;
			TH2_y_max = +550e-6;
		} else {
			TH2_y_min = -550e-6;
			TH2_y_max = -30e-6;
		}

		//yTicksDef = RightTicks(1., 0.5);
		xTicksDef = LeftTicks(200., 100.);

		// 45 bot
		//guide cnt =    (-200e-6, 490e-6)--(-500e-6, 450e-6)--(-600e-6, 220e-6)--(-400e-6, 160e-6)--(+200e-6, 160e-6)--(+500e-6, 220e-6)--(+400e-6, 490e-6)--cycle;
		//guide cnt_gl = (-200e-6, 480e-6)--(-480e-6, 440e-6)--(-580e-6, 225e-6)--(-400e-6, 170e-6)--(+200e-6, 170e-6)--(+480e-6, 225e-6)--(+380e-6, 480e-6)--cycle;

		// 45 top
		//guide cnt = (-200e-6, 170e-6)--(+400e-6, 170e-6)--(+520e-6, 200e-6)--(+400e-6, 480e-6)--(-430e-6, 480e-6)--(-550e-6, 400e-6)--(-600e-6, 200e-6)--cycle;
		//guide cnt_gl = (-200e-6, 180e-6)--(+400e-6, 180e-6)--(+500e-6, 205e-6)--(+380e-6, 470e-6)--(-420e-6, 470e-6)--(-530e-6, 400e-6)--(-580e-6, 210e-6)--cycle;

		// ----- left arm -----

		NewPad("$\th_x^{*}\ung{\mu rad}$", "$\th_y^{*}\ung{\mu rad}$", axesAbove=true);
		scale(Linear, Linear, Log);
		draw(scale(1e6, 1e6), RootGetObject(f, "selected - angles/h2_th_y_L_vs_th_x_L"), "def");

		draw(scale(1e6, 1e6), RootGetObject(f, "fiducial cuts/fc_L"), "l", black+1pt);
		//draw(scale(1e6, -1e6) * cnt, black+1pt);

		limits((TH2_x_min*1e6, TH2_y_min*1e6), (TH2_x_max*1e6, TH2_y_max*1e6), Crop);
		AttachLegend(BuildLegend("left arm", S), N);

		// ----- right arm -----

		NewPad("$\th_x^{*}\ung{\mu rad}$", "$\th_y^{*}\ung{\mu rad}$", axesAbove=true);
		scale(Linear, Linear, Log);
		draw(scale(1e6, 1e6), RootGetObject(f, "selected - angles/h2_th_y_R_vs_th_x_R"), "def");

		draw(scale(1e6, 1e6), RootGetObject(f, "fiducial cuts/fc_R"), "l", black+2pt);
		//draw(scale(1e6, -1e6) * cnt, black+1pt);

		limits((TH2_x_min*1e6, TH2_y_min*1e6), (TH2_x_max*1e6, TH2_y_max*1e6), Crop);
		AttachLegend(BuildLegend("right arm", S), N);

		// ----- both arms -----

		NewPad("$\th_x^{*}\ung{\mu rad}$", "$\th_y^{*}\ung{\mu rad}$", axesAbove=true);
		scale(Linear, Linear, Log);
		draw(scale(1e6, 1e6), RootGetObject(f, "selected - angles/h2_th_y_G_vs_th_x_G"), "def");

		draw(scale(1e6, 1e6), RootGetObject(f, "fiducial cuts/fc_G"), "l", black+2pt);
		//draw(scale(1e6, -1e6) * cnt, dashed);
		//draw(scale(1e6, -1e6) * cnt_gl, black+2pt);

		limits((TH2_x_min*1e6, TH2_y_min*1e6), (TH2_x_max*1e6, TH2_y_max*1e6), Crop);
		AttachLegend(BuildLegend("two-arm", S), N);
	}
}

GShipout(margin=1mm, hSkip=1mm);
