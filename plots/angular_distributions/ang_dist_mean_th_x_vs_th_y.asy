import root;
import pad_layout;
include "../common.asy";

string datasets[], dataset_fills[];
datasets.push("DS-fill7301/Totem1"); dataset_fills.push("7301");
//datasets.push("DS-fill7302/Totem1"); dataset_fills.push("7302");

string diagonals[], diagonal_labels[];
pen diagonal_pens[];
diagonals.push("45b_56t"); diagonal_labels.push("45 bot -- 56 top"); diagonal_pens.push(blue);
diagonals.push("45t_56b"); diagonal_labels.push("45 top -- 56 bot"); diagonal_pens.push(red);

string arms[], arm_ss[], arm_labels[];
arms.push("_L"); arm_ss.push("L"); arm_labels.push("left arm");
arms.push("_R"); arm_ss.push("R"); arm_labels.push("right arm");
arms.push(""); arm_ss.push(""); arm_labels.push("double arm");

xSizeDef = 12cm;
//xTicksDef = LeftTicks(Step=1, step=0.5);

TGraph_errorBar = None;

//----------------------------------------------------------------------------------------------------

NewPad(false);
for (int ai : arms.keys)
{
	NewPad(false);
	label("{\SetFontSizesXX " + arm_labels[ai] + " arm}");
}

//----------------------------------------------------------------------------------------------------

frame f_legend;

for (int dsi : datasets.keys)
{
	NewRow();

	NewPad(false);
	label("{\SetFontSizesXX " + datasets[dsi] + "}");

	for (int ai : arms.keys)
	{
		NewPad("$\th_y^{*"+arm_ss[ai]+"}\ung{\mu rad}$", "mode of~$\th_x^{*"+arm_ss[ai]+"}$");
		for (int dgni : diagonals.keys)
		{
			TF1_x_min = -inf;
			TF1_x_max = +inf;

			pen p = StdPen(dgni+1);

			string f = topDir+datasets[dsi]+"/distributions_"+diagonals[dgni]+".root";

			string base = "selected - angles/g_mode_th_x"+arms[ai]+"_vs_th_y"+arms[ai];

			draw(scale(1e6, 1e6), RootGetObject(f, base), "p", p, mCi+2pt+p, diagonal_labels[dgni]);
			draw(scale(1e6, 1e6), RootGetObject(f, base + "|pol1"), p + dashed);
		}

		limits((-300, -30), (300, 30), Crop);
	}
	
	f_legend = BuildLegend();

	NewPad(false);
	add(f_legend);
}

GShipout(vSkip=0mm);
