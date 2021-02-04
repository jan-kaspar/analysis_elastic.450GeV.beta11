import root;
import pad_layout;

string fills[];
fills.push("7302");

string units[];
string u_rps[][];
units.push("45-220-fr"); u_rps.push(new string[] {"23"});
units.push("45-210-fr"); u_rps.push(new string[] {"3"});
units.push("56-210-fr"); u_rps.push(new string[] {"103"});
units.push("56-220-fr"); u_rps.push(new string[] {"123"});

//----------------------------------------------------------------------------------------------------

NewPad(false);
for (int ui : units.keys)
	NewPadLabel(units[ui]);

for (string fill : fills)
{
	NewRow();

	NewPadLabel(fill);

	string f = "tracks_" + fill + ".root";

	for (int ui : units.keys)
	{
		NewPad("$x$", "$y$");
		scale(Linear, Linear, Log);

		for (int rpi : u_rps[ui].keys)
		{
			RootObject h2 = RootGetObject(f, "RP " + u_rps[ui][rpi] + "/h2_y_vs_x");
			draw(h2, "def");
		}

		limits((5, -10), (30, +10), Crop);
	}
}

GShipout(vSkip=0mm);
