import root;
import pad_layout;
include "../common.asy";

TH2_palette = Gradient(blue, heavygreen, yellow, red);

string f = topDir + "data/fill7301/Totem1/distributions_45t_56b.root";

string rp = "L_2_F";

//xTicksDef = LeftTicks(Step=10, step=5);
//yTicksDef = RightTicks(Step=10, step=5);
//yTicksDef = RightTicks(Step=0.5, step=0.1);

drawGridDef = true;

//----------------------------------------------------------------------------------------------------

TH2_y_min = -30;
TH2_y_max = +30;

//----------------------------------------------------------------------------------------------------

NewPad("$x\ung{mm}$", "$y\ung{mm}$");
scale(Linear, Linear, Log);

TH2_z_max = 6e2;

draw(RootGetObject(f, "hit distributions/vertical, aligned, before selection/h2_y_"+rp+"_vs_x_"+rp+"_al_nosel"), "p,bar");

limits((-15, -30), (+15, +30), Crop);
AttachLegend("before cuts", S, N);

//----------------------------------------------------------------------------------------------------

NewPad("$x\ung{mm}$", "$y\ung{mm}$");
scale(Linear, Linear, Log);

TH2_z_max = 2e3;

draw(RootGetObject(f, "hit distributions/vertical, aligned, after selection/h2_y_"+rp+"_vs_x_"+rp+"_al_sel"), "p,bar");

limits((-15, -30), (+15, +30), Crop);
AttachLegend("after cuts", S, N);
