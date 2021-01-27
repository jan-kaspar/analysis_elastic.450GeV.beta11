INCS = -I$(GSL_DIR)/include
LIBS = -L$(GSL_DIR)/lib -lgsl -lgslcblas -lm

all: algorithm_test
#	validation_vs_old validation_gaussian_approx mc_resolution_details validation_with_mc

algorithm_test: algorithm_test.cc ../common_definitions.h ../common_algorithms.h ../AcceptanceCalculator.h
	g++ --std=c++11 `root-config --libs` -lMinuit `root-config --cflags` -O3 -g -Wall -Wextra -I ../ $(INCS) $(LIBS) \
		algorithm_test.cc -o algorithm_test

#validation_vs_old: validation_vs_old.cc parameters_old.h ../common_definitions.h ../common_algorithms.h ../AcceptanceCalculator.h
#	g++ --std=c++11 `root-config --libs` -lMinuit `root-config --cflags` -O3 -g -Wall -Wextra \
#		-I ../ $(INCS) \
#		$(LIBS) \
#		validation_vs_old.cc -o validation_vs_old
#
#validation_gaussian_approx: validation_gaussian_approx.cc parameters_mc.h ../common_definitions.h ../common_algorithms.h ../AcceptanceCalculator.h
#	g++ --std=c++11 `root-config --libs` -lMinuit `root-config --cflags` -O3 -g -Wall -Wextra \
#		-I ../ $(INCS) \
#		$(LIBS) \
#		validation_gaussian_approx.cc -o validation_gaussian_approx
#
#mc_resolution_details: mc_resolution_details.cc parameters_mc.h ../common_definitions.h ../common_algorithms.h ../AcceptanceCalculator.h
#	g++ --std=c++11 `root-config --libs` -lMinuit `root-config --cflags` -O3 -g -Wall -Wextra \
#		-I ../ $(INCS) \
#		$(LIBS) \
#		mc_resolution_details.cc -o mc_resolution_details
#
#validation_with_mc: validation_with_mc.cc parameters_mc.h ../common_definitions.h ../common_algorithms.h ../AcceptanceCalculator.h
#	g++ --std=c++11 `root-config --libs` -lMinuit `root-config --cflags` -O3 -g -Wall -Wextra \
#		-I ../ $(INCS) \
#		$(LIBS) \
#		validation_with_mc.cc -o validation_with_mc
