#ifndef _numerical_integration_hh_
#define _numerical_integration_hh_

#include "TComplex.h"

#include <gsl/gsl_integration.h>
#include <gsl/gsl_errno.h>

//----------------------------------------------------------------------------------------------------

typedef double (* RealFunction)(double x, double *par, const void *obj);

typedef TComplex (* ComplexFunction)(double x, double *par, const void *obj);

//----------------------------------------------------------------------------------------------------

struct RealIntegPar
{
	RealFunction fcn;
	double *parameters;
	const void *object;

	RealIntegPar(RealFunction _f, double *_p, const void *_o) : fcn(_f), parameters(_p), object(_o) {}
};

//----------------------------------------------------------------------------------------------------

extern double RealIntegFcn(double x, void *vpar);

extern double RealIntegrate(RealFunction fcn, double *par, const void *object,
	double from, double to,
	double abs_err, double rel_err,
	unsigned long work_space_size, gsl_integration_workspace *work_space, const char *errorLabel="");

//----------------------------------------------------------------------------------------------------

struct OneCompIntegPar
{
	ComplexFunction fcn;
	double *parameters;
	const void *object;
	enum Part { pUndefined, pReal, pImaginary } part;

	OneCompIntegPar(ComplexFunction _f, double *_p, const void *_o, Part _pa = pUndefined) : fcn(_f), parameters(_p), object(_o), part(_pa) {}
};

//----------------------------------------------------------------------------------------------------

extern double OneCompIntegFcn(double x, void *par);

extern TComplex ComplexIntegrate(ComplexFunction fcn, double *par, const void *object,
	double from, double to,
	double abs_err, double rel_err,
	unsigned long work_space_size, gsl_integration_workspace *work_space, const char* errorLabel="");

#endif