// There is a bug on the line below. Figure it out, or kontr compilation fails.
#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include "cpu.h"



static bool isOKIndex(struct cpu *cpu, int32_t index);

#ifdef BONUS_CALL
static int call(struct cpu *cpu);

static int ret(struct cpu *cpu);
#endif

#ifdef BONUS_JMP
static int cmp(struct cpu *cpu);

static int jmp(struct cpu *cpu);

static int jz(struct cpu *cpu);

static int jnz(struct cpu *cpu);

static int jgt(struct cpu *cpu);
#endif

static int nop(struct cpu *cpu);

static int halt(struct cpu *cpu);

static int add(struct cpu *cpu);

static int sub(struct cpu *cpu);

static int mul(struct cpu *cpu);

static int diva(struct cpu *cpu);

static int inc(struct cpu *cpu);

static int dec(struct cpu *cpu);

static int loop(struct cpu *cpu);

static int movr(struct cpu *cpu);

static int load(struct cpu *cpu);

static int store(struct cpu *cpu);

static int inn(struct cpu *cpu);

static int get(struct cpu *cpu);

static int out(struct cpu *cpu);

static int put(struct cpu *cpu);

static int swap(struct cpu *cpu);

static int push(struct cpu *cpu);

static int pop(struct cpu *cpu);


int32_t *cpuCreateMemory(FILE *program, size_t stackCapacity, int32_t **stackBottom)
{
    assert(program != NULL);
    assert(stackBottom != NULL);
    char *memory = NULL;
    char *buffer = NULL;
    size_t allocSize = 0;
    size_t position = 0;
    int ch = fgetc(program);
    while (ch != EOF) {
        if (position + 1 > allocSize) {
            buffer = memory;
            allocSize += 4096;
            memory = (char *) realloc(memory, allocSize);
            if (!memory) {
                free(buffer);
                return NULL;
            }
        }
        memory[position++] = ch;
        ch = fgetc(program);
    }
    if (position != 0 && (position < sizeof(int32_t) || position % sizeof(int32_t) != 0)) {
        free(memory);

        return NULL;
    }
    while (allocSize - position < stackCapacity * sizeof(int32_t)) {
        allocSize += 4096;
        buffer = memory;
        memory = (char *) realloc(memory, allocSize);
        if (!memory) {
            free(buffer);
            return NULL;
        }
    }
    if (!memory) {

        return NULL;
    }
    memset(memory + position, 0, allocSize - position - stackCapacity * sizeof(int32_t));
    *stackBottom = (int32_t *) memory + allocSize / sizeof(int32_t) - 1;
    return (int32_t *) memory;
}

void cpuCreate(struct cpu *cpu, int32_t *memory, int32_t *stackBottom, size_t stackCapacity)
{
    assert(cpu != NULL);
    assert(memory != NULL);
    assert(stackBottom != NULL);
    cpu->stackSize = 0;
    cpu->memory = memory;
    cpu->stackBottom = stackBottom;
    #if  defined(BONUS_JMP)
    cpu->result = 0;
    #endif
    cpu->A = 0;
    cpu->B = 0;
    cpu->C = 0;
    cpu->D = 0;
    cpu->instructionPointer = 0;
    cpu->stackLimit = stackBottom - stackCapacity;
    cpu->status = cpuOK;
}

void cpuDestroy(struct cpu *cpu)
{
    assert(cpu != NULL);
    cpu->stackSize = 0;
    free(cpu->memory);
    cpu->memory = NULL;
    cpu->stackBottom = NULL;
    #if  defined(BONUS_JMP)
    cpu->result = 0;
    #endif
    cpu->A = 0;
    cpu->B = 0;
    cpu->C = 0;
    cpu->D = 0;
    cpu->instructionPointer = 0;
    cpu->stackLimit = NULL;
    cpu->status = cpuOK;
}

void cpuReset(struct cpu *cpu)
{
    assert(cpu != NULL);
    size_t size = cpu->stackBottom - cpu->stackLimit;
    memset(cpu->stackLimit + 1, 0, size * sizeof(int32_t));
    cpu->instructionPointer = 0;
    cpu->stackSize = 0;
    #if  defined(BONUS_JMP)
    cpu->result = 0;
    #endif
    cpu->A = 0;
    cpu->B = 0;
    cpu->C = 0;
    cpu->D = 0;
    cpu->status = cpuOK;
}

int cpuStatus(struct cpu *cpu)
{
    assert(cpu != NULL);
    return cpu->status;
}


#if  defined(BONUS_JMP) || defined(BONUS_CALL)
int cpuStep(struct cpu *cpu)
{
    assert(cpu != NULL);
    if (cpu->status != cpuOK) {
        return 0;
    }

    if (!isOKIndex(cpu, cpu->instructionPointer)) {
        return 0;
    }
    int32_t currentInstruction = cpu->memory[cpu->instructionPointer];
    if (0 > currentInstruction || currentInstruction > 25) {
        cpu->status = cpuIllegalInstruction;
        return 0;
    }
    int (*instructions[26])(struct cpu *cpu) = {&nop, &halt,&add, &sub, &mul, &diva, &inc, &dec, &loop, &movr, &load, &store, &inn, &get, &out, &put, &swap, &push, &pop, &cmp, &jmp, &jz, &jnz, &jgt, &call, &ret};

    instructions[currentInstruction](cpu);

    if (cpu->status != cpuOK) {
        return 0;
    }

    return 1;
}

#else
int cpuStep(struct cpu *cpu)
{
    assert(cpu != NULL);
    if (cpu->status != cpuOK) {
        return 0;
    }

    if (!isOKIndex(cpu, cpu->instructionPointer)) {
        return 0;
    }
    int32_t currentInstruction = cpu->memory[cpu->instructionPointer];
    if (0 > currentInstruction || currentInstruction > 18) {
        cpu->status = cpuIllegalInstruction;
        return 0;
    }
    int (*instructions[19])(struct cpu *cpu) = {&nop, &halt,&add, &sub, &mul, &diva, &inc, &dec, &loop, &movr, &load, &store, &inn, &get, &out, &put, &swap, &push, &pop};

    instructions[currentInstruction](cpu);

    if (cpu->status != cpuOK) {
        return 0;
    }

    return 1;
}
#endif

int cpuRun(struct cpu *cpu, size_t steps)
{
    assert(cpu != NULL);
    if (cpu->status != cpuOK) {
        return 0;
    }

    for (size_t i = 1; i <= steps; i++) {

        if (cpuStep(cpu) != 1 && cpu->status != cpuHalted) {
            return -i;
        } else if (cpu->status == cpuHalted) {
            return i;
        }
    }
    return steps;
}

int32_t cpuPeek(struct cpu *cpu, char reg)
{
    assert(cpu != NULL);

    if (reg == 'S') {
        return cpu->stackSize;
    }
    if (reg == 'I') {
        return cpu->instructionPointer;
    }
    if (reg == 'A') {
        return cpu->A;
    }
    if (reg == 'B') {
        return cpu->B;
    }
    if (reg == 'C') {
        return cpu->C;
    }
    if (reg == 'D') {
        return cpu->D;
    }
    #if  defined(BONUS_JMP)
    if (reg == 'R') {
        return cpu->result;
    }
    #endif
    return 0;
}


static bool isOKIndex(struct cpu *cpu, int32_t index)
{
    assert(cpu != NULL);

    if (index < 0 || index > cpu->stackLimit - cpu->memory) {
        cpu->status = cpuInvalidAddress;
        return false;
    }
    return true;
}

#ifdef BONUS_CALL
static int call(struct cpu *cpu) 
    assert(cpu != NULL);
    if (!isOKIndex(cpu, cpu->instructionPointer + 1)) {
        return 0;
    }
    int32_t index = cpu->memory[cpu->instructionPointer + 1];
    if (0 > index || index >= cpu->stackSize) {
        cpu->status = cpuInvalidStackOperation;
        return 0;
    }
    cpu->stackBottom[-cpu->stackSize] = cpu->instructionPointer;
    cpu->stackSize++;
    cpu->instructionPointer = cpu->memory[cpu->instructionPointer + 1];
    return 1;

static int ret(struct cpu *cpu) 
{
    assert(cpu != NULL);
    if (!isOKIndex(cpu, cpu->instructionPointer + 1)) {
        return 0;
    }
    int32_t index = cpu->stackBottom[-cpu->stackSize + 1];
    cpu->stackBottom[-cpu->stackSize + 1] = 0;
    cpu->stackSize--;
    cpu->instructionPointer += index;
}

    
#endif

#ifdef BONUS_JMP
static int cmp(struct cpu *cpu) 
{
    assert(cpu != NULL);
    if (!isOKIndex(cpu, cpu->instructionPointer + 1) || !isOKIndex(cpu, cpu->instructionPointer + 2)) {
        return 0;
    }
    int32_t currentOperand = cpu->memory[cpu->instructionPointer + 1];
    int32_t currentOperand2 = cpu->memory[cpu->instructionPointer + 2];
    if (currentOperand > 3 || currentOperand < 0 || currentOperand2 > 3 || currentOperand2 < 0) {
        cpu->status = cpuIllegalOperand;
        return 0;
    }
    int32_t *registers[4] = { &cpu->A, &cpu->B, &cpu->C, &cpu->D };
    cpu->result = *registers[currentOperand] - *registers[currentOperand2];
    cpu->instructionPointer += 3;
    return 1;
}

static int jmp(struct cpu *cpu) 
{
    assert(cpu != NULL);
    if (!isOKIndex(cpu, cpu->instructionPointer + 1)) {
        cpu->status = cpuInvalidAddress;
        return 0;
    }
    cpu->instructionPointer = cpu->memory[cpu->instructionPointer + 1];
    return 1;
}

static int jz(struct cpu *cpu)
{
    assert(cpu != NULL);
    if (cpu->result == 0) {
        if (!isOKIndex(cpu, cpu->instructionPointer + 1)) {
            cpu->status = cpuInvalidAddress;
            return 0;
        }
        cpu->instructionPointer = cpu->memory[cpu->instructionPointer + 1];
        return 1;
    }
    cpu->instructionPointer += 2;
    return 1;
}

static int jnz(struct cpu *cpu)
{
    assert(cpu != NULL);
    if (cpu->result != 0) {
        if (!isOKIndex(cpu, cpu->instructionPointer + 1)) {
            cpu->status = cpuInvalidAddress;
            return 0;
        }
        cpu->instructionPointer = cpu->memory[cpu->instructionPointer + 1];
        return 1;
    }
    cpu->instructionPointer += 2;
    return 1;
}

static int jgt(struct cpu *cpu)
{
    assert(cpu != NULL);
    if (cpu->result > 0) {
        if (!isOKIndex(cpu, cpu->instructionPointer + 1)) {
            cpu->status = cpuInvalidAddress;
            return 0;
        }
        cpu->instructionPointer = cpu->memory[cpu->instructionPointer + 1];
        return 1;
    }
    cpu->instructionPointer += 2;
    return 1;
}

#endif

static int nop(struct cpu *cpu)
{
    assert(cpu != NULL);
    cpu->instructionPointer++;
    return 1;
}

static int halt(struct cpu *cpu)
{
    assert(cpu != NULL);

    cpu->instructionPointer++;
    cpu->status = cpuHalted;
    return 0;
}

static int add(struct cpu *cpu)
{
    assert(cpu != NULL);

    if (!isOKIndex(cpu, cpu->instructionPointer + 1)) {
        return 0;
    }
    int32_t currentOperand = cpu->memory[cpu->instructionPointer + 1];
    if (currentOperand > 3 || currentOperand < 0) {
        cpu->status = cpuIllegalOperand;
        return 0;
    }
    int32_t *registers[4] = { &cpu->A, &cpu->B, &cpu->C, &cpu->D };
    cpu->A += *registers[currentOperand];
    cpu->instructionPointer += 2;
    #if  defined(BONUS_JMP)
    cpu->result = cpu->A;
    #endif
    return 1;
}


static int sub(struct cpu *cpu)
{
    assert(cpu != NULL);
    if (!isOKIndex(cpu, cpu->instructionPointer + 1)) {
        return 0;
    }
    int32_t currentOperand = cpu->memory[cpu->instructionPointer + 1];
    if (currentOperand > 3 || currentOperand < 0) {
        cpu->status = cpuIllegalOperand;
        return 0;
    }
    int32_t *registers[4] = { &cpu->A, &cpu->B, &cpu->C, &cpu->D };
    cpu->A -= *registers[currentOperand];
    cpu->instructionPointer += 2;
    #if  defined(BONUS_JMP)
    cpu->result = cpu->A;
    #endif
    return 1;
}

static int mul(struct cpu *cpu)
{
    assert(cpu != NULL);
    if (!isOKIndex(cpu, cpu->instructionPointer + 1)) {
        return 0;
    }
    int32_t currentOperand = cpu->memory[cpu->instructionPointer + 1];
    if (currentOperand > 3 || currentOperand < 0) {
        cpu->status = cpuIllegalOperand;
        cpu->instructionPointer += 2;
        return 0;
    }
    int32_t *registers[4] = { &cpu->A, &cpu->B, &cpu->C, &cpu->D };
    cpu->A *= *registers[currentOperand];
    cpu->instructionPointer += 2;
    #if  defined(BONUS_JMP)
    cpu->result = cpu->A;
    #endif
    return 1;
}

static int diva(struct cpu *cpu)
{
    assert(cpu != NULL);
    if (!isOKIndex(cpu, cpu->instructionPointer + 1)) {
        return 0;
    }
    int32_t currentOperand = cpu->memory[cpu->instructionPointer + 1];
    if (currentOperand > 3 || currentOperand < 0) {
        cpu->status = cpuIllegalOperand;
        return 0;
    }
    int32_t *registers[4] = { &cpu->A, &cpu->B, &cpu->C, &cpu->D };
    if (*registers[currentOperand] == 0) {
        cpu->status = cpuDivByZero;
        return 0;
    }
    cpu->A /= *registers[currentOperand];
    cpu->instructionPointer += 2;
    #if  defined(BONUS_JMP)
    cpu->result = cpu->A;
    #endif
    return 1;
}

static int inc(struct cpu *cpu)
{
    assert(cpu != NULL);
    if (!isOKIndex(cpu, cpu->instructionPointer + 1)) {
        return 0;
    }
    int32_t currentOperand = cpu->memory[cpu->instructionPointer + 1];
    if (currentOperand > 3 || currentOperand < 0) {
        cpu->status = cpuIllegalOperand;
        return 0;
    }
    int32_t *registers[4] = { &cpu->A, &cpu->B, &cpu->C, &cpu->D };
    *registers[currentOperand] += 1;
    cpu->instructionPointer += 2;
    #if  defined(BONUS_JMP)
    cpu->result = *registers[currentOperand];
    #endif
    return 1;
}

static int dec(struct cpu *cpu)
{
    assert(cpu != NULL);
    if (!isOKIndex(cpu, cpu->instructionPointer + 1)) {
        return 0;
    }
    int32_t currentOperand = cpu->memory[cpu->instructionPointer + 1];
    if (currentOperand > 3 || currentOperand < 0) {
        cpu->status = cpuIllegalOperand;
        return 0;
    }
    int32_t *registers[4] = { &cpu->A, &cpu->B, &cpu->C, &cpu->D };
    *registers[currentOperand] -= 1;
    #if  defined(BONUS_JMP)
    cpu->result = *registers[currentOperand];
    #endif
    cpu->instructionPointer += 2;
    return 1;
}

static int loop(struct cpu *cpu)
{
    assert(cpu != NULL);
    if (cpu->C != 0) {
        if (!isOKIndex(cpu, cpu->instructionPointer + 1)) {
            cpu->status = cpuInvalidAddress;
            return 0;
        }
        cpu->instructionPointer = cpu->memory[cpu->instructionPointer + 1] - 2;
    }
    cpu->instructionPointer += 2;
    return 1;
}


static int movr(struct cpu *cpu)
{
    assert(cpu != NULL);
    if (!isOKIndex(cpu, cpu->instructionPointer + 1) || !isOKIndex(cpu, cpu->instructionPointer + 2)) {
        return 0;
    }
    int32_t currentOperand = cpu->memory[cpu->instructionPointer + 1];
    int32_t num = cpu->memory[cpu->instructionPointer + 2];
    if (currentOperand > 3 || currentOperand < 0) {
        cpu->status = cpuIllegalOperand;
        return 0;
    }
    int32_t *registers[4] = { &cpu->A, &cpu->B, &cpu->C, &cpu->D };
    *registers[currentOperand] = num;
    cpu->instructionPointer += 3;

    return 1;
}

static int load(struct cpu *cpu)
{
    assert(cpu != NULL);
    if (!isOKIndex(cpu, cpu->instructionPointer + 1) || !isOKIndex(cpu, cpu->instructionPointer + 2)){
        return 0;
    }
    int32_t currentOperand = cpu->memory[cpu->instructionPointer + 1];
    if (currentOperand > 3 || currentOperand < 0) {
        cpu->status = cpuIllegalOperand;
        return 0;
    }
    int32_t num = cpu->memory[cpu->instructionPointer + 2];
    int32_t index = cpu->D + num;
    if (0 > index || index >= cpu->stackSize) {
        cpu->status = cpuInvalidStackOperation;
        return 0;
    }
    int32_t *registers[4] = { &cpu->A, &cpu->B, &cpu->C, &cpu->D };
    *registers[currentOperand] = cpu->stackBottom[-cpu->stackSize + 1 + index];
    cpu->instructionPointer += 3;
    return 1;
}

static int store(struct cpu *cpu)
{
    assert(cpu != NULL);
    if (!isOKIndex(cpu, cpu->instructionPointer + 1) || !isOKIndex(cpu, cpu->instructionPointer + 2)) {
        return 0;
    }
    int32_t currentOperand = cpu->memory[cpu->instructionPointer + 1];
    #if  defined(BONUS_JMP)
    if (currentOperand == 4) {
        currentOperand = 3;
    }
    if (currentOperand > 4 || currentOperand < 0) {
        cpu->status = cpuIllegalOperand;
        return 0;
    }
    int32_t *registers[5] = { &cpu->A, &cpu->B, &cpu->C, &cpu->D, &cpu->result };
    #else
    if (currentOperand > 3 || currentOperand < 0) {
        cpu->status = cpuIllegalOperand;
        return 0;
    }
    int32_t *registers[4] = { &cpu->A, &cpu->B, &cpu->C, &cpu->D };
    #endif
    int32_t num = cpu->memory[cpu->instructionPointer + 2];
    int32_t index = cpu->D + num;
    if (0 > index || index >= cpu->stackSize) {
        cpu->status = cpuInvalidStackOperation;
        return 0;
    }
    cpu->stackBottom[-cpu->stackSize +  1 + index] = *registers[currentOperand];
    cpu->instructionPointer += 3;
    return 1;
}

static int inn(struct cpu *cpu)
{
    assert(cpu != NULL);
    if (!isOKIndex(cpu, cpu->instructionPointer + 1)) {
        return 0;
    }
    int32_t currentOperand = cpu->memory[cpu->instructionPointer + 1];
    if (currentOperand > 3 || currentOperand < 0) {
        cpu->status = cpuIllegalOperand;
        return 0;
    }
    int64_t num = 0;
    int32_t *registers[4] = { &cpu->A, &cpu->B, &cpu->C, &cpu->D };
    int result = scanf("%ld", &num);
    if (result == -1) {
        cpu->C = 0;
        *registers[currentOperand] = -1;
        cpu->instructionPointer += 2;
        return 1;
    }
    if (result == 0 || INT32_MIN > num || num > INT32_MAX) {
        cpu->status = cpuIOError;
        return 0;
    }
    *registers[currentOperand] = (int32_t) num;
    cpu->instructionPointer += 2;
    return 1;
}

static int get(struct cpu *cpu)
{
    assert(cpu != NULL);
    if (!isOKIndex(cpu, cpu->instructionPointer + 1)) {
        return 0;
    }
    int32_t currentOperand = cpu->memory[cpu->instructionPointer + 1];
    if (currentOperand > 3 || currentOperand < 0) {
        cpu->status = cpuIllegalOperand;
        return 0;
    }
    int32_t *registers[4] = { &cpu->A, &cpu->B, &cpu->C, &cpu->D };
    int32_t num = getchar();
    if (num == EOF) {
        cpu->C = 0;
        *registers[currentOperand] = -1;
        cpu->instructionPointer += 2;
        return 1;
    }
    *registers[currentOperand] = num;
    cpu->instructionPointer += 2;
    return 1;
}

static int out(struct cpu *cpu)
{
    assert(cpu != NULL);
    if (!isOKIndex(cpu, cpu->instructionPointer + 1)) {
        return 0;
    }
    int32_t currentOperand = cpu->memory[cpu->instructionPointer + 1];
    #if  defined(BONUS_JMP)
    if (currentOperand == 4) {
        currentOperand = 3;
    }
    if (currentOperand > 4 || currentOperand < 0) {
        cpu->status = cpuIllegalOperand;
        return 0;
    }
    int32_t *registers[5] = { &cpu->A, &cpu->B, &cpu->C, &cpu->D, &cpu->result };
    #else
    if (currentOperand > 3 || currentOperand < 0) {
        cpu->status = cpuIllegalOperand;
        return 0;
    }
    int32_t *registers[5] = { &cpu->A, &cpu->B, &cpu->C, &cpu->D };
    #endif
    printf("%d", *registers[currentOperand]);
    cpu->instructionPointer += 2;
    return 1;
}

static int put(struct cpu *cpu)
{
    assert(cpu != NULL);
    if (!isOKIndex(cpu, cpu->instructionPointer + 1)) {
        return 0;
    }
    int32_t currentOperand = cpu->memory[cpu->instructionPointer + 1];
    if (currentOperand > 3 || currentOperand < 0) {
        cpu->status = cpuIllegalOperand;
        return 0;
    }
    int32_t *registers[4] = { &cpu->A, &cpu->B, &cpu->C, &cpu->D };
    int32_t ch = *registers[currentOperand];
    if (ch >= 256 || ch < 0) {
        cpu->status = cpuIllegalOperand;
        return 0;
    }
    putchar(ch);
    cpu->instructionPointer += 2;
    return 1;
}

static int swap(struct cpu *cpu)
{
    assert(cpu != NULL);
    if (!isOKIndex(cpu, cpu->instructionPointer + 1) || !isOKIndex(cpu, cpu->instructionPointer + 2)) {
        return 0;
    }
    int32_t currentOperand = cpu->memory[cpu->instructionPointer + 1];
    int32_t currentOperand2 = cpu->memory[cpu->instructionPointer + 2];
    if (currentOperand > 3 || currentOperand < 0 || currentOperand2 > 3 || currentOperand2 < 0) {
        cpu->status = cpuIllegalOperand;
        return 0;
    }
    int32_t *registers[4] = { &cpu->A, &cpu->B, &cpu->C, &cpu->D };
    int32_t swap = *registers[currentOperand];
    *registers[currentOperand] = *registers[currentOperand2];
    *registers[currentOperand2] = swap;
    cpu->instructionPointer += 3;
    return 1;
}

static int push(struct cpu *cpu)
{
    assert(cpu != NULL);
    if (!isOKIndex(cpu, cpu->instructionPointer + 1)) {
        return 0;
    }
    int32_t currentOperand = cpu->memory[cpu->instructionPointer + 1];
    #if  defined(BONUS_JMP)
    if (currentOperand == 4) {
        currentOperand = 3;
    }
    if (currentOperand > 4 || currentOperand < 0) {
        cpu->status = cpuIllegalOperand;
        return 0;
    }
    int32_t *registers[5] = { &cpu->A, &cpu->B, &cpu->C, &cpu->D, &cpu->result };
    #else
    if (currentOperand > 3 || currentOperand < 0) {
        cpu->status = cpuIllegalOperand;
        return 0;
    }
    int32_t *registers[5] = { &cpu->A, &cpu->B, &cpu->C, &cpu->D };
    #endif

    if (cpu->stackLimit + cpu->stackSize >= cpu->stackBottom) {
        cpu->status = cpuInvalidStackOperation;
        return 0;
    }
    cpu->stackBottom[-cpu->stackSize] = *registers[currentOperand];
    cpu->stackSize++;
    cpu->instructionPointer += 2;
    return 1;
}

static int pop(struct cpu *cpu)
{
    assert(cpu != NULL);
    if (!isOKIndex(cpu, cpu->instructionPointer + 1)) {
        return 0;
    }
    int32_t currentOperand = cpu->memory[cpu->instructionPointer + 1];
    if (currentOperand > 3 || currentOperand < 0) {
        cpu->status = cpuIllegalOperand;
        return 0;
    }
    if (cpu->stackSize <= 0) {
        cpu->status = cpuInvalidStackOperation;
        return 0;
    }

    int32_t *registers[4] = { &cpu->A, &cpu->B, &cpu->C, &cpu->D };
    *registers[currentOperand] = cpu->stackBottom[-cpu->stackSize + 1];
    cpu->stackBottom[-cpu->stackSize + 1] = 0;
    cpu->stackSize--;
    cpu->instructionPointer += 2;
    return 1;
}