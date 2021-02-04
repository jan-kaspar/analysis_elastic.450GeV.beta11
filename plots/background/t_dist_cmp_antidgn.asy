import root;
import pad_layout;
include "../common.asy";

string datasets[];
datasets.push("fill7301/Totem1");
datasets.push("fill7302/Totem1");

string diagonals[] = { "45b_56t", "45t_56b", "anti_45b_56b", "anti_45t_56t" };
string dgn_labels[] = { "45 bot -- 56 top", "45 top -- 56 bot", "45 bot -- 56 bot", "45 top -- 56 top" };

string cut_str = "cuts:1,2,5,6,7,8,9,10";

string binning = "eb";

xSizeDef = 8cm;

//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------

for (int dsi : datasets.keys)
{
	NewRow();

	NewPad(false);
	label(replace("{\SetFontSizesXX\vbox{\hbox{"+datasets[dsi]+"}}}", "_", "\_"));

	//--------------------

	NewPad("$|t|\ung{GeV^2}$", "$\d N / \d t\ung{a.u.}$");
	scale(Linear, Log);
	
	for (int dgni : diagonals.keys)
	{
		string f = topDir+"studies/background/"+datasets[dsi]+"/"+cut_str  + "/distributions_"+diagonals[dgni]+".root";
		//string f = topDir+datasets[dsi]+"/distributions_"+diagonals[dgni]+".root";
		draw(RootGetObject(f, "acceptance correction/" + binning + "/h_t_before"), "vl,eb", StdPen(dgni+1), dgn_labels[dgni]);
	}

	limits((0, 1e4), (0.10, 1e11), Crop);
	AttachLegend("before acceptance correction");

	//--------------------

	NewPad("$|t|\ung{GeV^2}$", "$\d N / \d t\ung{a.u.}$");
	scale(Linear, Log);
	
	for (int dgni : diagonals.keys)
	{
		string f = topDir+"studies/background/"+datasets[dsi]+"/"+cut_str  + "/distributions_"+diagonals[dgni]+".root";
		draw(RootGetObject(f, "acceptance correction/" + binning + "/h_t_after"), "vl,eb", StdPen(dgni+1), dgn_labels[dgni]);
	}

	limits((0, 1e4), (0.10, 1e11), Crop);
	AttachLegend("after acceptance correction");
}
