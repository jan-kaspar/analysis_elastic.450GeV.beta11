import root;
import pad_layout;
include "../common.asy";

string datasets[], dataset_fills[];
datasets.push("DS-fill7301/Totem1"); dataset_fills.push("7301");
datasets.push("DS-fill7302/Totem1"); dataset_fills.push("7302");

string diagonals[] = { "45b_56t", "45t_56b" };
string diagLabels[] = { "45 bot -- 56 top", "45 top -- 56 bot" };

string binning = "ub";

drawGridDef = true;

xSizeDef = 8cm;
ySizeDef = 8cm;

//----------------------------------------------------------------------------------------------------

for (int dsi : datasets.keys)
{
	xTicksDef = LeftTicks(0.02, 0.01);

	NewPad("$|t|\ung{GeV^2}$", "$\d\si/\d t\ung{mb/GeV^2}$");
	scale(Linear, Log);

	for (int dgni : diagonals.keys)
	{
		pen p = StdPen(dgni + 1);
		string pth = "normalization/"+binning+"/h_t_normalized";
		//string pth = "normalization+unfolding/"+binning+"/h_t_normalized_unsmeared";
		draw(RootGetObject(topDir+datasets[dsi]+"/distributions_"+diagonals[dgni]+".root", pth),
			"d0,eb", p, diagLabels[dgni]);
	}

	limits((0, 1e6), (0.1, 1e8), Crop);

	AttachLegend(datasets[dsi]);
}
