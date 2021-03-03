import root;
import pad_layout;
include "../common.asy";

string dataset = "data/fill7301/Totem1";

string f = topDir + dataset + "/alignment_final.root";

string slices[] = {
	"-100.0_-0.0",
	"-200.0_-100.0",
	"-300.0_-200.0",
	"-400.0_-300.0",
	"-500.0_-400.0",

	"NEW_ROW",

	"+0.0_+100.0",
	"+100.0_+200.0",
	"+200.0_+300.0",
	"+300.0_+400.0",
	"+400.0_+500.0",
};

//----------------------------------------------------------------------------------------------------

for (int sli : slices.keys)
{
	if (slices[sli] == "NEW_ROW")
	{
		NewRow();
		continue;
	}

	NewPad("$\th^*_y\ung{\mu rad}$");

	string base = "th_x slices/" + slices[sli];

	draw(scale(1e6, 1.), RootGetObject(f, base + "|proj"), "d0,eb", blue);
	draw(scale(1e6, 1.), RootGetObject(f, base + "|fit"), "l", red);

	string l = "$\th^*_x$ from " + replace(slices[sli], "_", " to ") + " $\mu rad$";

	AttachLegend(l, S, N);
}

GShipout(vSkip=0mm);
