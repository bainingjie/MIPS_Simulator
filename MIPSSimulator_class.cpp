#include <iostream>
#include <cmath>
#include <cstring>
#include <fstream>
#include <vector>
#include <algorithm>
#include "fpu_imitate.h"
#include <chrono>

using namespace std;

void print_int_as_32bits (int num){
    for (int i = 31; i>=0; i--){
        printf("%d",(num>>i) & 1);
    }
}

typedef union {
    float f;
    int32_t i;
} myfloat;

int32_t Memory[1024*1000];// memory

class MIPSSimulator
{
    private:
        string Registers[32]; //array to store names of registers
        int32_t RegisterValues[32]; //array to store values of CPU registers
        float FPURegisterValues[32];//array to store values of FPU registers
        int max_memory_index;
        long long int limit_of_exec;
        int32_t Mode; //to store the Mode of execution
        int Instruction_count [33];
        string Instructions [33];
        std::fstream myfile;
        std::fstream myfile2;
        std::fstream print_pc;
        std::fstream print_register;
        std::fstream print_register2;
        std::fstream print_instruction;
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
        void srl();

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
        void itof();

        void mtc1();

        void lwc1();
        void swc1();
        void ceqs();

        void outi();
        void outc();
        void readi();
        void readf();
        void floor();
        void ftoi();



        void ReadInstruction(int32_t line);
        void ParseInstruction();

        void ExecuteInstruction();
        void PrintRegister();
        void PrintRegister2();
        void PrintInstruction();
        void update_biggest(int a);

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
    string tempInstructions[]={"add","sub ","jr","slt","sll","srl","j","jal","beq","bne","addi","slti",
    "ori","lui","fadd","fsub","fmul","fdiv","fsqrt","floor","ftoi","itof","fless","ceqs","mtc1","lw","sw","lwc1","swc1","readf","readi","outc","outi"};
    if(mode == 1){
        // limit_of_exec = 429496720;
        limit_of_exec = 10000000000;
        Mode = 1;
    }else{
        limit_of_exec = num_input;
        Mode = 0;
    }

    for(int32_t i=0;i<32;i++)
    {
        Registers[i]=tempRegisters[i];
    }
    for(int32_t i=0;i<33;i++)
    {
        Instructions[i]=tempInstructions[i];
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

    for(int32_t i=0;i<33;i++)
    {
        Instruction_count[i]=0;
    }
    ifstream InputFile;
    InputFile.open(fileName.c_str(),ios::in);
    if(!InputFile)
    {
        // print_register<<"Error: File does not exist or could not be opened"<<endl;
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
    cout<<"begin"<<endl;
    for (int i = 0; i <= 31; i++){
        if(RegisterValues[i] != 0){
            cout<<Registers[i]<<": "<<RegisterValues[i]<<"(" << std::hex << RegisterValues[i] << ")" << std::dec << " | ";
        }
    }
    for (int i = 0; i <= 31; i++){
        if (FPURegisterValues[i] != 0.0){
            cout<<"$f"<<i<<": "<<FPURegisterValues[i]<<"(" << std::hex << FPURegisterValues[i] << ")" << std::dec << " | ";
        }

    }

    // print_register<<endl;
    // print_register<<Registers[2]<<": "<<RegisterValues[2]<< " | ";
    // print_register<<Registers[4]<<": "<<RegisterValues[4]<< " | ";
    // print_register<<Registers[16]<<": "<<RegisterValues[16]<< " | ";
    // print_register<<Registers[29]<<": "<<RegisterValues[29]<< " | ";
    // print_register<<Registers[31]<<": "<<RegisterValues[31]<<" | ";

    // for (int i = 4070; i<=4096; i++){
    //     print_register<<"Memory"<<i<<"  :"<<Memory[i]<<endl;
    // }
}

void MIPSSimulator::PrintRegister2()
{
    for (int i = 0; i <= 31; i++){
        if(RegisterValues[i] != 0){
            // print_register2<<Registers[i]<<": "<<RegisterValues[i]<<"(" << std::hex << RegisterValues[i] << ")" << std::dec << " | ";
        }
    }
    for (int i = 0; i <= 31; i++){
        if (FPURegisterValues[i] != 0.0){
            // print_register2<<"$f"<<i<<": "<<FPURegisterValues[i]<<"(" << std::hex << FPURegisterValues[i] << ")" << std::dec << " | ";
        }

    }

    // print_register<<endl;
    // print_register<<Registers[2]<<": "<<RegisterValues[2]<< " | ";
    // print_register<<Registers[4]<<": "<<RegisterValues[4]<< " | ";
    // print_register<<Registers[16]<<": "<<RegisterValues[16]<< " | ";
    // print_register<<Registers[29]<<": "<<RegisterValues[29]<< " | ";
    // print_register<<Registers[31]<<": "<<RegisterValues[31]<<" | ";

    // for (int i = 4070; i<=4096; i++){
    //     print_register<<"Memory"<<i<<"  :"<<Memory[i]<<endl;
    // }
}

// void MIPSSimulator::PrintInstruction(){
//     print_instruction.open ("print_instrcution&memory_analysis.txt", std::fstream::in | std::fstream::out | std::fstream::trunc);
//     for (int i = 0; i <= 32; i++){
//             print_instruction<<Instructions[i]<<": "<<Instruction_count[i]<<endl;
//     }
//     print_instruction<<"maximum index of memory being used:"<< max_memory_index <<endl;
//     print_instruction.close();
// }

/*set current_instruction*/
void MIPSSimulator::ReadInstruction(int32_t program_counter)
{
    current_instruction=InputProgram[program_counter];
}

/*call appropriate operation function based on the op and increment program counter*/
void MIPSSimulator::ExecuteInstruction()
{
    switch(Instruction_op)
    {
        //R-type
        case 0:
            if(Instruction_funct == 32){
                add();
                ProgramCounter += 4;
                Instruction_count[0]++;
            }else if (Instruction_funct == 34){
                sub();
                ProgramCounter += 4;
                Instruction_count[1]++;
            }else if (Instruction_funct == 8){
                jr();
                Instruction_count[2]++;
            }else if (Instruction_funct == 42){
                slt();
                ProgramCounter += 4;
                Instruction_count[3]++;
            }else if(Instruction_funct == 0){
                sll();
                ProgramCounter += 4;
                Instruction_count[4]++;
            }else if(Instruction_funct == 2){
                srl();
                ProgramCounter += 4;
                Instruction_count[5]++;
            }else{
                //puts("R-type instruction not found");
                ProgramCounter += 4;
            }
            break;

        //J type
        case 2:
            j();
            Instruction_count[6]++;
            break;
        case 3:
            jal();
            // ProgramCounter += 4;
            Instruction_count[7]++;
            break;

        case 4:
            beq();
            ProgramCounter += 4;
            Instruction_count[8]++;
            break;

        // I-type
        case 5:
            bne();
            ProgramCounter += 4;
            Instruction_count[9]++;
            break;

        case 8:
            addi();
            ProgramCounter += 4;
            Instruction_count[10]++;
            break;

        case 10:
            slti();
            ProgramCounter += 4;
            Instruction_count[11]++;
            break;

        case 13:
            ori();
            ProgramCounter += 4;
            Instruction_count[12]++;
            break;

        case 15:
            lui();
            ProgramCounter += 4;
            Instruction_count[13]++;
            break;

        case 17:
            if (r[3]==16){
                if(Instruction_funct == 0){
                    fadd();
                    Instruction_count[14]++;
                }else if(Instruction_funct == 1){
                    fsub();
                    Instruction_count[15]++;
                }else if(Instruction_funct == 2){
                    fmul();
                    Instruction_count[16]++;
                }else if(Instruction_funct == 3){
                    fdiv();
                    Instruction_count[17]++;
                }else if(Instruction_funct == 4){
                    fsqrt();
                    Instruction_count[18]++;
                }else if(Instruction_funct == 5){
                    floor();
                    Instruction_count[19]++;
                }else if(Instruction_funct == 6){
                    ftoi();
                    Instruction_count[20]++;
                }else if(Instruction_funct == 7){
                    itof();
                    Instruction_count[21]++;
                }else if(Instruction_funct == 60){
                    fless();
                    Instruction_count[22]++;
                }else if(Instruction_funct == 50){
                    ceqs();
                    Instruction_count[23]++;
                }
            }else if (r[3]==4){
                mtc1();
                Instruction_count[24]++;
            }else{
                //puts("fpu instruction not found");
            }
            ProgramCounter += 4;
            break;

        case 35:
            lw();
            ProgramCounter += 4;
            Instruction_count[25]++;
            break;
        case 43:
            sw();
            ProgramCounter += 4;
            Instruction_count[26]++;
            break;
        case 49:
            lwc1();
            ProgramCounter += 4;
            Instruction_count[27]++;
            break;
        case 57:
            swc1();
            ProgramCounter += 4;
            Instruction_count[28]++;
            break;
        case 60:
            readf();
            ProgramCounter += 4;
            Instruction_count[29]++;
            break;
        case 61:
            readi();
            ProgramCounter += 4;
            Instruction_count[30]++;
            break;
        case 62:
            outc();
            ProgramCounter += 4;
            Instruction_count[31]++;
            break;
        case 63:
            outi();
            ProgramCounter += 4;
            Instruction_count[32]++;
            break;
        default:
            cout<<"Error: Invalid instruction_op received"<<endl;
            // print_register<<"Error: Invalid instruction_op received"<<endl;
    }
}

void MIPSSimulator::update_biggest(int a){
    if(a> max_memory_index){
        max_memory_index = a;
    }
}

// R-type : opcode   rs  rt  rd
void MIPSSimulator::add()
{
    RegisterValues[r[2]]=RegisterValues[r[0]]+RegisterValues[r[1]];
    if(Mode == 0){
        // print_register<<"add is executed"<<endl;
    }
}

void MIPSSimulator::sub()
{
    RegisterValues[r[2]]=RegisterValues[r[0]]-RegisterValues[r[1]];
    if(Mode == 0){
        // print_register<<"sub is executed"<<endl;
    }
}

void MIPSSimulator::jr()
{
    ProgramCounter = RegisterValues[r[0]];
    if(Mode == 0){
        // print_register<<"jr is called"<<endl;
    }
}

void MIPSSimulator::slt()
{
    if(RegisterValues[r[0]]<RegisterValues[r[1]]){
        RegisterValues[r[2]] =1;
    }else{
        RegisterValues[r[2]] = 0;
    }
    if(Mode == 0){
        // print_register<<"slt is executed"<<endl;
    }
}

void MIPSSimulator::sll(){
    RegisterValues[r[2]]=RegisterValues[r[1]]<<r[3];
    if(Mode == 0){
        // print_register<<"sll is executed"<<endl;
    }
}


void MIPSSimulator::srl(){    
    int32_t temp_a =  r[3];
    if(temp_a>0){
        RegisterValues[r[2]]=RegisterValues[r[1]]>>1;
        RegisterValues[r[2]]=RegisterValues[r[2]]&0b01111111111111111111111111111111;
        temp_a --;
        RegisterValues[r[2]]=RegisterValues[r[2]]>>temp_a;
    }

    if(Mode == 0){
        // print_register<<"srl is executed"<<endl;
    }
}

// I-type : opcode   rs  rt  IMM
void MIPSSimulator::addi()
{
    RegisterValues[r[1]]=RegisterValues[r[0]]+r[2];
    if(Mode == 0){
        // print_register<<"addi is executed"<<endl;
    }
}

void MIPSSimulator::lui()
{
    RegisterValues[r[1]]= 0;
    RegisterValues[r[1]] = RegisterValues[r[1]] | (r[2]<<16);
    if(Mode == 0){
        // print_register<<"lui is executed"<<endl;
    }
}

void MIPSSimulator::ori()
{
    int32_t temp_imm =  r[2] & 0b1111111111111111;
    RegisterValues[r[1]]=RegisterValues[r[0]] | temp_imm;
    if(Mode == 0){
        // print_register<<"ori is executed"<<endl;
    }
}

void MIPSSimulator::slti()
{
    if (RegisterValues[r[0]]<r[2]){
        RegisterValues[r[1]]=1;
    }else{
        RegisterValues[r[1]]=0;
    }

    if(Mode == 0){
        // print_register<<"slti is executed"<<endl;
    }
}

void MIPSSimulator::bne()
{
    if(RegisterValues[r[0]] != RegisterValues[r[1]]){
        ProgramCounter += r[2]*4;
    }

    if(Mode == 0){
        // print_register<<"bne is executed"<<endl;
    }
}

void MIPSSimulator::beq()
{
    if(RegisterValues[r[0]] == RegisterValues[r[1]]){
        ProgramCounter += r[2]*4;
    }

    if(Mode == 0){
        // print_register<<"beq is executed"<<endl;
    }
}

void MIPSSimulator::lw()
{
    RegisterValues[r[1]]= Memory[r[2]+RegisterValues[r[0]]];
    if(Mode == 0){
        // print_register<<endl<<"lw is executed with value: "<< (Memory[r[2]+RegisterValues[r[0]]]) <<" address:  "<<RegisterValues[r[1]]<<endl;
    }
    update_biggest(r[2]+RegisterValues[r[0]]);
}


void MIPSSimulator::sw()
{
    Memory[RegisterValues[r[0]]+r[2]] =  RegisterValues[r[1]];
    if(Mode == 0){
        // print_register<<endl<<"sw is executed with value: "<<RegisterValues[r[1]]<<" address:  "<< RegisterValues[r[0]]+r[2]<<endl;
    }
    update_biggest(r[2]+RegisterValues[r[0]]);

}

// J type
void MIPSSimulator::j()
{
    int32_t temp_a = ProgramCounter + 4;
    temp_a = temp_a & 0b11110000000000000000000000000000;
    int32_t temp_b = r[0]*4;
    ProgramCounter = temp_a|temp_b;
    if(Mode == 0){
        // print_register<<"j is called"<<endl;
    }
}

void MIPSSimulator::jal()
{
    RegisterValues[31] = ProgramCounter + 4;


    int32_t temp_a = ProgramCounter + 4;
    temp_a = temp_a & 0b11110000000000000000000000000000;
    int32_t temp_b = r[0]*4;
    ProgramCounter = temp_a|temp_b;


    if(Mode == 0){
        // print_register<<"jal is called"<<endl;
    }

}

/* FPU */
void MIPSSimulator::fadd(){
    FPURegisterValues[r[2]] =  FPURegisterValues[r[0]] + FPURegisterValues[r[1]];
    if(Mode == 0){
        // print_register<<"fadd is executed"<<endl;
    }
}
void MIPSSimulator::fsub(){
    FPURegisterValues[r[2]] =  FPURegisterValues[r[1]] - FPURegisterValues[r[0]];
    if(Mode == 0){
        // print_register<<"fsub is executed"<<endl;
    }
}
void MIPSSimulator::fmul(){
    // FPURegisterValues[r[2]] =  FPURegisterValues[r[1]] * FPURegisterValues[r[0]];
    FPURegisterValues[r[2]] = myfmul(FPURegisterValues[r[1]],FPURegisterValues[r[0]]);
    if(Mode == 0){
        // print_register<<"fmul is executed"<<endl;
    }
}
void MIPSSimulator::fdiv(){
    // FPURegisterValues[r[2]] =  FPURegisterValues[r[1]] / FPURegisterValues[r[0]];
    FPURegisterValues[r[2]] = myfdiv(FPURegisterValues[r[1]],FPURegisterValues[r[0]]);
    if(Mode == 0){
        // print_register<<"fdiv is executed"<<endl;
    }
}
void MIPSSimulator::fsqrt(){
    // FPURegisterValues[r[2]] =  sqrt(FPURegisterValues[r[1]]);
    FPURegisterValues[r[2]] = myfsqrt(FPURegisterValues[r[1]]);
    if(Mode == 0){
        // print_register<<"fsqrt is executed"<<endl;
    }
}

void MIPSSimulator::fless(){
    // if(FPURegisterValues[r[1]]<FPURegisterValues[r[0]]){
    //     RegisterValues[r[2]]=1;
    // }else{
    //     RegisterValues[r[2]]=0;
    // }
    RegisterValues[r[2]] = myfless(FPURegisterValues[r[1]],FPURegisterValues[r[0]]);
    if(Mode == 0){
        // print_register<<"fless is executed with: if " <<FPURegisterValues[r[1]]<<"< "<< FPURegisterValues[r[0]] <<endl;
    }
}

void MIPSSimulator::itof(){
    // FPURegisterValues[r[2]] = (float)RegisterValues[r[1]];
    FPURegisterValues[r[2]] = myitof(RegisterValues[r[1]]);
    if(Mode == 0){
        // print_register<<"itof is executed with"<<FPURegisterValues[r[2]]<< endl;
    }
}

void MIPSSimulator::ftoi(){
    // int temp_a = (int)FPURegisterValues[r[1]];
    // float temp_b =  abs(FPURegisterValues[r[1]]-temp_a);
    // if(temp_b >= 0.5){
    //     if(temp_a>0){temp_a += 1;}else{
    //         temp_a -= 1;
    //     }
    // }
    // RegisterValues[r[2]] = temp_a;
    RegisterValues[r[2]]= myftoi(FPURegisterValues[r[1]]);
    if(Mode == 0){
        // print_register<<"ftoi is executed with"<<RegisterValues[r[2]]<< endl;
    }
}

void MIPSSimulator::floor(){
    // if(FPURegisterValues[r[1]]>=0){
    //     RegisterValues[r[2]] = (int)FPURegisterValues[r[1]];
    // }else{
    //     RegisterValues[r[2]] = (int)FPURegisterValues[r[1]]-1;
    // }
    FPURegisterValues[r[2]] = myfloor(FPURegisterValues[r[1]]);
    if(Mode == 0){
        // print_register<<"floor is executed with"<<RegisterValues[r[2]]<< endl;
    }
}
void MIPSSimulator::ceqs(){
    // if(FPURegisterValues[r[1]]==FPURegisterValues[r[0]]){
    //     RegisterValues[r[2]]=1;
    // }else{
    //     RegisterValues[r[2]]=0;
    // }
    RegisterValues[r[2]] = myfeq(FPURegisterValues[r[1]],FPURegisterValues[r[0]]);
    if(Mode == 0){
        // print_register<<"ceqs is executed"<<endl;
    }
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
    myfloat var;
    var.i = RegisterValues[r[0]];
    FPURegisterValues[r[1]] = var.f;
    // FPURegisterValues[r[1]]= int_to_binary_to_float(RegisterValues[r[0]]);
    if(Mode == 0){
        // print_register<<"mtc1 is executed"<<endl;
    }
}

void MIPSSimulator::lwc1(){
    // FPURegisterValues[r[1]]= int_to_binary_to_float(Memory[r[2]+RegisterValues[r[0]]]);
    myfloat var;
    var.i = Memory[r[2]+RegisterValues[r[0]]];
    FPURegisterValues[r[1]] = var.f;
    if(Mode == 0){
        // print_register<<"lwc1 is executed with value: "<< FPURegisterValues[r[1]] <<"from address:  "<<r[2]+RegisterValues[r[0]]<<endl;
    }
    update_biggest(r[2]+RegisterValues[r[0]]);
}

void MIPSSimulator::swc1(){
    myfloat var;
    var.f = FPURegisterValues[r[1]];
    Memory[RegisterValues[r[0]]+r[2]] = var.i ;
    if(Mode == 0){
        // print_register<<"swc1 is executed with value: "<<FPURegisterValues[r[1]]<<" address:  "<< RegisterValues[r[0]]+r[2]<<endl;
    }
    update_biggest(r[2]+RegisterValues[r[0]]);
}

void MIPSSimulator::outi(){
    if(Mode == 0){
        // print_register<<"outi is executed with"<<RegisterValues[r[1]]<<endl;
    }
    //cout<<RegisterValues[r[1]];
    myfile<<RegisterValues[r[1]];
    //myfile2<<RegisterValues[r[1]];
}
int count_ = 0;
void MIPSSimulator::outc(){
    if(Mode == 0){
        // print_register<<"outc is executed with"<<RegisterValues[r[1]]<<endl;
    }
    if (count_ == 2){
      myfile<<RegisterValues[r[1]]<<endl;
    }
    else{
      myfile<<RegisterValues[r[1]]<<" ";
    }

    count_ = (count_ + 1)%3;

    //myfile<<(char)RegisterValues[r[1]];
    myfile2<<(char)RegisterValues[r[1]];

    // cout<<(char)RegisterValues[r[1]];
    //cout<<'a';
    //printf("a");
}

void MIPSSimulator::readi(){
    int32_t a;
    cin>>a;
    RegisterValues[r[1]]= a;
    //PrintRegister();
    //cout<<"i\n"<<endl;
    //cout<<a<<endl;
    if(Mode == 0){
        // print_register<<"readi is executed with"<<RegisterValues[r[1]]<<endl;
    }
}

void MIPSSimulator::readf(){
    float a;
    cin>>a;
    FPURegisterValues[r[1]]= a;
    //PrintRegister();
    //cout<<"f\n"<<endl;
    //cout<<a<<endl;
    if(Mode == 0){
        // print_register<<"readf is executed with"<<FPURegisterValues[r[1]]<<endl;
    }
}



// return op , populate values in r[]
void MIPSSimulator::ParseInstruction(){
    if(current_instruction.size()!=32)
    {
        //cout<<"Parse Error: Invalid instruction"<<endl;
        // print_register<<"Parse Error: Invalid instruction"<<endl;
    }

    Instruction_op = stoi(current_instruction.substr(0,6), nullptr, 2);
    Instruction_funct = stoi(current_instruction.substr(26,6), nullptr, 2);
    // I-type
    // if(Instruction_op == 8 || Instruction_op == 5 || Instruction_op == 35 || Instruction_op == 43||Instruction_op == 4 ||Instruction_op == 10 ||Instruction_op == 15 ||Instruction_op == 13||Instruction_op == 49 || Instruction_op == 57){
    //     r[0] = stoi(current_instruction.substr(6,5), nullptr, 2);
    //     r[1] = stoi(current_instruction.substr(11,5), nullptr, 2);

    //     if(current_instruction[16] == '1'){
    //         char temp_imm[17];
    //         strcpy(temp_imm,current_instruction.substr(16,16));
    //         for (int i = 0; i<16; i++){
    //             if(temp_imm[i]=='1'){temp_imm[i]='0';}
    //             else{temp_imm[i]='1';}
    //         }
    //         r[2] = stoi(temp_imm, nullptr, 2);
    //         r[2] ++;
    //         r[2] = 0-r[2];
    //     }else{
    //         r[2] = stoi(current_instruction.substr(16,16), nullptr, 2);
    //     }
    // }
    // R-type                       outi
    if(Instruction_op == 0 || (Instruction_op <= 63 && Instruction_op >= 60) ){
        r[0] = stoi(current_instruction.substr(6,5), nullptr, 2);
        r[1] = stoi(current_instruction.substr(11,5), nullptr, 2);
        r[2] = stoi(current_instruction.substr(16,5), nullptr, 2);
         /* store shamt*/
        r[3] = stoi(current_instruction.substr(21,5), nullptr, 2);
    // J-type
    }else if (Instruction_op == 2 ||Instruction_op == 3){
        r[0] = stoi(current_instruction.substr(6,26), nullptr, 2);
    // FPU(f=s)
    }else if (Instruction_op == 17){
        // save fs to r[1]
        r[1] = stoi(current_instruction.substr(6,5), nullptr, 2);
        // save ft to r[0]
        r[0] = stoi(current_instruction.substr(11,5), nullptr, 2);
        // save fd to r[2]
        r[2] = stoi(current_instruction.substr(16,5), nullptr, 2);
        /* store the second filed */
        r[3] = stoi(current_instruction.substr(21,5), nullptr, 2);

    // I-type
    }else{
        r[0] = stoi(current_instruction.substr(6,5), nullptr, 2);
        r[1] = stoi(current_instruction.substr(11,5), nullptr, 2);

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
        }else{
            r[2] = stoi(current_instruction.substr(16,16), nullptr, 2);
        }
    }
    // else{
    //     //cout<<"Parese Error: Invalid Instruction_op"<<endl;
    //     // print_register<<"Parese Error: Invalid Instruction_op"<<endl;
    //     // ReportError();
    // }
}


//function to run the simulator
void MIPSSimulator::Execute()
{
    getchar(); //to remove effect of pressing enter key while starting
    int count_exec=0;

    myfile.open ("print_output.txt", std::fstream::in | std::fstream::out | std::fstream::trunc);
    myfile2.open("result.ppm", std::fstream::in | std::fstream::out | std::fstream::trunc);
    // print_pc.open ("print_pc.txt", std::fstream::in | std::fstream::out | std::fstream::trunc);
    // print_register.open ("print_register.txt", std::fstream::in | std::fstream::out | std::fstream::trunc);
    // print_register2.open ("print_register2.txt", std::fstream::in | std::fstream::out | std::fstream::trunc);

    while(ProgramCounter<NumberOfInstructions*4 && count_exec <= limit_of_exec)
    {
        // if(Mode == 0){
        //   print_register<<"PC before execution: "<<ProgramCounter<<endl;
        //   print_register << "count" << count_exec << endl;
        //     if (limit_of_exec - 1000 <= count_exec){
        //       print_register2<<"PC before execution: "<<ProgramCounter<<"(" << std::hex << ProgramCounter << ")" << std::dec<<endl;
        //       print_register2<< "count" << count_exec <<"(" << std::hex << count_exec << ")" << std::dec << endl;
        //       print_register2<<current_instruction<<endl;
        //     }
        // }
        /*
        if (count_exec > 17680000 && count_exec%10000==0){
          cout<<"live1"<<endl;
        }
        ReadInstruction(ProgramCounter/4); //set current_instruction
        if (count_exec > 17680000 && count_exec%10000==0){
          cout<<"live2"<<endl;
        }
        ParseInstruction(); // return op , populate values in r[]
        if (count_exec > 17680000 && count_exec%10000==0){
          cout<<"live3"<<endl;
          cout<<Instruction_op<<endl;
        }
        */
        // auto start = chrono::steady_clock::now();
        // ReadInstruction(ProgramCounter/4); //set current_instruction
        // auto end = chrono::steady_clock::now();
        // cout << "Elapsed time in read  : "
        // << chrono::duration_cast<chrono::nanoseconds>(end - start).count()
        // << " ns" << endl;

        // start = chrono::steady_clock::now();
        // ParseInstruction(); // return op , populate values in r[]
        // end = chrono::steady_clock::now();
        // cout << "Elapsed time in parse  : "
        // << chrono::duration_cast<chrono::nanoseconds>(end - start).count()
        // << " ns" << endl;

        // start = chrono::steady_clock::now();
        // ExecuteInstruction(); //call appropriate operation function based on the op and increment program counter
        // end = chrono::steady_clock::now();
        // cout << "Elapsed time in execute  : "
        // << chrono::duration_cast<chrono::nanoseconds>(end - start).count()
        // << " ns" << endl;


    
        ReadInstruction(ProgramCounter/4);
        ParseInstruction(); // return op , populate values in r[]
        ExecuteInstruction(); //call appropriate operation function based on the op and increment program counter


        /*
        if (count_exec > 11533800 && count_exec < 11534000) {
          cout << count_exec << endl;
          cout<<current_instruction<<endl;
          cout<<ProgramCounter<<endl;
          PrintRegister();
          cout<<"\n";
          cout<<"end"<<endl;
        }
        */
        if (count_exec%10000000==0 && count_exec != 0){
          cout<<"success:"<<count_exec<<endl;
        }
        count_exec++;
        // if(Mode == 0){
        //   PrintRegister();
        //   print_register<<"PC after　execution:"<<ProgramCounter<<endl;
        //   print_pc<<ProgramCounter<<endl;
        //   print_register<<"************ "<<endl;
        //   if (limit_of_exec - 1000 < count_exec){
        //     PrintRegister2();
        //     print_register2<<"PC after　execution:"<<ProgramCounter<<"(" << std::hex << ProgramCounter << ")" << std::dec<<endl;
        //     print_register2<<"************ "<<endl;
        //   }
        // }
        // count_exec++;
    }
    // if(Mode == 1){
    //     PrintRegister();
    // }
    // PrintInstruction();

    myfile.close();
    myfile2.close();
    // print_pc.close();
    // print_register.close();
    // print_register2.close();

}


int main()
{
    string path;
    int32_t num_input;

    cout<<endl<<"MIPS Simulator - team7"<<endl<<endl;
    cout<<"入力形式： [file path] ＋ [実行行数] "<<endl;
    cout<<"入力例：  ./sample_binary.txt  5 "<<endl;
    cout<<"ただし、実行行数=0の場合は全ての命令を実行する。 "<<endl;
    cin>>path>>num_input;

    if(num_input == 0){
        MIPSSimulator simulator(1,path,(int32_t)num_input);
        simulator.Execute();
    }
    else if (num_input >= 1) {
        MIPSSimulator simulator(0,path,(int32_t)num_input);
        simulator.Execute();
    }
    return 0;
}
