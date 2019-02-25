import root;
import pad_layout;
include "../run_info.asy";

string topDir = "../../";


string datasets[];
string periods[];
//datasets.push("DS-fill7301/Totem1"); periods.push("0");
datasets.push("DS-fill7302/Totem1"); periods.push("2");

string units[], unit_labels[];
units.push("L_2_F"); unit_labels.push("L-220-fr");
units.push("L_1_F"); unit_labels.push("L-210-fr");
units.push("R_1_F"); unit_labels.push("R-210-fr");
units.push("R_2_F"); unit_labels.push("R-220-fr" );

xSizeDef = 10cm;
drawGridDef = true;

TGraph_errorBar = None;

//----------------------------------------------------------------------------------------------------

for (int dsi : datasets.keys)
{
	string dataset = datasets[dsi];
	string period = "period " + periods[dsi];

	write(dataset);

	//--------------------
	NewRow();
	
	for (int ui : units.keys)
	{
		NewPad("$y\ung{mm}$", "$\hbox{mean } x\ung{mm}$");
	
		draw(RootGetObject(topDir+dataset+"/alignment.root", period + "/unit "+units[ui]+"/horizontal/horizontal profile/p"), "d0,eb", blue);
		draw(RootGetObject(topDir+dataset+"/alignment.root", period + "/unit "+units[ui]+"/horizontal/horizontal profile/p|ff"), "l", red+1pt);
		
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
	
		limits((-15, 1), (+15, 2e4), Crop);
		//AttachLegend(unit_labels[ui], NE, NE);
	}
	
	//--------------------
	NewRow();
	
	for (int ui : units.keys)
	{
		NewPad("bottom-RP $y$ shift$\ung{mm}$", "");
		currentpad.xTicks = LeftTicks(0.5, 0.1);
	
		draw(RootGetObject(topDir+dataset+"/alignment.root", period + "/unit "+units[ui]+"/vertical/g_max_diff"), "l,p", heavygreen, mCi+1pt+heavygreen);
	
		limits((-1.0, 0), (+1.0, 0.2), Crop);
		AttachLegend(unit_labels[ui], NE, NE);
	}

	//--------------------
	
	GShipout("alignment_details");
}
