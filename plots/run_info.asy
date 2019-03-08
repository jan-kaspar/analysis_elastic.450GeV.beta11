//----------------------------------------------------------------------------------------------------

transform swToHours = scale(1/3600, 1);
transform swToMinutes = scale(1/60, 1);

//----------------------------------------------------------------------------------------------------

string run_datasets[];
int runs[];
real ts_from[], ts_to[];
pen colors[];

void AddRun(string ds, int r, real f, real t, pen p = yellow)
{
	run_datasets.push(ds);
	runs.push(r);
	ts_from.push(f);
	ts_to.push(t);
	colors.push(p);
}

AddRun("7301", 324575, 58711, 60559);
AddRun("7301", 324576, 61090, 62660);

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
			x_min = min(x_min, ts_from[i]/3600);
			x_max = max(x_max, ts_to[i]/3600);
		}
	}

	// draw bands
	pen p = yellow+opacity(0.3);

	if (bands == "ds")
	{
		filldraw((x_min, y_min)--(x_max, y_min)--(x_max, y_max)--(x_min, y_max)--cycle, p, nullpen);
	}

	if (bands == "run")
	{
		for (int i : runs.keys)
		{
			if (run_datasets[i] != filter)
				continue;

			real x_min = ts_from[i]/3600, x_max = ts_to[i]/3600;
			filldraw((x_min, y_min)--(x_max, y_min)--(x_max, y_max)--(x_min, y_max)--cycle, p, nullpen);
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

			real x_min = ts_from[i]/3600, x_max = ts_to[i]/3600;
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

		yaxis(XEquals(ts_from[i]/3600, false), dashed);
		yaxis(XEquals(ts_to[i]/3600, false), dashed);
	}
}
