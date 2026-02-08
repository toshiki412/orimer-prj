#include "fsr_api.h"
#include "fsr400_driver.h"
#include <Arduino.h>

namespace orimer::fsr {

void Initialize()
{
    FsrDriver::GetInstance()->Initialize();
}

void Update(void)
{
    FsrDriver::GetInstance()->Update();
}

bool GetState(FsrState& state)
{
    return FsrDriver::GetInstance()->GetState(state);
}

bool IsInitialized(void)
{
    return FsrDriver::GetInstance()->IsInitialized();
}

} // namespace orimer::fsr
