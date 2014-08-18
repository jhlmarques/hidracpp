#include "cromagmachine.h"

CromagMachine::CromagMachine()
{
    registers = QVector<Register*>(2);
    QVector<Register*>::iterator i;
    for (i = registers.begin(); i != registers.end(); i++) {
        *i = new Register();
    }
    RA = registers[0];
    PC = registers[1];

    flags = QVector<Bit*>(3);
    QVector<Bit*>::iterator k;
    for (k = flags.begin(); k != flags.end(); k++) {
        *k = new Bit();
    }
    N = flags[0];
    Z = flags[1];
    C = flags[2];
    N->setValue(false);
    Z->setValue(true);
    C->setValue(false);

    memory = QVector<Byte*>(MEM_SIZE);
    QVector<Byte*>::iterator j;
    for (j = memory.begin(); j != memory.end(); j++) {
        *j = new Byte();
    }

    instructions = QVector<Instruction*>(16);
    instructions[0]  = new Instruction("nop",   0, 0);
    instructions[1]  = new Instruction("str",  16, 1);
    instructions[2]  = new Instruction("ldr",  32, 1);
    instructions[3]  = new Instruction("add",  48, 1);
    instructions[4]  = new Instruction( "or",  64, 1);
    instructions[5]  = new Instruction("and",  80, 1);
    instructions[6]  = new Instruction("not",  96, 0);
    instructions[8]  = new Instruction("jmp", 128, 1);
    instructions[9]  = new Instruction( "jn", 144, 1);
    instructions[10] = new Instruction( "jz", 160, 1);
    instructions[11] = new Instruction( "jc", 176, 1);
    instructions[14] = new Instruction("shr", 224, 0);
    instructions[15] = new Instruction("hlt", 240, 0);
}

void CromagMachine::printStatusDebug()
{
    std::cout << std::endl;
    std::cout << "RA: " << (int)RA->getValue() << std::endl;;
    std::cout << "N: " << (int)N->getValue() << "Z: " << (int)Z->getValue() << "C: " << (int)C->getValue();;
}

void CromagMachine::load(QString filename) {

}

void CromagMachine::save(QString filename){

}

void CromagMachine::step() {
    const Instruction *currentInstruction = getInstructionFromValue(memory[PC->getValue()]->getValue());
    Byte *operand;

    if(currentInstruction->getNumberOfArguments() == 1) {
        PC->incrementValue();

        Byte *endOperand = NULL;
        switch (currentInstruction->getValue() & 0x03) {
        case 0x00: // modo de enderecamento direto
            endOperand = memory[PC->getValue()];
            break;
        case 0x01: // modo indireto
            Byte *endPointer;
            endPointer = memory[PC->getValue()];
            endOperand = memory[endPointer->getValue()];
            break;
        }

        operand = memory[endOperand->getValue()];
    }

    PC->incrementValue();
    if(PC->getValue() == 0) { // ADICIONAR BREAKPOINT
        this->running = false;
    }

    switch (currentInstruction->getValue()) {
    case 0x00: // nop
        break;
    case 0x10: // str
        operand->setValue(RA->getValue());
        break;
    case 0x20: // ldr
        RA->setValue(operand->getValue());
        N->setValue(RA->getValue() > MAX_VALUE_SIGN);
        Z->setValue(RA->getValue() == 0);
        C->setValue(false);
        break;
    case 0x30: // add
        RA->setValue((operand->getValue() + RA->getValue()) & MAX_VALUE);
        N->setValue(RA->getValue() > MAX_VALUE_SIGN);
        Z->setValue(RA->getValue() == 0);
        C->setValue((RA->getValue() + operand->getValue()) > MAX_VALUE);
        break;
    case 0x40: // or
        RA->setValue(operand->getValue() | RA->getValue());
        N->setValue(RA->getValue() > MAX_VALUE_SIGN);
        Z->setValue(RA->getValue() == 0);
        C->setValue(false);
        break;
    case 0x50: // and
        RA->setValue(operand->getValue() & RA->getValue());
        N->setValue(RA->getValue() > MAX_VALUE_SIGN);
        Z->setValue(RA->getValue() == 0);
        C->setValue(false);
        break;
    case 0x60: // not
        RA->setValue(~RA->getValue());
        N->setValue(RA->getValue() > MAX_VALUE_SIGN);
        Z->setValue(RA->getValue() == 0);
        C->setValue(false);
        break;
    case 0x80: // jmp
        PC->setValue(operand->getValue());
        break;
    case 0x90: // jn
        if (N) PC->setValue(operand->getValue());
        break;
    case 0xA0: // jz
        if (Z) PC->setValue(operand->getValue());
        break;
    case 0xB0: // jc
        if (C) PC->setValue(operand->getValue());
        break;
    case 0xE0: // shr
        C->setValue((RA->getValue() & 0x01) == 1);
        RA->setValue(RA->getValue() >> 1);
        N->setValue(RA->getValue() > MAX_VALUE_SIGN);
        Z->setValue(RA->getValue() == 0);
        break;
    case 0xF0: // hlt
        this->running = false;
        break;
    }
}

void CromagMachine::run()
{
    this->running = true;
    while (this->running) {
        this->step();
    }
}

void CromagMachine::assemble(QString filename)
{

}

Instruction* CromagMachine::getInstructionFromValue(int value)
{
    QVector<Instruction*>::iterator i;
    value &= 0b11110000; // Filter bits

    for( i = instructions.begin(); i != instructions.end(); i++) {
        if((*i)->getValue() == value) {
            return (*i);
        }
    }
    return NULL;
}
Instruction* CromagMachine::getInstructionFromMnemonic(QString desiredInstruction) {
    QVector<Instruction*>::iterator i;
    for( i = instructions.begin(); i != instructions.end(); i++) {
        if((*i)->getMnemonic() == desiredInstruction) {
            return (*i);
        }
    }
    return NULL;
}
