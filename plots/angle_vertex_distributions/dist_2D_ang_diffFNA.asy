include "shared.asy";

f_tags.push("_L"); f_labels.push("left arm"); f_shorts.push("L");
f_tags.push("_R"); f_labels.push("right arm"); f_shorts.push("R");

//----------------------------------------------------------------------------------------------------

ResetPlots();
plots.push("th_x<F>_diffNA_vs_th_x<F>"); x_scales.push(1e6); y_scales.push(1e6); lab_hs.push("$\th_x^{*<F>}\ung{\mu rad}$"); lab_vs.push("$\De^{N-A}\th_x^{*<F>}\ung{\mu rad}$"); y_mins.push(-150); y_maxs.push(+150);
plots.push("th_x<F>_diffNA_vs_vtx_x<F>"); x_scales.push(1e3); y_scales.push(1e6); lab_hs.push("$x^{*<F>}\ung{\mu m}$"); lab_vs.push("$\De^{N-A}\th_x^{*<F>}\ung{\mu rad}$"); y_mins.push(-150); y_maxs.push(+150);
plots.push("th_x<F>_diffNA_vs_th_y<F>"); x_scales.push(1e6); y_scales.push(1e6); lab_hs.push("$\th_y^{*<F>}\ung{\mu rad}$"); lab_vs.push("$\De^{N-A}\th_x^{*<F>}\ung{\mu rad}$"); y_mins.push(-150); y_maxs.push(+150);
plots.push("th_x<F>_diffNA_vs_vtx_y<F>"); x_scales.push(1e3); y_scales.push(1e6); lab_hs.push("$y^{*<F>}\ung{\mu m}$"); lab_vs.push("$\De^{N-A}\th_x^{*<F>}\ung{\mu rad}$"); y_mins.push(-150); y_maxs.push(+150);

PlotOne("dist_2D_ang_diffFNA_x_N");

//----------------------------------------------------------------------------------------------------

ResetPlots();
plots.push("th_x<F>_diffFA_vs_th_x<F>"); x_scales.push(1e6); y_scales.push(1e6); lab_hs.push("$\th_x^{*<F>}\ung{\mu rad}$"); lab_vs.push("$\De^{F-A}\th_x^{*<F>}\ung{\mu rad}$"); y_mins.push(-150); y_maxs.push(+150);
plots.push("th_x<F>_diffFA_vs_vtx_x<F>"); x_scales.push(1e3); y_scales.push(1e6); lab_hs.push("$x^{*<F>}\ung{\mu m}$"); lab_vs.push("$\De^{F-A}\th_x^{*<F>}\ung{\mu rad}$"); y_mins.push(-150); y_maxs.push(+150);
plots.push("th_x<F>_diffFA_vs_th_y<F>"); x_scales.push(1e6); y_scales.push(1e6); lab_hs.push("$\th_y^{*<F>}\ung{\mu rad}$"); lab_vs.push("$\De^{F-A}\th_x^{*<F>}\ung{\mu rad}$"); y_mins.push(-150); y_maxs.push(+150);
plots.push("th_x<F>_diffFA_vs_vtx_y<F>"); x_scales.push(1e3); y_scales.push(1e6); lab_hs.push("$y^{*<F>}\ung{\mu m}$"); lab_vs.push("$\De^{F-A}\th_x^{*<F>}\ung{\mu rad}$"); y_mins.push(-150); y_maxs.push(+150);

PlotOne("dist_2D_ang_diffFNA_x_F");

//----------------------------------------------------------------------------------------------------

ResetPlots();
plots.push("th_y<F>_diffNA_vs_th_x<F>"); x_scales.push(1e6); y_scales.push(1e6); lab_hs.push("$\th_x^{*<F>}\ung{\mu rad}$"); lab_vs.push("$\De^{N-A}\th_y^{*<F>}\ung{\mu rad}$"); y_mins.push(-150); y_maxs.push(+150);
plots.push("th_y<F>_diffNA_vs_vtx_x<F>"); x_scales.push(1e3); y_scales.push(1e6); lab_hs.push("$x^{*<F>}\ung{\mu m}$"); lab_vs.push("$\De^{N-A}\th_y^{*<F>}\ung{\mu rad}$"); y_mins.push(-150); y_maxs.push(+150);
plots.push("th_y<F>_diffNA_vs_th_y<F>"); x_scales.push(1e6); y_scales.push(1e6); lab_hs.push("$\th_y^{*<F>}\ung{\mu rad}$"); lab_vs.push("$\De^{N-A}\th_y^{*<F>}\ung{\mu rad}$"); y_mins.push(-150); y_maxs.push(+150);
plots.push("th_y<F>_diffNA_vs_vtx_y<F>"); x_scales.push(1e3); y_scales.push(1e6); lab_hs.push("$y^{*<F>}\ung{\mu m}$"); lab_vs.push("$\De^{N-A}\th_y^{*<F>}\ung{\mu rad}$"); y_mins.push(-150); y_maxs.push(+150);

PlotOne("dist_2D_ang_diffFNA_y_N");

//----------------------------------------------------------------------------------------------------

ResetPlots();
plots.push("th_y<F>_diffFA_vs_th_x<F>"); x_scales.push(1e6); y_scales.push(1e6); lab_hs.push("$\th_x^{*<F>}\ung{\mu rad}$"); lab_vs.push("$\De^{F-A}\th_y^{*<F>}\ung{\mu rad}$"); y_mins.push(-150); y_maxs.push(+150);
plots.push("th_y<F>_diffFA_vs_vtx_x<F>"); x_scales.push(1e3); y_scales.push(1e6); lab_hs.push("$x^{*<F>}\ung{\mu m}$"); lab_vs.push("$\De^{F-A}\th_y^{*<F>}\ung{\mu rad}$"); y_mins.push(-150); y_maxs.push(+150);
plots.push("th_y<F>_diffFA_vs_th_y<F>"); x_scales.push(1e6); y_scales.push(1e6); lab_hs.push("$\th_y^{*<F>}\ung{\mu rad}$"); lab_vs.push("$\De^{F-A}\th_y^{*<F>}\ung{\mu rad}$"); y_mins.push(-150); y_maxs.push(+150);
plots.push("th_y<F>_diffFA_vs_vtx_y<F>"); x_scales.push(1e3); y_scales.push(1e6); lab_hs.push("$y^{*<F>}\ung{\mu m}$"); lab_vs.push("$\De^{F-A}\th_y^{*<F>}\ung{\mu rad}$"); y_mins.push(-150); y_maxs.push(+150);

PlotOne("dist_2D_ang_diffFNA_y_F");
