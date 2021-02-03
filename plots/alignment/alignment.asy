import root;
import pad_layout;
include "../run_info.asy";

string topDir = "../../";

string datasets[], fills[];
datasets.push("DS-fill7301/Totem1"); fills.push("7301");
datasets.push("DS-fill7302/Totem1"); fills.push("7302");

string units[], unit_labels[];
units.push("L_2_F"); unit_labels.push("L-220-fr");
units.push("L_1_F"); unit_labels.push("L-210-fr");
units.push("R_1_F"); unit_labels.push("R-210-fr");
units.push("R_2_F"); unit_labels.push("R-220-fr");

xSizeDef = 10cm;
xTicksDef = LeftTicks(Step=1., step=0.5);
drawGridDef = true;

TGraph_errorBar = None;

bool drawFit = true;
bool drawGlobalFit = false;
bool centreToFit = false;

//----------------------------------------------------------------------------------------------------

void SetPadWidth()
{
	//real timespan = currentpicture.userMax2().x - currentpicture.userMin2().x;
	//currentpad.xSize = 10cm * timespan/10;
	currentpad.xSize = 15cm;
}

//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------

for (int ui : units.keys)
{
	NewPad(false);
	label("{\SetFontSizesXX " + unit_labels[ui] + "}");
}

//----------------------------------------------------------------------------------------------------
NewRow();

for (int ui : units.keys)
{
	NewPad("time $\ung{h}$", "tilt $\ung{mrad}$", axesAbove=false);
	currentpad.yTicks = RightTicks(2., 1.1);

	real y_cen = 0.;
	real y_min = y_cen - 10, y_max = y_cen + 10;

	for (int dsi : datasets.keys)
	{
		string dataset = datasets[dsi];

		DrawFillBands(fills[dsi], y_min, y_max);

		//draw(swToHours, RootGetObject(topDir+dataset+"/alignment.root", "global/"+units[ui]+"/a_p"), "p,eb", magenta, mCi+1pt+magenta);
		//draw(swToHours, RootGetObject(topDir+dataset+"/alignment.root", "global/"+units[ui]+"/a_g"), "p,eb", heavygreen, mCi+1pt+heavygreen);
	
		draw(swToHours, RootGetObject(topDir+dataset+"/alignment.root", "global/"+units[ui]+"/a"), "p,eb", blue, mCi+1pt+blue);

		if (drawFit)
			draw(swToHours, RootGetObject(topDir+dataset+"/alignment_fit.root", units[ui]+"/a_fit"), "l", red);
	}
	
	if (drawGlobalFit)
	{
		RootObject fit = RootGetObject(topDir+"/alignment/global_fit.root", units[ui]+"/a_fit");
		real unc = 5;

		draw(shift(0, +unc)*swToHours, fit, "l", red+dashed);
		draw(shift(0,    0)*swToHours, fit, "l", red+2pt);
		draw(shift(0, -unc)*swToHours, fit, "l", red+dashed);
	}

	ylimits(y_min, y_max, Crop);
	//AttachLegend(unit_labels[ui], SE, SE);

	SetPadWidth();
}

//----------------------------------------------------------------------------------------------------
NewRow();

for (int ui : units.keys)
{
	NewPad("time $\ung{h}$", "horizontal position $\ung{\mu m}$", axesAbove=false);
	currentpad.yTicks = RightTicks(500., 100.);

	real y_cen = 0;
	real y_min = y_cen - 500, y_max = y_cen + 1000;

	for (int dsi : datasets.keys)
	{
		string dataset = datasets[dsi];
		DrawFillBands(fills[dsi], y_min, y_max);

		//draw(swToHours, RootGetObject(topDir+dataset+"/alignment.root", "global/"+units[ui]+"/b_p"), "p,eb", magenta, mCi+1pt+magenta);
		//draw(swToHours, RootGetObject(topDir+dataset+"/alignment.root", "global/"+units[ui]+"/b_g"), "p,eb", heavygreen, mCi+1pt+heavygreen);

		draw(swToHours, RootGetObject(topDir+dataset+"/alignment.root", "global/"+units[ui]+"/b"), "p,eb", blue+1pt, mCi+1pt+blue);

		if (drawFit)
			draw(swToHours, RootGetObject(topDir+dataset+"/alignment_fit.root", units[ui]+"/b_fit"), "l", red);
	}
	
	if (drawGlobalFit)
	{
		RootObject fit = RootGetObject(topDir+"/alignment/global_fit.root", units[ui]+"/b_fit");
		real unc = 100;

		draw(shift(0, +unc)*swToHours, fit, "l", red+dashed);
		draw(shift(0,    0)*swToHours, fit, "l", red+2pt);
		draw(shift(0, -unc)*swToHours, fit, "l", red+dashed);
	}

	ylimits(y_min, y_max, Crop);
	//AttachLegend(unit_labels[ui], SE, SE);

	SetPadWidth();
}

//----------------------------------------------------------------------------------------------------
NewRow();

for (int ui : units.keys)
{
	NewPad("time $\ung{h}$", "vertical position $\ung{\mu m}$", axesAbove=false);
	currentpad.yTicks = RightTicks(100., 50.);

	real y_cen = 0;
	real y_min = y_cen - 500, y_max = y_cen + 500;

	for (int dsi : datasets.keys)
	{
		string dataset = datasets[dsi];
		DrawFillBands(fills[dsi], y_min, y_max);

		draw(swToHours, RootGetObject(topDir+dataset+"/alignment.root", "global/"+units[ui]+"/c_min_diff"), "p,eb", cyan, mCi+1pt+cyan);
		draw(swToHours, RootGetObject(topDir+dataset+"/alignment.root", "global/"+units[ui]+"/c_prob"), "p,eb", blue, mCi+1pt+blue);
		draw(swToHours, RootGetObject(topDir+dataset+"/alignment.root", "global/"+units[ui]+"/c_mean_diff_sq"), "p,eb", magenta, mCi+1pt+magenta);
		draw(swToHours, RootGetObject(topDir+dataset+"/alignment.root", "global/"+units[ui]+"/c_hist_chi_sq"), "p,eb", heavygreen, mCi+1pt+heavygreen);
			
		//draw(swToHours, RootGetObject(topDir+dataset+"/alignment.root", "global/"+units[ui]+"/c"), "p,l,eb", blue+1pt, mCi+1pt+blue);

		if (drawFit)
			draw(swToHours, RootGetObject(topDir+dataset+"/alignment_fit.root", units[ui]+"/c_fit"), "l", red);
	}
	
	if (drawGlobalFit)
	{
		RootObject fit = RootGetObject(topDir+"/alignment/global_fit.root", units[ui]+"/c_fit");
		real unc = 100;

		draw(shift(0, +unc)*swToHours, fit, "l", red+dashed);
		draw(shift(0,    0)*swToHours, fit, "l", red+2pt);
		draw(shift(0, -unc)*swToHours, fit, "l", red+dashed);
	}

	ylimits(y_min, y_max, Crop);
	//AttachLegend(unit_labels[ui], SE, SE);

	SetPadWidth();
}

//----------------------------------------------------------------------------------------------------

GShipout("alignment", hSkip=1mm, vSkip=0mm);
