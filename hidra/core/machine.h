#ifndef MACHINE_H
#define MACHINE_H

#include <QObject>
#include <QVector>
#include <QString>
#include <QFile>
#include <QPair>
#include <iostream>

#include "byte.h"
#include "flag.h"
#include "register.h"
#include "instruction.h"
#include "assembler.h"

class Assembler;

class Machine : public QObject
{
    Q_OBJECT
    friend class Assembler;

public:
    explicit Machine(QObject *parent = 0);

    virtual void printStatusDebug() = 0;

    virtual void load(QString filename) = 0;
    virtual void save(QString filename) = 0;

    virtual void step() = 0;
    //virtual void run() = 0;

    virtual const Instruction* getInstructionFromValue(int) = 0;
    virtual const Instruction* getInstructionFromMnemonic(QString) = 0;

    bool isRunning() const;
    void setRunning(bool running);

    int getBreakpoint() const;
    void setBreakpoint(int value);

    QVector<Byte *> getMemory() const;
    void setMemory(const QVector<Byte *> &value);
    void clearMemory();

    int getNumberOfFlags() const;
    QString getFlagName(int id) const;
    int  getFlagValue(int id) const;
    void setFlagValue(int id, int value);
    void clearFlags();

    int getNumberOfRegisters() const;
    QString getRegisterName(int id) const;
    int  getRegisterValue(int id) const;
    void setRegisterValue(int id, int value);
    void clearRegisters();

    int getPCValue() const;
    void setPCValue(int value);
    int getPCCorrespondingLine();

    QVector<Instruction *> getInstructions() const;
    void setInstructions(const QVector<Instruction *> &value);

protected:

    QVector<Register*> registers;
    Register* PC;
    QVector<Byte*> memory;

    QVector<int> correspondingLine;
    QVector<Flag*> flags;
    QVector<Instruction*> instructions;
    bool running;
    int breakpoint;

};


#endif // MACHINE_H
