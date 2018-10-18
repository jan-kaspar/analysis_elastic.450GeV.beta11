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

//AddRun("DS-323880", 323880, 44998, 51215);
//AddRun("DS-323892", 323892, 54686, 56376);
AddRun("DS-323893", 323893, 56448, 59057);
AddRun("DS-323899", 323899, 62655, 64763);
AddRun("DS-323907", 323907, 67805, 70714);
AddRun("DS-323919", 323919, 72030, 74553);
AddRun("DS-323932", 323932, 78947, 79490);
AddRun("DS-323933", 323933, 79730, 83781);
AddRun("DS-323934", 323934, 84355, 85178);

// in hours
real time_min = 0;
real time_max = 5;

//----------------------------------------------------------------------------------------------------

void DrawRunBands(string ds_filter="", real y_min=0, real y_max=0, bool details=true)
{
	for (int i : runs.keys)
	{
		if (ds_filter != "")
			if (run_datasets[i] != ds_filter)
				continue;

		//yaxis(XEquals(ts_from[i]/3600, false), dashed);
		//yaxis(XEquals(ts_to[i]/3600, false), dashed);
		real x_min = ts_from[i]/3600, x_max = ts_to[i]/3600;

		pen p = (details) ? colors[i]+opacity(0.3) : yellow+opacity(0.3);
		filldraw((x_min, y_min)--(x_max, y_min)--(x_max, y_max)--(x_min, y_max)--cycle, p, nullpen);

		if (details)
		{
			label(format("{\SmallerFonts %u}", runs[i]), ((x_min + x_max)/2, y_max), S);
			//label(run_datasets[i], ((x_min + x_max)/2, y_max), S);
		}
	}
}

//----------------------------------------------------------------------------------------------------

void DrawRunBoundaries(string ds_filter="")
{
	for (int i : runs.keys)
	{
		if (ds_filter != "")
			if (run_datasets[i] != ds_filter)
				continue;

		yaxis(XEquals(ts_from[i]/3600, false), dashed);
		yaxis(XEquals(ts_to[i]/3600, false), dashed);
	}
}
