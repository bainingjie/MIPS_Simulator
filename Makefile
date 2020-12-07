all: MIPSSimulator_class.cpp fpu_imitate.cpp fpu_imitate.h
	g++ -std=c++11 MIPSSimulator_class_yama.cpp fpu_imitate.cpp -o simulator_7 
clean:
	rm simulator_7 print* result.ppm