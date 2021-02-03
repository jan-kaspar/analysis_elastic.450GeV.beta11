import root;
import pad_layout;
include "../common.asy";

string datasets[], dataset_fills[];
datasets.push("data/fill7301/Totem1"); dataset_fills.push("7301");
datasets.push("data/fill7302/Totem1"); dataset_fills.push("7302");

TH2_palette = Gradient(blue, heavygreen, yellow, red);

transform xyswitch = (0, 0, 0, 1, 1, 0);

TGraph_errorBar = None;

//----------------------------------------------------------------------------------------------------

for (int dsi : datasets.keys)
{
	if (dsi == 4)
		NewRow();

	real ySize = 6cm;

	NewPad("$\th_x^{*}\ung{\mu rad}$", "$\th_y^{*}\ung{\mu rad}$", ySize/150*150, ySize, axesAbove=true);
	//currentpad.xTicks = LeftTicks(50., 10.);
	scale(Linear, Linear, Log);
	//TH2_zLabel = "(corrected) events per bin";
	TH2_paletteBarWidth = 0.05;
	
	// z scale
	//TH2_z_min = 5.5;
	//TH2_z_max = 3.75;

	// 45 bottom - 56 top
	string f = top_dir+"/"+datasets[dsi]+"/distributions_45b_56t.root";
	draw(scale(1e6, 1e6), RootGetObject(f, "selected - angles/h2_th_y_vs_th_x"), "def");
	draw(scale(1e6, 1e6) * xyswitch, RootGetObject(f, "selected - angles/g_mode_th_x_vs_th_y"), "p");
	
	// 45 top - 56 bottom
	string f = top_dir+"/"+datasets[dsi]+"/distributions_45t_56b.root";
	draw(scale(1e6, 1e6), RootGetObject(f, "selected - angles/h2_th_y_vs_th_x"), "p");
	draw(scale(1e6, 1e6) * xyswitch, RootGetObject(f, "selected - angles/g_mode_th_x_vs_th_y"), "p");
	
	limits((-1000, -1000), (1000, 1000), Crop);
	AttachLegend(datasets[dsi]);
}

GShipout(margin=1mm, hSkip=1mm);
