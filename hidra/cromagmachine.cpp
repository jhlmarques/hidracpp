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
    bool err = false;
    QFile memFile(filename);
    memFile.open(QFile::ReadOnly);
    QDataStream stream(&memFile);
    stream.setByteOrder(QDataStream::BigEndian);
    unsigned char buffer;
    unsigned char machineIdentifier[4] = {3, 'N', 'D', 'R'};    //machine identifier
    int i;
    for(i = 0; i < 4; i++) {
        stream >> buffer;
        if(buffer != machineIdentifier[i]) {
            err = false;
            break;
        }
    }
    i = 0;
    if(!err) {
        while(!stream.atEnd()) {
            stream >> buffer;
            memory[i++]->setValue((unsigned char)buffer);
            stream >> buffer;
        }
    }
    memFile.close();
}

void CromagMachine::save(QString filename){
    QFile memFile(filename);
    memFile.open(QFile::WriteOnly);
    QDataStream stream(&memFile);
    stream.setByteOrder(QDataStream::BigEndian);

    stream << (unsigned char)3 << (unsigned char)'N' << (unsigned char)'D' << (unsigned char)'R'; //prefixo identificador da maquina (basicamente o que muda em cada maquina

    foreach (Byte *byte, memory) {
        stream << byte->getValue() << (unsigned char)0;
    }
    memFile.close();
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
//    NeanderMachine *outputMachine = new NeanderMachine();
    QHash<QString, int> labelsMap;
    QFile sourceFile(filename);
    sourceFile.open(QIODevice::ReadOnly | QIODevice::Text);
    QString source = sourceFile.readAll();
    QStringList sourceLines = source.split("\n", QString::SkipEmptyParts);  //separa o arquivo fonte por linhas de codigo
    QStringList::iterator i;
    for(i = sourceLines.begin(); i != sourceLines.end(); i++) {
        (*i) = (*i).split(';').at(0).toLower().simplified();    //elimina os comentarios do codigo
    }
    unsigned int sumAux;
    sourceLines.removeAll("");  //remove as linhas em branco

    int pc = 0;
    //QVector<Byte *> memory = outputMachine->getMemory();
    for(i = sourceLines.begin(); i != sourceLines.end(); i++) {
        QStringList line = (*i).split(" ", QString::SkipEmptyParts);
         std::cout << line.join(" ").toStdString() << std::endl;
        Instruction *atual;
        if (line.at(0).contains(QRegExp("(.*:)"))) {
            QString key = line.first();
            (*i).replace(key, "");
            key.chop(1);
            labelsMap.insert(key, pc);
        } else if(line.at(0) == "org") {
            pc = line.at(1).toInt();
        } else if(line.at(0) == "db") {
            pc++;
        } else if(line.at(0) == "dw") {
            pc += 2;
        } else if(line.at(0) == "dab") {
            if(line.at(1).contains(QRegExp("(\\d+\\(\\d+\\))"))) {
                QStringList dabValue = line.at(1).split("(");
                dabValue.last().chop(1);
                pc += dabValue.first().toInt();
            }
        } else if(line.at(0) == "daw") {
            if(line.at(1).contains(QRegExp("(\\d+\\(\\d+\\))"))) {
                QStringList dawValue = line.at(1).split("(");
                dawValue.last().chop(1);
                pc += dawValue.first().toInt() * 2;
            }
        } else {
            atual = getInstructionFromMnemonic(line.at(0));
            pc += 1 + atual->getNumberOfArguments();
        }
    }
    sourceLines.removeAll("");  //remove as linhas em branco
    foreach(QString key, labelsMap.keys()) {
        sourceLines.replaceInStrings(QString(key), QString::number(labelsMap.value(key)));
    }
    pc = 0;
    for(i = sourceLines.begin(); i != sourceLines.end(); i++) {
        Instruction *atual;

        QStringList line = (*i).split(" ", QString::SkipEmptyParts);

        if (line.at(0).contains(QRegExp("(.*:)"))) {
            //skip
        } else if(line.at(0) == "org") {
            pc = line.at(1).toInt();
        } else if(line.at(0) == "db") {
            memory[pc++]->setValue((unsigned char)line.last().toInt());
        } else if(line.at(0) == "dw") {
            int word = line.last().toInt();
            memory[pc++]->setValue((unsigned char)((word & 0xFF00)>>8) );
            memory[pc++]->setValue((unsigned char)(word & 0x00FF) );
        } else if(line.at(0) == "dab") {
            if(line.at(1).contains(QRegExp("(\\d+\\(\\d+\\))"))) {
                QStringList dabValue = line.at(1).split("(");
                dabValue.last().chop(1);
                for(int i = 0; i < dabValue.first().toInt(); i++) {
                    memory[pc++]->setValue((unsigned char) dabValue.last().toInt());
                }
            }
        } else if(line.at(0) == "daw") {
            if(line.at(1).contains(QRegExp("(\\d+\\(\\d+\\))"))) {
                QStringList dabValue = line.at(1).split("(");
                dabValue.last().chop(1);
                for(int i = 0; i < dabValue.first().toInt(); i++) {
                    int word = dabValue.last().toInt();
                    memory[pc++]->setValue((unsigned char)((word & 0xFF00)>>8));
                    memory[pc++]->setValue((unsigned char)(word & 0x00FF) );
                }
            }
        } else {
            atual = getInstructionFromMnemonic(line.at(0));
            line.replace(0, QString::number(atual->getValue()));
            memory[pc]->setValue((unsigned char)line[0].toInt());
            for(int i=1; i<line.length();i++)
            {

              /*  if(line[i]=="a" || line[i]=="A" && i==1)
                {
                    sumAux=memory[pc]->getValue();
                    memory[pc]->setValue();
                }
                else*/ if((line[i]=="b" || line[i]=="B") && i==1)
                {
                    sumAux= (int)memory[pc]->getValue() + 4;
                    memory[pc]->setValue((unsigned char)sumAux);
                    sumAux= (int)memory[pc]->getValue();
                }
                else if((line[i]=="x" || line[i]=="X") && i==1)
                {
                    memory[pc++]->setValue(memory[pc]->getValue()+5);
                }
                else if(line[i].contains("#"))
                {
                    sumAux= (int)memory[pc]->getValue() + 2;
                    memory[pc++]->setValue((unsigned char)sumAux);
                    line[i].replace("#","");
                    memory[pc++]->setValue((unsigned char)line[i].toInt());
                }
                else if(line[i].contains(",i$") || line[i].contains(",I$"))
                {
                    memory[pc]->setValue(memory[pc]->getValue()+(unsigned char)1);
                    memory[pc++]->setValue((unsigned char)line[i].toInt());
                }
                else if(line[i].contains(",x$") || line[i].contains(",X$"))
                {
                    memory[pc]->setValue(memory[pc]->getValue()+(unsigned char)3);
                    memory[pc++]->setValue((unsigned char)line[i].toInt());
                }



            //foreach (QString byte, line) {
                //
            }
        }

    }
    //outputMachine->setMemory(memory);
    //outputMachine->printStatusDebug();
    QString outputFilename = filename.split('.').at(0) + ".mem";
    save(outputFilename);
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
