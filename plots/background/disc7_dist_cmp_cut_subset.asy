import root;
import pad_layout;
include "../common.asy";

string datasets[];
datasets.push("DS-fill7301/Totem1");
datasets.push("DS-fill7302/Totem1");

string diagonals[] = { "45b_56t", "45t_56b" };
string dgn_labels[] = { "45b -- 56t", "45t -- 56b" };

xSizeDef = 8cm;

//----------------------------------------------------------------------------------------------------

string dataset;
string diagonal;

void MakeComparison(string quantity, real xscale, string unit, string obj, real xlimit, real sigma,
	real xStep, real xstep,
	string combinations[], pen comb_pens[])
{
	NewPad(quantity+"$\ung{"+unit+"}$", "", xTicks=LeftTicks(xStep, xstep));
	scale(Linear, Log(true));
	for (int ci : combinations.keys)
	{
		string f = topDir+"/background_studies/"+dataset+"/"+combinations[ci]+"/distributions_"+diagonal+".root";
		draw(scale(xscale, 1), RootGetObject(f, "elastic cuts/"+obj), "vl",
			comb_pens[ci], replace(combinations[ci], "_", "\_"));	
	}

	yaxis(XEquals(-4*sigma, false), dashed);
	yaxis(XEquals(+4*sigma, false), dashed);

	xlimits(-xlimit, +xlimit, Crop);
	AttachLegend(quantity, NW, NE);
}

//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
	

NewPad(false);
for (int dgi : diagonals.keys)
	NewPadLabel(dgn_labels[dgi]);

for (int dsi : datasets.keys)
{
	dataset = datasets[dsi];

	write("* " + dataset);
	
	NewRow();

	NewPadLabel(dataset);

	for (int dgi : diagonals.keys)
	{
		diagonal = diagonals[dgi];

		string combinations[];
		pen comb_pens[];
		
		combinations.push("no_cuts"); comb_pens.push(black+2pt);
		combinations.push("cuts:1"); comb_pens.push(orange);
		combinations.push("cuts:1,2"); comb_pens.push(cyan);
		combinations.push("cuts:1,2,5,6"); comb_pens.push(magenta);
		//combinations.push("cuts:1,2,5,6,8"); comb_pens.push(gray);
		combinations.push("cuts:1,2,5,6,8,9"); comb_pens.push(blue);
		combinations.push("cuts:1,2,5,6,8,9,10"); comb_pens.push(red+2pt);
		
		MakeComparison("$\De^{\rm R-L} x^*$", 1e0, "mm", "cut 7/h_cq7", xlimit=1.5, sigma=0.12, xStep=0.5, xstep=0.1, combinations, comb_pens);
	}
}

GShipout(vSkip=1mm);
