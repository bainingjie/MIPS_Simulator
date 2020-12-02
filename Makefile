all: MIPSSimulator_class.cpp fpu_imitate.cpp fpu_imitate.h
	g++ MIPSSimulator_class.cpp fpu_imitate.cpp -o simulator_7 

clean:
	rm simulator_7 print*