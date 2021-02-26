import root;
import pad_layout;
include "../common.asy";

string datasets[], fills[];
datasets.push("data/fill7301/Totem1"); fills.push("7301");
datasets.push("data/fill7302/Totem1"); fills.push("7302");

string diagonals[];
diagonals.push("45b_56t");
diagonals.push("45t_56b");

string rps[], rp_labels[];
rps.push("L_2_F"); rp_labels.push("45-220-fr");
rps.push("L_1_F"); rp_labels.push("45-210-fr");
rps.push("R_1_F"); rp_labels.push("56-210-fr");
rps.push("R_2_F"); rp_labels.push("56-220-fr" );

string slices[], s_labels[];
slices.push("slice_th_x_-400_-300"); s_labels.push("$-400 < \th^*_x < -300\un{\mu rad}$");
slices.push("slice_th_x_-300_-200"); s_labels.push("$-300 < \th^*_x < -200\un{\mu rad}$");
slices.push("slice_th_x_-200_-100"); s_labels.push("$-200 < \th^*_x < -100\un{\mu rad}$");
slices.push("slice_th_x_-100_+0"); s_labels.push("$-100 < \th^*_x < 0\un{\mu rad}$");
slices.push("slice_th_x_+0_+100"); s_labels.push("$0 < \th^*_x < +100\un{\mu rad}$");
slices.push("slice_th_x_+100_+200"); s_labels.push("$+100 < \th^*_x < +200\un{\mu rad}$");
slices.push("slice_th_x_+200_+300"); s_labels.push("$+200 < \th^*_x < +300\un{\mu rad}$");
slices.push("slice_th_x_+300_+400"); s_labels.push("$+300 < \th^*_x < +400\un{\mu rad}$");

xSizeDef = 6cm;
ySizeDef = 5cm;
yTicksDef = RightTicks(5., 1.);

//----------------------------------------------------------------------------------------------------

for (int dsi : datasets.keys)
{
	real rp_eff_cen[] = { 0.98, 0.98, 0.98, 0.98};

	for (int dgi : diagonals.keys)
	{
		NewPage();

		string f = topDir + datasets[dsi] + "/eff3outof4_fit_" + diagonals[dgi] + ".root";

		NewRow();

		NewPadLabel(replace("\vbox{\SetFontSizesXX\hbox{dataset: "+datasets[dsi]+"}\hbox{diagonal: "+diagonals[dgi]+"}}", "_", "\_"));
		
		for (int rpi : rps.keys)
			NewPadLabel(rp_labels[rpi]);
		
		frame fLegend;

		for (int sli : slices.keys)
		{
			NewRow();

			NewPadLabel(s_labels[sli]);

			for (int rpi : rps.keys)
			{
				NewPad("$\th_y^*\ung{\mu rad}$", "\ung{\%}");
				currentpad.yTicks = RightTicks(1., 0.5);

				string p_base = rps[rpi] + "/" + slices[sli] + "/cmp";

				draw(scale(1e6, 100), RootGetObject(f, p_base+"|h_th_y_slice"), "vl,eb", blue, "efficiency histogram");

				RootObject g_fit = RootGetObject(f, p_base+"|g_fit");
				draw(shift(0, +2) * scale(1e6, 100), g_fit, "l", red+dashed);
				draw(shift(0, +0) * scale(1e6, 100), g_fit, "l", red+1pt, "fit");
				draw(shift(0, -2) * scale(1e6, 100), g_fit, "l", red+dashed);

				real eff_cen = rp_eff_cen[rpi]; 
				if (fills[dsi] == "7302" && diagonals[dgi] == "45t_56b" && rps[rpi] == "L_1_F")
					eff_cen = 0.85;

				limits((200, 100*eff_cen - 10), (550, 100*eff_cen + 2), Crop);
				fLegend = BuildLegend();
			}
		}
	}
}

GShipout("eff3outof4_fits", vSkip=0pt);
