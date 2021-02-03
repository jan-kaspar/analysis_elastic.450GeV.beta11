import root;
import pad_layout;
include "../common.asy";
include "../run_info.asy";

string fills[], f_datasets[];
fills.push("7301"); f_datasets.push("DS-fill7301/Totem1");
fills.push("7302"); f_datasets.push("DS-fill7302/Totem1");

string diagonals[], d_labels[];
pen d_pens[];
diagonals.push("45b_56t"); d_labels.push("45b -- 56t"); d_pens.push(red);
diagonals.push("45t_56b"); d_labels.push("45t -- 56b"); d_pens.push(blue);

string quantities[], q_timber[], q_labels[];
real q_maxs[], q_Ticks[], q_ticks[];
quantities.push("th_x"); q_timber.push("bd_comb_h"); q_labels.push("\th^*_x"); q_maxs.push(100.); q_Ticks.push(10.); q_ticks.push(2.);
quantities.push("th_y"); q_timber.push("bd_comb_v"); q_labels.push("\th^*_y"); q_maxs.push(100.); q_Ticks.push(10.); q_ticks.push(2.);

xSizeDef = 40cm;
ySizeDef = 6cm;

xTicksDef = LeftTicks(1., 0.5);

TGraph_errorBar = None;

//----------------------------------------------------------------------------------------------------

for (int qi : quantities.keys)
{
	NewPad("time$\ung{h}$", "$\si[\De^{R-L}" + q_labels[qi] + "]\ung{\mu rad}$");
	currentpad.yTicks = RightTicks(q_Ticks[qi], q_ticks[qi]);

	for (int fi : fills.keys)
	{
		string ft = topDir + f_datasets[fi] + "/process_timber.root";

		DrawBands(fills[fi], bands="run", labels="ds", 0., q_maxs[qi]);

		for (int dgni : diagonals.keys)
		{
			string f = topDir + f_datasets[fi] + "/distributions_" + diagonals[dgni] + ".root";
			TGraph_reducePoints = 1;
			draw(scale(1./3600, 1e6), RootGetObject(f, "time dependences/gRMS_diffLR_" + quantities[qi] + "_vs_time"), "l,p", d_pens[dgni], d_labels[dgni]);
		}

		TGraph_reducePoints = 30;
		draw(scale(1./3600, 1e6), RootGetObject(ft, "g_" + q_timber[qi]), black, "BSRTs");

		ylimits(0., q_maxs[qi], Crop);
	}

	GShipout(quantities[qi] + "_diffLR_vs_time");
}
