import root;
import pad_layout;

string topDir = "../../data/all-good/Totem1/";

string dgns[], d_labels[];
dgns.push("45b_56t"); d_labels.push("45 bot -- 56 top");
dgns.push("45t_56b"); d_labels.push("45 top -- 56 bot");

//----------------------------------------------------------------------------------------------------

void DrawOne(string fn, string con, string con_h_lab, string con_v_lab, string tests[])
{
	for (int dgni : dgns.keys)
	{
		NewRow();

		NewPadLabel(d_labels[dgni]);

		/*
		NewPad(con_h_lab, con_v_lab);
		for (int ti : tests.keys)
		{
			pen p = StdPen(ti);

			RootObject obj = RootGetObject(topDir + tests[ti] + "/distributions_" + dgns[dgni] + ".root", con);
			if (obj.InheritsFrom("TH1"))
				draw(obj, "vl", p);
			if (obj.InheritsFrom("TGraph"))
				draw(scale(1e6, 1e6), obj, "l", p);
		}

		if (find(fn, "th_") == 0)
		{
			currentpad.xTicks = LeftTicks(100., 50.);
			currentpad.xSize = 10cm;
		}
		*/

		NewPad("$x^*$");
		for (int ti : tests.keys)
			draw(RootGetObject(topDir + tests[ti] + "/distributions_" + dgns[dgni] + ".root", "selected - vertex/h_vtx_x"), "vl", StdPen(ti));

		NewPad("$y^*$");
		for (int ti : tests.keys)
			draw(RootGetObject(topDir + tests[ti] + "/distributions_" + dgns[dgni] + ".root", "selected - vertex/h_vtx_y"), "vl", StdPen(ti));

		NewPad("$\th_x^*\ung{\mu rad}$", "$\th_y^*\ung{\mu rad}$", xTicks = LeftTicks(100., 50.), xSize = 8cm);
		for (int ti : tests.keys)
			draw(scale(1e6, 1e6), RootGetObject(topDir + tests[ti] + "/distributions_" + dgns[dgni] + ".root", "fiducial cuts/fc_G"), "l", StdPen(ti));

		NewPad("$|t|\ung{GeV^2}$", xSize=10cm, xTicks = LeftTicks(0.01, 0.005));
		scale(Linear, Log);
		for (int ti : tests.keys)
		{
			pen p = StdPen(ti);
			draw(RootGetObject(topDir + tests[ti] + "/distributions_" + dgns[dgni] + ".root", "normalization/eb/h_t_normalized"), "n,eb", p);
		}

		limits((0, 1e0), (0.09, 1e2), Crop);
	}

	GShipout(fn);
}

//----------------------------------------------------------------------------------------------------

string tests[] = new string[] { "default", "vtx_x_low", "vtx_x_mid", "vtx_x_hig" };
DrawOne("vtx_x", "selected - vertex/h_vtx_x", "$x^*$", "", tests);

string tests[] = new string[] { "default", "vtx_y_low", "vtx_y_mid", "vtx_y_hig" };
DrawOne("vtx_y", "selected - vertex/h_vtx_y", "$y^*$", "", tests);

string tests[] = new string[] { "default", "th_x_low", "th_x_mid", "th_x_hig" };
DrawOne("th_x", "fiducial cuts/fc_G", "$\th_x^*\ung{\mu rad}$", "$\th_y^*\ung{\mu rad}$", tests);

string tests[] = new string[] { "default", "th_y_low", "th_y_mid", "th_y_hig" };
DrawOne("th_y", "fiducial cuts/fc_G", "$\th_x^*\ung{\mu rad}$", "$\th_y^*\ung{\mu rad}$", tests);
