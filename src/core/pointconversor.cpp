#include "pointconversor.h"
#include "invalidconversorinput.h"

// TODO: implement human notation WIHTOUT double float (or any float).
// With float, there are some 64 bits numbers which cannot be represented
// because double float only really has 52 bits for mantissa.

PointConversor::PointConversor()
{
    digits = 0;
    sign = false;
    exponent = 0;
    normality = PointConversor::NORMAL;
}

PointConversor& PointConversor::inputHalfFloatRaw(uint16_t number)
{
    return this->inputGenericFloatRaw(number, 10, 5);
}

PointConversor& PointConversor::inputSingleFloatRaw(float number)
{
    union { float number; uint32_t bits; } bitCast { number };
    return this->inputGenericFloatRaw(bitCast.bits, 23, 8);
}

PointConversor& PointConversor::inputDoubleFloatRaw(double number)
{
    union { double number; uint64_t bits; } bitCast { number };
    return this->inputGenericFloatRaw(bitCast.bits, 52, 11);
}

PointConversor& PointConversor::inputFixed8Raw(uint8_t number, int16_t pointPos, Signedness signedness)
{
    return this->inputGenericFixedRaw(number, 8, pointPos, signedness);
}

PointConversor& PointConversor::inputFixed16Raw(uint16_t number, int16_t pointPos, Signedness signedness)
{
    return this->inputGenericFixedRaw(number, 16, pointPos, signedness);
}

PointConversor& PointConversor::inputFixed32Raw(uint32_t number, int16_t pointPos, Signedness signedness)
{
    return this->inputGenericFixedRaw(number, 32, pointPos, signedness);
}

PointConversor& PointConversor::inputFixed64Raw(uint64_t number, int16_t pointPos, Signedness signedness)
{
    return this->inputGenericFixedRaw(number, 64, pointPos, signedness);
}

PointConversor& PointConversor::inputHalfFloat(QString const& number)
{
    return this->inputGenericFloat(number, 10, 5);
}

PointConversor& PointConversor::inputSingleFloat(QString const& number)
{
    return this->inputGenericFloat(number, 23, 8);
}

PointConversor& PointConversor::inputDoubleFloat(QString const& number)
{
    return this->inputGenericFloat(number, 52, 11);
}

PointConversor& PointConversor::inputFixed8(QString const& number, Signedness signedness)
{
    return this->inputGenericFixed(number, 8, signedness);
}

PointConversor& PointConversor::inputFixed16(QString const& number, Signedness signedness)
{
    return this->inputGenericFixed(number, 16, signedness);
}

PointConversor& PointConversor::inputFixed32(QString const& number, Signedness signedness)
{
    return this->inputGenericFixed(number, 32, signedness);
}

PointConversor& PointConversor::inputFixed64(QString const& number, Signedness signedness)
{
    return this->inputGenericFixed(number, 64, signedness);
}

PointConversor& PointConversor::inputHumanNotation(QString const& number)
{
    bool ok = true;
    double asDoubleFloat = number.toDouble(&ok);
    if (!ok) {
        throw InvalidConversorInput("Número não está na notação humana adequada");
    }
    return this->inputDoubleFloatRaw(asDoubleFloat);
}

uint16_t PointConversor::outputHalfFloatRaw()
{
    return this->outputGenericFloatRaw(10, 5);
}

float PointConversor::outputSingleFloatRaw()
{
    uint32_t number = this->outputGenericFloatRaw(23, 8);
    union { uint32_t bits; float number; } bitCast { number };
    return bitCast.number;
}

double PointConversor::outputDoubleFloatRaw()
{
    uint64_t number = this->outputGenericFloatRaw(52, 11);
    union { uint64_t bits; double number; } bitCast { number };
    return bitCast.number;
}

uint8_t PointConversor::outputFixed8Raw(int16_t pointPos, Signedness signedness)
{
    return this->outputGenericFixedRaw(8, pointPos, signedness);
}

uint16_t PointConversor::outputFixed16Raw(int16_t pointPos, Signedness signedness)
{
    return this->outputGenericFixedRaw(16, pointPos, signedness);
}

uint32_t PointConversor::outputFixed32Raw(int16_t pointPos, Signedness signedness)
{
    return this->outputGenericFixedRaw(32, pointPos, signedness);
}

uint64_t PointConversor::outputFixed64Raw(int16_t pointPos, Signedness signedness)
{
    return this->outputGenericFixedRaw(64, pointPos, signedness);
}

QString PointConversor::outputHalfFloat()
{
    return this->outputGenericFloat(10, 5);
}

QString PointConversor::outputSingleFloat()
{
    return this->outputGenericFloat(23, 8);
}

QString PointConversor::outputDoubleFloat()
{
    return this->outputGenericFloat(52, 11);
}

QString PointConversor::outputFixed8(int16_t pointPos, Signedness signedness)
{
    return this->outputGenericFixed(8, pointPos, signedness);
}

QString PointConversor::outputFixed16(int16_t pointPos, Signedness signedness)
{
    return this->outputGenericFixed(16, pointPos, signedness);
}

QString PointConversor::outputFixed32(int16_t pointPos, Signedness signedness)
{
    return this->outputGenericFixed(32, pointPos, signedness);
}

QString PointConversor::outputFixed64(int16_t pointPos, Signedness signedness)
{
    return this->outputGenericFixed(64, pointPos, signedness);
}

QString PointConversor::outputHumanNotation()
{
    double asDoubleFloat = this->outputDoubleFloatRaw();
    QString rendered = QString::number(asDoubleFloat, 'f', 50);

    if (rendered.contains('.')) {
        int trailingPos = rendered.length() - 1;
        while (trailingPos > 2 && rendered[trailingPos] == '0') {
            trailingPos--;
        }
        rendered.truncate(trailingPos + 1);
    }

    return rendered;
}

PointConversor& PointConversor::inputGenericFloatRaw(uint64_t number, uint16_t mantissaSize, uint16_t exponentSize)
{
    uint64_t mantissaMask = ((uint64_t) 1 << mantissaSize) - 1;
    int16_t exponentMask = ((uint64_t) 1 << exponentSize) - 1;
    uint64_t finalBit = ((uint64_t) 1 << mantissaSize);

    validateFloatSpec(mantissaSize, exponentSize);

    sign = number >> (exponentSize + mantissaSize);
    digits = number & mantissaMask;
    exponent = (number >> mantissaSize) & exponentMask;
    if (exponent != exponentMask) {
        if (exponent == 0) {
            exponent++;
        } else {
            digits |= finalBit;
        }
        normality = PointConversor::NORMAL;
        exponent -= mantissaSize + (exponentMask >> 1);
    } else if (digits == 0) {
        normality = PointConversor::INFINITY_NAN;
    } else {
        normality = PointConversor::NOT_A_NUMBER;
    }

    return *this;
}

PointConversor& PointConversor::inputGenericFixedRaw(uint64_t number, int16_t width, int16_t pointPos, Signedness signedness)
{
    uint64_t signMask = (uint64_t) 1 << (width - 1);
    uint64_t extendMask = ~((uint64_t) 0) >> (MAX_WIDTH - width);

    validateFixedSpec(width, pointPos, signedness);

    normality = PointConversor::NORMAL;

    switch (signedness) {
    case UNSIGNED:
        sign = false;
        break;
    case TWOS_COMPL:
        sign = (number & signMask) != 0;
        break;
    }

    exponent = -pointPos;

    digits = number;
    if (sign) {
        digits = (~digits + 1) & extendMask;
    }

    return *this;
}

uint64_t PointConversor::outputGenericFloatRaw(uint16_t mantissaSize, uint16_t exponentSize)
{
    int16_t numExponent;
    uint64_t numDigits;

    uint64_t mantissaMask = ((uint64_t) 1 << mantissaSize) - 1;
    int16_t exponentMask = ((uint64_t) 1 << exponentSize) - 1;
    uint64_t finalBit = (uint64_t) 1 << mantissaSize;

    bool hasFinalBit;
    uint64_t roundRight = 0;

    validateFloatSpec(mantissaSize, exponentSize);

    switch (normality) {
    case PointConversor::NORMAL:
        numExponent = exponent + (exponentMask >> 1) + mantissaSize;
        numDigits = digits;

        while (numDigits < finalBit && numDigits != 0) {
            numDigits <<= 1;
            numExponent -= 1;
        }

        while (numDigits >= (finalBit << 1)) {
            roundRight = numDigits & 1;
            numDigits >>= 1;
            numExponent += 1;
        }

        if (numExponent <= 0) {
            while (numExponent < 0) {
                numDigits >>= 1;
                numExponent += 1;
            }
            roundRight = numDigits & 1;
            numDigits >>= 1;
        }

        hasFinalBit = numDigits & finalBit;
        numDigits &= ~finalBit;
        numDigits += roundRight;

        while (numExponent > exponentMask) {
            numDigits <<= 1;
            numExponent -= 1;
        }

        if (!hasFinalBit && numDigits == 0) {
            numExponent = 0;
        } else if (numExponent >= exponentMask) {
            numExponent = exponentMask;
            numDigits = 0;
        }
        break;
    case PointConversor::INFINITY_NAN:
        numExponent = exponentMask;
        numDigits = 0;
        break;
    case PointConversor::NOT_A_NUMBER:
        numExponent = exponentMask;
        numDigits = 1;
        break;
    }

    uint64_t number = numDigits & mantissaMask;
    number |= (uint64_t) (numExponent & exponentMask) << mantissaSize;
    number |= (uint64_t) sign << (mantissaSize + exponentSize);
    return number;
}

PointConversor& PointConversor::inputGenericFloat(QString const &number, uint16_t mantissaSize, uint16_t exponentSize)
{
    uint64_t bits = 0;
    bool hasChars = false;
    int count = 0;

    for (QChar numChar : number) {
        if (numChar == '0' || numChar == '1') {
            bits <<= 1;
            bits |= numChar.digitValue();
            if (bits != 0) {
                count++;
            }
            if (count > mantissaSize + exponentSize + 1) {
                throw InvalidConversorInput("Entrada é muito grande");
            }
            hasChars = true;
        } else if (numChar != ' ') {
            throw InvalidConversorInput("Entrada precisa conter apenas 0 ou 1");
        }
    }

    if (!hasChars) {
        throw InvalidConversorInput("Entrada não pode ser vazia");
    }

    return this->inputGenericFloatRaw(bits, mantissaSize, exponentSize);
}

PointConversor& PointConversor::inputGenericFixed(QString const &number, int16_t width, Signedness signedness)
{
    uint64_t bits = 0;
    bool hasChars = false;
    bool pointFound = false;
    int16_t pointPos = 0;
    int count = 0;

    for (QChar numChar : number) {
        if (numChar == '0' || numChar == '1') {
            bits <<= 1;
            bits |= numChar.digitValue();
            if (bits != 0 || pointFound) {
                count++;
            }
            if (count > width && numChar != '0') {
                throw InvalidConversorInput("Entrada é muito grande");
            }
            hasChars = true;
        } else if (numChar == '.') {
            if (pointFound) {
                throw InvalidConversorInput("Entrada pode conter apenas um ponto");
            }
            pointFound = true;
            pointPos = count;
        } else if (numChar != ' ') {
            throw InvalidConversorInput("Entrada precisa conter apenas 0 ou 1 (ou ponto)");
        }
    }

    if (!hasChars) {
        throw InvalidConversorInput("Entrada não pode ser vazia");
    }
    if (!pointFound) {
        throw InvalidConversorInput("O número em ponto fixo deve conter um ponto.");
    }
    if (pointPos >= width || count - pointPos >= width) {
        throw InvalidConversorInput("O ponto não pode estar fora dos limites.");
    }

    return this->inputGenericFixedRaw(bits, width, count - pointPos, signedness);
}

uint64_t PointConversor::outputGenericFixedRaw(int16_t width, int16_t pointPos, Signedness signedness)
{
    uint64_t number = digits;
    int16_t numExponent = exponent;

    uint64_t finalBit;
    uint64_t mantissaMask;
    uint64_t digitsMask = ~((uint64_t) 0) >> (MAX_WIDTH - width);
    uint64_t roundLeft = 0;
    uint64_t roundRight = 0;

    validateFixedSpec(width, pointPos, signedness);

    switch (signedness) {
    case UNSIGNED:
        finalBit = (uint64_t) 1 << (width - 1);
        break;
    case TWOS_COMPL:
        finalBit = (uint64_t) 1 << (width - 2);
        break;
    }
    mantissaMask = (finalBit << 1) - 1;

    switch (normality) {
    case NORMAL:
        while (numExponent > -pointPos) {
            roundLeft |= number & finalBit;
            numExponent -= 1;
            number <<= 1;
        }

        number |= roundLeft;

        while (numExponent < -pointPos) {
            roundRight = (number & 1) != 0;
            numExponent += 1;
            number >>= 1;
        }

        number &= mantissaMask;

        if (digitsMask != number) {
            number += roundRight;
        }
        if (sign) {
            switch (signedness) {
            case UNSIGNED:
                throw InvalidConversorInput("Formato de saída é sem sinal, mas entrada é negativa.");
            case TWOS_COMPL:
                number = (~number + 1) & digitsMask;
                break;
            }
        }
        break;

    case INFINITY_NAN:
        number = mantissaMask;
        if (sign) {
            switch (signedness) {
            case UNSIGNED:
                throw InvalidConversorInput("Formato de saída é sem sinal, mas entrada é negativa.");
            case TWOS_COMPL:
                number = ((~number + 1) & digitsMask) - 1;
                break;
            }
        }
        break;

    case NOT_A_NUMBER:
        throw InvalidConversorInput("Entrada é um NAN (Not A Number)");
    }

    return number;
}

QString PointConversor::outputGenericFloat(uint16_t mantissaSize, uint16_t exponentSize)
{
    uint64_t bits = this->outputGenericFloatRaw(mantissaSize, exponentSize);
    QString output;

    for (uint64_t i = mantissaSize + exponentSize + 1; i > 0; i--) {
        uint64_t mask = (uint64_t) 1 << (i - 1);
        output.append(bits & mask ? '1' : '0');
    }
    return output;
}

QString PointConversor::outputGenericFixed(int16_t width, int16_t pointPos, Signedness signedness)
{
    uint64_t bits = this->outputGenericFixedRaw(width, pointPos, signedness);
    QString output;

    for (int i = width - 1; i >= pointPos; i--) {
        if (i >= 0) {
            uint64_t mask = (uint64_t) 1 << (uint64_t) i;
            output.append(bits & mask ? '1' : '0');
        } else {
            output.append('0');
        }
    }

    output.append('.');

    for (int i = pointPos - 1; i >= 0; i--) {
        if (i < width) {
            uint64_t mask = (uint64_t) 1 << (uint64_t) i;
            output.append(bits & mask ? '1' : '0');
        } else {
            output.append('0');
        }
    }

    return output;
}

void PointConversor::validateFloatSpec(uint16_t mantissaSize, uint16_t exponentSize)
{
    if (mantissaSize < MIN_MANTISSA_SIZE) {
        throw InvalidConversorInput(QString("Tamanho da mantissa não pode ser menor que ") + QString::number(MIN_MANTISSA_SIZE));
    }
    if (exponentSize < MIN_EXPONENT_SIZE) {
        throw InvalidConversorInput(QString("Tamanho do expoente não pode ser menor que ") + QString::number(MIN_EXPONENT_SIZE));
    }
    if (mantissaSize + exponentSize > MAX_WIDTH - 1) {
        throw InvalidConversorInput(
                    QString("É necessário um bit de sinal, mantissa e expoente juntos devem ser menor que ")
                    + QString::number(MAX_WIDTH - 1));
    }
}

void PointConversor::validateFixedSpec(int16_t width, int16_t pointPos, Signedness signedness)
{
    if (width < MIN_FIXED_WIDTH) {
        throw InvalidConversorInput(QString("Largura não pode ser menor que ") + QString::number(MIN_FIXED_WIDTH));
    }
    if (width > MAX_WIDTH) {
        throw InvalidConversorInput(QString("Largura não pode ser maior que ") +  QString::number(MAX_WIDTH));
    }
    if (pointPos > width || pointPos < 0) {
        throw InvalidConversorInput("Ponto fora da quantidade de bits disponíveis.");
    }

    switch (signedness) {
    case UNSIGNED:
        if (pointPos > width || pointPos < 0) {
            throw InvalidConversorInput("Ponto fora da quantidade de bits disponíveis.");
        }
        break;

    case TWOS_COMPL:
        if (pointPos > width - 1 || pointPos < 0) {
            throw InvalidConversorInput("Ponto fora da quantidade de bits significativos disponíveis.");
        }
        break;
    }
}

int16_t PointConversor::exponentModTen(int16_t numExponent)
{
    int16_t table[] = { 6, 2, 4, 8 };
    int16_t table_size = sizeof(table) / sizeof(table[0]);

    if (numExponent > 0) {
        return table[numExponent % table_size];
    }
    if (numExponent < 0) {
        return table[-numExponent % table_size];
    }
    return 1;
}
