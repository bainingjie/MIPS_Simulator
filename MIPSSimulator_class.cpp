#include <iostream>
#include <cmath>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>

using namespace std;

//class for the MIPS Simulator
class MIPSSimulator
{
    private:
        string Registers[32]; //array to store names of registers
        int32_t RegisterValues[32]; //array to store values of registers

        int32_t Mode; //to store the Mode of execution
        
        vector<string> InputProgram; //to store the input program, element stands for

        int32_t NumberOfInstructions; //to store the number of lines in the program
        string current_instruction; 
        int32_t ProgramCounter; 
        int32_t r[3]; //to store the id of operand or constant

        // void add();
        void addi();
        // void sub();
        // void mul();
        // void andf();
        // void andi();
        // void orf();
        // void ori();
        // void nor();
        // void slt();
        // void slti();
        // void lw();
        // void sw();
        // void beq();
        // void bne();
        // void j();


        void ReadInstruction(int32_t line);
        int32_t ParseInstruction();
        // void ReportError();
        void ExecuteInstruction(int32_t instruction);

    public:
        MIPSSimulator(int32_t mode, string fileName);
        void Execute();
        // void displayState();//display state at end
};



/*
constructor to 
initialize values of registers&instruction set
read inputfile and initialize InputProgram object
 */

MIPSSimulator::MIPSSimulator(int32_t mode, string fileName)
{
    NumberOfInstructions=0;
    ProgramCounter=0;
    string tempRegisters[]={"zero","at","v0","v1","a0","a1","a2","a3","t0","t1","t2","t3","t4","t5","t6",
    "t7","s0","s1","s2","s3","s4","s5","s6","s7","t8","t9","k0","k1","gp","sp","s8","ra"}; //names of registers
    for(int32_t i=0;i<32;i++)
    {
        Registers[i]=tempRegisters[i];
    }
    for(int32_t i=0;i<32;i++)
    {
        RegisterValues[i]=0; 
    }

    Mode = mode; 
    ifstream InputFile;
    InputFile.open(fileName.c_str(),ios::in); 
    if(!InputFile) 
    {
        cout<<"Error: File does not exist or could not be opened"<<endl;
        exit(1);
    }

    string tempString; 
    while(getline(InputFile,tempString)) 
    {
        NumberOfInstructions++;
        InputProgram.push_back(tempString); 
    }
    InputFile.close();
}


//function to run the simulator
void MIPSSimulator::Execute()
{
    char test = getchar(); //to remove effect of pressing enter key while starting
    cout<<"getchar() in Execute: "<<test <<endl;
    while(ProgramCounter<NumberOfInstructions) 
    {

        ReadInstruction(ProgramCounter); //set current_instruction
        int32_t instruction_op = ParseInstruction(); // return op , populate values in r[]
        ExecuteInstruction(instruction_op); //call appropriate operation function based on the op and increment program counter
    }
    cout<<endl<<"Execution completed successfully"<<endl<<endl;
    cout<<"r[1]:"<<RegisterValues[r[1]]<<endl;
}


/*set current_instruction*/
void MIPSSimulator::ReadInstruction(int32_t program_counter)
{
    current_instruction=InputProgram[program_counter];
}

/*call appropriate operation function based on the op and increment program counter*/
void MIPSSimulator::ExecuteInstruction(int32_t instruction_op)
{
    switch(instruction_op) 
    {
        // case 0:
        //     add();
        //     break;
        // case 1:
        //     sub();
        //     break;
        // case 2:
        //     mul();
        //     break;
        // case 3:
        //     andf();
        //     break;
        // case 4:
        //     orf();
        //     break;
        // case 5:
        //     nor();
        //     break;
        // case 6:
        //     slt();
        //     break;
        case 8:
            addi();
            ProgramCounter++;
            break;
        // case 8:
        //     andi();
        //     break;
        // case 9:
        //     ori();
        //     break;
        // case 10:
        //     slti();
        //     break;
        // case 11:
        //     lw();
        //     break;
        // case 12:
        //     sw();
        //     break;
        // case 13:
        //     beq();
        //     break;
        // case 14:
        //     bne();
        //     break;
        // case 15:
        //     j();
        //     break;
        // case 16:
        //     halt();
        //     break;
        // case -2: //if instruction containing label, ignore
        //     break;
        default:
            cout<<"Error: Invalid instruction_op received"<<endl;
            // ReportError();
    }
}

// opcode   rs  rt  IMM
void MIPSSimulator::addi()
{
    RegisterValues[r[1]]=RegisterValues[r[0]]+r[2];
}

// return op , populate values in r[]
int32_t MIPSSimulator::ParseInstruction()
{
    if(current_instruction.size()!=32) 
    {
        cout<<"Error: Invalid instruction"<<endl;
        // ReportError();
    }

    int32_t OperationID = stoi(current_instruction.substr(0,6).c_str(), nullptr, 2);  

    if(OperationID == 8){
    r[0] = stoi(current_instruction.substr(6,5).c_str(), nullptr, 2);
    r[1] = stoi(current_instruction.substr(11,5).c_str(), nullptr, 2);
    r[2] = stoi(current_instruction.substr(16,16).c_str(), nullptr, 2);
    }
    else{
        cout<<"Error: Invalid OperationID"<<endl;
        // ReportError();   
    }
    return OperationID;
}

int main()
{
    string path;
    int32_t mode;
    cout<<endl<<"MIPS Simulator - team7"<<endl<<endl;
    cout<<"Two modes are available:"<<endl<<endl<<"1. Step by Step Mode - View state after each instruction"<<endl<<"2. Execution Mode - View state after end of execution"<<endl<<endl;
    cout<<"Please enter the relative path of the input file and mode "<<endl;
    cin>>path>>mode;

    if(mode!=1 && mode!=2) 
    {
        cout<<"Error: Invalid Mode"<<endl;
        return 1;
    }

    MIPSSimulator simulator(mode-1,path); //create and initialize simulator
    simulator.Execute(); //execute simulator
    return 0;
}
