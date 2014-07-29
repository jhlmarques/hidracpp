#include "register.h"

Register::Register()
{
    this->value = 0;
    this->numOfBits = 8;
}

Register::Register(int numOfBits)
{
    this->value = 0;
    this->numOfBits = numOfBits;
}

int Register::getValue() const
{
    return value;
}

void Register::setValue(int value)
{
    this->value = value % (1 << numOfBits); // Trim value to number of bits
}

void Register::incrementValue()
{
    this->value = (value + 1) % (1 << numOfBits); // In case of overflow, go back to zero
}

int Register::getNumOfBits() const
{
    return numOfBits;
}

void Register::setNumOfBits(int value)
{
    this->numOfBits = value;
}


