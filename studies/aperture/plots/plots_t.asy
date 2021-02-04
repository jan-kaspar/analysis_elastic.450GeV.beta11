import root;
import pad_layout;

string topDir = "../";

string f_si = topDir + "single_arm.root";
string f_do = topDir + "double_arm.root";

//----------------------------------------------------------------------------------------------------

NewPadLabel("single arm");

NewPad("$|t|\ung{GeV^2}$");

draw(RootGetObject(f_si, "arm 1/h_t_all"), black, "input to MC");
draw(RootGetObject(f_si, "arm 1/h_t_acc_vtx_neg"), red, "with acceptance, vtx neglected");
draw(RootGetObject(f_si, "arm 1/h_t_acc_vtx_inc"), blue, "with acceptance, vtx included");

//limits((0, 0), (0.05, 7e5), Crop);

AttachLegend(S, N);


NewPad("$|t|\ung{GeV^2}$", "(vtx included) / (vtx neglected)");

draw(RootGetObject(f_si, "arm 1/h_t_acc_vtx_inc_over_neg"), "vl", magenta);

limits((0, 0.0), (0.05, 1.2), Crop);

//----------------------------------------------------------------------------------------------------

NewRow();

NewPadLabel("double arm");

NewPad("$|t|\ung{GeV^2}$");

draw(RootGetObject(f_do, "h_t_all"), black, "input to MC");
draw(RootGetObject(f_do, "h_t_acc_vtx_neg"), red, "with acceptance, vtx neglected");
draw(RootGetObject(f_do, "h_t_acc_vtx_inc"), blue, "with acceptance, vtx included");

//limits((0, 0), (0.05, 7e5), Crop);

AttachLegend(S, N);


NewPad("$|t|\ung{GeV^2}$", "(vtx included) / (vtx neglected)");

draw(RootGetObject(f_do, "h_t_acc_vtx_inc_over_neg"), "vl", magenta);

limits((0, 0.0), (0.05, 1.2), Crop);
