import root;
import pad_layout;
include "../common.asy";
include "../run_info.asy";

string fills[], datasets[];
fills.push("7301"); datasets.push("data/fill7301/Totem1");
fills.push("7302"); datasets.push("data/fill7302/Totem1");

string diagonals[] = { "45b_56t", "45t_56b" };
string dgn_labels[] = { "45b -- 56t", "45t -- 56b" };
pen dgn_pens[] = { blue, red };

string cuts[], c_units[];
real c_scales[];
real c_rms_min[], c_rms_max[];
cuts.push("1"); c_units.push("\mu rad"); c_scales.push(1e6); c_rms_min.push(40.); c_rms_max.push(70);
cuts.push("2"); c_units.push("\mu rad"); c_scales.push(1e6); c_rms_min.push(30); c_rms_max.push(60);

cuts.push("5"); c_units.push("mm"); c_scales.push(1); c_rms_min.push(0.05); c_rms_max.push(0.11);
cuts.push("6"); c_units.push("mm"); c_scales.push(1); c_rms_min.push(0.05); c_rms_max.push(0.11);

cuts.push("7"); c_units.push("mm"); c_scales.push(1); c_rms_min.push(0.10); c_rms_max.push(0.13);
cuts.push("8"); c_units.push("mm"); c_scales.push(1); c_rms_min.push(0.3); c_rms_max.push(0.4);

cuts.push("9"); c_units.push("mm"); c_scales.push(1); c_rms_min.push(0.10); c_rms_max.push(0.40);
cuts.push("10"); c_units.push("mm"); c_scales.push(1); c_rms_min.push(0.10); c_rms_max.push(0.40);

string quantities[], q_options[], q_labels[];
//quantities.push("p_cq_time"); q_options.push("eb,d0"); q_labels.push("mean vs.~time");
quantities.push("g_cq_RMS"); q_options.push("p,d0"); q_labels.push("RMS vs.~time");

xSizeDef = 10cm;
ySizeDef = 6cm;

xTicksDef = LeftTicks(1., 0.5);

TGraph_errorBar = None;

//TGraph_reducePoints = 10;

//----------------------------------------------------------------------------------------------------

void SetPadWidth()
{
	real factorHoursToSize = 6cm / 3;

	real timespan = currentpicture.userMax2().x - currentpicture.userMin2().x;
	currentpad.xSize = timespan * factorHoursToSize;
}

//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------

for (int ci : cuts.keys)
{
	for (int qi : quantities.keys)
	{
		NewRow();

		NewPad(false);
		label("{\SetFontSizesXX\vtop{\hbox{cut " + cuts[ci] + ":}\hbox{" + q_labels[qi] + "}}}");

		for (int dsi : datasets.keys)
		{
			NewPad("time $\ung{h}$", "$\ung{" + c_units[ci] + "}$");
			//currentpad.yTicks = RightTicks(1., 0.2);
			//real y_min = -3, y_max = +2;
			
			if (quantities[qi] == "g_cq_RMS")
				DrawRunBands(fills[dsi], c_rms_min[ci], c_rms_max[ci]);
		
			for (int dgni : diagonals.keys)
			{
				string f = topDir+datasets[dsi]+"/distributions_"+diagonals[dgni]+".root";

				RootObject obj = RootGetObject(f, "elastic cuts/cut " + cuts[ci] + "/" + quantities[qi] + cuts[ci]);

				if (obj.InheritsFrom("TGraph"))
					draw(swToHours*scale(1, c_scales[ci]), obj, q_options[qi], dgn_pens[dgni], mCi+2pt+dgn_pens[dgni], dgn_labels[dgni]);
				else
					draw(swToHours*scale(1, c_scales[ci]), obj, q_options[qi], dgn_pens[dgni], dgn_labels[dgni]);

				string obj_name_par = "elastic cuts/cut " + cuts[ci] + "/g_cut_parameters";
				RootObject obj_par = RootGetObject(f, obj_name_par);
				real ax[] = {0}, ay[] = {0};
				obj_par.vExec("GetPoint", 0, ax, ay); real cca = ay[0];
				obj_par.vExec("GetPoint", 1, ax, ay); real ccb = ay[0];
				obj_par.vExec("GetPoint", 2, ax, ay); real ccc = ay[0];
				obj_par.vExec("GetPoint", 3, ax, ay); real csi = ay[0];
				obj_par.vExec("GetPoint", 4, ax, ay); real n_si = ay[0];
		
				if (quantities[qi] == "g_cq_RMS")
				{
					xaxis(YEquals(csi * c_scales[ci], false), dgn_pens[dgni]+1pt+dashed);
				}
			}

			if (quantities[qi] == "g_cq_RMS")
			{
				ylimits(c_rms_min[ci], c_rms_max[ci], Crop);
			}
		
			SetPadWidth();
		}
	}
}


//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------

frame f_leg = BuildLegend();

NewRow();

NewPad(false);
attach(f_leg);


GShipout(vSkip=0mm);
