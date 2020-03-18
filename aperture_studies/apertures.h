struct Optics
{
	double v_x, L_x, v_y, L_y;
};

//----------------------------------------------------------------------------------------------------

struct Aperture
{
	std::string type;
	double p1, p2, p3, p4;
};

//----------------------------------------------------------------------------------------------------

struct Element
{
	std::string name;
	double s;
	Aperture aperture;
	Optics optics;
};

//----------------------------------------------------------------------------------------------------

vector<unsigned int> CheckApertures(const vector<Element> &elements, double th_x, double th_y, double vtx_x, double vtx_y)
{
	vector<unsigned int> indices;

	for (unsigned int ei = 0; ei < elements.size(); ++ei)
	{
		const auto &e = elements[ei];

		const double x = e.optics.L_x * th_x + e.optics.v_x * vtx_x;
		const double y = e.optics.L_y * th_y + e.optics.v_y * vtx_y;

		//printf("%10s: th_x = %.2E, vtx_x = %.2E -> x = %.2E\n", e.name.c_str(), th_x, vtx_x, x);

		bool inAperture = false;

		if (e.aperture.type == "RECTELLIPSE")
		{
			const double r2eff = pow(x/e.aperture.p3, 2) + pow(y/e.aperture.p4, 2);
			inAperture = (fabs(x) < e.aperture.p1 && fabs(y) < e.aperture.p2 && r2eff < 1.);
		}

		if (!inAperture)
			indices.push_back(ei);
	}

	return indices;
}
