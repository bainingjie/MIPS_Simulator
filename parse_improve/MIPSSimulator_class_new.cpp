#include <iostream>
#include <cmath>
#include <cstring>
#include <fstream>
#include <vector>
#include <algorithm>
#include <sys/time.h>
#include "fpu_imitate.h"
#include "mytype.h"

// #include "depend_function.h"
using namespace std;

void PrintRegister()
{
    print_register<<"begin"<<endl;
    for (int i = 0; i <= 31; i++){
        if(RegisterValues[i] != 0){
            print_register<<Registers[i]<<": "<<RegisterValues[i]<<"(" << std::hex << RegisterValues[i] << ")" << std::dec << " | ";
        }
    }
    for (int i = 0; i <= 31; i++){
        if (FPURegisterValues[i] != 0.0){
            print_register<<"$f"<<i<<": "<<FPURegisterValues[i]<<"(" << std::hex << FPURegisterValues[i] << ")" << std::dec << " | ";
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

/*set current_instruction*/
void ReadInstruction(int32_t program_counter)
{
    current_instruction = InputProgram3[program_counter];
}


/*call appropriate operation function based on the op and increment program counter*/
void update_biggest(int a){
    if(a> max_memory_index){
        max_memory_index = a;
    }
}

// R-type : opcode   rs  rt  rd
void add()
{
    RegisterValues[r[2]]=RegisterValues[r[0]]+RegisterValues[r[1]];
    if(Mode == 0){
        print_register<<"add is executed"<<endl;
    }
}

void sub()
{
    RegisterValues[r[2]]=RegisterValues[r[0]]-RegisterValues[r[1]];
    if(Mode == 0){
        print_register<<"sub is executed"<<endl;
    }
}

void jr()
{
    ProgramCounter = RegisterValues[r[0]];
    if(Mode == 0){
        print_register<<"jr is called"<<endl;
    }
}

void slt()
{
    if(RegisterValues[r[0]]<RegisterValues[r[1]]){
        RegisterValues[r[2]] =1;
    }else{
        RegisterValues[r[2]] = 0;
    }
    if(Mode == 0){
        print_register<<"slt is executed"<<endl;
    }
}

void sll(){
    RegisterValues[r[2]]=RegisterValues[r[1]]<<r[3];
    if(Mode == 0){
        print_register<<"sll is executed"<<endl;
    }
}

void srl(){    
    int32_t temp_a =  r[3];
    if(temp_a>0){
        RegisterValues[r[2]]=RegisterValues[r[1]]>>1;
        RegisterValues[r[2]]=RegisterValues[r[2]]&0b01111111111111111111111111111111;
        temp_a --;
        RegisterValues[r[2]]=RegisterValues[r[2]]>>temp_a;
    }

    if(Mode == 0){
        print_register<<"srl is executed"<<endl;
    }
}

// I-type : opcode   rs  rt  IMM
void addi()
{
    RegisterValues[r[1]]=RegisterValues[r[0]]+r[2];
    if(Mode == 0){
        print_register<<"addi is executed"<<endl;
    }
}

void lui()
{
    RegisterValues[r[1]]= 0;
    RegisterValues[r[1]] = RegisterValues[r[1]] | (r[2]<<16);
    if(Mode == 0){
        print_register<<"lui is executed"<<endl;
    }
}

void ori()
{
    int temp_imm =  r[2] & 0b1111111111111111;
    RegisterValues[r[1]]=RegisterValues[r[0]] | temp_imm;
    if(Mode == 0){
        print_register<<"ori is executed"<<endl;
    }
}

void slti()
{
    if (RegisterValues[r[0]]<r[2]){
        RegisterValues[r[1]]=1;
    }else{
        RegisterValues[r[1]]=0;
    }

    if(Mode == 0){
        print_register<<"slti is executed"<<endl;
    }
}

void bne()
{
    if(RegisterValues[r[0]] != RegisterValues[r[1]]){
        ProgramCounter += r[2]*4;
    }

    if(Mode == 0){
        print_register<<"bne is executed"<<endl;
    }
}

void beq()
{
    if(RegisterValues[r[0]] == RegisterValues[r[1]]){
        ProgramCounter += r[2]*4;
    }

    if(Mode == 0){
        print_register<<"beq is executed"<<endl;
    }
}

void lw()
{
    RegisterValues[r[1]]= Memory[r[2]+RegisterValues[r[0]]];
    if(Mode == 0){
        print_register<<endl<<"lw is executed with value: "<< (Memory[r[2]+RegisterValues[r[0]]]) <<" address:  "<<RegisterValues[r[1]]<<endl;
    }
    update_biggest(r[2]+RegisterValues[r[0]]);
}


void sw()
{
    Memory[RegisterValues[r[0]]+r[2]] =  RegisterValues[r[1]];
    if(Mode == 0){
        print_register<<endl<<"sw is executed with value: "<<RegisterValues[r[1]]<<" address:  "<< RegisterValues[r[0]]+r[2]<<endl;
    }
    update_biggest(r[2]+RegisterValues[r[0]]);

}

// J type
void j()
{
    int32_t temp_a = ProgramCounter + 4;
    temp_a = temp_a & 0b11110000000000000000000000000000;
    int32_t temp_b = r[0]*4;
    ProgramCounter = temp_a|temp_b;
    if(Mode == 0){
        print_register<<"j is called"<<endl;
    }
}

void jal()
{
    RegisterValues[31] = ProgramCounter + 4;

    int32_t temp_a = ProgramCounter + 4;
    temp_a = temp_a & 0b11110000000000000000000000000000;
    int32_t temp_b = r[0]*4;
    ProgramCounter = temp_a|temp_b;

    if(Mode == 0){
        print_register<<"jal is called"<<endl;
    }

}

/* FPU */
void fadd(){
    FPURegisterValues[r[2]] =  FPURegisterValues[r[0]] + FPURegisterValues[r[1]];
    if(Mode == 0){
        print_register<<"fadd is executed"<<endl;
    }
}
void fsub(){
    FPURegisterValues[r[2]] =  FPURegisterValues[r[1]] - FPURegisterValues[r[0]];
    if(Mode == 0){
        print_register<<"fsub is executed"<<endl;
    }
}
void fmul(){
    // FPURegisterValues[r[2]] =  FPURegisterValues[r[1]] * FPURegisterValues[r[0]];
    FPURegisterValues[r[2]] = myfmul(FPURegisterValues[r[1]],FPURegisterValues[r[0]]);
    if(Mode == 0){
        print_register<<"fmul is executed"<<endl;
    }
}
void fdiv(){
    // FPURegisterValues[r[2]] =  FPURegisterValues[r[1]] / FPURegisterValues[r[0]];
    FPURegisterValues[r[2]] = myfdiv(FPURegisterValues[r[1]],FPURegisterValues[r[0]]);
    if(Mode == 0){
        print_register<<"fdiv is executed"<<endl;
    }
}
void fsqrt(){
    // FPURegisterValues[r[2]] =  sqrt(FPURegisterValues[r[1]]);
    FPURegisterValues[r[2]] = myfsqrt(FPURegisterValues[r[1]]);
    if(Mode == 0){
        print_register<<"fsqrt is executed"<<endl;
    }
}

void fless(){
    // if(FPURegisterValues[r[1]]<FPURegisterValues[r[0]]){
    //     RegisterValues[r[2]]=1;
    // }else{
    //     RegisterValues[r[2]]=0;
    // }
    RegisterValues[r[2]] = myfless(FPURegisterValues[r[1]],FPURegisterValues[r[0]]);
    if(Mode == 0){
        print_register<<"fless is executed with: if " <<FPURegisterValues[r[1]]<<"< "<< FPURegisterValues[r[0]] <<endl;
    }
}

void itof(){
    // FPURegisterValues[r[2]] = (float)RegisterValues[r[1]];
    FPURegisterValues[r[2]] = myitof(RegisterValues[r[1]]);
    if(Mode == 0){
        print_register<<"itof is executed with"<<FPURegisterValues[r[2]]<< endl;
    }
}

void ftoi(){
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
        print_register<<"ftoi is executed with"<<RegisterValues[r[2]]<< endl;
    }
}

void floor(){
    // if(FPURegisterValues[r[1]]>=0){
    //     RegisterValues[r[2]] = (int)FPURegisterValues[r[1]];
    // }else{
    //     RegisterValues[r[2]] = (int)FPURegisterValues[r[1]]-1;
    // }
    FPURegisterValues[r[2]] = myfloor(FPURegisterValues[r[1]]);
    if(Mode == 0){
        print_register<<"floor is executed with"<<RegisterValues[r[2]]<< endl;
    }
}
void ceqs(){
    // if(FPURegisterValues[r[1]]==FPURegisterValues[r[0]]){
    //     RegisterValues[r[2]]=1;
    // }else{
    //     RegisterValues[r[2]]=0;
    // }
    RegisterValues[r[2]] = myfeq(FPURegisterValues[r[1]],FPURegisterValues[r[0]]);
    if(Mode == 0){
        print_register<<"ceqs is executed"<<endl;
    }
}


void mtc1(){
    myfloat var;
    var.i = RegisterValues[r[0]];
    FPURegisterValues[r[1]] = var.f;
    // FPURegisterValues[r[1]]= int_to_binary_to_float(RegisterValues[r[0]]);
    if(Mode == 0){
        print_register<<"mtc1 is executed"<<endl;
    }
}

void lwc1(){
    // FPURegisterValues[r[1]]= int_to_binary_to_float(Memory[r[2]+RegisterValues[r[0]]]);
    myfloat var;
    var.i = Memory[r[2]+RegisterValues[r[0]]];
    FPURegisterValues[r[1]] = var.f;
    if(Mode == 0){
        print_register<<"lwc1 is executed with value: "<< FPURegisterValues[r[1]] <<"from address:  "<<r[2]+RegisterValues[r[0]]<<endl;
    }
    update_biggest(r[2]+RegisterValues[r[0]]);
}

void swc1(){
    myfloat var;
    var.f = FPURegisterValues[r[1]];
    Memory[RegisterValues[r[0]]+r[2]] = var.i ;
    if(Mode == 0){
        print_register<<"swc1 is executed with value: "<<FPURegisterValues[r[1]]<<" address:  "<< RegisterValues[r[0]]+r[2]<<endl;
    }
    update_biggest(r[2]+RegisterValues[r[0]]);
}

void outi(){
    if(Mode == 0){
        print_register<<"outi is executed with"<<RegisterValues[r[1]]<<endl;
    }
    //cout<<RegisterValues[r[1]];
    myfile<<RegisterValues[r[1]];
    //myfile2<<RegisterValues[r[1]];
}

int count_ = 0;
void outc(){
    if(Mode == 0){
        print_register<<"outc is executed with"<<RegisterValues[r[1]]<<endl;
    }
    if (count_ == 2){
      myfile<<RegisterValues[r[1]]<<endl;
    }
    else{
      myfile<<RegisterValues[r[1]]<<" ";
    }

    count_ = (count_ + 1)%3;

    myfile2<<(char)RegisterValues[r[1]];

}

void readi(){
    int32_t a;
    InputFile>>a;
    RegisterValues[r[1]]= a;
    if(Mode == 0){
        print_register<<"readi is executed with"<<RegisterValues[r[1]]<<endl;
    }
}

void readf(){
    float a;
    InputFile>>a;
    FPURegisterValues[r[1]]= a;
    if(Mode == 0){
        print_register<<"readf is executed with"<<FPURegisterValues[r[1]]<<endl;
    }
}


void init(){
    NumberOfInstructions=0;
    count_exec = 0;
    ProgramCounter=0;
    string tempRegisters[]={"zero","at","v0","v1","a0","a1","a2","a3","t0","t1","t2","t3","t4","t5","t6",
    "t7","s0","s1","s2","s3","s4","s5","s6","s7","t8","t9","k0","k1","gp","sp","s8","ra"}; //names of registers
    string tempInstructions[]={"add","sub ","jr","slt","sll","srl","j","jal","beq","bne","addi","slti",
    "ori","lui","fadd","fsub","fmul","fdiv","fsqrt","floor","ftoi","itof","fless","ceqs","mtc1","lw","sw","lwc1","swc1","readf","readi","outc","outi"};

    
    if(num_input == 0){
        // limit_of_exec = 429496720;
        limit_of_exec = 10000000000;
    }else{
        limit_of_exec = num_input;
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
        cout<<"Error: File does not exist or could not be opened"<<endl;
        exit(1);
    }
 
    string tempString;
    unsigned int tempInt;
    inst_t tempInst;
    while(getline(InputFile,tempString))
    {
        NumberOfInstructions++;
        //stoiが31文字までしか認識しなかったから少し面倒な書き方になっているけど要するにint型にしているだけ
        tempInt = stoi(tempString.substr(0,31), nullptr, 2);
        //cout << tempString[31] << endl;
        if (tempString[31] == '1'){
          tempInt = tempInt*2 + 1;
        }
        else{
          tempInt = tempInt*2;
        }
        //InputProgram2にpush
        tempInst.inst_32 = tempInt;
        InputProgram3.push_back(tempInst);
    }
    InputFile.close();
}


void setItypeRegister(){
        r[0] = current_instruction.itype.r0;
        r[1] = current_instruction.itype.r1;
        r[2] = current_instruction.itype.imm;
        if (r[2]>32768){ 
            //65536=10000000000000000
          r[2] = -(65536-r[2]);
        }
}

void setRtypeRegister(){
        r[0] = current_instruction.rtype.r0;
        r[1] = current_instruction.rtype.r1;
        r[2] = current_instruction.rtype.r2;
        r[3] = current_instruction.rtype.r3;
}

void setJtypeRegister(){
        r[0] = current_instruction.jtype.r0;
}

void setFtypeRegister(){
        r[1] = current_instruction.rtype.r0;
        r[0] = current_instruction.rtype.r1;
        r[2] = current_instruction.rtype.r2;
        r[3] = current_instruction.rtype.r3;
}

void ParseAndExecuteInstruction()
{
    Instruction_op = current_instruction.rtype.op;
    Instruction_funct = current_instruction.rtype.r4;
    // cout<<"op: "<<Instruction_op<<endl;
    // cout<<"r0: "<<current_instruction.rtype.r0<<endl;
    // cout<<"r1: "<<current_instruction.rtype.r1<<endl;
    // cout<<"r2: "<<current_instruction.rtype.r2<<endl;
    // cout<<"r3: "<<current_instruction.rtype.r3<<endl;
    // cout<<"r4: "<<current_instruction.rtype.r4<<endl;
    switch(Instruction_op)
    {
        //R-type
        case 0:
            setRtypeRegister();
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
                puts("Parse Error: R-type instruction not found");
                ProgramCounter += 4;
            }
            break;

        //J type
        case 2:
            setJtypeRegister();
            j();
            Instruction_count[6]++;
            break;
        case 3:
            setJtypeRegister();
            jal();
            // ProgramCounter += 4;
            Instruction_count[7]++;
            break;

        // I-type
        case 4:
            setItypeRegister();
            beq();
            ProgramCounter += 4;
            Instruction_count[8]++;
            break;


        case 5:
            setItypeRegister();
            bne();
            ProgramCounter += 4;
            Instruction_count[9]++;
            break;

        case 8:
            setItypeRegister();
            addi();
            ProgramCounter += 4;
            Instruction_count[10]++;
            break;

        case 10:
            setItypeRegister();
            slti();
            ProgramCounter += 4;
            Instruction_count[11]++;
            break;

        case 13:
            setItypeRegister();
            ori();
            ProgramCounter += 4;
            Instruction_count[12]++;
            break;

        case 15:
            setItypeRegister();
            lui();
            ProgramCounter += 4;
            Instruction_count[13]++;
            break;
        case 17:
            setFtypeRegister();
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
                puts("Parse Error: fpu instruction not found");
            }
            ProgramCounter += 4;
            break;

        case 35:
            setItypeRegister();
            lw();
            ProgramCounter += 4;
            Instruction_count[25]++;
            break;
        case 43:
            setItypeRegister();
            sw();
            ProgramCounter += 4;
            Instruction_count[26]++;
            break;
        case 49:
            setItypeRegister();
            lwc1();
            ProgramCounter += 4;
            Instruction_count[27]++;
            break;
        case 57:
            setItypeRegister();
            swc1();
            ProgramCounter += 4;
            Instruction_count[28]++;
            break;
// Rtype 
        case 60:
            setRtypeRegister();
            readf();
            ProgramCounter += 4;
            Instruction_count[29]++;
            break;
        case 61:
            setRtypeRegister();
            readi();
            ProgramCounter += 4;
            Instruction_count[30]++;
            break;
        case 62:
            setRtypeRegister();
            outc();
            ProgramCounter += 4;
            Instruction_count[31]++;
            break;
        case 63:
            setRtypeRegister();
            outi();
            ProgramCounter += 4;
            Instruction_count[32]++;
            break;
        default:
            cout<<"Parse Error: Invalid instruction_op received"<<endl;
    }
}


void execute()
{
    getchar(); //to remove effect of pressing enter key while starting
    long long prev_pc;
    myfile.open ("print_output.txt", std::fstream::in | std::fstream::out | std::fstream::trunc);
    myfile2.open("result.ppm", std::fstream::in | std::fstream::out | std::fstream::trunc);
    // print_pc.open ("print_pc.txt", std::fstream::in | std::fstream::out | std::fstream::trunc);

    print_register.open ("print_register.txt", std::fstream::in | std::fstream::out | std::fstream::trunc);
    // print_register2.open ("print_register2.txt", std::fstream::in | std::fstream::out | std::fstream::trunc);
    print_analysis.open("print_analysis.txt", std::fstream::in | std::fstream::out | std::fstream::trunc);
    unsigned int sec;
    int nsec;
    double d_sec;

    struct timespec start_time, end_time;
    clock_gettime(CLOCK_REALTIME, &start_time);

    while(prev_pc!= ProgramCounter && ProgramCounter<NumberOfInstructions*4&& count_exec<limit_of_exec)
    {
        prev_pc = ProgramCounter;

        // auto start = chrono::steady_clock::now();
        ReadInstruction(ProgramCounter/4); //set current_instruction
        // auto end = chrono::steady_clock::now();
        // cout << "Elapsed time in read : "
        // << chrono::duration_cast<chrono::nanoseconds>(end - start).count()
        // << " ns" << endl;

        // start = chrono::steady_clock::now();
        // ExecuteInstruction(); //call appropriate operation function based on the op and increment program counter
        ParseAndExecuteInstruction();
        // end = chrono::steady_clock::now();
        // cout << "Elapsed time in execute : "
        // << chrono::duration_cast<chrono::nanoseconds>(end - start).count()
        // << " ns" << endl;

        if (count_exec%50000000==0 && count_exec != 0){
          cout<<"current_count_exec:"<<count_exec<<endl;
        }

        count_exec++;

        if(Mode == 0){
          print_register<<"count_exec:"<<count_exec<<endl;
          PrintRegister();
          print_register<<"***** PC after　execution: *****"<<ProgramCounter<<endl;
        }
    }

    clock_gettime(CLOCK_REALTIME, &end_time);
    sec = end_time.tv_sec - start_time.tv_sec;
    nsec = end_time.tv_nsec - start_time.tv_nsec;
    d_sec = (double)sec + (double)nsec/(1000 * 1000 * 1000);

    cout <<"*********end of execution**********"<< endl;
    cout << "time_consumed:"<< d_sec << endl;
    cout<<" execution_count: "<<count_exec<<endl;
    PrintRegister();

    /* print_analysis*/ 
    for (int i = 0; i <= 32; i++){
            print_analysis<<Instructions[i]<<": "<<Instruction_count[i]<<endl;
    }
    print_analysis<<"maximum index of memory being used:"<< max_memory_index <<endl;


    myfile.close();
    myfile2.close();
    print_analysis.close();
    // myfile3.close();
    // print_pc.close();
    print_register.close();
    // print_register2.close();
    if(inputFileCheck == 1){
        InputFile.close();
    }

}


int main()
{

    cout<<endl<<"MIPS Simulator - team7"<<endl<<endl;
    cout<<"入力形式： [binary file path] ＋ [実行行数]"<<endl;
    cout<<"入力例：  ./sample_binary.txt  5  << "<<endl;
    cout<<"ただし、[実行行数]=0の場合は全ての命令を実行する。 "<<endl;
    cin>>fileName>>num_input;
    cout<<"Do you want to print registers by step? Enter 1 for yes, and 0 for no  <<"<<endl;
    cin >> Mode;
    Mode = 1-Mode;
    cout<<"Do you have any input file? Enter 1 for yes, and 0 for no  << "<<endl;
    cin >> inputFileCheck;
    if(inputFileCheck == 1){
        cout<<"Please enter the path of input file"<<endl;
        cin>>fileName2;
        InputFile.open(fileName2.c_str(),ios::in);
        if(!InputFile)
        {
            cout<<"Error: Input file does not exist or could not be opened"<<endl;
            exit(1);
        }
        cout<<"Execution has started with input file."<<endl;
    }else{
        cout<<"Execution has started without input file."<<endl;
    }
    init();
    execute();
    return 0;
}
