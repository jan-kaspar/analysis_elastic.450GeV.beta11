import root;
import pad_layout;

string top_dir = "../../";

string datasets[] = {
	"DS-fill7301/Totem1",
	//"DS-fill7302/Totem1",
};

string diagonals[], diagonal_labels[];
diagonals.push("45b_56t"); diagonal_labels.push("45 bot -- 56 top");
diagonals.push("45t_56b"); diagonal_labels.push("45 top -- 56 bot");

string cuts[], cut_labels[];
pen cut_pens[];
cuts.push("L"); cut_labels.push("left arm"); cut_pens.push(red);
cuts.push("R"); cut_labels.push("right arm"); cut_pens.push(blue+dashed);
cuts.push("G"); cut_labels.push("global"); cut_pens.push(heavygreen);

xSizeDef = 8cm;
ySizeDef = 8cm;

//----------------------------------------------------------------------------------------------------

for (int dsi : datasets.keys)
{
	NewRow();
	
	NewPad(false);
	label("\vbox{\SetFontSizesXX\hbox{" + datasets[dsi] + "}}");

	for (int dgni : diagonals.keys)
	{
		string f = top_dir+"/"+datasets[dsi]+"/distributions_" + diagonals[dgni] + ".root";

		TH2_x_min = -800e-6;
		TH2_x_max = +800e-6;

		if (diagonals[dgni] == "45b_56t")
		{
			TH2_y_min = +100e-6;
			TH2_y_max = +550e-6;
		} else {
			TH2_y_min = -550e-6;
			TH2_y_max = -100e-6;
		}

		//yTicksDef = RightTicks(1., 0.5);

		NewPad("$\th_x^{*}\ung{\mu rad}$", "$\th_y^{*}\ung{\mu rad}$");
		currentpad.xTicks = LeftTicks(200., 100.);

		for (int ci : cuts.keys)
		{
			RootObject obj = RootGetObject(f, "fiducial cuts/fc_"+cuts[ci]);
			draw(scale(1e6, 1e6), obj, "l", cut_pens[ci], cut_labels[ci]);

			if (cuts[ci] != "G")
			{
				real sh = 2;
			
				//draw(shift(+sh, 0)*scale(1e6, 1e6), obj, "l", StdPen(ci+1) + dashed);
				//draw(shift(-sh, 0)*scale(1e6, 1e6), obj, "l", StdPen(ci+1) + dashed);
			}

			if (cuts[ci] == "G")
			{
				/*
				real y = obj.rExec("Eval", 0) * 1e6;

				draw((0, y), mCi+3pt+StdPen(ci+1));
				pair alig = (diagonals[dgni] == "45b_56t") ? NW : SE;
				label(format("%#.3f", y), (0, y), alig, StdPen(ci+1));
				*/
			}
		}

		limits((TH2_x_min*1e6, TH2_y_min*1e6), (TH2_x_max*1e6, TH2_y_max*1e6), Crop);
		AttachLegend(diagonal_labels[dgni], NW, NE);
	}
}

GShipout(margin=1mm, hSkip=1mm);
