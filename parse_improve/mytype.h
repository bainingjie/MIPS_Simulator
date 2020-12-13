using namespace std;
typedef union inst_t {
    uint32_t inst_32;
    struct rtype {
        unsigned int r4: 6,r3: 5,r2 : 5,r1: 5,r0: 5,op:6;
    }rtype;
    struct itype
    {   
        unsigned int imm : 16,r1:5,r0: 5,op:6; // -> signed
    }itype;
    struct jtype
    {   
        unsigned int r0: 26,op:6;
    }jtype;
}inst_t;

typedef union myfloat {
    float f;
    int32_t i;
} myfloat;


int32_t Memory[1024*1000];// memory
string Registers[32]; //array to store names of registers
int32_t RegisterValues[32]; //array to store values of CPU registers
float FPURegisterValues[32];//array to store values of FPU registers
int32_t r[4]; //to store the id of operand or constant

int32_t ProgramCounter;
long long count_exec;
long long int limit_of_exec;
int32_t NumberOfInstructions; //to store the number of lines in the program

int max_memory_index;
// long long count_outc;

int Instruction_count [33];
string Instructions [33];

std::fstream myfile;
std::fstream myfile2;
std::fstream print_register;
// std::fstream print_register2;
std::fstream print_analysis;
std::fstream InputFile;
vector<inst_t> InputProgram3;


// string current_instruction;
inst_t current_instruction;
// unsigned int current_instruction2;



int32_t Instruction_op;
int32_t Instruction_funct;

// int32_t mode;
string fileName;
string fileName2;
int32_t num_input;
int32_t Mode; //to store the Mode of execution
int inputFileCheck;
