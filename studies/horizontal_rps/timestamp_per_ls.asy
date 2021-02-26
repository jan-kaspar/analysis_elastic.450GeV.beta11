import root;
import pad_layout;

string f = "rate_per_ls.root";

string runs[];
// fill 7301
runs.push("324575");
runs.push("324576");

// fill 7302
runs.push("324578");
runs.push("324579");
runs.push("324580");
runs.push("324581");

yTicksDef = RightTicks(0.1, 0.05);

//----------------------------------------------------------------------------------------------------

for (int runi : runs.keys)
{
	NewRow();
	
	NewPadLabel("run " + runs[runi]);

	NewPad("ls", "timestamp$\ung{h}$");

	RootObject g = RootGetObject(f, "timestamps/run " + runs[runi] + "/g_timestamps");
	if (g.valid)
		draw(scale(1, 1./3600), g, "l", red);

	//limits((0, 0), (200, 5e4), Crop);
}
