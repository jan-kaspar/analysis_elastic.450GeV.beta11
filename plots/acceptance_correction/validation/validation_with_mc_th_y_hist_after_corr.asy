import root;
import pad_layout;
include "../../common.asy";

string rows[] = {
	"simulations/0.12,1E7,uncorr/seed1/validation_with_mc.root",
//	"simulations/0.12,1E7,corr/seed1/validation_with_mc.root",
};

string th_x_ranges[] = {
	"-5.0E-06_+5.0E-06", 
	"+1.0E-04_+1.1E-04", 
	"+3.0E-04_+3.1E-04", 
};

//----------------------------------------------------------------------------------------------------

NewPad(false);

for (string th_x_range : th_x_ranges)
{
	NewPad(false);
	label("$\th_x^*$ from " + replace(th_x_range, "_", " to "));
}

for (string row : rows)
{
	NewRow();

	NewPad(false);
	label(replace(row, "_", "\_"));

	string f = topDir + "test/acceptance_code/" + row;

	for (string th_x_range : th_x_ranges)
	{
		string dir = "th_x_ranges/" + th_x_range + "/";

		NewPad("$\theta^*_y\ung{\mu rad}$", "$\hbox{smearing + fiducial cuts + smearing corr.} \over \hbox{smearing}$");
		draw(scale(1e6, 1), RootGetObject(f, dir + "h_th_ratio_cut_corr_over_no_cut"), "eb", magenta);
		
		limits((150, 0.8), (600, 1.1), Crop);
	
		xaxis(YEquals(1, false), dashed);
	}
}

GShipout(vSkip=1mm);
