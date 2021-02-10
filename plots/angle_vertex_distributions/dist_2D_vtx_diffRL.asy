include "shared.asy";

dir = "selected - vertex";

f_tags.push(""); f_labels.push("right - left"); f_shorts.push("R-L");

//----------------------------------------------------------------------------------------------------

ResetPlots();
plots.push("vtx_x_diffRL_vs_th_x_G"); x_scales.push(1e6); y_scales.push(1e3); lab_hs.push("$\th_x^{*}\ung{\mu rad}$"); lab_vs.push("$\De^{R-L}x^{*}\ung{\mu m}$"); y_mins.push(-150); y_maxs.push(+150);
plots.push("vtx_x_diffRL_vs_th_y_G"); x_scales.push(1e6); y_scales.push(1e3); lab_hs.push("$\th_y^{*}\ung{\mu rad}$"); lab_vs.push("$\De^{R-L}x^{*}\ung{\mu m}$"); y_mins.push(-150); y_maxs.push(+150);
plots.push("vtx_x_diffRL_vs_vtx_x_G"); x_scales.push(1e3); y_scales.push(1e3); lab_hs.push("$x^*\ung{\mu m}$"); lab_vs.push("$\De^{R-L}x^{*}\ung{\mu m}$"); y_mins.push(-150); y_maxs.push(+150);
plots.push("vtx_x_diffRL_vs_vtx_y_G"); x_scales.push(1e3); y_scales.push(1e3); lab_hs.push("$y^*\ung{\mu m}$"); lab_vs.push("$\De^{R-L}x^{*}\ung{\mu m}$"); y_mins.push(-150); y_maxs.push(+150);

PlotOne("dist_2D_vtx_diffRL_x");

//----------------------------------------------------------------------------------------------------

ResetPlots();
plots.push("vtx_y_diffRL_vs_th_x_G"); x_scales.push(1e6); y_scales.push(1e3); lab_hs.push("$\th_x^{*}\ung{\mu rad}$"); lab_vs.push("$\De^{R-L}y^{*}\ung{\mu m}$"); y_mins.push(-100); y_maxs.push(+100);
plots.push("vtx_y_diffRL_vs_th_y_G"); x_scales.push(1e6); y_scales.push(1e3); lab_hs.push("$\th_y^{*}\ung{\mu rad}$"); lab_vs.push("$\De^{R-L}y^{*}\ung{\mu m}$"); y_mins.push(-100); y_maxs.push(+100);
plots.push("vtx_y_diffRL_vs_vtx_x_G"); x_scales.push(1e3); y_scales.push(1e3); lab_hs.push("$x^*\ung{\mu m}$"); lab_vs.push("$\De^{R-L}y^{*}\ung{\mu m}$"); y_mins.push(-100); y_maxs.push(+100);
plots.push("vtx_y_diffRL_vs_vtx_y_G"); x_scales.push(1e3); y_scales.push(1e3); lab_hs.push("$y^*\ung{\mu m}$"); lab_vs.push("$\De^{R-L}y^{*}\ung{\mu m}$"); y_mins.push(-100); y_maxs.push(+100);

PlotOne("dist_2D_vtx_diffRL_y");
