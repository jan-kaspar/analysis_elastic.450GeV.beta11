import root;
import pad_layout;
include "../common.asy";

xSizeDef = 10cm;
ySizeDef = 8cm;

string dataset = "data/fill7302/Totem1";

//----------------------------------------------------------------------------------------------------

string binnings[] = {
	"sb1",
	"sb2",
	"sb3",
};

//----------------------------------------------------------------------------------------------------

NewPad("$|t|\ung{GeV^2}$", "$t$ bin size$\ung{GeV^2}$");
scale(Linear, Log);

//currentpad.xTicks = LeftTicks(0.3, 0.1);
//currentpad.yTicks = RightTicks(0.05, 0.01);

AddToLegend("<{\it resolution considerations:}");
draw(shift(0, log10(1)), RootGetObject(topDir+"studies/binning/generators.root", "g_rms_t"), red+longdashed, "1 smearing sigma");
draw(shift(0, log10(2)), RootGetObject(topDir+"studies/binning/generators.root", "g_rms_t"), blue+longdashed, "2 smearing sigma");
draw(shift(0, log10(3)), RootGetObject(topDir+"studies/binning/generators.root", "g_rms_t"), heavygreen+longdashed, "3 smearing sigma");

AddToLegend("<{\it fixed statistical uncertainty:}");
draw(RootGetObject(topDir+"studies/binning/generators.root", "g_bs_stat_unc_0.020"), red+dashed, "$2\un{\%}$");
draw(RootGetObject(topDir+"studies/binning/generators.root", "g_bs_stat_unc_0.010"), blue+dashed, "$1\un{\%}$");
draw(RootGetObject(topDir+"studies/binning/generators.root", "g_bs_stat_unc_0.005"), heavygreen+dashed, "$0.5\un{\%}$");

AddToLegend("<{\it binnings in analysis:}");
for (int bi : binnings.keys)
{
	pen p = StdPen(bi+1);
	draw(RootGetObject(topDir + dataset + "/distributions_45b_56t.root", "binning/g_binning_"+binnings[bi]), "p,l,d0", p+0.2pt, mCi+0.8pt+p, binnings[bi]);
}

limits((0, 1e-4), (0.10, 4e-2), Crop);

frame f_legend = BuildLegend();

NewPad(false);
add(shift(0, 100)*f_legend);
