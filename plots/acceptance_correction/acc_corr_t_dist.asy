import root;
import pad_layout;
include "../common.asy";

string datasets[] = {
	"DS-fill7301/Totem1",
};

string diagonals[], diagonal_labels[];
diagonals.push("45b_56t"); diagonal_labels.push("45 bot -- 56 top");
diagonals.push("45t_56b"); diagonal_labels.push("45 top -- 56 bot");

string binning = "eb";

TH2_palette = Gradient(blue, heavygreen, yellow, red);

xSizeDef = 8cm;

//----------------------------------------------------------------------------------------------------

NewPad(false);
for (int dgni : diagonals.keys)
	NewPadLabel(diagonal_labels[dgni]);

//----------------------------------------------------------------------------------------------------

for (int dsi : datasets.keys)
{
	NewRow();

	NewPad(false);
	label("\vbox{\SetFontSizesXX\hbox{" + datasets[dsi] + "}}");

	for (int dgni : diagonals.keys)
	{
		string f = top_dir+"/"+datasets[dsi]+"/distributions_" + diagonals[dgni] + ".root";

		NewPad("$|t|\ung{GeV^2}$", "$\d N/\d t$");
		currentpad.xTicks = LeftTicks(0.01, 0.005);

		string baseDir = "acceptance correction/" + binning + "/";
		draw(RootGetObject(f, baseDir+"/h_t_before"), "vl", blue, "before");
		draw(RootGetObject(f, baseDir+"/h_t_after"), "vl", red, "after");

		xlimits(0, 0.10, Crop);
		
		AttachLegend();
	}
}

GShipout(margin=1mm, hSkip=1mm, vSkip=1mm);
