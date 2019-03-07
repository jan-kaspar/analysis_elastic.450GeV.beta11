import root;
import pad_layout;

string f = "../../reconstruction_formulae/test_formulae_graph.root";

void DrawSet(string desc, string iq, real scale_x, string unit, string formulae[], string labels[])
{
	string complementary = (iq == "x" || iq == "y") ? "angle" : "vertex";

	NewPad(false);
	for (int fi : formulae.keys)
	{
		//NewPad(false);
		NewPadLabel(labels[fi]);
	}

	NewRow();

	NewPadLabel(desc);

	frame lf;

	for (int fi : formulae.keys)
	{
		string formula = formulae[fi];
	
		NewPad("$"+iq+"^{*, sim}\ung{"+unit+"}$", "std.~dev.~of $"+iq+"^{*, reco} - "+iq+"^{*, sim}\ung{"+unit+"}$");
		scale(Linear, Linear(true));
		draw(scale(scale_x, 1), RootGetObject(f, formula+"/pitch/g_stddev"), "l,p", black, mCi+1pt+black, "pitch");
		draw(scale(scale_x, 1), RootGetObject(f, formula+"/beamDiv/g_stddev"), "l,p", red, mCi+1pt+red, "beamDiv");
		draw(scale(scale_x, 1), RootGetObject(f, formula+"/"+complementary+"/g_stddev"), "l,p", blue, mCi+1pt+blue, ""+complementary+"");
		draw(scale(scale_x, 1), RootGetObject(f, formula+"/pitch,beamDiv,"+complementary+"/g_stddev"), "l,p", magenta, mCi+1pt+magenta, "pitch,beamDiv,"+complementary+"");

		lf = BuildLegend();
		
		/*
		NewPad("$"+iq+"^{*, sim}\ung{"+unit+"}$", "std.~dev.~of $"+iq+"^{*, reco} - "+iq+"^{*, sim}\ung{\mu rad}$");
		currentpicture.legend = prev_pic.legend;
		scale(Linear, Linear(true));
		draw(scale(scale_x, 1), RootGetObject(f, formula+"/optics/g_stddev"), "l,p", heavygreen, mCi+1pt+heavygreen, "optics");
		draw(scale(scale_x, 1), RootGetObject(f, formula+"/"+complementary+",optics/g_stddev"), "l,p", brown, mCi+1pt+brown, ""+complementary+",optics");
		prev_pic = currentpicture;
		
		NewPad("$"+iq+"^{*, sim}\ung{"+unit+"}$", "std.~dev.~of $"+iq+"^{*, reco} - "+iq+"^{*, sim}\ung{\mu rad}$");
		currentpicture.legend = prev_pic.legend;
		scale(Linear, Linear(true));
		draw(scale(scale_x, 1), RootGetObject(f, formula+"/pitch,beamDiv,"+complementary+",optics/g_stddev"), "l,p", orange+1pt, mCi+1.5pt+orange, "pitch,beamDiv,"+complementary+",optics");
		*/


	}

	NewPad(false);
	attach(lf);
}

//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------

string formulae[], labels[];
formulae.push("theta_x_one_arm_hit, th_x_L"); labels.push("N-F average of $x/L_x$");
formulae.push("theta_x_one_arm_regr, th_x_L"); labels.push("2-RP lin.~regression");
DrawSet("single arm", "\th_x", 1e6, "\mu rad", formulae, labels);

NewRow();

string formulae[], labels[];
formulae.push("theta_x_two_arm_hit_LRavg, th_x"); labels.push("L-R avg., N-F average of $x/L_x$");
formulae.push("theta_x_two_arm_regr_LRavg, th_x"); labels.push("L-R avg., 2-RP lin.~regr.");
formulae.push("theta_x_two_arm_full_regr, th_x"); labels.push("4-RP lin.~regression");
DrawSet("double arm", "\th_x", 1e6, "\mu rad", formulae, labels);

GShipout("plot_formulae_graphs_desc_th_x", hSkip=5mm, vSkip=3mm);

//----------------------------------------------------------------------------------------------------

string formulae[], labels[];
formulae.push("theta_y_one_arm_hit, th_y_L"); labels.push("N-F average of $y/L_y$");
formulae.push("theta_y_one_arm_regr, th_y_L"); labels.push("2-RP lin.~regression");
DrawSet("single arm", "\th_x", 1e6, "\mu rad", formulae, labels);

NewRow();

string formulae[], labels[];
formulae.push("theta_y_two_arm_hit_LRavg, th_y"); labels.push("L-R avg., N-F average of $y/L_y$");
formulae.push("theta_y_two_arm_regr_LRavg, th_y"); labels.push("L-R avg., 2-RP lin.~regr.");
DrawSet("double arm", "\th_y", 1e6, "\mu rad", formulae, labels);

GShipout("plot_formulae_graphs_desc_th_y", hSkip=5mm, vSkip=3mm);

//----------------------------------------------------------------------------------------------------

string formulae[], labels[];
formulae.push("vtx_x_one_arm_regr, vtx_x_L"); labels.push("2-RP lin.~regression");
DrawSet("single arm", "x", 1e3, "\mu m", formulae, labels);

NewRow();

string formulae[], labels[];
formulae.push("vtx_x_two_arm_regr_LRavg, vtx_x"); labels.push("L-R avg., 2-RP lin.~regr.");
DrawSet("double arm", "x", 1e3, "\mu m", formulae, labels);

GShipout("plot_formulae_graphs_desc_vtx_x", hSkip=5mm, vSkip=3mm);

//----------------------------------------------------------------------------------------------------

string formulae[], labels[];
formulae.push("vtx_y_one_arm_regr, vtx_y_L"); labels.push("2-RP lin.~regression");
DrawSet("single arm", "y", 1e3, "\mu m", formulae, labels);

NewRow();

string formulae[], labels[];
formulae.push("vtx_y_two_arm_regr_LRavg, vtx_y"); labels.push("L-R avg., 2-RP lin.~regr.");
DrawSet("double arm", "y", 1e3, "\mu m", formulae, labels);

GShipout("plot_formulae_graphs_desc_vtx_y", hSkip=5mm, vSkip=3mm);
