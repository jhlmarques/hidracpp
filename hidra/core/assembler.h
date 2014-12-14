#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QHash>
#include <QRegExp>

#include "machine.h"

class Machine;

class Assembler : public QObject
{
    Q_OBJECT

public:

    enum ErrorCode
    {
        noError = 0,
        wrongNumberOfArguments,
        invalidInstruction,
        invalidAddress,
        invalidValue,
        invalidLabel,
        invalidArgument,
        duplicatedLabel,
        memoryOverflow,
        notImplemented,
    };

    Assembler(Machine &machine);
    void assemble(QString sourceCode);
    bool wasBuildSuccessful() const;

signals:

    void buildErrorDetected(QString);

private:

    Assembler::ErrorCode obeyDirective(QString mnemonic, QString arguments, bool reserveOnly);
    void emitError(int lineNumber, Assembler::ErrorCode errorCode);

    // Assembler memory
    void clearAssemblerMemory();
    void copyAssemblerMemoryToMachineMemory();
    Assembler::ErrorCode reserveAssemblerMemory(int sizeToReserve);

    // Assembler checks
    bool isValidValue(QString valueString, int min, int max);
    bool isValidByteValue(QString valueString);
    bool isValidAddress(QString addressString);

    Assembler::ErrorCode mountInstruction(QString mnemonic, QString arguments, QHash<QString, int> &labelPCMap);

    Machine &machine;
    Register *PC;

    QVector<Byte*> assemblerMemory;
    QVector<bool> reserved;
    bool buildSuccessful;

};

#endif // ASSEMBLER_H
