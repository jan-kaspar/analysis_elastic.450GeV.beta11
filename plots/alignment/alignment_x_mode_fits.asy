import root;
import pad_layout;
include "../common.asy";

string dataset = "data/fill7302/Totem1";
string period = "0";

string units[], unit_labels[];
units.push("L_2_F"); unit_labels.push("L-220-fr");
units.push("L_1_F"); unit_labels.push("L-210-fr");
units.push("R_1_F"); unit_labels.push("R-210-fr");
units.push("R_2_F"); unit_labels.push("R-220-fr");

string y_slices[];
y_slices.push("-11.0_-10.5");
y_slices.push("-9.5_-9.0");

y_slices.push("-12.5_-12.0");
y_slices.push("-10.5_-10.0");
y_slices.push("-8.5_-8.0");
y_slices.push("-6.5_-6.0");
y_slices.push("-4.5_-4.0");

y_slices.push("+4.0_+4.5");
y_slices.push("+6.0_+6.5");
y_slices.push("+8.0_+8.5");
y_slices.push("+10.0_+10.5");
y_slices.push("+12.0_+12.5");

//----------------------------------------------------------------------------------------------------

NewPad(false);
for (int ui : units.keys)
	NewPadLabel(unit_labels[ui]);

for (int ysi : y_slices.keys)
{
	NewRow();

	NewPadLabel(replace(y_slices[ysi], "_", " to "));

	for (int ui : units.keys)
	{
		string f = topDir + dataset + "/alignment.root";
		string base = "period " + period + "/unit " + units[ui] + "/horizontal/horizontal profile max/h_x_" + y_slices[ysi];

		NewPad("$x\ung{mm}$");
 
		RootObject hist = RootGetObject(f, base + "", error = false);

		if (!hist.valid)
			continue;

		draw(hist, "vl,eb", blue);
		draw(RootGetObject(f, base + "|f_gauss"), "l", red+1pt);

		xlimits(-5, +5, Crop);
	}

}
