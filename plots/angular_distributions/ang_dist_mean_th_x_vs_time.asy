import root;
import pad_layout;
include "../common.asy";
include "../run_info.asy";

string datasets[], dataset_fills[];
datasets.push("DS-fill7301/Totem1"); dataset_fills.push("7301");
//datasets.push("DS-fill7302/Totem1"); dataset_fills.push("7302");

string diagonals[], diagonal_labels[];
pen diagonal_pens[];
diagonals.push("45b_56t"); diagonal_labels.push("45 bot -- 56 top"); diagonal_pens.push(blue);
diagonals.push("45t_56b"); diagonal_labels.push("45 top -- 56 bot"); diagonal_pens.push(red);

string arms[], arm_labels[];
arms.push("_L1F"); arm_labels.push("left arm, 210-far");
arms.push("_L2F"); arm_labels.push("left arm, 220-far");
arms.push("_L"); arm_labels.push("left arm");
arms.push("_R1F"); arm_labels.push("right arm, 210-far");
arms.push("_R2F"); arm_labels.push("right arm, 220-far");
arms.push("_R"); arm_labels.push("right arm");
arms.push(""); arm_labels.push("double arm");

xTicksDef = LeftTicks(Step=1, step=0.5);

//----------------------------------------------------------------------------------------------------

void SetPadWidth()
{
	real factorHoursToSize = 8cm / 3;

	real timespan = currentpicture.userMax2().x - currentpicture.userMin2().x;
	currentpad.xSize = timespan * factorHoursToSize;
}

//----------------------------------------------------------------------------------------------------
NewRow();

for (int ai : arms.keys)
{
	NewRow();

	NewPad(false);
	label("{\SetFontSizesXX " + arm_labels[ai] + "}");
	
	for (int dsi : datasets.keys)
	{
		NewPad("time $\ung{h}$", "mean of $\th_x^*\ung{\mu rad}$");

		currentpad.yTicks = RightTicks(10., 5.);
		real y_min = -50, y_max = +50;

		DrawRunBands(dataset_fills[dsi], y_min, y_max);

		for (int dgni : diagonals.keys)
		{
			string f = topDir+datasets[dsi]+"/distributions_"+diagonals[dgni]+".root";
			pen p = diagonal_pens[dgni];
			draw(swToHours*scale(1, 1e6), RootGetObject(f, "time dependences/p_th_x"+arms[ai]+"_vs_time"),
				"p,eb,d0", p, diagonal_labels[dgni]);
		}

		ylimits(y_min, y_max, Crop);
		SetPadWidth();
	}
	
	frame f_legend = BuildLegend();

	NewPad(false);
	add(f_legend);
}

GShipout(vSkip=0);
