import patterns;

//----------------------------------------------------------------------------------------------------

transform swToHours = scale(1/3600, 1);
transform swToMinutes = scale(1/60, 1);

//----------------------------------------------------------------------------------------------------

int runs[];
string run_datasets[];
real run_ts_from[], run_ts_to[];
pen run_colors[];

void AddRun(string ds, int r, real f, real t, pen p = yellow + opacity(0.3))
{
	run_datasets.push(ds);
	runs.push(r);
	run_ts_from.push(f);
	run_ts_to.push(t);
	run_colors.push(p);
}

//----------------------------------------------------------------------------------------------------

string excl_datasets[];
real excl_ts_from[], excl_ts_to[];

void AddExcl(string ds, real f, real t)
{
	excl_datasets.push(ds);
	excl_ts_from.push(f);
	excl_ts_to.push(t);
}

pen p_excl = black + opacity(0.4);

// fill 7301
AddRun("7301", 324575, 58711, 60559);
AddRun("7301", 324576, 61090, 62660);

// fill 7302
AddRun("7302", 324578, 69013, 73405);
AddRun("7302", 324579, 73563, 75862);
AddRun("7302", 324580, 75983, 76430);
AddRun("7302", 324581, 76491, 76799);

// in hours
real time_min = 0;
real time_max = 5;

//----------------------------------------------------------------------------------------------------

void DrawBands(string filter="", string bands="ds", string labels="ds", real y_min=0, real y_max=0)
{
	// determine min and max of x for the selection
	real x_min = 1e100, x_max = -1e100;
	for (int i : runs.keys)
	{
		if (run_datasets[i] == filter)
		{
			x_min = min(x_min, run_ts_from[i]/3600);
			x_max = max(x_max, run_ts_to[i]/3600);
		}
	}

	// draw bands
	if (bands == "ds")
	{
		pen p = yellow+opacity(0.3);

		filldraw((x_min, y_min)--(x_max, y_min)--(x_max, y_max)--(x_min, y_max)--cycle, p, nullpen);
	}

	if (bands == "run")
	{
		for (int i : runs.keys)
		{
			if (run_datasets[i] != filter)
				continue;

			real x_min = run_ts_from[i]/3600, x_max = run_ts_to[i]/3600;
			filldraw((x_min, y_min)--(x_max, y_min)--(x_max, y_max)--(x_min, y_max)--cycle, run_colors[i], nullpen);
		}
	}

	// draw exclusions
	{
		for (int i : excl_datasets.keys)
		{
			if (excl_datasets[i] == filter)
			{
				real x_min = excl_ts_from[i]/3600, x_max = excl_ts_to[i]/3600;
				filldraw((x_min, y_min)--(x_max, y_min)--(x_max, y_max)--(x_min, y_max)--cycle, p_excl, nullpen);
			}
		}
	}

	// draw labels
	if (labels == "ds")
	{
		label("{\SmallerFonts " + filter + "}", ((x_min + x_max)/2, y_max), S);
	}

	if (labels == "run")
	{
		for (int i : runs.keys)
		{
			if (run_datasets[i] != filter)
				continue;

			real x_min = run_ts_from[i]/3600, x_max = run_ts_to[i]/3600;
			label(format("{\SmallerFonts %u}", runs[i]), ((x_min + x_max)/2, y_max), S);
		}
	}
}

//----------------------------------------------------------------------------------------------------

void DrawFillBands(string filter="", real y_min=0, real y_max=0, bool details=true)
{
	DrawBands(filter, bands="ds", labels=(details) ? "ds" : "", y_min, y_max);
}

//----------------------------------------------------------------------------------------------------

void DrawRunBands(string filter="", real y_min=0, real y_max=0, bool details=true)
{
	DrawBands(filter, bands="run", labels=(details) ? "run" : "", y_min, y_max);
}

//----------------------------------------------------------------------------------------------------

void DrawRunBoundaries(string filter="")
{
	for (int i : runs.keys)
	{
		if (filter != "")
			if (run_datasets[i] != filter)
				continue;

		yaxis(XEquals(run_ts_from[i]/3600, false), dashed);
		yaxis(XEquals(run_ts_to[i]/3600, false), dashed);
	}
}
