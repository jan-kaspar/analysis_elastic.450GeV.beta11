include "shared.asy";

dir = "selected - vertex";

f_tags.push("_L"); f_labels.push("left arm"); f_shorts.push("L");
f_tags.push("_R"); f_labels.push("right arm"); f_shorts.push("R");
f_tags.push("_G"); f_labels.push("both arms"); f_shorts.push("G");

//rebin = false;

//----------------------------------------------------------------------------------------------------

ResetPlots();
plots.push("vtx_x<F>_vs_th_x<F>"); x_scales.push(1e6); y_scales.push(1e3); lab_hs.push("$\th_x^{*}\ung{\mu rad}$"); lab_vs.push("$x^{*}\ung{\mu m}$"); y_mins.push(-150); y_maxs.push(+150);
plots.push("vtx_x<F>_vs_th_y<F>"); x_scales.push(1e6); y_scales.push(1e3); lab_hs.push("$\th_y^{*}\ung{\mu rad}$"); lab_vs.push("$x^{*}\ung{\mu m}$"); y_mins.push(-150); y_maxs.push(+150);
//plots.push("vtx_x<F>_vs_vtx_x<F>"); x_scales.push(1e3); y_scales.push(1e3); lab_hs.push("$x^*\ung{\mu m}$"); lab_vs.push("$x^{*}\ung{\mu m}$"); y_mins.push(-150); y_maxs.push(+150);
plots.push("vtx_x<F>_vs_vtx_y<F>"); x_scales.push(1e3); y_scales.push(1e3); lab_hs.push("$y^*\ung{\mu m}$"); lab_vs.push("$x^{*}\ung{\mu m}$"); y_mins.push(-150); y_maxs.push(+150);

PlotOne("dist_2D_vtx_full_x");

//----------------------------------------------------------------------------------------------------

ResetPlots();
plots.push("vtx_y<F>_vs_th_x<F>"); x_scales.push(1e6); y_scales.push(1e3); lab_hs.push("$\th_x^{*}\ung{\mu rad}$"); lab_vs.push("$y^{*}\ung{\mu m}$"); y_mins.push(-100); y_maxs.push(+100);
plots.push("vtx_y<F>_vs_th_y<F>"); x_scales.push(1e6); y_scales.push(1e3); lab_hs.push("$\th_y^{*}\ung{\mu rad}$"); lab_vs.push("$y^{*}\ung{\mu m}$"); y_mins.push(-100); y_maxs.push(+100);
plots.push("vtx_y<F>_vs_vtx_x<F>"); x_scales.push(1e3); y_scales.push(1e3); lab_hs.push("$x^*\ung{\mu m}$"); lab_vs.push("$y^{*}\ung{\mu m}$"); y_mins.push(-100); y_maxs.push(+100);
//plots.push("vtx_y<F>_vs_vtx_y<F>"); x_scales.push(1e3); y_scales.push(1e3); lab_hs.push("$y^*\ung{\mu m}$"); lab_vs.push("$y^{*}\ung{\mu m}$"); y_mins.push(-100); y_maxs.push(+100);

PlotOne("dist_2D_vtx_full_y");
