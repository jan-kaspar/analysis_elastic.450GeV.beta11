import root;
import pad_layout;
include "../common.asy";

string datasets[];
real d_z_maxs[];
datasets.push("data/fill7301/Totem1"); d_z_maxs.push(1e4);
datasets.push("data/fill7302/Totem1"); d_z_maxs.push(5e4);

//string unit_labels[] = { "XRPV.B6L5.B2", "XRPV.D6L5.B2", "XRPV.D6R5.B1", "XRPV.B6R5.B1" };
string units[];
string unit_labels[];
real unit_x_mins[], unit_x_maxs[];
units.push("L_2_F"); unit_labels.push("45-220-fr"); unit_x_mins.push(-2); unit_x_maxs.push(+1.7);
units.push("L_1_F"); unit_labels.push("45-210-fr"); unit_x_mins.push(-2.5); unit_x_maxs.push(+2.5);
units.push("R_1_F"); unit_labels.push("56-210-fr"); unit_x_mins.push(-3.5); unit_x_maxs.push(+3);
units.push("R_2_F"); unit_labels.push("56-220-fr"); unit_x_mins.push(-2.5); unit_x_maxs.push(+2);

drawGridDef = true;

TH2_palette = Gradient(white, blue, heavygreen, yellow, red, black);

/*
TH2_x_min = -10;
TH2_x_max = +10;
*/
TH2_y_min = -30;
TH2_y_max = +30;

//string selection = "before selection";
string selection = "after selection";

//----------------------------------------------------------------------------------------------------

for (int dsi : datasets.keys)
{
	NewRow();

	NewPad(false);
	label("{\SetFontSizesXX " + datasets[dsi] + "}");

	string file_45b = topDir + datasets[dsi] + "/distributions_45b_56t.root";
	string file_45t = topDir + datasets[dsi] + "/distributions_45t_56b.root";

	for (int ui : units.keys)
	{
		NewPad("$x\ung{mm}$", "$y\ung{mm}$", 6cm, 6cm);
		scale(Linear, Linear, Log);

		TH2_z_min = 1e0;
		TH2_z_max = d_z_maxs[dsi];

		string tag;
		if (selection == "before selection") tag = "_al_nosel";
		if (selection == "after selection") tag = "_al_sel";

		RootGetObject(file_45b, "hit distributions/vertical, aligned, "+selection+"/h2_y_"+units[ui]+"_vs_x_"+units[ui]+tag);
		robj.vExec("Rebin2D", 2, 4);
		draw(robj, "p,bar");

		RootGetObject(file_45t, "hit distributions/vertical, aligned, "+selection+"/h2_y_"+units[ui]+"_vs_x_"+units[ui]+tag);
		robj.vExec("Rebin2D", 2, 4);
		draw(robj, "p");
		
		//draw(RootGetObject(file_45t, "hit distributions/vertical, not aligned, after selection/h2_y_"+units[ui]+"_vs_x_"+units[ui]+"_noal_sel"), "p");

		draw((unit_x_mins[ui], -30)--(unit_x_mins[ui], +30), black+dashed);
		draw((unit_x_maxs[ui], -30)--(unit_x_maxs[ui], +30), black+dashed);

		//limits((-15, -30), (15, 30), Crop);
		limits((-8, -30), (8, 30), Crop);
		
		AttachLegend(replace(unit_labels[ui], "_", "\_"));
	}
}

GShipout(hSkip=1mm, vSkip=1mm);
