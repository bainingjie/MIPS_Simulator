#include <iostream>
#include <cmath>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>

using namespace std;

//class for the MIPS Simulator

void print_int_as_32bits (int num){
    for (int i = 31; i>=0; i--){
        printf("%d",(num>>i) & 1);
    }
    // puts("");
}

typedef union { 
    float f; 
    int32_t i;
} myfloat; 

class MIPSSimulator
{
    private: 
        string Registers[32]; //array to store names of registers
        int32_t RegisterValues[32]; //array to store values of CPU registers
        float FPURegisterValues[32];//array to store values of FPU registers
        int32_t Memory[1024*100];// memory
        int limit_of_exec;
        int32_t Mode; //to store the Mode of execution
        
        vector<string> InputProgram; //to store the input program, element stands for

        int32_t NumberOfInstructions; //to store the number of lines in the program
        string current_instruction; 
        int32_t ProgramCounter; 
        int32_t r[4]; //to store the id of operand or constant


        int32_t Instruction_op; 
        int32_t Instruction_funct;

        void add();
        void addi();
        void jr();
        void bne();
        void lw();
        void sw();

        void sub();
        void sll();
        // void mul();
        // void andf();
        // void andi();
        // void orf();

        // void nor();

        void slt();
        void slti();
        void beq();

        void j();
        void jal();

        void lui();
        void ori();


        void fadd();
        void fsub();
        void fmul();
        void fdiv();
        void fsqrt();
        void fless();

        void mtc1();

        void lwc1();
        void swc1();
        void ceqs();
        
        void ReadInstruction(int32_t line);
        void ParseInstruction();
        // void ReportError();
        void ExecuteInstruction();
        void PrintRegister();

    public:
        MIPSSimulator(int32_t mode, string fileName, int32_t num_input);
        void Execute();
        // void displayState();//display state at end
};



/*
constructor to 
initialize values of registers&instruction set
read inputfile and initialize InputProgram object
 */

MIPSSimulator::MIPSSimulator(int32_t mode, string fileName, int32_t num_input)
{
    NumberOfInstructions=0;
    ProgramCounter=0;
    string tempRegisters[]={"zero","at","v0","v1","a0","a1","a2","a3","t0","t1","t2","t3","t4","t5","t6",
    "t7","s0","s1","s2","s3","s4","s5","s6","s7","t8","t9","k0","k1","gp","sp","s8","ra"}; //names of registers
    limit_of_exec = num_input;

    for(int32_t i=0;i<32;i++)
    {
        Registers[i]=tempRegisters[i];
    }
    for(int32_t i=0;i<32;i++)
    {
        RegisterValues[i]=0; 
        FPURegisterValues[i] = 0.0;
    }
    for(int32_t i=0;i<1024*4;i++)
    {
        Memory[i]=0; 
    }
    Mode = 0; 
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
    for (int i = 0; i <= 31; i++){
        cout<<Registers[i]<<": "<<RegisterValues[i]<<" | ";
        
        /*print as binary*/ 
        // cout<<Registers[i]<<": ";
        // print_int_as_32bits(RegisterValues[i]);
        // cout<<" | ";
    }
    for (int i = 0; i <= 31; i++){
        cout<<"FPU:"<<Registers[i]<<": "<<FPURegisterValues[i]<<" | ";
    }
    
    // cout<<endl;
    // cout<<Registers[2]<<": "<<RegisterValues[2]<< " | ";
    // cout<<Registers[4]<<": "<<RegisterValues[4]<< " | ";
    // cout<<Registers[16]<<": "<<RegisterValues[16]<< " | ";
    // cout<<Registers[29]<<": "<<RegisterValues[29]<< " | ";
    // cout<<Registers[31]<<": "<<RegisterValues[31]<<" | ";

    // for (int i = 4070; i<=4096; i++){
    //     cout<<"Memory"<<i<<"  :"<<Memory[i]<<endl;
    // }
}

/*set current_instruction*/
void MIPSSimulator::ReadInstruction(int32_t program_counter)
{
    current_instruction=InputProgram[program_counter];
}

/*call appropriate operation function based on the op and increment program counter*/
void MIPSSimulator::ExecuteInstruction()
{
    // cout<<Instruction_op<<endl;
    switch(Instruction_op) 
    {
        //R-type
        case 0: 
            if(Instruction_funct == 32){
                add();
                ProgramCounter++;
            }else if (Instruction_funct == 34){
                sub();
                ProgramCounter++;
            }else if (Instruction_funct == 8){
                jr();
            }else if (Instruction_funct == 42){
                slt();
                ProgramCounter++;
            }else if(Instruction_funct == 0){
                sll();
                ProgramCounter++;
            }else{
                puts("R-type instruction not found");
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


        case 4:
            beq();
            ProgramCounter++;
            break;

        case 13:
            ori();
            ProgramCounter++;
            break;

        case 15:
            lui();
            ProgramCounter++;
            break;
        case 17:
            if (r[3]==16){
                if(Instruction_funct == 0){
                    fadd();
                }else if(Instruction_funct == 1){
                    fsub();
                }else if(Instruction_funct == 2){
                    fmul();
                }else if(Instruction_funct == 3){
                    fdiv();
                }else if(Instruction_funct == 4){
                    fsqrt();
                }else if(Instruction_funct == 60){
                    fless();
                }else if(Instruction_funct == 50){
                    ceqs();
                }
            }else if (r[3]==4){
                mtc1();
            }else{
                puts("fpu instruction not found");
            }
            ProgramCounter++;
            break;
        // case -2: //if instruction containing label, ignore
        //     break;
        case 49:
            lwc1();
            ProgramCounter++;
            break;
        case 57:
            swc1();
            ProgramCounter++;
            cout<<"before break"<<endl;
            break;
            cout<<"after break"<<endl;

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
void MIPSSimulator::sub()
{
    RegisterValues[r[2]]=RegisterValues[r[0]]-RegisterValues[r[1]];
}
void MIPSSimulator::jr()
{
    ProgramCounter = RegisterValues[r[0]];
    cout<<"jr is called"<<endl;
}

void MIPSSimulator::slt()
{
    if(RegisterValues[r[0]]<RegisterValues[r[1]]){
        RegisterValues[r[2]] =1;
    }else{
        RegisterValues[r[2]] = 0;
    }
}
void MIPSSimulator::sll(){
    RegisterValues[r[2]]=RegisterValues[r[1]]<<r[3];
}
// I-type : opcode   rs  rt  IMM
void MIPSSimulator::addi()
{
    RegisterValues[r[1]]=RegisterValues[r[0]]+r[2];
}


void MIPSSimulator::lui()
{
    RegisterValues[r[1]]= 0;
    RegisterValues[r[1]] = RegisterValues[r[1]] | (r[2]<<16);
    cout<<"lui is executed"<<endl;
}


// R[$rt] ← R[$rs] | {0 × 16, imm}
void MIPSSimulator::ori()
{
    int temp_imm =  r[2] & 0b1111111111111111;
    RegisterValues[r[1]]=RegisterValues[r[0]] | temp_imm;
    cout<<"ori is executed"<<endl;
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
    cout<<endl<<"lw is executed with value: "<< (Memory[r[2]+RegisterValues[r[0]]]) <<" address:  "<<RegisterValues[r[1]]<<endl;
}


void MIPSSimulator::sw()
{
    cout<<endl<<"sw is executed with value: "<<RegisterValues[r[1]]<<" address:  "<< RegisterValues[r[0]]+r[2]<<endl;
    Memory[RegisterValues[r[0]]+r[2]] =  RegisterValues[r[1]];
}

// J type
void MIPSSimulator::j()
{
    ProgramCounter = r[0];
    // ProgramCounter = r[0]<<2;
    cout<<"j is called"<<endl;
}

void MIPSSimulator::jal()
{
    RegisterValues[31] = ProgramCounter + 1;
    ProgramCounter = r[0];
    cout<<"jal is called"<<endl;
    // ProgramCounter = r[0] <<2;

}

/* FPU */
void MIPSSimulator::fadd(){
    FPURegisterValues[r[2]] =  FPURegisterValues[r[0]] + FPURegisterValues[r[1]];
}
void MIPSSimulator::fsub(){
    FPURegisterValues[r[2]] =  FPURegisterValues[r[1]] - FPURegisterValues[r[0]]; 
}
void MIPSSimulator::fmul(){
    FPURegisterValues[r[2]] =  FPURegisterValues[r[1]] * FPURegisterValues[r[0]];  
}
void MIPSSimulator::fdiv(){
    FPURegisterValues[r[2]] =  FPURegisterValues[r[1]] / FPURegisterValues[r[0]];
}
void MIPSSimulator::fsqrt(){
    FPURegisterValues[r[2]] =  sqrt(FPURegisterValues[r[1]]);
}

void MIPSSimulator::fless(){
    if(FPURegisterValues[r[1]]<FPURegisterValues[r[0]]){
        RegisterValues[r[2]]=1;
    }else{
        RegisterValues[r[2]]=0;
    }
}

void MIPSSimulator::ceqs(){
    if(FPURegisterValues[r[1]]==FPURegisterValues[r[0]]){
        RegisterValues[r[2]]=1;
    }else{
        RegisterValues[r[2]]=0;
    }
    cout<<"ceqs is executed"<<endl;
}


float int_to_binary_to_float(int32_t org){
    /* convert int to 2's comp binary, which is stored in binary_a */ 
    int32_t a = abs(org);
    int binary_a[32];
    for (int i = 31; i>=0; i--){
        binary_a[i]=a%2;
        a = a/2;
    }

    if (org<0){
        for (int i = 0; i<=31; i++){
            binary_a[i]=1-binary_a[i];
        } 
        for (int i = 31; i>=0; i--){
            if(binary_a[i]==0){
                binary_a[i]=1;
                break;
            }else{
                binary_a[i]=0;
            }
        }
    } 
    /* to float*/ 
    /*1 + 8 + 23*/ 
    float result = 1.0;
    int exp = -127;
        /*calculate fraction*/
    for(int i = 31; i>= 9; i--){
        result += pow(2,(8-i))*binary_a[i];
    }
        /*calculate exp*/
    for(int i = 8; i>=1; i--){  
        exp += pow(2,(8-i))*binary_a[i];
    }
    result = result * pow(2,exp);
    /*calculate sign*/
    if (binary_a[0]==1){
        result = -result;
    }
    return result;

}

void MIPSSimulator::mtc1(){
    cout<<"r[0]:"<<r[0]<<"r[1]:"<<r[1]<<"r[2]:"<<r[2]<<endl;
    FPURegisterValues[r[1]]= int_to_binary_to_float(RegisterValues[r[0]]);
    cout<<"mtc1 is executed"<<endl;
}

void MIPSSimulator::lwc1(){
    // FPURegisterValues[r[1]]= int_to_binary_to_float(Memory[r[2]+RegisterValues[r[0]]]);
    
    myfloat var;
    var.i = Memory[r[2]+RegisterValues[r[0]]];
    FPURegisterValues[r[1]] = var.f;

    cout<<"lwc1 is executed with value: "<< FPURegisterValues[r[1]] <<"from address:  "<<r[2]+RegisterValues[r[0]]<<endl;
}

void MIPSSimulator::swc1(){
    myfloat var;
    var.f = FPURegisterValues[r[1]];
    Memory[RegisterValues[r[0]]+r[2]] = var.i ;
    cout<<"swc1 is executed with value: "<<FPURegisterValues[r[1]]<<" address:  "<< RegisterValues[r[0]]+r[2]<<endl;
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
    cout<<"instruction_op : "<<Instruction_op<<endl;
    puts("parse instruction");
    // I-type
    if(Instruction_op == 8 || Instruction_op == 5 || Instruction_op == 35 || Instruction_op == 43||Instruction_op == 4 ||Instruction_op == 10 ||Instruction_op == 15 ||Instruction_op == 13||Instruction_op == 49 || Instruction_op == 57){
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
            // cout<<"r[2]:"<<r[2]<<endl; 
        }else{
            r[2] = stoi(current_instruction.substr(16,16).c_str(), nullptr, 2);
        }
    }
    // R-type
    else if(Instruction_op == 0){
        r[0] = stoi(current_instruction.substr(6,5).c_str(), nullptr, 2);
        r[1] = stoi(current_instruction.substr(11,5).c_str(), nullptr, 2);
        r[2] = stoi(current_instruction.substr(16,5).c_str(), nullptr, 2);
         /* store shamt*/ 
        r[3] = stoi(current_instruction.substr(21,5).c_str(), nullptr, 2);
    // J-type
    }else if (Instruction_op == 2 ||Instruction_op == 3){
        r[0] = stoi(current_instruction.substr(6,26).c_str(), nullptr, 2);
        // cout<<"r[0]: "<<r[0]<<endl;
    // FPU(f=s)
    }else if (Instruction_op == 17){
        // save ft
        r[0] = stoi(current_instruction.substr(11,5).c_str(), nullptr, 2);
        // save fs
        r[1] = stoi(current_instruction.substr(6,5).c_str(), nullptr, 2);
        cout<<"r[1] in parese: "<<current_instruction.substr(0,32).c_str()<<endl;
        // save fd
        r[2] = stoi(current_instruction.substr(16,5).c_str(), nullptr, 2);
        /* store the second filed */ 
        r[3] = stoi(current_instruction.substr(21,5).c_str(), nullptr, 2);

    }else{
        cout<<"current_instruction:"<<current_instruction<<endl;
        cout<<"opcode: "<<  Instruction_op <<endl;
        cout<<"Error: Invalid Instruction_op"<<endl;
        // ReportError();   
    }
}

//function to run the simulator
void MIPSSimulator::Execute()
{
    getchar(); //to remove effect of pressing enter key while starting
    // cout<<"getchar() in Execute: "<<test <<endl;
    int count_exec=0;
    // PrintRegisterName();
    while(ProgramCounter<NumberOfInstructions && count_exec <= limit_of_exec) 
    // while(ProgramCounter<NumberOfInstructions) 
    {   
        if(Mode == 0){
            cout<<"PC before execution: "<<ProgramCounter<<" | ";
        }
        // if (ProgramCounter%4 == 0){print_flag = 1;restr ++;}
        ReadInstruction(ProgramCounter); //set current_instruction
        cout<<"before pares in exe"<<endl;
        ParseInstruction(); // return op , populate values in r[]
        ExecuteInstruction(); //call appropriate operation function based on the op and increment program counter
        
        // if(print_flag == 1){PrintRegister(); }
        if(Mode == 0){
            PrintRegister(); 
            cout<<"PC after　execution: "<<ProgramCounter<<endl;
            cout<<"************ "<<endl;
        }
        count_exec++;
    }
    if(Mode == 1){
        PrintRegister(); 
    }
    // cout<<endl<<"Execution completed successfully"<<endl<<endl;
    // cout<<"$v0:"<<RegisterValues[2]<<endl;
}




int main()
{
    string path;
    int num_input;
    // int32_t mode;
    cout<<endl<<"MIPS Simulator - team7"<<endl<<endl;
    cout<<"Please enter the relative path of the input file and number of executions, such as  './sample_binary.txt' 5 "<<endl;
    cin>>path>>num_input;
    // if(mode!=1 && mode!=2) 
    // {
    //     cout<<"Error: Invalid Mode"<<endl;
    //     return 1;
    // }

    MIPSSimulator simulator(0,path,num_input); //create and initialize simulator
    simulator.Execute(); //execute simulator
    int test = 3;
    print_int_as_32bits(test);
    return 0;
}
