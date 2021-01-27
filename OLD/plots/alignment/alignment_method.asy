import root;
import pad_layout;

string topDir = "../../";

string dataset = "DS-fill7301/Totem1";
string period = "0";
int period_idx = 0; // period index in global graph
string unit = "L_2_F";

string f = topDir + dataset + "/alignment.root";
string dir = "period "+period+"/unit "+unit;

transform xyswitch = (0, 0, 0, 1, 1, 0);

//TGraph_reducePoints = 10;
TGraph_nPointsLimit = 10000;

real x_min = -4, x_max = +4;
real y_min = -15, y_max = +15;

xTicksDef = LeftTicks(1., 0.5);

TGraph_errorBar = None;

TH2_palette = Gradient(white, black);

//----------------------------------------------------------------------------------------------------

real GetYResult()
{
	RootObject obj = RootGetObject(f, "global/"+unit+"/c");
	real x[] = { 0. };
	real y[] = { 0. };
	obj.vExec("GetPoint", period_idx, x, y);

	write(x[0]);
	write(y[0]);

	return y[0] * 1e-3;
}

//----------------------------------------------------------------------------------------------------

NewPad(false);
label("{\SetFontSizesXX hit distribution}");

NewPad(false);

NewPad(false);
label("{\SetFontSizesXX vertical centre}");

//--------------------
NewRow();

NewPad("$x\ung{mm}$", "$y\ung{mm}$");
//draw(RootGetObject(topDir+dataset+"/distributions_45b_56t.root", "alignment/"+period+"/g_y_"+unit+"_vs_x_"+unit+"_sel"), "p");
//draw(RootGetObject(topDir+dataset+"/distributions_45t_56b.root", "alignment/"+period+"/g_y_"+unit+"_vs_x_"+unit+"_sel"), "p");

draw(RootGetObject(f, dir+"/horizontal/horizontal profile max/h2_y_vs_x"), "def");

draw(xyswitch, RootGetObject(f, dir+"/horizontal/horizontal profile max/g_x_max_vs_y"), "p", green);
limits((x_min, y_min), (x_max, y_max), Crop);

NewPad(false, autoSize=false);
draw((0, 0)--(50, 0), EndArrow);


NewPad("number of entries", "$y\ung{mm}$");
currentpad.xTicks = LeftTicks(2000., 1000.);
draw(xyswitch, RootGetObject(f, dir+"/vertical/y_hist|y_hist_range"), "vl", black);
real y_beam = GetYResult();
draw((0, y_beam)--(10000, y_beam), blue+1pt);
limits((0, y_min), (10000, y_max), Crop);

//--------------------
NewRow();

NewPad(false, autoSize=false);
draw((0, 0)--(0, -50), EndArrow);

NewPad(false, autoSize=false);

NewPad(false, autoSize=false);
draw((0, 0)--(0, -50), EndArrow);

//--------------------
NewRow();

NewPad(false);
label("{\SetFontSizesXX elastic peak axis}");

NewPad(false);

NewPad(false);
label("{\SetFontSizesXX intersection: beam position}");

//--------------------
NewRow();

NewPad("$x\ung{mm}$", "$y\ung{mm}$");
//draw(RootGetObject(topDir+dataset+"/distributions_45b_56t.root", "alignment/"+period+"/g_y_"+unit+"_vs_x_"+unit+"_sel"), "p");
//draw(RootGetObject(topDir+dataset+"/distributions_45t_56b.root", "alignment/"+period+"/g_y_"+unit+"_vs_x_"+unit+"_sel"), "p");

draw(RootGetObject(f, dir+"/horizontal/horizontal profile max/h2_y_vs_x"), "def");

draw(xyswitch, RootGetObject(f, dir+"/horizontal/horizontal profile max/g_x_max_vs_y|ff"), "l", red+1pt);
draw(xyswitch, RootGetObject(f, dir+"/horizontal/horizontal profile max/g_x_max_vs_y"), "p", green);
limits((x_min, y_min), (x_max, y_max), Crop);

NewPad(false, autoSize=false);
draw((0, 0)--(50, 0), EndArrow);

NewPad("$x\ung{mm}$", "$y\ung{mm}$");
draw((-10, y_beam)--(+10, y_beam), blue+1pt);
draw(xyswitch, RootGetObject(f, dir+"/horizontal/horizontal profile max/g_x_max_vs_y|ff"), "l", red+1pt);
limits((x_min, y_min), (x_max, y_max), Crop);

GShipout(hSkip=2mm, vSkip=0mm);
