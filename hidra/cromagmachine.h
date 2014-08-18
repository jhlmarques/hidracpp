#ifndef CROMAGMACHINE_H
#define CROMAGMACHINE_H

#include "machine.h"

class CromagMachine : public Machine
{
public:
public:
    CromagMachine();

    virtual void printStatusDebug();

    virtual void load(QString filename);
    virtual void save(QString filename);

    virtual void step();
    virtual void run();
    virtual void assemble(QString filename);

    virtual Instruction* getInstructionFromValue(int);
    virtual Instruction* getInstructionFromMnemonic(QString);

private:
    Register *RA;
    Bit *N, *Z, *C;
    static const int MEM_SIZE = 256;
    static const int MAX_VALUE = 255;
    static const int MAX_VALUE_SIGN = 127;
};

#endif // CROMAGMACHINE_H
