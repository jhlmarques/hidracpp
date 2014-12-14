#include "machine.h"

Machine::Machine(QObject *parent) :
    QObject(parent)
{
}

int Machine::getBreakpoint() const
{
    return breakpoint;
}

void Machine::setBreakpoint(int value)
{
    if (value > memory.size() || value < 0)
        breakpoint = 0;
    else
        breakpoint = value;
}

QVector<Byte *> Machine::getMemory() const
{
    return memory;
}

void Machine::setMemory(const QVector<Byte *> &value)
{
    memory = value;
}

void Machine::clearMemory()
{
    for (int i=0; i<memory.size(); i++)
        memory[i]->setValue(0);
}

int Machine::getNumberOfRegisters() const
{
    return registers.count();
}

QString Machine::getRegisterName(int id) const
{
    return registers[id]->getName();
}

int Machine::getRegisterValue(int id) const
{
    return registers[id]->getValue();
}

void Machine::setRegisterValue(int id, int value)
{
    registers[id]->setValue(value);
}

void Machine::clearRegisters()
{
    for (int i=0; i<registers.size(); i++)
        registers[i]->setValue(0);
}

int Machine::getPCValue() const
{
    return PC->getValue();
}

void Machine::setPCValue(int value)
{
    PC->setValue(value);
}

int Machine::getPCCorrespondingLine()
{
    if (!correspondingLine.isEmpty())
        return correspondingLine[PC->getValue()];
    else
        return -1;
}

QVector<Instruction *> Machine::getInstructions() const
{
    return instructions;
}

void Machine::setInstructions(const QVector<Instruction *> &value)
{
    instructions = value;
}

int Machine::getNumberOfFlags() const
{
    return flags.count();
}

QString Machine::getFlagName(int id) const
{
    return flags[id]->getName();
}

int Machine::getFlagValue(int id) const
{
    return flags[id]->getValue();
}

void Machine::setFlagValue(int id, int value)
{
    flags[id]->setValue(value);
}

void Machine::clearFlags()
{
    for (int i=0; i<flags.size(); i++)
        flags[i]->resetValue();
}



void Machine::setRunning(bool running)
{
    this->running = running;
}

bool Machine::isRunning() const
{
    return this->running;
}

