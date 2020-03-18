import root;
import pad_layout;

string f = "single_side.root";

NewPad("$|t|\ung{GeV^2}$");

draw(RootGetObject(f, "arm 1/h_t_all"), black, "input to MC");
draw(RootGetObject(f, "arm 1/h_t_acc_vtx_neg"), red, "with acceptance, vtx neglected");
draw(RootGetObject(f, "arm 1/h_t_acc_vtx_inc"), blue, "with acceptance, vtx included");

//limits((0, 0), (0.1, 7e5), Crop);

AttachLegend(S, N);


NewPad("$|t|\ung{GeV^2}$", "(vtx included) / (vtx neglected)");

draw(RootGetObject(f, "arm 1/h_t_acc_vtx_inc_over_neg"), "vl", magenta);

limits((0, 0.8), (0.10, 1.2), Crop);
