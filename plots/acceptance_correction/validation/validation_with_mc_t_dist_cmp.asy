import root;
import pad_layout;
include "../../common.asy";

string rows[] = {
	"simulations/<t_max>,1E7,uncorr/seed1/validation_with_mc.root",
//	"simulations/<t_max>,1E7,corr/seed1/validation_with_mc.root",
};

string binning = "sb1";
string t_max = "0.12";

//----------------------------------------------------------------------------------------------------

void PlotAll(string f, string binning)
{
	draw(RootGetObject(f, binning + "/h_t_tr"), "eb", blue, "el.~simu.");
	draw(RootGetObject(f, binning + "/h_t_re_no_acc"), "eb", heavygreen, "el.~simu.~+ smearing");
	draw(RootGetObject(f, binning + "/h_t_re_acc_phi"), "eb", black, "el.~simu.~+ smearing + fid.~cut (phi) + acc.~corr (phi)");
	draw(RootGetObject(f, binning + "/h_t_re_acc"), "eb", red, "el.~simu.~+ smearing + fid.~cut (all) + acc.~corr (all)");
}

//----------------------------------------------------------------------------------------------------

for (string row : rows)
{
	NewRow();

	NewPad(false);
	string lab = replace(row, "_", "\_");
	lab = replace(lab, "<t\_max>", "$\langle t_{\rm max}\rangle$");
	label(lab);

	//--------------------

	string f = topDir + "test/acceptance_code/" + replace(row, "<t_max>", t_max);
	
	NewPad("$|t|\ung{GeV^2}$", "$\d\si/\d t\ung{a.u.}$");
	scale(Linear, Log);
	PlotAll(f, binning);
	limits((0.007, 5e1), (0.10, 4e2), Crop);
	
	NewPad("$|t|\ung{GeV^2}$", "$\d\si/\d t\ung{a.u.}$");
	scale(Linear, Log);
	PlotAll(f, binning);
	limits((0.007, 1e2), (0.04, 4e2), Crop);

	AttachLegend(NW, NE);
}
