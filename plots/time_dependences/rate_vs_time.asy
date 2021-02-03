import root;
import pad_layout;
include "../common.asy";
include "../run_info.asy";

string datasets[], fills[];
datasets.push("DS-fill7301/Totem1"); fills.push("7301");
datasets.push("DS-fill7302/Totem1"); fills.push("7302");

string diagonals[] = { "45b_56t", "45t_56b" };
string dgn_labels[] = { "45b -- 56t", "45t -- 56b" };

string types[], t_labels[];
pen t_pens[];
types.push("input"); t_labels.push("coincidence of 2 RPs (220-fr)"); t_pens.push(heavygreen);
types.push("dgn"); t_labels.push("coincidence of 4 RPs (220-fr and 210-fr)"); t_pens.push(red);
types.push("sel"); t_labels.push("selected"); t_pens.push(blue);

xSizeDef = 10cm;
ySizeDef = 6cm;

//xTicksDef = LeftTicks(1., 0.5);

TGraph_errorBar = None;

int rebin = 30;

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
		NewPad("time from 14 Oct 2018$\ung{h}$", "rate$\ung{Hz}$");
		//currentpad.yTicks = RightTicks(1., 0.2);
		real y_min = 0, y_max = 300;

		DrawRunBands(fills[dsi], y_min, y_max);

		for (int ti : types.keys)
		{
			RootObject hist = RootGetObject(topDir+datasets[dsi]+"/distributions_"+diagonals[dgni]+".root", "metadata/rate cmp|h_timestamp_" + types[ti], error=false);
			if (!hist.valid)
				continue;

			hist.vExec("Rebin", rebin);

			draw(scale(1./3600, 1./rebin), hist, "vl", t_pens[ti]);
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
