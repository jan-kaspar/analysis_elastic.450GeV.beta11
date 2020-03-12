import root;
import pad_layout;

string topDir = "../../";

string diagonals[], diagLabels[];
diagonals.push("45b_56t"); diagLabels.push("45 bot -- 56 top");
diagonals.push("45t_56b"); diagLabels.push("45 top -- 56 bot");

drawGridDef = true;

string binning = "ub";

//----------------------------------------------------------------------------------------------------

NewPad("$|t|\ung{GeV^2}$", "$\d\si/\d t\ung{mb/GeV^2}$", 12cm, 9cm);

for (int dgni : diagonals.keys)
{
	pen p = StdPen(dgni + 1);

	draw(RootGetObject(topDir+"DS-merged/merged.root", binning + "/merged/" + diagonals[dgni] + "/h_dsdt"),
		"d0,eb", p, diagLabels[dgni]);

	AddToLegend(format("events $%.2E$", robj.rExec("GetEntries")));
}

//currentpad.xTicks = LeftTicks(0.002, 0.001);
//currentpad.yTicks = RightTicks(100., 50.);
//limits((0, 400), (0.01, 1000), Crop);

AttachLegend();
