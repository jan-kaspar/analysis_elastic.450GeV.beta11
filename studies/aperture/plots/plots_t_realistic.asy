import root;
import pad_layout;

string topDir = "../";

string f_do = topDir + "double_arm_realistic.root";

//----------------------------------------------------------------------------------------------------

NewRow();

NewPadLabel("double arm, realistic");

NewPad("$|t|\ung{GeV^2}$");

draw(RootGetObject(f_do, "h_t_all"), "vl", black, "input to MC");
draw(RootGetObject(f_do, "h_t_acc_vtx_neg_std_corr"), "vl", red, "with acceptance, vtx neglected + std.~corr.");
draw(RootGetObject(f_do, "h_t_acc_vtx_inc_std_corr"), "vl", blue, "with acceptance, vtx included + std.~corr.");

//limits((0, 0), (0.05, 7e5), Crop);

AttachLegend(S, N);


NewPad("$|t|\ung{GeV^2}$", "(with acc. + std.~corr.) / (all)");

draw(RootGetObject(f_do, "h_t_acc_vtx_neg_std_corr_over_all"), "vl", red);
draw(RootGetObject(f_do, "h_t_acc_vtx_inc_std_corr_over_all"), "vl", blue);

limits((0, 0.80), (0.05, 1.05), Crop);
