include "shared.asy";

f_tags.push(""); f_labels.push(""); f_shorts.push("");

//----------------------------------------------------------------------------------------------------

ResetPlots();
plots.push("th_x_diffRL_vs_th_x_G"); x_scales.push(1e6); y_scales.push(1e6); lab_hs.push("$\th_x^{*}\ung{\mu rad}$"); lab_vs.push("$\De^{R-L}\th_x^{*}\ung{\mu rad}$"); y_mins.push(-150); y_maxs.push(+150);
plots.push("th_x_L_diffFN_vs_th_x_L"); x_scales.push(1e6); y_scales.push(1e6); lab_hs.push("$\th_x^{*L}\ung{\mu rad}$"); lab_vs.push("$\De^{F-N}\th_x^{*L}\ung{\mu rad}$"); y_mins.push(-150); y_maxs.push(+150);
plots.push("th_x_R_diffFN_vs_th_x_R"); x_scales.push(1e6); y_scales.push(1e6); lab_hs.push("$\th_x^{*R}\ung{\mu rad}$"); lab_vs.push("$\De^{F-N}\th_x^{*R}\ung{\mu rad}$"); y_mins.push(-150); y_maxs.push(+150);

PlotOne("dist_2D_ang_diff_for_tuning_x");

//----------------------------------------------------------------------------------------------------

ResetPlots();
plots.push("th_y_diffRL_vs_th_y_G"); x_scales.push(1e6); y_scales.push(1e6); lab_hs.push("$\th_y^{*}\ung{\mu rad}$"); lab_vs.push("$\De^{R-L}\th_y^{*}\ung{\mu rad}$"); y_mins.push(-150); y_maxs.push(+150);
plots.push("th_y_L_diffFN_vs_th_y_L"); x_scales.push(1e6); y_scales.push(1e6); lab_hs.push("$\th_y^{*L}\ung{\mu rad}$"); lab_vs.push("$\De^{F-N}\th_y^{*L}\ung{\mu rad}$"); y_mins.push(-150); y_maxs.push(+150);
plots.push("th_y_R_diffFN_vs_th_y_R"); x_scales.push(1e6); y_scales.push(1e6); lab_hs.push("$\th_y^{*R}\ung{\mu rad}$"); lab_vs.push("$\De^{F-N}\th_y^{*R}\ung{\mu rad}$"); y_mins.push(-150); y_maxs.push(+150);

PlotOne("dist_2D_ang_diff_for_tuning_y");
