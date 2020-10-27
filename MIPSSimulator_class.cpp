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

        int32_t Memory[1024*100];// memory

        int32_t Mode; //to store the Mode of execution
        
        vector<string> InputProgram; //to store the input program, element stands for

        int32_t NumberOfInstructions; //to store the number of lines in the program
        string current_instruction; 
        int32_t ProgramCounter; 
        int32_t r[3]; //to store the id of operand or constant


        int32_t Instruction_op; 
        int32_t Instruction_funct;

        void add();
        void addi();
        void jr();
        void bne();
        void lw();
        void sw();
        
        // void sub();
        // void mul();
        // void andf();
        // void andi();
        // void orf();
        // void ori();
        // void nor();

        void slt();
        void slti();
        void beq();

        void j();
        void jal();


        void ReadInstruction(int32_t line);
        void ParseInstruction();
        // void ReportError();
        void ExecuteInstruction();
        void PrintRegister();

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
    for(int32_t i=0;i<1024*4;i++)
    {
        Memory[i]=0; 
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

void MIPSSimulator::PrintRegister()
{
    // for (int i = 0; i <= 31; i++){
    //     cout<<Registers[i]<<": "<<RegisterValues[i]<<endl;
    // }
    cout<<Registers[2]<<": "<<RegisterValues[2]<<endl;
    cout<<Registers[4]<<": "<<RegisterValues[4]<<endl;
    cout<<Registers[16]<<": "<<RegisterValues[16]<<endl;
    cout<<Registers[29]<<": "<<RegisterValues[29]<<endl;
    cout<<Registers[31]<<": "<<RegisterValues[31]<<endl;

    for (int i = 4070; i<=4096; i++){
        cout<<"Memory"<<i<<"  :"<<Memory[i]<<endl;
    }
}



/*set current_instruction*/
void MIPSSimulator::ReadInstruction(int32_t program_counter)
{
    current_instruction=InputProgram[program_counter];
}

/*call appropriate operation function based on the op and increment program counter*/
void MIPSSimulator::ExecuteInstruction()
{
    cout<<Instruction_op<<endl;
    switch(Instruction_op) 
    {
        //R-type
        case 0: 
            if(Instruction_funct == 32){
                add();
                ProgramCounter++;
            }else if (Instruction_funct == 8){
                jr();
            }else if (Instruction_funct == 42){
                slt();
                ProgramCounter++;
            }else{
                ProgramCounter++;
            }
            break;

        // I-type
        case 5: 
            bne();
            ProgramCounter++;
            break;
        case 35:
            lw();
            ProgramCounter++;
            break;
        case 43:
            sw();
            ProgramCounter++;
            break;
        case 8: 
            addi();
            ProgramCounter++;
            break;
        //J type
        case 2: 
            j();
            // ProgramCounter++;
            break;
        case 3: 
            jal();
            // ProgramCounter++;
            break;

        // case 8:
        //     andi();
        //     break;
        // case 9:
        //     ori();
        //     break;
        case 10:
            slti();
            ProgramCounter++;
            break;

        case 4:
            beq();
            ProgramCounter++;
            break;


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

// R-type : opcode   rs  rt  rd
void MIPSSimulator::add()
{
    RegisterValues[r[2]]=RegisterValues[r[0]]+RegisterValues[r[1]];
}
void MIPSSimulator::jr()
{
    ProgramCounter = RegisterValues[r[0]];
}

void MIPSSimulator::slt()
{
    if(RegisterValues[r[0]]<RegisterValues[r[1]]){
        RegisterValues[r[2]] =1;
    }else{
        RegisterValues[r[2]] = 0;
    }
}
// I-type : opcode   rs  rt  IMM
void MIPSSimulator::addi()
{
    RegisterValues[r[1]]=RegisterValues[r[0]]+r[2];
}

void MIPSSimulator::slti()
{
    if (RegisterValues[r[0]]<r[2]){
        RegisterValues[r[1]]=1;
    }else{
        RegisterValues[r[1]]=0;
    }
    
}
void MIPSSimulator::bne()
{
    if(RegisterValues[r[0]] != RegisterValues[r[1]]){
        ProgramCounter += r[2];
        // ProgramCounter += r[2]<<2 ;
        // int temp = r[2] << 2;
        // cout<<"bne jump: "<< temp <<endl;
    }
}

void MIPSSimulator::beq()
{
    if(RegisterValues[r[0]] == RegisterValues[r[1]]){
        ProgramCounter += r[2];
        // ProgramCounter += r[2]<<2 ;
    }
}

void MIPSSimulator::lw()
{
    RegisterValues[r[1]]= Memory[r[2]+RegisterValues[r[0]]];
}

void MIPSSimulator::sw()
{
    cout<<"****sw :  "<<(RegisterValues[r[1]]+r[2])<<"   "<< RegisterValues[r[0]]<<endl;
    Memory[RegisterValues[r[0]]+r[2]] =  RegisterValues[r[1]];
}

// J type
void MIPSSimulator::j()
{
    ProgramCounter = r[0];
    // ProgramCounter = r[0]<<2;
}

void MIPSSimulator::jal()
{
    RegisterValues[31] = ProgramCounter + 1;

    ProgramCounter = r[0];
    // ProgramCounter = r[0] <<2;

}


// return op , populate values in r[]
void MIPSSimulator::ParseInstruction()
{
    if(current_instruction.size()!=32) 
    {
        cout<<"Error: Invalid instruction"<<endl;
        // ReportError();
    }

    Instruction_op = stoi(current_instruction.substr(0,6).c_str(), nullptr, 2);  
    Instruction_funct = stoi(current_instruction.substr(26,6).c_str(), nullptr, 2);  

    // I-type
    if(Instruction_op == 8 || Instruction_op == 5 || Instruction_op == 35 || Instruction_op == 43||Instruction_op == 4 ||Instruction_op == 10){
        r[0] = stoi(current_instruction.substr(6,5).c_str(), nullptr, 2);
        r[1] = stoi(current_instruction.substr(11,5).c_str(), nullptr, 2);
        // r[2] = stoi(current_instruction.substr(16,16).c_str(), nullptr, 2);


        if(current_instruction[16] == '1'){
            char temp_imm[17];
            strcpy(temp_imm,current_instruction.substr(16,16).c_str());
            for (int i = 0; i<16; i++){
                if(temp_imm[i]=='1'){temp_imm[i]='0';}
                else{temp_imm[i]='1';}
            }
            r[2] = stoi(temp_imm, nullptr, 2);
            r[2] ++;
            r[2] = 0-r[2];
            cout<<"r[2]:"<<r[2]<<endl; 
        }else{
            r[2] = stoi(current_instruction.substr(16,16).c_str(), nullptr, 2);
        }
    }
    // R-type
    else if(Instruction_op == 0){
        r[0] = stoi(current_instruction.substr(6,5).c_str(), nullptr, 2);
        r[1] = stoi(current_instruction.substr(11,5).c_str(), nullptr, 2);
        r[2] = stoi(current_instruction.substr(16,5).c_str(), nullptr, 2);
    // J-type
    }else if (Instruction_op == 2 ||Instruction_op == 3){
        r[0] = stoi(current_instruction.substr(6,26).c_str(), nullptr, 2);
        // cout<<"r[0]: "<<r[0]<<endl;
    }else{
        cout<<"Error: Invalid Instruction_op"<<endl;
        // ReportError();   
    }
}

//function to run the simulator
void MIPSSimulator::Execute()
{
    char test = getchar(); //to remove effect of pressing enter key while starting
    cout<<"getchar() in Execute: "<<test <<endl;
    

    while(ProgramCounter<NumberOfInstructions) 
    // while(ProgramCounter<NumberOfInstructions) 
    {   
        if(mode == 0){
            cout<<"PC before: "<<ProgramCounter<<endl;
        }
        // if (ProgramCounter%4 == 0){print_flag = 1;restr ++;}
        ReadInstruction(ProgramCounter); //set current_instruction
        ParseInstruction(); // return op , populate values in r[]
        ExecuteInstruction(); //call appropriate operation function based on the op and increment program counter
        
        // if(print_flag == 1){PrintRegister(); }
        if(mode == 0){
            PrintRegister(); 
            cout<<"PC after: "<<ProgramCounter<<endl;
            cout<<"************ "<<endl;
        }
        if(mode == 1){
            PrintRegister(); 
        }

    }
    // cout<<endl<<"Execution completed successfully"<<endl<<endl;
    // cout<<"$v0:"<<RegisterValues[2]<<endl;
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
