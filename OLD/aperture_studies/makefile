all: single_arm double_arm

single_arm: single_arm.cc ../common_definitions.h ../common_parameters.h apertures.h aperture_list.h
	g++ --std=c++11 `root-config --libs` -lMinuit `root-config --cflags` -O3 -g -Wall -Wextra \
		single_arm.cc -o single_arm

double_arm: double_arm.cc ../common_definitions.h ../common_parameters.h apertures.h aperture_list.h
	g++ --std=c++11 `root-config --libs` -lMinuit `root-config --cflags` -O3 -g -Wall -Wextra \
		double_arm.cc -o double_arm
