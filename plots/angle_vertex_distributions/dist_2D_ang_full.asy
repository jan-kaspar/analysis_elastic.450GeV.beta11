include "shared.asy";

f_tags.push("_L"); f_labels.push("left arm"); f_shorts.push("L");
f_tags.push("_R"); f_labels.push("right arm"); f_shorts.push("R");
f_tags.push("_G"); f_labels.push("both arms"); f_shorts.push("G");

//----------------------------------------------------------------------------------------------------

ResetPlots();
//plots.push("th_x<F>_vs_th_x<F>"); x_scales.push(1e6); y_scales.push(1e6); lab_hs.push("$\th_x^{*<F>}\ung{\mu rad}$"); lab_vs.push("$\th_x^{*<F>}\ung{\mu rad}$"); y_mins.push(-150); y_maxs.push(+150);
plots.push("th_x<F>_vs_vtx_x<F>"); x_scales.push(1e3); y_scales.push(1e6); lab_hs.push("$x^{*<F>}\ung{\mu m}$"); lab_vs.push("$\th_x^{*<F>}\ung{\mu rad}$"); y_mins.push(-150); y_maxs.push(+150);
plots.push("th_x<F>_vs_th_y<F>"); x_scales.push(1e6); y_scales.push(1e6); lab_hs.push("$\th_y^{*<F>}\ung{\mu rad}$"); lab_vs.push("$\th_x^{*<F>}\ung{\mu rad}$"); y_mins.push(-150); y_maxs.push(+150);
plots.push("th_x<F>_vs_vtx_y<F>"); x_scales.push(1e3); y_scales.push(1e6); lab_hs.push("$y^{*<F>}\ung{\mu m}$"); lab_vs.push("$\th_x^{*<F>}\ung{\mu rad}$"); y_mins.push(-150); y_maxs.push(+150);

PlotOne("dist_2D_ang_full_x");

//----------------------------------------------------------------------------------------------------

ResetPlots();
plots.push("th_y<F>_vs_th_x<F>"); x_scales.push(1e6); y_scales.push(1e6); lab_hs.push("$\th_x^{*<F>}\ung{\mu rad}$"); lab_vs.push("$\th_y^{*<F>}\ung{\mu rad}$"); y_mins.push(-150); y_maxs.push(+150);
plots.push("th_y<F>_vs_vtx_x<F>"); x_scales.push(1e3); y_scales.push(1e6); lab_hs.push("$x^{*<F>}\ung{\mu m}$"); lab_vs.push("$\th_y^{*<F>}\ung{\mu rad}$"); y_mins.push(-150); y_maxs.push(+150);
//plots.push("th_y<F>_vs_th_y<F>"); x_scales.push(1e6); y_scales.push(1e6); lab_hs.push("$\th_y^{*<F>}\ung{\mu rad}$"); lab_vs.push("$\th_y^{*<F>}\ung{\mu rad}$"); y_mins.push(-150); y_maxs.push(+150);
plots.push("th_y<F>_vs_vtx_y<F>"); x_scales.push(1e3); y_scales.push(1e6); lab_hs.push("$y^{*<F>}\ung{\mu m}$"); lab_vs.push("$\th_y^{*<F>}\ung{\mu rad}$"); y_mins.push(-150); y_maxs.push(+150);

PlotOne("dist_2D_ang_full_y");
