import root;
import pad_layout;
include "../common.asy";

string datasets[], fills[];
datasets.push("data/fill7280/Totem1"); fills.push("7280");
datasets.push("data/fill7281/Totem1"); fills.push("7281");
datasets.push("data/fill7282/Totem1"); fills.push("7282");
datasets.push("data/fill7283/Totem1"); fills.push("7283");
datasets.push("data/fill7284/Totem1"); fills.push("7284");
datasets.push("data/fill7285/Totem1"); fills.push("7285");
datasets.push("data/fill7289/Totem1"); fills.push("7289");
datasets.push("data/fill7291/Totem1"); fills.push("7291");

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
