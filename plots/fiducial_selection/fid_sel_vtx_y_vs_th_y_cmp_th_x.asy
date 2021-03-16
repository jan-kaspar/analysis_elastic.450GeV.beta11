import root;
import pad_layout;
include "../common.asy";

string datasets[] = {
	"data/fill7301/Totem1",
	//"data/fill7302/Totem1",
};

string diagonals[], diagonal_labels[];
real diagonal_sign[];
diagonals.push("45b_56t"); diagonal_labels.push("45 bot -- 56 top"); diagonal_sign.push(+1);
//diagonals.push("45t_56b"); diagonal_labels.push("45 top -- 56 bot"); diagonal_sign.push(-1);

string thx_tags[], thx_labels[];
//thx_tags.push("-500"); thx_labels.push("$\th^*_x = -500\un{\mu rad}$");
thx_tags.push("-400"); thx_labels.push("$\th^*_x = -400\un{\mu rad}$");
thx_tags.push("+0"); thx_labels.push("$\th^*_x = 0\un{\mu rad}$");
thx_tags.push("+350"); thx_labels.push("$\th^*_x = +350\un{\mu rad}$");
//thx_tags.push("+450"); thx_labels.push("$\th^*_x = +450\un{\mu rad}$");

string cuts[], cut_labels[];
cuts.push("L"); cut_labels.push("left arm");
cuts.push("R"); cut_labels.push("right arm");
cuts.push("G"); cut_labels.push("global");

xSizeDef = 8cm;
ySizeDef = 8cm;

//----------------------------------------------------------------------------------------------------

NewPad(false);
NewPad(false);
for (int cti : cuts.keys)
	NewPadLabel(cut_labels[cti]);

for (int dsi : datasets.keys)
{
	for (int dgni : diagonals.keys)
	{
		string f = topDir + datasets[dsi] + "/distributions_" + diagonals[dgni] + ".root";

		bool ds_dgn_label_printed = false;

		for (int thxi : thx_tags.keys)
		{
			NewRow();

			NewPadLabel((ds_dgn_label_printed) ? "" : "\vbox{\hbox{" + datasets[dsi] + "}\hbox{" + diagonal_labels[dgni] + "}}");
			ds_dgn_label_printed = true;

			NewPadLabel(thx_labels[thxi]);

			for (int cti : cuts.keys)
			{
				string c = cuts[cti];

				NewPad("$\th^*_y\ung{\mu rad}$", "$y^*\ung{\mu m}$");

				RootObject h2 = RootGetObject(f, "selected - vertex/slice th_x=" + thx_tags[thxi] + "/h2_vtx_y_" + c + "_vs_th_y_" + c);

				RootObject g_min = RootGetObject(f, "fiducial cuts/fc_" + c + "/g_th_y_min_at_th_x_" + thx_tags[thxi]);
				RootObject g_max = RootGetObject(f, "fiducial cuts/fc_" + c + "/g_th_y_max_at_th_x_" + thx_tags[thxi]);

				draw(scale(1e6, 1e3), h2);

				draw(scale(1e6 * diagonal_sign[dgni], 1e3), g_min, "l", magenta+2pt);
				draw(scale(1e6 * diagonal_sign[dgni], 1e3), g_max, "l", red+2pt);

				real x_min = 100 * diagonal_sign[dgni], x_max = 600 * diagonal_sign[dgni];
				if (x_min > x_max)
				{
					real bla = x_min;
					x_min = x_max;
					x_max = bla;
				}

				draw((x_min, 0)--(x_max, 0), black+2pt);
			}
		}
	}
}

GShipout(margin=1mm, hSkip=1mm);
