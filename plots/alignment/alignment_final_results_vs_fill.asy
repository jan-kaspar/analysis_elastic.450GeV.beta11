import root;
import pad_layout;
include "../common.asy";

string datasets[], fills[];
datasets.push("data/fill7301/Totem1"); fills.push("7301");
datasets.push("data/fill7302/Totem1"); fills.push("7302");

//----------------------------------------------------------------------------------------------------

void DrawOne(int idx, string label)
{
	NewPad("dataset index", label, xSize = 10cm);

	for (int dsi : datasets.keys)
	{
		RootObject g_results = RootGetObject(topDir + datasets[dsi] + "/alignment_final.root", "g_results");

		real ax[] = {0.};
		real ay[] = {0.};

		g_results.vExec("GetPoint", idx, ax, ay);	
		real mu = ax[0] * 1e6, mu_unc = ay[0] * 1e6;

		draw((dsi, mu), mCi+3pt+red);
		draw((dsi, mu-mu_unc)--(dsi, mu+mu_unc), red);
	}

	limits((-1, -4), (datasets.length, 1), Crop);
}

//----------------------------------------------------------------------------------------------------

DrawOne(0, "$\mu_x\ung{\mu rad}$");
DrawOne(1, "$\mu_y\ung{\mu rad}$");
