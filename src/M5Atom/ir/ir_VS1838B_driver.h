#pragma once

namespace orimer::ir {

class IrVs1838bDriver
{
public:
    explicit IrVs1838bDriver(int pin);

    void Begin();
    bool IsReceiving() const;
    int  ReadRaw() const;

private:
    int m_pin;
};

}
