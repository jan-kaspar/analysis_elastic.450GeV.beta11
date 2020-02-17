import root;
import pad_layout;

string top_dir = "../../";

string datasets[], dataset_fills[];
datasets.push("DS-fill7301/Totem1"); dataset_fills.push("7301");
datasets.push("DS-fill7302/Totem1"); dataset_fills.push("7302");

TH2_palette = Gradient(blue, heavygreen, yellow, red);

//----------------------------------------------------------------------------------------------------

for (int dsi : datasets.keys)
{
	if (dsi == 4)
		NewRow();

	real ySize = 6cm;

	NewPad("$\th_x^{*}\ung{\mu rad}$", "$\th_y^{*}\ung{\mu rad}$", ySize/150*150, ySize);
	//currentpad.xTicks = LeftTicks(50., 10.);
	scale(Linear, Linear, Log);
	//TH2_zLabel = "(corrected) events per bin";
	TH2_paletteBarWidth = 0.05;
	
	// z scale
	//TH2_z_min = 5.5;
	//TH2_z_max = 3.75;

	// 45 bottom - 56 top
	draw(scale(1e6, 1e6), RootGetObject(top_dir+"/"+datasets[dsi]+"/distributions_45b_56t.root", "selected - angles/h_th_y_vs_th_x"), "def");
	
	// 45 top - 56 bottom
	draw(scale(1e6, 1e6), RootGetObject(top_dir+"/"+datasets[dsi]+"/distributions_45t_56b.root", "selected - angles/h_th_y_vs_th_x"), "p");
	
	limits((-1000, -1000), (1000, 1000), Crop);
	AttachLegend(datasets[dsi]);
}

GShipout(margin=1mm, hSkip=1mm);
