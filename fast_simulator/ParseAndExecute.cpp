using namespace std;
void setItypeRegister(){
        r[0] = current_instruction.itype.r0;
        r[1] = current_instruction.itype.r1;
        r[2] = current_instruction.itype.imm;
        // if (r[2]>32768){ 
        if (r[2]>131072){ 
            //65536=10000000000000000
          // r[2] = -(65536-r[2]);
          r[2] = -(262144-r[2]);
        }
        // Register_record[r[0]] += 1;
        // Register_record[r[1]] += 1;
}

void setRtypeRegister(){
        r[0] = current_instruction.rtype.r0;
        r[1] = current_instruction.rtype.r1;
        r[2] = current_instruction.rtype.r2;
        r[3] = current_instruction.rtype.r3;
        // Register_record[r[0]] += 1;
        // Register_record[r[1]] += 1;
        // Register_record[r[2]] += 1;
}

void setJtypeRegister(){
        r[0] = current_instruction.jtype.r0;
        // Register_record[r[0]] += 1;
}

void setFtypeRegister(){
        r[1] = current_instruction.rtype.r0;
        r[0] = current_instruction.rtype.r1;
        r[2] = current_instruction.rtype.r2;
        r[3] = current_instruction.rtype.r3;
        // FPU_Register_record[r[0]] += 1;
        // FPU_Register_record[r[1]] += 1;
        // FPU_Register_record[r[2]] += 1;
}

void f_sub(){
    if(r[3]==17){
        r[1] += 32;
    }else if(r[3]==18){
        r[0] += 32;
    }else if (r[3]==4){
        mtc1();
        Instruction_count[24]++;
        return;
    }
    // if (r[3]==16){
    switch(Instruction_funct)
    {

        case 0:
            fadd();
            Instruction_count[14]++;
            break;
        case 1:
            fsub();
            Instruction_count[15]++;
            break;
        case 2:
            fmul();
            Instruction_count[16]++;
            break;
        case 3:
            fdiv();
            Instruction_count[17]++;
            break;
        case 4:
            fsqrt();
            Instruction_count[18]++;
            break;
        case 5:
            floor();
            Instruction_count[19]++;
            break;
        case 6:
            ftoi();
            Instruction_count[20]++;
            break;
        case 7:
            itof();
            Instruction_count[21]++;
            break;
        case 60:
            fless();
            Instruction_count[22]++;
            break;
        case 50:
            ceqs();
            Instruction_count[23]++;
            break;
        default:
            puts("Parse Error: fpu instruction not found");
    }
}

void ParseAndExecuteInstruction()
{
    Instruction_op = current_instruction.rtype.op;
    Instruction_funct = current_instruction.rtype.r4;
    // cout<<"op: "<<Instruction_op<<endl;
    // cout<<"r0: "<<current_instruction.jtype.r0<<endl;
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
                ProgramCounter += 1;
                Instruction_count[0]++;
            }else if (Instruction_funct == 34){
                sub();
                ProgramCounter += 1;
                Instruction_count[1]++;
            }else if (Instruction_funct == 8){
                jr();
                Instruction_count[2]++;
            }else if (Instruction_funct == 42){
                slt();
                ProgramCounter += 1;
                Instruction_count[3]++;
            }else if(Instruction_funct == 0){
                sll();
                ProgramCounter += 1;
                Instruction_count[4]++;
            }else if(Instruction_funct == 2){
                srl();
                ProgramCounter += 1;
                Instruction_count[5]++;
            }else{
                puts("Parse Error: R-type instruction not found");
                ProgramCounter += 1;
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
            // ProgramCounter += 1;
            Instruction_count[7]++;
            break;

        // I-type
        case 4:
            setItypeRegister();
            beq();
            ProgramCounter += 1;
            Instruction_count[8]++;
            break;


        case 5:
            setItypeRegister();
            bne();
            ProgramCounter += 1;
            Instruction_count[9]++;
            break;

        case 8:
            setItypeRegister();
            addi();
            ProgramCounter += 1;
            Instruction_count[10]++;
            break;

        case 10:
            setItypeRegister();
            slti();
            ProgramCounter += 1;
            Instruction_count[11]++;
            break;

        case 13:
            setItypeRegister();
            ori();
            ProgramCounter += 1;
            Instruction_count[12]++;
            break;

        case 15:
            setItypeRegister();
            lui();
            ProgramCounter += 1;
            Instruction_count[13]++;
            break;
        case 17:
            setFtypeRegister();
            // if (r[3]==16){
            //     if(Instruction_funct == 0){
            //         fadd();
            //         Instruction_count[14]++;
            //     }else if(Instruction_funct == 1){
            //         fsub();
            //         Instruction_count[15]++;
            //     }else if(Instruction_funct == 2){
            //         fmul();
            //         Instruction_count[16]++;
            //     }else if(Instruction_funct == 3){
            //         fdiv();
            //         Instruction_count[17]++;
            //     }else if(Instruction_funct == 4){
            //         fsqrt();
            //         Instruction_count[18]++;
            //     }else if(Instruction_funct == 5){
            //         floor();
            //         Instruction_count[19]++;
            //     }else if(Instruction_funct == 6){
            //         ftoi();
            //         Instruction_count[20]++;
            //     }else if(Instruction_funct == 7){
            //         itof();
            //         Instruction_count[21]++;
            //     }else if(Instruction_funct == 60){
            //         fless();
            //         Instruction_count[22]++;
            //     }else if(Instruction_funct == 50){
            //         ceqs();
            //         Instruction_count[23]++;
            //     }
            // }else if (r[3]==4){
            //     mtc1();
            //     Instruction_count[24]++;
            // }else if (r[3] == 17){

            // }else{
            //     puts("Parse Error: fpu instruction not found");
            // }
            f_sub();
            ProgramCounter += 1;
            break;

        case 29:
            setItypeRegister();
            fori();
            ProgramCounter += 1;
            Instruction_count[33]++;
            break;
        case 31:
            setItypeRegister();
            flui();
            ProgramCounter += 1;
            Instruction_count[34]++;
            break;

        case 32:
            setItypeRegister();
            bslt();
            ProgramCounter += 1;
            // Instruction_count[8]++;
            break;
        case 33:
            setItypeRegister();
            bslti();
            ProgramCounter += 1;
            // Instruction_count[8]++;
            break;
        case 34:
            setItypeRegister();
            bfless();
            // Instruction_count[22]++;
            ProgramCounter += 1;
            break;
        case 35:
            setItypeRegister();
            lw();
            ProgramCounter += 1;
            Instruction_count[25]++;
            break;
        case 36:
            setItypeRegister();
            bfeq();
            // Instruction_count[22]++;
            ProgramCounter += 1;
            break;
        case 37:
            setItypeRegister();
            bflessi();
            // Instruction_count[22]++;
            ProgramCounter += 1;
            break;
        case 38:
            setItypeRegister();
            bflessi2();
            // Instruction_count[22]++;
            ProgramCounter += 1;
            break;
        case 43:
            setItypeRegister();
            sw();
            ProgramCounter += 1;
            Instruction_count[26]++;
            break;
        case 49:
            setItypeRegister();
            lwc1();
            ProgramCounter += 1;
            Instruction_count[27]++;
            break;
        case 57:
            setItypeRegister();
            swc1();
            ProgramCounter += 1;
            Instruction_count[28]++;
            break;
// Rtype 
        case 60:
            setRtypeRegister();
            readf();
            ProgramCounter += 1;
            Instruction_count[29]++;
            break;
        case 61:
            setRtypeRegister();
            readi();
            ProgramCounter += 1;
            Instruction_count[30]++;
            break;
        case 62:
            setRtypeRegister();
            outc();
            ProgramCounter += 1;
            Instruction_count[31]++;
            break;
        case 63:
            setRtypeRegister();
            outi();
            ProgramCounter += 1;
            Instruction_count[32]++;
            break;
        default:
            cout<<"Parse Error: Invalid instruction_op received"<<endl;
    }
}