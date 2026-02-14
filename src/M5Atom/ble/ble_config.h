#pragma once

#include <string>
#include <cstdint>

namespace orimer::ble 
{
    namespace atom
    {
        // M5Atom の BLE 設定値
        const std::string ServerName = 
            "M5Atom";
        const std::string ServiceUuid =
            "4fafc201-1fb5-459e-8fcc-c5c9c331914b";
        const std::string CharacteristicUuid =
            "beb5483e-36e1-4688-b7f5-ea07361b26a8";
    }

    namespace coospo
    {
        // Coospo の BLE 設定値
        const std::string ServerName = 
            "HW9";
        const std::string ServiceUuid =
            "180D";
        const std::string CharacteristicUuid =
            "2A37";
    } // namespace coospo

} // namespace orimer::ble