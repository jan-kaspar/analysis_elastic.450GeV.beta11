import root;
import pad_layout;
include "../common.asy";

string datasets[] = {
	"DS-fill7301/Totem1",
	"DS-fill7302/Totem1",
};

real thetas[] = { 200, 300, 400, 500, 600, 700 };
//real thetas[] = { 250 };

string dgns[];
dgns.push("45b_56t");
dgns.push("45t_56b");

TH2_palette = Gradient(blue, heavygreen, yellow, red);

//----------------------------------------------------------------------------------------------------

void DrawFullArc(real th)
{
	draw(scale(th)*unitcircle, dotted);
	label(rotate(-90)*Label(format("\SmallerFonts $%.0f$", th)), (th, 0), 0.5E, Fill(white+opacity(0.8)));
}

//----------------------------------------------------------------------------------------------------

for (int dsi : datasets.keys)
{
	real ySize = 6cm;

	NewPad("$\th_x^{*}\ung{\mu rad}$", "$\th_y^{*}\ung{\mu rad}$", ySize/150*150, ySize);
	currentpad.xTicks = LeftTicks(200., 100.);
	scale(Linear, Linear, Log);
	//TH2_zLabel = "(corrected) events per bin";
	TH2_paletteBarWidth = 0.05;
	
	// full arcs
	label("$\th^*\!=$", (50, 0), 0.5W, Fill(white+opacity(0.8)));
	for (real theta : thetas)
		DrawFullArc(theta);
	//label(rotate(-90)*Label("\SmallerFonts $\rm\mu rad$"), (380, 0), (0., 0), Fill(white+opacity(0.8)));

	// data
	TH2_z_min = 1;
	TH2_z_max = 3e3;

	for (int dgni : dgns.keys)
	{
		string opt = (dgni == 0) ? "def" : "p";
		draw(scale(1e6, 1e6), RootGetObject(topDir+"/"+datasets[dsi]+"/distributions_" + dgns[dgni] + ".root", "acceptance correction/h2_th_y_vs_th_x_after"), opt);
	}
	
	// accepted arcs
	for (real theta : thetas)
	{
		for (int dgni : dgns.keys)
		{
			string f = topDir+"/"+datasets[dsi]+"/distributions_" + dgns[dgni] + ".root";

			for (string name : RootGetListOfObjects(f, "fiducial cuts"))
			{
				if (find(name, format("arc_%.0f", theta)) == 0)
				{
					//write(name);
					draw(scale(1e6, 1e6), RootGetObject(f, "fiducial cuts/" + name), "l", black+1pt);
				}
			}
		}
	}

	/*
	label("\vbox{\hbox{detector}\hbox{edges}}", (-300, -230), SE, magenta, Fill(white));
	draw((-300, -235)--(-130, 30), magenta, EndArrow());
	draw((-300, -235)--(-110, -30), magenta, EndArrow());

	label("\vbox{\hbox{LHC}\hbox{appertures}}", (-300, 230), S, red, Fill(white));
	draw((-300, 230)--(-140, 130), red, EndArrow);
	draw((-300, 230)--(-190, -130), red, EndArrow);
	*/
	
	limits((-800, -800), (800, 800), Crop);
	AttachLegend(datasets[dsi]);
}

GShipout(margin=1mm, hSkip=1mm);
