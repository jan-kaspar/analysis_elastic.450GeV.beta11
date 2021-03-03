import root;
import pad_layout;
include "../common.asy";
include "../run_info.asy";

string datasets[], fills[];
datasets.push("data/fill7301/Totem1"); fills.push("7301");
datasets.push("data/fill7302/Totem1"); fills.push("7302");

string units[], unit_labels[];
units.push("L_2_F"); unit_labels.push("45-220-fr");
units.push("L_1_F"); unit_labels.push("45-210-fr");
units.push("R_1_F"); unit_labels.push("56-210-fr");
units.push("R_2_F"); unit_labels.push("56-220-fr");

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
	real unc = 5;

	for (int dsi : datasets.keys)
	{
		currentpicture.legend.delete();

		string dataset = datasets[dsi];

		DrawFillBands(fills[dsi], y_min, y_max);

		//draw(swToHours, RootGetObject(topDir+dataset+"/alignment.root", "global/"+units[ui]+"/a_p"), "p,eb", magenta, mCi+1pt+magenta);
		//draw(swToHours, RootGetObject(topDir+dataset+"/alignment.root", "global/"+units[ui]+"/a_g"), "p,eb", heavygreen, mCi+1pt+heavygreen);
	
		draw(swToHours, RootGetObject(topDir+dataset+"/alignment.root", "global/"+units[ui]+"/a_pm"), "p,eb", blue, mCi+1pt+blue, "method profile max");

		if (drawFit)
		{
			RootObject fit = RootGetObject(topDir+dataset+"/alignment_fit.root", units[ui]+"/a_fit");

			draw(shift(0, +unc)*swToHours, fit, "l", red+dashed);
			draw(shift(0,    0)*swToHours, fit, "l", red+2pt, "fit");
			draw(shift(0, -unc)*swToHours, fit, "l", red+dashed, "uncertainty");
		}
	}
	
	if (drawGlobalFit)
	{
		RootObject fit = RootGetObject(topDir+"/alignment/global_fit.root", units[ui]+"/a_fit");

		draw(shift(0, +unc)*swToHours, fit, "l", red+dashed);
		draw(shift(0,    0)*swToHours, fit, "l", red+2pt);
		draw(shift(0, -unc)*swToHours, fit, "l", red+dashed);
	}

	ylimits(y_min, y_max, Crop);
	//AttachLegend(unit_labels[ui], SE, SE);

	SetPadWidth();
}

frame f_legend = BuildLegend();

NewPad(false);
attach(f_legend);

//----------------------------------------------------------------------------------------------------
NewRow();

for (int ui : units.keys)
{
	NewPad("time $\ung{h}$", "horizontal position $\ung{\mu m}$", axesAbove=false);
	currentpad.yTicks = RightTicks(200., 100.);

	real y_cen = 0;
	real y_min = y_cen - 500, y_max = y_cen + 1000;
	real unc = 200;

	for (int dsi : datasets.keys)
	{
		currentpicture.legend.delete();

		string dataset = datasets[dsi];
		DrawFillBands(fills[dsi], y_min, y_max);

		//draw(swToHours, RootGetObject(topDir+dataset+"/alignment.root", "global/"+units[ui]+"/b_p"), "p,eb", magenta, mCi+1pt+magenta);
		//draw(swToHours, RootGetObject(topDir+dataset+"/alignment.root", "global/"+units[ui]+"/b_g"), "p,eb", heavygreen, mCi+1pt+heavygreen);

		draw(swToHours, RootGetObject(topDir+dataset+"/alignment.root", "global/"+units[ui]+"/b_pm"), "p,eb", blue+1pt, mCi+1pt+blue, "method profile max");

		if (drawFit)
		{
			RootObject fit = RootGetObject(topDir+dataset+"/alignment_fit.root", units[ui]+"/b_fit");

			draw(shift(0, +unc)*swToHours, fit, "l", red+dashed);
			draw(shift(0,    0)*swToHours, fit, "l", red+2pt, "fit");
			draw(shift(0, -unc)*swToHours, fit, "l", red+dashed, "uncertainty");
		}
	}
	
	if (drawGlobalFit)
	{
		RootObject fit = RootGetObject(topDir+"/alignment/global_fit.root", units[ui]+"/b_fit");

		draw(shift(0, +unc)*swToHours, fit, "l", red+dashed);
		draw(shift(0,    0)*swToHours, fit, "l", red+2pt);
		draw(shift(0, -unc)*swToHours, fit, "l", red+dashed);
	}

	ylimits(y_min, y_max, Crop);
	//AttachLegend(unit_labels[ui], SE, SE);

	SetPadWidth();
}

frame f_legend = BuildLegend();

NewPad(false);
attach(f_legend);

//----------------------------------------------------------------------------------------------------
NewRow();

for (int ui : units.keys)
{
	NewPad("time $\ung{h}$", "vertical position $\ung{\mu m}$", axesAbove=false);
	currentpad.yTicks = RightTicks(100., 50.);

	real y_cen = 0;
	real y_min = y_cen - 500, y_max = y_cen + 500;
	real unc = 300;

	for (int dsi : datasets.keys)
	{
		currentpicture.legend.delete();

		string dataset = datasets[dsi];
		DrawFillBands(fills[dsi], y_min, y_max);

		draw(swToHours, RootGetObject(topDir+dataset+"/alignment.root", "global/"+units[ui]+"/c_fit"), "p,eb", magenta, mCi+1pt+magenta, "method fit");
		draw(swToHours, RootGetObject(topDir+dataset+"/alignment.root", "global/"+units[ui]+"/c_shift"), "p,eb", heavygreen, mCi+1pt+heavygreen, "method shift");
		draw(swToHours, RootGetObject(topDir+dataset+"/alignment.root", "global/"+units[ui]+"/c"), "p,eb", blue, mCi+1pt+blue, "combination");

		/*
		draw(swToHours, RootGetObject(topDir+dataset+"/alignment.root", "global/"+units[ui]+"/c_min_diff"), "p,eb", cyan, mCi+1pt+cyan);
		draw(swToHours, RootGetObject(topDir+dataset+"/alignment.root", "global/"+units[ui]+"/c_prob"), "p,eb", blue, mCi+1pt+blue);
		draw(swToHours, RootGetObject(topDir+dataset+"/alignment.root", "global/"+units[ui]+"/c_mean_diff_sq"), "p,eb", magenta, mCi+1pt+magenta);
		draw(swToHours, RootGetObject(topDir+dataset+"/alignment.root", "global/"+units[ui]+"/c_hist_chi_sq"), "p,eb", heavygreen, mCi+1pt+heavygreen);
		*/
			
		//draw(swToHours, RootGetObject(topDir+dataset+"/alignment.root", "global/"+units[ui]+"/c"), "p,l,eb", blue+1pt, mCi+1pt+blue);

		if (drawFit)
		{
			RootObject fit = RootGetObject(topDir+dataset+"/alignment_fit.root", units[ui]+"/c_fit");

			draw(shift(0, +unc)*swToHours, fit, "l", red+dashed);
			draw(shift(0,    0)*swToHours, fit, "l", red+2pt, "fit");
			draw(shift(0, -unc)*swToHours, fit, "l", red+dashed, "uncertainty");
		}
	}
	
	if (drawGlobalFit)
	{
		RootObject fit = RootGetObject(topDir+"/alignment/global_fit.root", units[ui]+"/c_fit");

		draw(shift(0, +unc)*swToHours, fit, "l", red+dashed);
		draw(shift(0,    0)*swToHours, fit, "l", red+2pt);
		draw(shift(0, -unc)*swToHours, fit, "l", red+dashed);
	}

	ylimits(y_min, y_max, Crop);
	//AttachLegend(unit_labels[ui], SE, SE);

	SetPadWidth();
}

frame f_legend = BuildLegend();

NewPad(false);
attach(f_legend);

//----------------------------------------------------------------------------------------------------

GShipout("alignment", hSkip=1mm, vSkip=0mm);
