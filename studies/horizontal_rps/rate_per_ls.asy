import root;
import pad_layout;

string f = "rate_per_ls.root";

string rps[];
rps.push("23");
rps.push("3");
rps.push("103");
rps.push("123");

string runs[];
// fill 7301
runs.push("324575");
runs.push("324576");

// fill 7302
runs.push("324578");
runs.push("324579");
runs.push("324580");
runs.push("324581");

//----------------------------------------------------------------------------------------------------

NewPad(false);
for (int rpi : rps.keys)
	NewPadLabel(rps[rpi]);

for (int runi : runs.keys)
{
	NewRow();
	
	NewPadLabel("run " + runs[runi]);

	for (int rpi : rps.keys)
	{
		NewPad("ls", "tracks per ls");

		RootObject g = RootGetObject(f, "RP " + rps[rpi] + "/run " + runs[runi]);
		if (g.valid)
			draw(g, "l,p", red, mCi+2pt+red);

		limits((0, 0), (200, 5e4), Crop);
	}
}
