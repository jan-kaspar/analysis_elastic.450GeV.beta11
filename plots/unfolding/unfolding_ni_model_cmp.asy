import root;
import pad_layout;
include "../common.asy";

string topDir = "../../";

string diagonals[] = { "45t_56b" };
string diagonal_labels[] = { "45t -- 56b" };

string models[] = {
	"fit-1",
};

string dataset = "DS-fill7301/Totem1";

xSizeDef = 10cm;
ySizeDef = 8cm;

TGraph_x_min = t_min;

//----------------------------------------------------------------------------------------------------

for (int dgni : diagonals.keys)
{
	NewRow();

	NewPad(false);
	label("\vbox{\SetFontSizesXX\hbox{" + dataset + "}\hbox{" + diagonal_labels[dgni] + "}}");

	NewPad("$|t|\ung{GeV^2}$", "mutiplicative correction");
	currentpad.yTicks = RightTicks(0.002, 0.001);

	for (int mi : models.keys)
	{
		string f = topDir + dataset + "/unfolding_cf_ni_" + diagonals[dgni] + ".root";
		draw(RootGetObject(f, models[mi] + "/g_t_corr"), StdPen(mi+1), replace(models[mi], "_", "\_"));
	}

	limits((0, 0.99), (0.10, 1.01), Crop);

	AttachLegend(SE, SE);
}

GShipout(margin=1mm);
