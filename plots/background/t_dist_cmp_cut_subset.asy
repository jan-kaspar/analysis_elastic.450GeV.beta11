import root;
import pad_layout;

string datasets[];
datasets.push("DS-fill7301/Totem1");
datasets.push("DS-fill7302/Totem1");

string diagonals[] = { "45b_56t", "45t_56b" };
string dgn_labels[] = { "45b -- 56t", "45t -- 56b" };

string topDir = "../../";

string histogram = "acceptance correction/eb/h_t_after";
string histogram_Nev = "acceptance correction/eb/h_t_Nev_after_no_corr";

string combinations[];
pen comb_pens[];

combinations.push("no_cuts"); comb_pens.push(gray);
combinations.push("cuts:1"); comb_pens.push(black);
combinations.push("cuts:1,2"); comb_pens.push(magenta);
combinations.push("cuts:1,2,5,6"); comb_pens.push(blue);
combinations.push("cuts:1,2,5,6,7,8"); comb_pens.push(heavygreen);
combinations.push("cuts:1,2,5,6,7,8,9,10"); comb_pens.push(red+1pt);

string ref_comb = "cuts:1,2,5,6,7,8,9,10";

xSizeDef = 10cm;

//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------

void PlotRatio(RootObject o, RootObject r, pen p)
{
	int N = r.iExec("GetNbinsX");
	for (int i = 1; i <= N; ++i)
	{
		real c = r.rExec("GetBinCenter", i);
		real v = o.rExec("GetBinContent", i);
		real v_r = r.rExec("GetBinContent", i);

		real ratio = (v_r != 0) ? (v / v_r - 1.) * 100.: 0.;
		real ratio_unc = 0. * 100;

		draw(Scale((c, ratio)), mCi+p);
		draw(Scale((c, ratio-ratio_unc))--Scale((c, ratio+ratio_unc)), p);
	}
}

//----------------------------------------------------------------------------------------------------

void PlotDifference(RootObject o, RootObject r, pen p)
{
	int N = r.iExec("GetNbinsX");
	for (int i = 1; i <= N; ++i)
	{
		real c = r.rExec("GetBinCenter", i);
		real v = o.rExec("GetBinContent", i);
		real v_r = r.rExec("GetBinContent", i);

		real diff = v - v_r;
		real diff_unc = 0;

		draw(Scale((c, diff)), mCi+p);
		//draw(Scale((c, ratio-ratio_unc))--Scale((c, ratio+ratio_unc)), p);
	}
}

//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------

for (int dsi : datasets.keys)
{
	for (int dgi : diagonals.keys)
	{
		NewRow();

		string dataset = datasets[dsi];
		string diagonal = diagonals[dgi];

		NewPad(false);
		label(replace("{\SetFontSizesXX\vbox{\hbox{"+dataset+"}\hbox{"+dgn_labels[dgi]+"}}}", "_", "\_"));

		//--------------------

		/*
		NewPad("$|t|\ung{GeV^2}$", "$\d\sigma/\d t\ung{mb/GeV^2}$");
		//currentpad.xTicks = LeftTicks(0.005, 0.001);
		scale(Linear, Log);
		for (int ci : combinations.keys)
		{
			string f = topDir+"background_studies/"+dataset+"/"+combinations[ci]+"/distributions_"+diagonal+".root";
			draw(RootGetObject(f, histogram), "eb", comb_pens[ci]);
		}
		
		limits((0, 1e5), (0.10, 1e8), Crop);
		*/

		//--------------------

		NewPad("$|t|\ung{GeV^2}$", "$\d\sigma/\d t\ung{mb/GeV^2}$");
		//currentpad.xTicks = LeftTicks(0.005, 0.001);
		scale(Linear, Log);
		for (int ci : combinations.keys)
		{
			string label = replace(combinations[ci], "_", "\_");
			if (combinations[ci] == ref_comb)
				label += " (reference)";
		
			string f = topDir+"background_studies/"+dataset+"/"+combinations[ci]+"/distributions_"+diagonal+".root";
			draw(RootGetObject(f, histogram), "eb", comb_pens[ci], label);
		}
		
		frame fLegend = BuildLegend();
		
		limits((0, 1e6), (0.10, 1e8), Crop);

		//--------------------
		
		NewPad(false);
		attach(fLegend);

		//--------------------
		
		string ref_f = topDir+"/background_studies/"+dataset+"/"+ref_comb+"/distributions_"+diagonal+".root";
		RootObject ref_o = RootGetObject(ref_f, histogram);
		
		RootObject ref_oN = RootGetObject(ref_f, histogram_Nev);
		
		//--------------------

		NewPad("$|t|\ung{GeV^2}$", "$\d\sigma/\d t$: (test - ref) / ref$\ung{\%}$");
		//currentpad.xTicks = LeftTicks(0.005, 0.001);
		for (int ci : combinations.keys)
		{
			string f = topDir+"/background_studies/"+dataset+"/"+combinations[ci]+"/distributions_"+diagonal+".root";
			RootObject o = RootGetObject(f, histogram);
			PlotRatio(o, ref_o, comb_pens[ci]);
		}
		
		limits((0, -2), (0.10, 10), Crop);
		
		//--------------------

		NewPad("$|t|\ung{GeV^2}$", "$\d\sigma/\d t$: (test - ref) / ref$\ung{\%}$");
		//currentpad.xTicks = LeftTicks(0.005, 0.001);
		for (int ci : combinations.keys)
		{
			string f = topDir+"/background_studies/"+dataset+"/"+combinations[ci]+"/distributions_"+diagonal+".root";
			RootObject o = RootGetObject(f, histogram);
			PlotRatio(o, ref_o, comb_pens[ci]);
		}
		
		limits((0, -1), (0.10, 3), Crop);
		
		//--------------------
		
		/*
		NewPad("$|t|\ung{GeV^2}$", "$\d\sigma/\d t$: (test - ref) / ref$\ung{\%}$");
		for (int ci : combinations.keys)
		{
			//if (combinations[ci] == ref_comb)
			//	continue;
		
			string f = topDir+"/background_studies/"+dataset+"/"+combinations[ci]+"/distributions_"+diagonal+".root";
			RootObject o = RootGetObject(f, histogram);
			PlotRatio(o, ref_o, comb_pens[ci]);
		}
		
		limits((0, -0.5), (0.2, 1.5), Crop);
		*/
		
		//--------------------
		
		/*
		NewPad("$|t|\ung{GeV^2}$", "$N_{ev}$: (test - ref) / ref$\ung{\%}$");
		for (int ci : combinations.keys)
		{
			//if (combinations[ci] == ref_comb)
			//	continue;
		
			string f = topDir+"/background_studies/"+dataset+"/"+combinations[ci]+"/distributions_"+diagonal+".root";
			RootObject o = RootGetObject(f, histogram_Nev);
			PlotDifference(o, ref_oN, comb_pens[ci]);
		}
		
		limits((0, -1), (0.2, 20), Crop);
		*/
	}
}
