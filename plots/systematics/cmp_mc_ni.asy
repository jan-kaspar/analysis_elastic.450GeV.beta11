import root;
import pad_layout;
import common_code;
include "../common.asy";

xSizeDef = 6cm;

string diagonals[], diagonal_labels[];
diagonals.push("45b_56t"); diagonal_labels.push("45 bot -- 56 top");
diagonals.push("45t_56b"); diagonal_labels.push("45 top -- 56 bot");

real z_t_maxs[], z_t_Steps[], z_t_steps[], z_e_maxs[], z_e_Steps[], z_e_steps[];
//z_t_maxs.push(0.004); z_t_Steps.push(0.002); z_t_steps.push(0.001); z_e_maxs.push(0.04); z_e_Steps.push(0.01); z_e_steps.push(0.005);
//z_t_maxs.push(0.2); z_t_Steps.push(0.05); z_t_steps.push(0.01); z_e_maxs.push(0.02); z_e_Steps.push(0.005); z_e_steps.push(0.001);
z_t_maxs.push(0.10); z_t_Steps.push(0.02); z_t_steps.push(0.01); z_e_maxs.push(0.06); z_e_Steps.push(0.01); z_e_steps.push(0.005);

string mc_source = "studies/systematics/data-mc/1E7";
string mc_binning = "sb1";

AddAllModes();
//FilterModes("sh-thy-LRasym");
//FilterModes("sh-thy", "sc-thy");
//FilterModes("sh-thy-LRasym");
//FilterModes("dx-non-gauss");
//FilterModes("sc-thxy");

TH1_x_min = t_min_axis;

//----------------------------------------------------------------------------------------------------

bool legend_drawn = false;

NewPad(false);

for (int dgni : diagonals.keys)
{
	for (int zi : z_t_maxs.keys)
	{
		NewPad(false);	
		label("{\SetFontSizesXX " + diagonal_labels[dgni] + "}");
	}
}

for (int mi : modes.keys)
{
	NewRow();
	write("* " + modes[mi].mc_tag);

	NewPad(false);
	label("{\SetFontSizesXX " + modes[mi].label + "}");

	for (int dgni : diagonals.keys)
	{
		for (int zi : z_t_maxs.keys)
		{
			TH1_x_max = TGraph_x_max = 2 * z_t_maxs[zi];
			
			NewPad("$|t|\ung{GeV^2}$", "systematic effect");

			// ----- MC -----

			string mc_f = topDir + mc_source + "/" + diagonals[dgni] + "/mc_process.root";
			string objPath = modes[mi].mc_tag + "/" + mc_binning + "/h_eff";

			RootObject os = RootGetObject(mc_f, objPath, error=false);
			if (os.valid)
				draw(shift(0, -modes[mi].mc_ref), os, "eb", heavygreen, "Monte-Carlo");

			// ----- numerical integration -----

			string ni_f = topDir + "studies/systematics/data-ni/" + diagonals[dgni] + "/ni_process.root";

			string objPath = modes[mi].ni_tag + "/g_eff";

			RootObject os = RootGetObject(ni_f, objPath, error=true);
			if (os.valid)
				draw(shift(0, -modes[mi].ni_ref), os, "l,d0", red+2pt, "numerical intergration");

			// ----- finalisation -----

			real t_Step = z_t_Steps[zi];
			real t_step = z_t_steps[zi];
			real e_Step = z_e_Steps[zi];
			real e_step = z_e_steps[zi];

			real t_min = 0;
			real t_max = z_t_maxs[zi];
			real e_min = -z_e_maxs[zi];
			real e_max = z_e_maxs[zi];

			if (modes[mi].mc_tag == "norm")
				{ e_min = 0; e_max = +0.15; e_Step = 0.05; e_step = 0.01; }

			currentpad.xTicks = LeftTicks(t_Step, t_step);
			currentpad.yTicks = RightTicks(e_Step, e_step);
		
			limits((0, -e_max), (t_max, e_max), Crop);
		
			xaxis(YEquals(0, false), dashed);
			yaxis(XEquals(t_min_axis, false), dashed);
	
			if (!legend_drawn)
			{
				frame f_legend = BuildLegend();
				NewPad(false, 0, 0);
				add(f_legend);
				legend_drawn = true;
			}
		}
	}
}

GShipout(hSkip=1mm, vSkip=1mm);
