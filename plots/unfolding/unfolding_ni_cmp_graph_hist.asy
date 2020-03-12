import root;
import pad_layout;
include "../common.asy";

string topDir = "../../";

string diagonals[] = { "45t_56b" };
string diagonal_labels[] = { "45t -- 56b" };

string model = "fit-1";

string dataset = "DS-fill7301/Totem1";

string binnings[] = {
	"ub",
	"eb",
};

xSizeDef = 10cm;
ySizeDef = 8cm;

TGraph_x_min = t_min;
TH1_x_min = t_min;

//----------------------------------------------------------------------------------------------------

void PlotAll(string f)
{
	draw(RootGetObject(f, model + "/g_t_corr"), black+1pt, "graph");

	for (int bi : binnings.keys)
	{
		draw(RootGetObject(f, model + "/" + binnings[bi]), "vl", StdPen(bi+1), binnings[bi]);
	}
}

//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------

for (int dgni : diagonals.keys)
{
	NewRow();

	NewPad(false);
	label("\vbox{\SetFontSizesXX\hbox{" + dataset + "}\hbox{" + diagonal_labels[dgni] + "}}");

	string f = topDir + dataset + "/unfolding_cf_ni_" + diagonals[dgni] + ".root";

	NewPad("$|t|\ung{GeV^2}$", "mutiplicative correction");
	currentpad.yTicks = RightTicks(0.002, 0.001);

	PlotAll(f);

	limits((0, 0.99), (0.10, 1.01), Crop);

	AttachLegend(SE, SE);
}

GShipout(margin=1mm);
