import root;
import pad_layout;

string topDir = "../";

string f_si = topDir + "single_arm.root";
string f_do = topDir + "double_arm.root";

int rebin = 2;

TH2_palette = Gradient(blue, heavygreen, yellow, red);

//----------------------------------------------------------------------------------------------------

NewPad(false);

NewPadLabel("acceptance, vertex neglected");

NewPadLabel("acceptance, vertex included");

//----------------------------------------------------------------------------------------------------

NewRow();

NewPadLabel("single arm");

NewPad("$\th^*_x\ung{\mu rad}$", "$\th^*_y\ung{\mu rad}$", axesAbove=true);
RootObject h2 = RootGetObject(f_si, "arm 1/h2_th_y_vs_th_x_acc_vtx_neg_rat");
h2.vExec("Rebin2D", rebin, rebin);
h2.vExec("Scale", 1./rebin/rebin);
draw(scale(1e6, 1e6), h2);
limits((-500, -200), (+500, +200), Crop);

NewPad("$\th^*_x\ung{\mu rad}$", "$\th^*_y\ung{\mu rad}$", axesAbove=true);
RootObject h2 = RootGetObject(f_si, "arm 1/h2_th_y_vs_th_x_acc_vtx_inc_rat");
h2.vExec("Rebin2D", rebin, rebin);
h2.vExec("Scale", 1./rebin/rebin);
draw(scale(1e6, 1e6), h2);
limits((-500, -200), (+500, +200), Crop);

//----------------------------------------------------------------------------------------------------

NewRow();

NewPadLabel("double arm");

NewPad("$\th^*_x\ung{\mu rad}$", "$\th^*_y\ung{\mu rad}$", axesAbove=true);
RootObject h2 = RootGetObject(f_do, "h2_th_y_vs_th_x_acc_vtx_neg_rat");
h2.vExec("Rebin2D", rebin, rebin);
h2.vExec("Scale", 1./rebin/rebin);
draw(scale(1e6, 1e6), h2);
limits((-500, -200), (+500, +200), Crop);

NewPad("$\th^*_x\ung{\mu rad}$", "$\th^*_y\ung{\mu rad}$", axesAbove=true);
RootObject h2 = RootGetObject(f_do, "h2_th_y_vs_th_x_acc_vtx_inc_rat");
h2.vExec("Rebin2D", rebin, rebin);
h2.vExec("Scale", 1./rebin/rebin);
draw(scale(1e6, 1e6), h2);
limits((-500, -200), (+500, +200), Crop);

//----------------------------------------------------------------------------------------------------

GShipout(vSkip=0mm);
