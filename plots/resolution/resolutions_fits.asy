import root;
import pad_layout;
include "../common.asy";
include "../run_info.asy";

string datasets[], dataset_fills[];
datasets.push("data/fill7301/Totem1"); dataset_fills.push("7301");
datasets.push("data/fill7302/Totem1"); dataset_fills.push("7302");

string diagonals[] = { "45b_56t", "45t_56b" };
string dgn_labels[] = { "45b -- 56t", "45t -- 56b" };
pen dgn_pens[] = { blue, red };

xSizeDef = 10cm;
ySizeDef = 6cm;

xTicksDef = LeftTicks(1., 0.5);

TGraph_errorBar = None;

//TGraph_reducePoints = 10;

bool plotUncertainties = false;

//----------------------------------------------------------------------------------------------------

void SetPadWidth()
{
	real factorHoursToSize = 10cm / 3;

	real timespan = currentpicture.userMax2().x - currentpicture.userMin2().x;
	currentpad.xSize = timespan * factorHoursToSize;
}

//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------

for (int dsi : datasets.keys)
{
	NewPad("time $\ung{h}$", "std.~dev.~of $\De^{R-L}\th_x^*\ung{\mu rad}$");
	currentpad.yTicks = RightTicks(1., 0.5);
	real y_min = 50., y_max = 60.;
	
	DrawRunBands(dataset_fills[dsi], y_min, y_max);

	for (int dgni : diagonals.keys)
	{
		string f = topDir+datasets[dsi]+"/distributions_"+diagonals[dgni]+".root";

		draw(swToHours*scale(1, 1e6),
			RootGetObject(f, "time dependences/gRMS_diffLR_th_x_vs_time"), "p,eb,d0",
			dgn_pens[dgni], mCi+2pt+dgn_pens[dgni], dgn_labels[dgni]);

		RootObject fit = RootGetObject(topDir+datasets[dsi]+"/resolution_fit_"+diagonals[dgni]+".root", "d_x/g_fits");

		draw(swToHours*scale(1, 1e6), fit, "l",	dgn_pens[dgni]);

		if (plotUncertainties)
		{
			real unc = 0.3;	// mu rad
			draw(shift(0, +unc) * swToHours*scale(1, 1e6), fit, "l", dgn_pens[dgni] + dashed);
			draw(shift(0, -unc) * swToHours*scale(1, 1e6), fit, "l", dgn_pens[dgni] + dashed);
		}

		// validation
		/*
		draw(swToHours*scale(1, 1e6),
			RootGetObject(f, "time dependences/p_input_d_x_rms_vs_time"), "p,eb,d0", magenta+1pt);
		*/
	}

	ylimits(y_min, y_max, Crop);
	SetPadWidth();
}

//----------------------------------------------------------------------------------------------------

NewRow();

for (int dsi : datasets.keys)
{
	NewPad("time $\ung{h}$", "std.~dev.~of $\De^{R-L}\th_y^*\ung{\mu rad}$");
	currentpad.yTicks = RightTicks(1., 0.5);
	real y_min = 40., y_max = 50.;

	DrawRunBands(dataset_fills[dsi], y_min, y_max);

	for (int dgni : diagonals.keys)
	{
		string f = topDir+datasets[dsi]+"/distributions_"+diagonals[dgni]+".root";

		draw(swToHours*scale(1, 1e6),
			RootGetObject(f, "time dependences/gRMS_diffLR_th_y_vs_time"), "p,eb,d0",
			dgn_pens[dgni], mCi+2pt+dgn_pens[dgni], dgn_labels[dgni]);

		RootObject fit = RootGetObject(topDir+datasets[dsi]+"/resolution_fit_"+diagonals[dgni]+".root", "d_y/g_fits");

		draw(swToHours*scale(1, 1e6), fit, "l",	dgn_pens[dgni]);

		if (plotUncertainties)
		{
			real unc = 0.007;	// mu rad
			draw(shift(0, +unc) * swToHours*scale(1, 1e6), fit, "l", dgn_pens[dgni] + dashed);
			draw(shift(0, -unc) * swToHours*scale(1, 1e6), fit, "l", dgn_pens[dgni] + dashed);
		}

		// validation
		/*
		draw(swToHours*scale(1, 1e6),
			RootGetObject(f, "time dependences/p_input_d_y_rms_vs_time"), "p,eb,d0", magenta+1pt);
		*/
	}
	
	ylimits(y_min, y_max, Crop);
	SetPadWidth();
}

frame f_leg = BuildLegend();

//----------------------------------------------------------------------------------------------------

NewRow();

NewPad(false);
attach(f_leg);

GShipout(vSkip=0mm);
