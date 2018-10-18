import root;
import pad_layout;
include "../run_info.asy";

string datasets[];
datasets.push("DS-fill7280");
datasets.push("DS-fill7281");
datasets.push("DS-fill7286");

string diagonals[] = { "45b_56t", "45t_56b" };
string dgn_labels[] = { "45b -- 56t", "45t -- 56b" };

string types[], t_labels[];
pen t_pens[];
types.push("input"); t_labels.push("coincidence of 2 RPs (220-fr)"); t_pens.push(heavygreen);
types.push("dgn"); t_labels.push("coincidence of 4 RPs (220-fr and 210-fr)"); t_pens.push(red);
types.push("sel"); t_labels.push("selected"); t_pens.push(blue);

string topDir = "../../";

xSizeDef = 10cm;
ySizeDef = 6cm;

//xTicksDef = LeftTicks(1., 0.5);

TGraph_errorBar = None;

int rebin = 30;

transform swToMinutes = scale(1/60, 1);

//----------------------------------------------------------------------------------------------------

void SetPadWidth()
{
	/*
	real factorHoursToSize = 50cm / 3;
	real timespan = currentpicture.userMax2().x - currentpicture.userMin2().x;
	currentpad.xSize = timespan * factorHoursToSize;
	*/

	currentpad.xSize = 12cm;
}

//----------------------------------------------------------------------------------------------------

NewPad();

for (int dgni : diagonals.keys)
{
	NewPad(false);
	label("{\SetFontSizesXX " + dgn_labels[dgni] + "}");
}

for (int dsi : datasets.keys)
{
	NewRow();
	
	NewPad(false);
	label("{\SetFontSizesXX " + datasets[dsi] + "}");

	for (int dgni : diagonals.keys)
	{
		NewPad("time $\ung{min}$", "rate$\ung{Hz}$");
		//currentpad.yTicks = RightTicks(1., 0.2);
		real y_min = 0, y_max = 10;

		//DrawRunBands(fills[dsi], y_min, y_max);

		for (int ti : types.keys)
		{
			RootObject hist = RootGetObject(topDir+datasets[dsi]+"/distributions_"+diagonals[dgni]+".root", "metadata/rate cmp|h_timestamp_" + types[ti]);
			hist.vExec("Rebin", rebin);

			draw(scale(1., 1./rebin) * swToMinutes, hist, "vl", t_pens[ti]);
		}

		ylimits(y_min, y_max, Crop);
		SetPadWidth();
	}
}

NewPad(false);
for (int ti : types.keys)
	AddToLegend(t_labels[ti], t_pens[ti]);
AttachLegend();

GShipout(vSkip=0mm);
