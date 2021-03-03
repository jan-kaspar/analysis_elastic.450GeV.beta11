import root;
import pad_layout;
include "../common.asy";
include "../run_info.asy";

string datasets[];
string periods[];
datasets.push("data/fill7301/Totem1"); periods.push("0");
datasets.push("data/fill7302/Totem1"); periods.push("0");
datasets.push("data/fill7302/Totem1"); periods.push("2");
datasets.push("data/fill7302/Totem1"); periods.push("3");

string units[], unit_labels[];
units.push("L_2_F"); unit_labels.push("L-220-fr");
units.push("L_1_F"); unit_labels.push("L-210-fr");
units.push("R_1_F"); unit_labels.push("R-210-fr");
units.push("R_2_F"); unit_labels.push("R-220-fr");

xSizeDef = 10cm;
drawGridDef = true;

TGraph_errorBar = None;

//----------------------------------------------------------------------------------------------------

for (int dsi : datasets.keys)
{
	NewPage();

	string dataset = datasets[dsi];
	string period = "period " + periods[dsi];

	write(dataset);

	NewPadLabel("\vbox{\hbox{" + dataset + "}\hbox{" + period + "}}");

	//--------------------
	NewRow();
	
	for (int ui : units.keys)
	{
		NewPad("$y\ung{mm}$", "$\hbox{central } x\ung{mm}$");
	
		//draw(RootGetObject(topDir+dataset+"/alignment.root", period + "/unit "+units[ui]+"/horizontal/horizontal profile/p"), "d0,eb", blue);
		//draw(RootGetObject(topDir+dataset+"/alignment.root", period + "/unit "+units[ui]+"/horizontal/horizontal profile/p|ff"), "l", red+1pt);

		draw(RootGetObject(topDir+dataset+"/alignment.root", period + "/unit "+units[ui]+"/horizontal/horizontal profile max/g_x_max_vs_y"), "p", blue);
		draw(RootGetObject(topDir+dataset+"/alignment.root", period + "/unit "+units[ui]+"/horizontal/horizontal profile max/g_x_max_vs_y|ff"), "l", red+1pt);
		
		limits((-15, -1.5), (+15, +1.5), Crop);
		AttachLegend(unit_labels[ui], NE, NE);
	}
	
	//--------------------
	NewRow();
	
	for (int ui : units.keys)
	{
		NewPad("$y\ung{mm}$", "");
		scale(Linear, Log);
	
		draw(RootGetObject(topDir+dataset+"/alignment.root", period + "/unit "+units[ui]+"/vertical/y_hist|y_hist"), "d0,vl", blue);
		draw(RootGetObject(topDir+dataset+"/alignment.root", period + "/unit "+units[ui]+"/vertical/y_hist|y_hist_range"), "d0,vl", red+1pt);
	
		limits((-15, 1e1), (+15, 5e4), Crop);
		AttachLegend(unit_labels[ui], NE, NE);
	}
	
	//--------------------
	NewRow();
	
	for (int ui : units.keys)
	{
		NewPad("bottom-RP $y$ shift$\ung{mm}$", "");
		currentpad.xTicks = LeftTicks(0.5, 0.1);
	
		draw(RootGetObject(topDir+dataset+"/alignment.root", period + "/unit "+units[ui]+"/vertical/shift/g_max_diff"), "l,p", heavygreen, mCi+1pt+heavygreen);
	
		limits((-2.0, 0), (+2.0, 0.2), Crop);
		AttachLegend(unit_labels[ui], NE, NE);
	}
	
	//--------------------
	NewRow();
	
	for (int ui : units.keys)
	{
		NewPad("$y\ung{mm}$", "");
	
		draw(RootGetObject(topDir+dataset+"/alignment.root", period + "/unit "+units[ui]+"/vertical/fit/y_hist"), "d0,vl", red);
		draw(RootGetObject(topDir+dataset+"/alignment.root", period + "/unit "+units[ui]+"/vertical/fit/y_hist|f"), "l", blue);
	
		//limits((-30, 1e2), (+30, 1e3), Crop);
		AttachLegend(unit_labels[ui], NE, NE);
	}
}

GShipout("alignment_details");
