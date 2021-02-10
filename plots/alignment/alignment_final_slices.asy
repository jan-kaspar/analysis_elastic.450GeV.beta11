import root;
import pad_layout;
include "../common.asy";

string dataset = "data/fill7291/Totem1";

string f = topDir + dataset + "/alignment_final.root";

string slices[] = {
	"-25.0_-0.0",
	"-50.0_-25.0",
	"-75.0_-50.0",
	"-100.0_-75.0",
	"-150.0_-100.0",
	"-200.0_-150.0",

	"NEW_ROW",

	"+0.0_+25.0",
	"+25.0_+50.0",
	"+50.0_+75.0",
	"+75.0_+100.0",
	"+100.0_+150.0",
	"+150.0_+200.0",
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
