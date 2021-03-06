#include "classes/common_init.hh"
#include "classes/command_line_tools.hh"

#include "TFile.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TF1.h"
#include "TMath.h"
#include "TProfile.h"
#include "TDirectory.h"
#include "TKey.h"

#include <vector>
#include <map>
#include <cmath>
#include <algorithm>

using namespace std;

//----------------------------------------------------------------------------------------------------

bool saveDetails = false;

//----------------------------------------------------------------------------------------------------

struct result
{
	double value, uncertainty;

	result(double _v=0., double _u=0.) : value(_v), uncertainty(_u)
	{
	}

	static result Combine(const result &a, const result &b)
	{
		return result((a.value + b.value)/2., (a.uncertainty + b.uncertainty)/2.);
	}
};

//----------------------------------------------------------------------------------------------------

TF1 *f_gauss = nullptr;

void DoHorizontalProfileMax(TGraph *g_t, TGraph *g_b,
		double y_min_top, double y_min_bot, double y_max_top, double y_max_bot,
		map<string, map<signed int, result> > &results, signed int period)
{
	printf(">> DoHorizontalProfileMax\n");

	TGraphErrors *g_x_max_vs_y = new TGraphErrors();

	TH2D *h2_y_vs_x = new TH2D("", ";x;y", 100, -4., +4., 100, -25., +25.);
	bool first = true;

	for (double y_min = -15.; y_min < +15.; y_min += 0.5)
	{
		const double y_max = y_min + 0.5;

		char buf[100];
		sprintf(buf, "h_x_%+02.1f_%+02.1f", y_min, y_max);
		TH1 *h_x = new TH1D(buf, ";x", 100, -10., +10.);

		for (const auto g : { g_t, g_b })
		{
			for (int i = 0; i < g->GetN(); i++)
			{
				double x, y;
				g->GetPoint(i, x, y);

				bool inTop = (y_min_top < y && y < y_max_top);
				bool inBot = (y_min_bot < y && y < y_max_bot);
				if (! (inTop || inBot))
					continue;

				if (y >= y_min && y < y_max)
					h_x->Fill(x);

				if (first)
					h2_y_vs_x->Fill(x, y);
			}
		}

		first = false;

		if (h_x->GetEntries() < 800)
		{
			delete h_x;
			continue;
		}

		// rough estimate of maximum
		double x_max = 0., v_max = -1.;
		for (int bi = 1; bi <= h_x->GetNbinsX(); ++bi)
		{
			const double x = h_x->GetBinCenter(bi);
			const double v = h_x->GetBinContent(bi);

			if (v > v_max)
			{
				x_max = x;
				v_max = v;
			}
		}

		//printf("%f, %f : x_max = %f, v_max = %f\n", y_min, y_max, x_max, v_max);

		// maximum fit
		f_gauss->SetParameters(v_max, x_max, 3.0);
		f_gauss->FixParameter(0, v_max);
		f_gauss->FixParameter(1, x_max);
		h_x->Fit(f_gauss, "Q", "", -2.5, +2.5);

		f_gauss->ReleaseParameter(0);
		f_gauss->ReleaseParameter(1);
		h_x->Fit(f_gauss, "Q", "", -2.5, +2.5);

		const double x_mu = f_gauss->GetParameter(1);
		double x_si = fabs(f_gauss->GetParameter(2));
		if (x_si > 1.5)
			x_si = 1.5;
		const double x_lim_min = x_mu - 1. * x_si;
		const double x_lim_max = x_mu + 1. * x_si;

		h_x->Fit(f_gauss, "Q", "", x_lim_min, x_lim_max);
		h_x->Fit(f_gauss, "Q", "", x_lim_min, x_lim_max);

		h_x->Write();

		// add point
		int idx = g_x_max_vs_y->GetN();
		g_x_max_vs_y->SetPoint(idx, (y_max + y_min)/2., f_gauss->GetParameter(1));
		g_x_max_vs_y->SetPointError(idx, (y_max - y_min)/2., f_gauss->GetParError(1));
	}

	h2_y_vs_x->Write("h2_y_vs_x");

	// fit
	TF1 *ff = new TF1("ff", "[0] + x*[1]");
	ff->FixParameter(1, 0.);
	g_x_max_vs_y->Fit(ff, "Q", "");
	g_x_max_vs_y->Write("g_x_max_vs_y");
	ff->SetParError(1, 1E-3);

	printf("\ta = %.2f +- %.2f mrad\n", ff->GetParameter(1)*1E3, ff->GetParError(1)*1E3);
	printf("\tb = %.1f +- %.1f um\n", ff->GetParameter(0)*1E3, ff->GetParError(0)*1E3);

	results["a_pm"][period] = result(ff->GetParameter(1)*1E3, ff->GetParError(1)*1E3);
	results["b_pm"][period] = result(ff->GetParameter(0)*1E3, ff->GetParError(0)*1E3);
}

//----------------------------------------------------------------------------------------------------

void DoHorizontalProfile(TGraph *g_t, TGraph *g_b,
		double y_min_top, double y_min_bot, double y_max_top, double y_max_bot,
		map<string, map<signed int, result> > &results, signed int period)
{
	printf(">> DoHorizontalProfile\n");

	TProfile *p = new TProfile("p", ";y   (mm);mean x   (mm)", 100, -30., +30.);

	// make profile - top pot
	for (int i = 0; i < g_t->GetN(); i++)
	{
		double x, y;
		g_t->GetPoint(i, x, y);
		if (y > y_min_top && y < y_max_top)
			p->Fill(y, x);
	}
	
	// make profile - bottom pot
	for (int i = 0; i < g_b->GetN(); i++)
	{
		double x, y;
		g_b->GetPoint(i, x, y);
		if (y > y_min_bot && y < y_max_bot)
			p->Fill(y, x);
	}

	// regularize low-statistics bins
	for (int bi = 1; bi <= p->GetNbinsX(); ++bi)
	{
		if (p->GetBinEntries(bi) < 4)
		{
			p->SetBinContent(bi, 0.);
			p->SetBinError(bi, 0.);
		}
	}

	// fit
	TF1 *ff = new TF1("ff", "[0] + x*[1]");
	p->Fit(ff, "Q", "");
	p->Write();

	printf("\ta = %.2f +- %.2f mrad\n", ff->GetParameter(1)*1E3, ff->GetParError(1)*1E3);
	printf("\tb = %.1f +- %.1f um\n", ff->GetParameter(0)*1E3, ff->GetParError(0)*1E3);

	results["a_p"][period] = result(ff->GetParameter(1)*1E3, ff->GetParError(1)*1E3);
	results["b_p"][period] = result(ff->GetParameter(0)*1E3, ff->GetParError(0)*1E3);
}

//----------------------------------------------------------------------------------------------------

double a, b, ae, be;

/**
	\par sigma_w sigma for weight calculation
	\par sigma_u sigma for uncertainty calculation
**/
void OneFitIteration(TGraph *g, vector<double> &ws, double sigma_u, double sigma_w)
{
	// transformation to xi-eta space
	double al = atan(a);
	double C = cos(al), S = sin(al);

	// fit in xi-eta space
	double Sw = 0., Swx = 0., Swe = 0., Swxe = 0., Swxx = 0.;
	for (int i = 0; i < g->GetN(); i++)
	{
		double x = 0., y = 0.;
		g->GetPoint(i, x, y);
		double w = ws[i];

		double xi = C*x + S*(y-b);
		double eta = -S*x + C*(y-b);

		Sw += w;
		Swx += w*xi;
		Swe += w*eta;
		Swxe += w*xi*eta;
		Swxx += w*xi*xi;
	}

	// calculate corrections to a and b
	double det = - Swx*Swx + Swxx*Sw;
	double ap = (Sw*Swxe - Swx*Swe) / det;
	double bp = (-Swx*Swxe + Swxx*Swe) / det;

	double alp = atan(ap);
	a = tan(al + alp);
	b += bp * (cos(al) + sin(al)*tan(al+alp));

	ae = sqrt(Sw / det) * sigma_u;
	be = sqrt(Swxx / det) * sigma_u;
	
	// recompute weights
	for (int i = 0; i < g->GetN(); i++)
	{
		double x = 0., y = 0.;
		g->GetPoint(i, x, y);

		//double xi = C*x + S*(y-b);
		double eta = -S*x + C*(y-b);

		double si = sigma_w;
		double nd = eta/si;
		double cH = 1.345;
		double w = (nd <= cH) ? 1. : cH / nd;
		w *= w;
		ws[i] = w;
	}
}

//----------------------------------------------------------------------------------------------------

void DoHorizontalGraphFit(TGraph *g_t, TGraph *g_b,
	double y_min_top, double y_min_bot,
	double y_max_top, double y_max_bot,
	map<string, map<signed int, result> > &results, signed int period)
{
	printf(">> DoHorizontalGraphFit\n");
	
	// merge graphs, invert x <--> y
	TGraph *g = new TGraph();
	TH1D *h_x = new TH1D("h_x", ";x   (mm)", 100, 0., 0.);
	g->SetName("merged");
	g->SetTitle(";y   (mm);x   (mm)");
	for (int i = 0; i < g_t->GetN(); i++)
	{
		double x, y;
		g_t->GetPoint(i, x, y);
		if (y >= y_min_top && y <= y_max_top)
		{
			g->SetPoint(g->GetN(), y, x);
			h_x->Fill(x);
		}
	}

	for (int i = 0; i < g_b->GetN(); i++)
	{
		double x, y;
		g_b->GetPoint(i, x, y);
		if (y >= y_min_bot && y <= y_max_bot)
		{
			g->SetPoint(g->GetN(), y, x);
			h_x->Fill(x);
		}
	}

	h_x->Write();
	double si_x = h_x->GetRMS();

	// reset fit parameters
	a = b = ae = be = 0.;

	// reset weights
	vector<double> w;
	for (int i = 0; i < g->GetN(); i++)
		w.push_back(1.);

	// run iterations
	unsigned int iterations = 2;
	for (unsigned int i = 0; i < iterations; i++)
	{
		//printf("    iteration %u\n", i);
		OneFitIteration(g, w, si_x, 10.);
		//printf("	a = %.2E +- %.2E, b = %.2E +- %.2E\n", a, ae, b, be);
	}
	
	printf("\ta = %.2f +- %.2f mrad\n", a*1E3, ae*1E3);
	printf("\tb = %.1f +- %.1f um\n", b*1E3, be*1E3);

	if (true)
	{
		TCanvas *c = new TCanvas();
		c->SetName("horizontal fit");
		g->Draw("ap");
		TF1 *ff = new TF1("ff", "[0]*x + [1]", -30., +30.);
		ff->SetParameters(a, b);
		ff->Draw("same");
		c->Write();
	}

	results["a_g"][period] = result(a*1E3, ae*1E3);
	results["b_g"][period] = result(b*1E3, be*1E3);

	delete g;
}

//----------------------------------------------------------------------------------------------------

void DoHorizontalAlignment(TGraph *g_t, TGraph *g_b, const Analysis::AlignmentYRange &r, const string &unit,
	map<string, map<signed int, result> > &results, signed int period)
{
	printf(">> DoHorizontalAlignment\n");

	TDirectory *baseDir = gDirectory;

	printf("\ty_min_top = %.3f, y_min_bot = %.3f\n\ty_max_top = %.3f, y_max_bot = %.3f\n",
		r.top_min, r.bot_min, r.top_max, r.bot_max);
	
	gDirectory = baseDir->mkdir("horizontal profile");
	DoHorizontalProfile(g_t, g_b, r.top_min, r.bot_min, r.top_max, r.bot_max, results, period);

	gDirectory = baseDir->mkdir("horizontal profile max");
	// this method can profit from a little wider range
	Analysis::AlignmentYRange r_pm = r;
	if (unit == "L_2_F") { r_pm.bot_max = -4.0; r_pm.top_min = +4.5; }
	if (unit == "L_1_F") { r_pm.bot_max = -4.5; r_pm.top_min = +4.0; }
	if (unit == "R_1_F") { r_pm.bot_max = -4.0; r_pm.top_min = +4.0; }
	if (unit == "R_2_F") { r_pm.bot_max = -4.0; r_pm.top_min = +4.0; }
	DoHorizontalProfileMax(g_t, g_b, r_pm.top_min, r_pm.bot_min, r_pm.top_max, r_pm.bot_max, results, period);

	gDirectory = baseDir->mkdir("horizontal graph fit");
	DoHorizontalGraphFit(g_t, g_b, r.top_min, r.bot_min, r.top_max, r.bot_max, results, period);
	
	//results["a"][period] = result::Combine(results["a_p"][period], results["a_g"][period]);
	//results["b"][period] = result::Combine(results["b_p"][period], results["b_g"][period]);

	results["a"][period] = results["a_pm"][period];
	results["b"][period] = results["b_pm"][period];
}

//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------

struct entry
{
	double v;		// in mm
	double w;		// weight
	entry(double _v=0., double _w=1.) : v(_v), w(_w) {}

	friend bool operator!= (entry &e1, entry &e2);
	friend bool operator< (entry &e1, entry &e2);
};

bool operator!= (entry &e1, entry &e2)
{
	return (e1.v != e2.v);
}

bool operator< (entry &e1, entry &e2)
{
	return (e1.v < e2.v);
}

//----------------------------------------------------------------------------------------------------

void DoVerticalAlignmentFit(TH1D *y_hist, map<string, map<signed int, result> > &results, signed int period)
{
	TF1 *ff = new TF1("f", "[n1]*exp(-(x-[cen])*(x-[cen])/2./[si1]/[si1])");

	const double eta = y_hist->GetBinContent(y_hist->FindBin(8.));

	ff->SetParameter("cen", 0.);
	ff->SetParameter("n1", 2*eta);
	ff->SetParameter("si1", 8.);

	y_hist->Fit(ff, "Q");

	y_hist->Write("y_hist");

	// fill results
	const auto pIdx = ff->GetParNumber("cen");
	results["c_fit"][period] = result(ff->GetParameter(pIdx) * 1E3, ff->GetParError(pIdx) * 1E3);	// mm to um

	// clean up
	delete ff;
}

//----------------------------------------------------------------------------------------------------

void DoVerticalAlignment(TGraph *g_t, TGraph *gw_t, TGraph *g_b, TGraph *gw_b,
	const Analysis::AlignmentYRange &r, const string &unit, double c_exp,
	map<string, map<signed int, result> > &results, signed int period)
{
	printf(">> DoVerticalAlignment\n");

	TDirectory *d_top = gDirectory;

	// horizontal selection
	double x_min = -50., x_max = +50.;	// mm
	if (unit == "L_2_F") { x_min = -2.0; x_max = +1.7; };
	if (unit == "L_1_F") { x_min = -2.5; x_max = +2.5; };
	if (unit == "R_1_F") { x_min = -3.5; x_max = +3.0; };
	if (unit == "R_2_F") { x_min = -2.5; x_max = +2.0; };

	// rely entirely on user-provided cuts
	double bs_y_cut = 0.;
	printf("\tbs_y_cut = %.3f mm\n", bs_y_cut);

	// prepare samples, determine ranges 
	TH1D *y_hist = new TH1D("y_hist", "", 300, -30., +30.); y_hist->SetLineColor(4);
	TH1D *y_hist_range = new TH1D("y_hist_range", "", 300, -30., +30.); y_hist_range->SetLineColor(2);
	
	// min and max are treated as possitive values for both top and bottom pots
	double y_min_b = 1E100, y_min_t = 1E100;
	double y_max_b = -1E100, y_max_t = -1E100;
	
	vector<entry> sample_t, sample_b;

	double *xa = g_t->GetX();
	double *ya = g_t->GetY();
	double *wa = gw_t->GetY();
	for (int i = 0; i < g_t->GetN(); i++)
	{
		const double x = xa[i];
		const double y = ya[i];

		// skip bad fits
		if (y <= 0.)
			continue;

		// horizontal selection
		if (x < x_min || x > x_max)
			continue;
		
		y_min_t = min(y_min_t, y);
		y_max_t = max(y_max_t, y);
		y_hist->Fill(y, wa[i]);
		
		sample_t.push_back(entry(y, wa[i]));
	}
	
	xa = g_b->GetX();
	ya = g_b->GetY();
	wa = gw_b->GetY();
	for (int i = 0; i < g_b->GetN(); i++)
	{
		const double x = xa[i];
		const double y = ya[i];

		// skip bad fits
		if (y >= 0.)
			continue;

		// horizontal selection
		if (x < x_min || x > x_max)
			continue;
		
		y_min_b = min(y_min_b, -y);
		y_max_b = max(y_max_b, -y);
		y_hist->Fill(y, wa[i]);
		
		sample_b.push_back(entry(-y, wa[i]));
	}

	// determine ranges
	printf("\tbefore cuts\n");
	printf("\t\ty_min_b = %.3f mm, y_min_t = %.3f mm\n", y_min_b, y_min_t);
	printf("\t\ty_max_b = %.3f mm, y_max_t = %.3f mm\n", y_max_b, y_max_t);

	y_min_t = max(y_min_t + bs_y_cut, r.top_min);
	y_min_b = max(y_min_b + bs_y_cut, -r.bot_max);

	y_max_t = min(y_max_t - bs_y_cut, r.top_max);
	y_max_b = min(y_max_b - bs_y_cut, -r.bot_min);

	printf("\tafter cuts\n");
	printf("\t\ty_min_b = %.3f mm, y_min_t = %.3f mm\n", y_min_b, y_min_t);
	printf("\t\ty_max_b = %.3f mm, y_max_t = %.3f mm\n", y_max_b, y_max_t);

	// build y distribution respecting ranges
	for (const auto &e : sample_t)
		if (e.v > y_min_t && e.v < y_max_t)
			y_hist_range->Fill(e.v, e.w);

	for (const auto &e : sample_b)
		if (e.v > y_min_b && e.v < y_max_b)
			y_hist_range->Fill(-e.v, e.w);

	// remove potentially non-completely-filled bins
	{
		for (int bi = y_hist_range->FindBin(-29.); bi <= y_hist_range->GetNbinsX(); ++bi)
		{
			if (y_hist_range->GetBinContent(bi) > 0)
			{
				y_hist_range->SetBinContent(bi, 0.);
				y_hist_range->SetBinError(bi, 0.);
				break;
			}
		}

		int bi0 = y_hist_range->FindBin(0.);

		for (int bi = bi0; bi >= 1; --bi)
		{
			if (y_hist_range->GetBinContent(bi) > 0)
			{
				y_hist_range->SetBinContent(bi, 0.);
				y_hist_range->SetBinError(bi, 0.);
				break;
			}
		}

		for (int bi = bi0; bi <= y_hist_range->GetNbinsX(); ++bi)
		{
			if (y_hist_range->GetBinContent(bi) > 0)
			{
				y_hist_range->SetBinContent(bi, 0.);
				y_hist_range->SetBinError(bi, 0.);
				break;
			}
		}

		for (int bi = y_hist_range->FindBin(+29.); bi >= 1; --bi)
		{
			if (y_hist_range->GetBinContent(bi) > 0)
			{
				y_hist_range->SetBinContent(bi, 0.);
				y_hist_range->SetBinError(bi, 0.);
				break;
			}
		}
	}

	// save histograms
	TCanvas *c = new TCanvas();
	c->SetName("y_hist");
	c->SetLogy(1);
	y_hist->Draw();
	y_hist_range->Draw("same");
	c->Write();

	// run "fit" method
	gDirectory = d_top->mkdir("fit");
	DoVerticalAlignmentFit(y_hist_range, results, period);

	// clean up
	delete c;
	delete y_hist;
	delete y_hist_range;

	// continue with "shift" method
	gDirectory = d_top->mkdir("shift");

	//printf("\t\t\t - sorting\n");
	//sort(sample_t.begin(), sample_t.end());
	//sort(sample_b.begin(), sample_b.end());

	// determine shift range
	double s_exp = 2. * c_exp - (y_min_t - y_min_b);
	double s_min = s_exp - 2.5, s_max = s_exp + 2.5;
	double s_step = 0.1;

	printf("\tshift range: %.2E to %.2E, shift step = %.2E\n", s_min, s_max, s_step);

	// result variables
	double bsh_min_diff=0., bsh_prob=0., bsh_mean_diff_sq=0., bsh_hist_chi_sq=0.;

	// find the best match
	TGraph *g_max_diff = new TGraph(); g_max_diff->SetName("g_max_diff");
	TGraph *g_prob = new TGraph(); g_prob->SetName("g_prob");
	TGraph *g_z = new TGraph(); g_z->SetName("g_z");
	TGraph *g_mean_diff_sq = new TGraph(); g_mean_diff_sq->SetName("g_mean_diff_sq");
	TGraph *g_hist_chi_sq = new TGraph(); g_hist_chi_sq->SetName("g_hist_chi_sq");
	double min_diff = 1E100, max_prob = 0.;
	TDirectory *baseDir = gDirectory;

	for (double sh = s_min; sh <= s_max; sh += s_step)
	{
		char buf[100];

		//bool saveExtra = (sh > (1. -0.5*s_step) && sh < (1. + 0.5*s_step));
		bool saveExtra = false;

		if (saveDetails || saveExtra)
		{
			sprintf(buf, "shift = %.3f", sh);
			gDirectory = baseDir->mkdir(buf);
		}

		//printf("\tsh = %.3f mm\n", sh);

		// determine actual boundaries
		double bot_sh = - y_min_b + y_min_t + sh;
		double y_min = max(y_min_t, y_min_b + bot_sh), y_max = min(y_max_t, y_max_b + bot_sh);
		//printf("\t\ty_min = %.3f mm, y_max = %.3f mm\n", y_min, y_max);

		// build increase maps
		map<double, double> m_t;
		double norm_t = 0., N_t = 0.;
		for (unsigned int i = 0; i < sample_t.size(); i++)
		{
			double y = sample_t[i].v;
			double w = sample_t[i].w;
			if (y >= y_min && y < y_max)
			{
				//printf("y1: %E\n", y);
				m_t[y] += w;
				norm_t += w;
				N_t++;
			}
		}

		map<double, double> m_b;
		double norm_b = 0., N_b = 0.;
		for (unsigned int i = 0; i < sample_b.size(); i++)
		{
			double y = sample_b[i].v + bot_sh;
			double w = sample_b[i].w;
			if (y >= y_min && y < y_max) {
				m_b[y] += w;
				norm_b += w;
				N_b++;
			}
		}
		
		//printf("\t\tnorm_t = %u, norm_b = %u\n", norm_t, norm_b);
	
		// build y histograms
		unsigned int N_bins = 100;
		//printf("\t\tN_bins = %u\n", N_bins);

		TH1D *y_hist_t = new TH1D("y_hist_t", "", N_bins, y_min, y_max);
		y_hist_t->Sumw2();
		for (map<double, double>::iterator it = m_t.begin(); it != m_t.end(); ++it)
			y_hist_t->Fill(it->first, it->second);
		y_hist_t->SetLineColor(2);

		TH1D *y_hist_b = new TH1D("y_hist_b", "", N_bins, y_min, y_max);
		y_hist_b->Sumw2();
		for (map<double, double>::iterator it = m_b.begin(); it != m_b.end(); ++it)
			y_hist_b->Fill(it->first, it->second);
		y_hist_b->SetLineColor(4);
		
		// calculate histogram chi^2
		double hist_chi_sq = 0.;
		int hist_n = 0;
		for (int i = 1; i <= y_hist_t->GetNbinsX(); i++)
		{
			double diff = y_hist_t->GetBinContent(i) - y_hist_b->GetBinContent(i);
			double eff_err_sq = pow(y_hist_t->GetBinError(i), 2.) + pow(y_hist_b->GetBinError(i), 2.);
			if (eff_err_sq <= 0)
				continue;
			hist_chi_sq += diff*diff / eff_err_sq;
			hist_n++;
		}

		if (hist_n > 0)
		{
			//hist_chi_sq /= hist_n;
			g_hist_chi_sq->SetPoint(g_hist_chi_sq->GetN(), sh, hist_chi_sq);
		}
		//printf("\t\tmean hist chi^2 = %E, contribution points %u\n", hist_chi_sq, hist_n);
		
		// save debug plots
		if (saveDetails || saveExtra)
		{
			y_hist_t->Write();
			y_hist_b->Write();

			TCanvas *c = new TCanvas();
			c->SetName("y hist cmp");
			y_hist_t->Draw();
			y_hist_b->Draw("sames");
			c->Write();
			delete c;

			// build cumulative graphs
			TGraph *g_y_cumul_t = new TGraph(); g_y_cumul_t->SetLineColor(2);
			TGraph *g_y_cumul_b = new TGraph(); g_y_cumul_b->SetLineColor(4);
			double sum = 0.;
			for (int i = 1; i <= y_hist_t->GetNbinsX(); i++)
			{
				sum += y_hist_t->GetBinContent(i);
				g_y_cumul_t->SetPoint(g_y_cumul_t->GetN(), y_hist_t->GetBinCenter(i), sum);
			}
			sum = 0.;
			for (int i = 1; i <= y_hist_b->GetNbinsX(); i++)
			{
				sum += y_hist_b->GetBinContent(i);
				g_y_cumul_b->SetPoint(g_y_cumul_b->GetN(), y_hist_b->GetBinCenter(i), sum);
			}
			c = new TCanvas();
			c->SetName("y cumul cmp");
			g_y_cumul_t->Draw("al");
			g_y_cumul_b->Draw("l");
			c->Write();

			delete g_y_cumul_t;
			delete g_y_cumul_b;
			delete c;
		}

		// build sum vectors S
		double norm_m = (norm_t + norm_b) / 2.;
		double sum = 0;
		vector<double> S_t;
		S_t.reserve(m_t.size());
		S_t.push_back(0.);
		//TGraph *g_t = new TGraph();
		for (map<double, double>::iterator it = m_t.begin(); it != m_t.end(); ++it)
		{
			sum += it->second;
			S_t.push_back(sum);
			//g_t->SetPoint(g_t->GetN(), it->first, sum);
		}
		
		vector<double> S_b;
		S_b.reserve(m_b.size());
		S_b.push_back(0.);
		//TGraph *g_b = new TGraph();
		sum = 0.;
		for (map<double, double>::iterator it = m_b.begin(); it != m_b.end(); ++it)
		{
			sum += it->second;
			S_b.push_back(sum);
			g_b->SetPoint(g_b->GetN(), it->first, sum);
		}

		/*
		if (saveDetails || saveExtra)
		{
			TCanvas *c = new TCanvas();
			c->SetName("F cmp");
			g_t->SetLineColor(2);
			g_t->Draw("al");
			g_b->SetLineColor(4);
			g_b->Draw("l");
			c->Write();	
			delete c;
		}
		*/

		// build sorted array of all increase points
		vector< pair<double, unsigned int> > ipa;
		map<double, double>::iterator it_t = m_t.begin();
		map<double, double>::iterator it_b = m_b.begin();

		while (it_t != m_t.end() && it_b != m_b.end())
		{
			if (it_t == m_t.end())
			{
				ipa.push_back({it_b->first, 2});
				++it_b;
				continue;
			}

			if (it_b == m_b.end())
			{
				ipa.push_back({it_t->first, 1});
				++it_t;
				continue;
			}

			if (it_t->first < it_b->first)
			{
				ipa.push_back({it_t->first, 1});
				++it_t;
			} else {
				ipa.push_back({it_b->first, 2});
				++it_b;
			}
		}

		// find the maximum vertical difference and sum of squares of vertical differences
		unsigned int idx_t = 0, idx_b = 0;
		double max_diff = 0.;
		double sum_diff_sq = 0.;
		unsigned int n_diff = 0;
		//TGraphErrors *g_F_diff = new TGraphErrors(); g_F_diff->SetName("g_F_diff"); g_F_diff->SetLineColor(6);
		for (vector< pair<double, unsigned int> >::iterator it = ipa.begin(); it != ipa.end(); ++it)
		{
			unsigned int num = it->second;
			//double y = it->first;

			if ((num & 0x1) > 0)
				idx_t++;
			if ((num & 0x2) > 0)
				idx_b++;

			double v_t = S_t[idx_t];
			double v_b = S_b[idx_b];

			double diff = (v_b - v_t) / norm_m;
			max_diff = max(max_diff, fabs(diff));
			n_diff++;

			double F_t = S_t[idx_t] / norm_t;
			double si_sq_t = norm_t * norm_t / N_t * F_t * (1. - F_t);
			double F_b = S_b[idx_b] / norm_b;
			double si_sq_b = norm_b * norm_b / N_b * F_b * (1. - F_b);

			//printf("\ty=%E, F_t=%E, F_b=%E\n", y, F_t, F_b);

			double th = 1E-3;
			if (F_t < th || (1. - F_t) < th)
				continue;
			if (F_b < th || (1. - F_b) < th)
				continue;

			sum_diff_sq += pow(v_b - v_t, 2.) / (si_sq_t + si_sq_b);

			/*
			int idx = g_F_diff->GetN();
			g_F_diff->SetPoint(idx, y, v_b - v_t);
			g_F_diff->SetPointError(idx, 0., sqrt(si_sq_t + si_sq_b));
			//printf("\t\t\tnum=%u, idx_t=%u, idx_b=%u | x=%E | v_t=%E, v_b=%E, diff=%E, max_diff after=%E\n", num, idx_t, idx_b, x, v_t, v_b, diff, max_diff);
			*/
		}

		/*
		if (saveDetails || saveExtra)
			g_F_diff->Write();
		*/

		//double kCorr = sqrt(double(norm_t) * double(norm_b) / (norm_t + norm_b));
		double kCorr = sqrt(double(norm_m) / 2.);

		double z = max_diff * kCorr;
		double prob = TMath::KolmogorovProb(z);

		if (n_diff > 0)
		{
			//double mean_diff_sq = sum_diff_sq / n_diff;
			double mean_diff_sq = sum_diff_sq;
			g_mean_diff_sq->SetPoint(g_mean_diff_sq->GetN(), sh, mean_diff_sq);
		}

		if (norm_t > 0 && norm_b > 0)
		{
			g_prob->SetPoint(g_prob->GetN(), sh, prob);
			g_z->SetPoint(g_z->GetN(), sh, z);
			g_max_diff->SetPoint(g_max_diff->GetN(), sh, max_diff);

			if (min_diff > max_diff)
			{
				min_diff = max_diff;
				bsh_min_diff = sh;
			}
	
			if (max_prob < prob)
			{
				max_prob = prob;
				bsh_prob = sh;
			}
		}

		//printf("\t\tmax_diff = %E, z = %E (corr %E), prob = %E\n", max_diff, z, kCorr, prob);

		// clean
		delete y_hist_t;
		delete y_hist_b;
		//delete g_t;
		//delete g_b;
		//delete g_F_diff;
	}

	// evaluate best shifts
	TF1 *ff = new TF1("ff", "[0] + [1]*x + [2]*x*x");
	
	double fit_margin = 0.5;
	g_mean_diff_sq->Fit(ff, "Q", "", bsh_prob-fit_margin, bsh_prob+fit_margin);
	bsh_mean_diff_sq = - ff->GetParameter(1) / 2. / ff->GetParameter(2);
	double bsh_unc_mean_diff_sq = 1./sqrt(ff->GetParameter(2));

	fit_margin = 0.5;
	g_hist_chi_sq->Fit(ff, "Q", "", bsh_prob-fit_margin, bsh_prob+fit_margin);
	bsh_hist_chi_sq = - ff->GetParameter(1) / 2. / ff->GetParameter(2);
	double bsh_unc_hist_chi_sq = 1./sqrt(ff->GetParameter(2));

	gDirectory = baseDir;
	g_max_diff->Write();
	g_z->Write();
	g_prob->Write();
	g_mean_diff_sq->Write();
	g_hist_chi_sq->Write();

	printf("\tbest shifts: min_diff = %.3f, prop = %.3f, mean_diff_sq = %.3f, hist_chi_sq = %.3f\n",
		bsh_min_diff, bsh_prob, bsh_mean_diff_sq, bsh_hist_chi_sq);

	// evaluate beam positions
	double de_y_min_diff = - (-y_min_b + y_min_t + bsh_min_diff) / 2.;
	double de_y_prob = - (-y_min_b + y_min_t + bsh_prob) / 2.;
	double de_y_mean_diff_sq = - (-y_min_b + y_min_t + bsh_mean_diff_sq) / 2., de_y_unc_mean_diff_sq = bsh_unc_mean_diff_sq / 2.;
	double de_y_hist_chi_sq = - (-y_min_b + y_min_t + bsh_hist_chi_sq) / 2., de_y_unc_hist_chi_sq = bsh_unc_hist_chi_sq / 2.;
	
	printf("\tde_y:\n\t\tmin_diff :     %.0f um\n\t\tprob :         %.0f um\n\t\tmean_diff_sq : (%.0f +- %.0f) um\n\t\thist_chi_sq :  (%.0f +- %.0f) um\n",
		de_y_min_diff*1E3,
		de_y_prob*1E3,
		de_y_mean_diff_sq*1E3, de_y_unc_mean_diff_sq*1E3,
		de_y_hist_chi_sq*1E3, de_y_unc_hist_chi_sq*1E3);

	// make final "shift" result
	const double center = - (de_y_min_diff + de_y_mean_diff_sq) / 2.;
	const double center_unc = max(s_step, de_y_unc_hist_chi_sq);
	printf("\tcenter:\n\t\t(%.0f +- %.0f) um\n", center*1E3, center_unc*1E3);

	results["c_shift"][period] = result(center*1E3, center_unc*1E3);
	
	results["c_min_diff"][period] = result(-de_y_min_diff*1E3, 0.);
	results["c_prob"][period] = result(-de_y_prob*1E3, 0.);
	results["c_mean_diff_sq"][period] = result(-de_y_mean_diff_sq*1E3, de_y_unc_mean_diff_sq*1E3);
	results["c_hist_chi_sq"][period] = result(-de_y_hist_chi_sq*1E3, de_y_unc_hist_chi_sq*1E3);

	// combine "fit" and "shift" methods
	results["c"][period] = result::Combine(results["c_fit"][period], results["c_shift"][period]);

	gDirectory = d_top;
}

//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------

void PrintUsage()
{
	printf("USAGE: program <option> <option>\n");
	printf("OPTIONS:\n");
	printf("    -cfg <file>       config file\n");
	printf("    -dgn <string>     diagonal\n");
}

//----------------------------------------------------------------------------------------------------

int main(int argc, const char **argv)
{
	// defaults
	string cfg_file = "config.py";
	string diagonal_input = "";

	// parse command line
	for (int argi = 1; (argi < argc) && (cl_error == 0); ++argi)
	{
		if (strcmp(argv[argi], "-h") == 0 || strcmp(argv[argi], "--help") == 0)
		{
			cl_error = 1;
			continue;
		}

		if (TestStringParameter(argc, argv, argi, "-cfg", cfg_file)) continue;
		if (TestStringParameter(argc, argv, argi, "-dgn", diagonal_input)) continue;

		printf("ERROR: unknown option '%s'.\n", argv[argi]);
		cl_error = 1;
	}

	if (cl_error)
	{
		PrintUsage();
		return 1;
	}

	// run initialisation
	if (Init(cfg_file, diagonal_input) != 0)
		return 2;

	// compatibility check
	if (cfg.diagonal != dCombined)
		return rcIncompatibleDiagonal;

	// print settings
	PrintConfiguration();

	// input files
	TFile *inF_45b = new TFile("distributions_45b_56t.root");
	TFile *inF_45t = new TFile("distributions_45t_56b.root");
	
	// prepare output
	TFile *outF = new TFile("alignment.root", "recreate");

	// initialisations
	char buf[1000];

	f_gauss = new TF1("f_gauss", "[0] * exp(- (x-[1])*(x-[1]) / 2 / [2]/[2])");

	// configuration
	vector<string> units;
	vector<double> deYExp;	// expected value of de_y in mm
	units.push_back("L_2_F"); deYExp.push_back(+0.);
	units.push_back("L_1_F"); deYExp.push_back(-0.2);

	units.push_back("R_1_F"); deYExp.push_back(-0.1);
	units.push_back("R_2_F"); deYExp.push_back(-0.);

	// get list of periods
	vector<signed int> periods;
	TDirectory *baseDir = (TDirectory *) inF_45b->Get("alignment");
	TIter next(baseDir->GetListOfKeys());
	TObject *o;
	while ((o = next()))
	{
		TKey *k = (TKey *) o;
		if (k->IsFolder())
			periods.push_back(atoi(k->GetName()));
	}

	// run alignment algorithms for all units and all periods
	map<string, map<string, map<signed int, result> > > results;

	for (unsigned int pi = 0; pi < periods.size(); pi++)
	{
		printf("\n\n************************************************** period %i **************************************************\n", periods[pi]);
		sprintf(buf, "period %i", periods[pi]);
		TDirectory *perDir = outF->mkdir(buf);

		for (unsigned int ui = 0; ui < units.size(); ui++)
		{
			printf("\n---------- unit %s ----------\n\n", units[ui].c_str());
	
			sprintf(buf, "alignment/%i/", periods[pi]);

			TGraph *g_t = (TGraph *) inF_45t->Get((string(buf)+"g_y_"+units[ui]+"_vs_x_"+units[ui]+"_sel").c_str());
			TGraph *gw_t = (TGraph *) inF_45t->Get((string(buf)+"g_w_vs_timestamp_sel").c_str());
			TGraph *g_b = (TGraph *) inF_45b->Get((string(buf)+"g_y_"+units[ui]+"_vs_x_"+units[ui]+"_sel").c_str());
			TGraph *gw_b = (TGraph *) inF_45b->Get((string(buf)+"g_w_vs_timestamp_sel").c_str());
			
			if (g_t == nullptr || gw_t == nullptr || g_b == nullptr || gw_b == nullptr)
			{
				printf("ERROR: missing input g_t = %p, gw_t = %p, g_b = %p, gw_b = %p\n", g_t, gw_t, g_b, gw_b);
				continue;
			}
	
			if (units[ui][0] == 'R')
			{
				swap(g_t, g_b);
				swap(gw_t, gw_b);
			}

			unsigned int effective_entries = g_t->GetN() + g_b->GetN();
			if (effective_entries < 200)
			{
				printf("too few entries: %u, skipping.\n", effective_entries);
				continue;
			}

			// get y ranges
			const Analysis::AlignmentYRange &r = anal.alignmentYRanges[units[ui]];
	
			sprintf(buf, "unit %s", units[ui].c_str());
			TDirectory *unitDir = perDir->mkdir(buf);
			
			gDirectory = unitDir->mkdir("horizontal");
			DoHorizontalAlignment(g_t, g_b, r, units[ui], results[units[ui]], periods[pi]);
			
			gDirectory = unitDir->mkdir("vertical");
			DoVerticalAlignment(g_t, gw_t, g_b, gw_b, r, units[ui], deYExp[ui], results[units[ui]], periods[pi]);
		}
	}

	TDirectory *glDir = outF->mkdir("global");
	for (map<string, map<string, map<signed int, result> > >::iterator uit = results.begin(); uit != results.end(); ++uit)
	{
		gDirectory = glDir->mkdir(uit->first.c_str());

		for (map<string, map<signed int, result> >::iterator qit = uit->second.begin(); qit != uit->second.end(); ++qit)
		{
			TGraphErrors *g = new TGraphErrors();
			g->SetName(qit->first.c_str());

			for (map<signed int, result>::iterator pit = qit->second.begin(); pit != qit->second.end(); ++pit)
			{
				double t = (pit->first + 0.5) * anal.alignment_ts + anal.alignment_t0;
				double et = 0.5 * anal.alignment_ts;

				int idx = g->GetN();
				g->SetPoint(idx, t, pit->second.value);
				g->SetPointError(idx, et, pit->second.uncertainty);
			}

			g->Write();
		}

	}

	delete outF;
	return 0;
}
