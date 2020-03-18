all: single_side

single_side: single_side.cc ../common_definitions.h ../common_parameters.h apertures.h aperture_list.h
	g++ --std=c++11 `root-config --libs` -lMinuit `root-config --cflags` -O3 -g -Wall -Wextra \
		single_side.cc -o single_side
