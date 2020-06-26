 #include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct instruction_ {
    char operation[8];
    char rs[5];
    char rt[5];
    char rd[5];
    int  sa;
} Instruction;

void scanInstruction(Instruction* instruction);

int compile(Instruction* instruction);

int regPrefixBaseValue(char* prefix);

int regToInt (char* reg);

int operationToFuncInt(char* operation);

int argumentOrder(char* operation);

void intToBinaryString(int number, char* string);

void printResult(Instruction* instruction);


int main() {
    printf("Enter an R-format instruction:\n");
    Instruction test;
    scanInstruction(&test);
    printResult(&test);

    return 0;
}

void scanInstruction(Instruction* instruction) {

    instruction->rs[0] = '0';
    instruction->rt[0] = '0';
    instruction->rd[0] = '0';
    instruction->rs[1] = 0;
    instruction->rt[1] = 0;
    instruction->rd[1] = 0;
    instruction->sa = 0;

    scanf("%s", instruction->operation);

    char regs[32];
    char* reg;
    const char s[2] = ",";

    scanf("%[^\n]", regs);

    reg = strtok(regs, s);

    int mask = 0xf00;
    for (int i = 0; i < 3; i++) {
        int regLocation = (mask & argumentOrder(instruction->operation)) >> (2 - i) * 4;
        if (regLocation == 1) {
            sscanf(reg, "%s", instruction->rs);
        } else if (regLocation == 2) {
            sscanf(reg, "%s", instruction->rt);
        } else if (regLocation == 3) {
            sscanf(reg, "%s", instruction->rd);
        } else if (regLocation == 4) {
            sscanf(reg, "%d", &(instruction->sa));
        }
        mask = mask >> 4;
        reg = strtok(NULL, s);
    }
}

int compile(Instruction* instruction) {
    return
            operationToFuncInt(instruction->operation)
            + (instruction->sa << 6)
            + (regToInt(instruction->rd) << 11)
            + (regToInt(instruction->rt) << 16)
            + (regToInt(instruction->rs) << 21);
}

int regPrefixBaseValue(char* prefix) {
    if (strcmp(prefix, "zero") == 0) return 0;
    if (strcmp(prefix, "at"  ) == 0) return 1;
    if (strcmp(prefix, "v"   ) == 0) return 2;
    if (strcmp(prefix, "a"   ) == 0) return 4;
    if (strcmp(prefix, "t"   ) == 0) return 8;
    if (strcmp(prefix, "s"   ) == 0) return 16;
    if (strcmp(prefix, "k"   ) == 0) return 26;
    if (strcmp(prefix, "gp"  ) == 0) return 28;
    if (strcmp(prefix, "sp"  ) == 0) return 29;
    if (strcmp(prefix, "fp"  ) == 0) return 30;
    if (strcmp(prefix, "ra"  ) == 0) return 31;
    return -1;
}

int regToInt (char* reg) {

    char prefix[32];
    char digit[3];


    sscanf(reg, "%[^0-9]", prefix);
    int baseValue = regPrefixBaseValue(prefix);

    if (baseValue < 0) {
        baseValue = 0;
        sscanf(reg, "%[0-9]", digit);
    } else {
        sscanf(reg, "%*[^0-9]%1[0-9]", digit);
    }


    int digitValue = atoi(digit);


    if (strcmp(prefix, "t") == 0 && digitValue > 7) {
        digitValue += 8;
    }

    return baseValue + digitValue;
}

int operationToFuncInt(char* operation) {
    if (strcmp(operation, "add"    ) == 0) return 0b100000;
    if (strcmp(operation, "addu"   ) == 0) return 0b100001;
    if (strcmp(operation, "and"    ) == 0) return 0b100100;
    if (strcmp(operation, "break"  ) == 0) return 0b001101;
    if (strcmp(operation, "div"    ) == 0) return 0b011010;
    if (strcmp(operation, "divu"   ) == 0) return 0b011011;
    if (strcmp(operation, "jalr"   ) == 0) return 0b001001;
    if (strcmp(operation, "jr"     ) == 0) return 0b001000;
    if (strcmp(operation, "mfhi"   ) == 0) return 0b010000;
    if (strcmp(operation, "mflo"   ) == 0) return 0b010010;
    if (strcmp(operation, "mthi"   ) == 0) return 0b010001;
    if (strcmp(operation, "mtlo"   ) == 0) return 0b010011;
    if (strcmp(operation, "mult"   ) == 0) return 0b011000;
    if (strcmp(operation, "multu"  ) == 0) return 0b011001;
    if (strcmp(operation, "nor"    ) == 0) return 0b100111;
    if (strcmp(operation, "or"     ) == 0) return 0b100101;
    if (strcmp(operation, "sll"    ) == 0) return 0b000000;
    if (strcmp(operation, "sllv"   ) == 0) return 0b000100;
    if (strcmp(operation, "slt"    ) == 0) return 0b101010;
    if (strcmp(operation, "sltu"   ) == 0) return 0b101011;
    if (strcmp(operation, "sra"    ) == 0) return 0b000011;
    if (strcmp(operation, "srav"   ) == 0) return 0b000111;
    if (strcmp(operation, "srl"    ) == 0) return 0b000010;
    if (strcmp(operation, "srlv"   ) == 0) return 0b000110;
    if (strcmp(operation, "sub"    ) == 0) return 0b100010;
    if (strcmp(operation, "subu"   ) == 0) return 0b100011;
    if (strcmp(operation, "syscall") == 0) return 0b001100;
    if (strcmp(operation, "xor"    ) == 0) return 0b100110;
    return 0;
}


void intToBinaryString(int number, char* string) {
    for (int i = 0; i < 32; i++) {
        int mask = 0x1 << i;
        string[31 - i] = number & mask ? '1' : '0';
    }
    string[32] = 0;
}

void printResult(Instruction* instruction) {
    char result[33];
    intToBinaryString(compile(instruction), result);
    printf(
            "Operation: %s\n"
            "Rs: %s (R%d)\n"
            "Rt: %s (R%d)\n"
            "Rd: %s (R%d)\n"
            "Shamt: %d\n"
            "Funct: %d\n"
            "Machine code: %s",
            instruction->operation,
            instruction->rs,
            regToInt(instruction->rs),
            instruction->rt,
            regToInt(instruction->rt),
            instruction->rd,
            regToInt(instruction->rd),
            instruction->sa,
            operationToFuncInt(instruction->operation),
            result
            );
}
