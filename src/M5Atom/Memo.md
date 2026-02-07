# driver 設計
[ Pro Controller ]
        ↓  (BLE HID)
[ Atom Lite A ]
  - HID Host (Central)
  - Input → ActionStruct
  - BLE Server (GATT)
        ↓  (BLE GATT)
[ Atom Lite B ]
  - BLE Client (Central)
  - ActionStruct → Motor

src/M5Atom/
├─ main.cpp
└─ ble/
　 |- ble_types.h
   ├─ ble_server.h
   ├─ ble_server.cpp
   ├─ ble_client.h
   └─ ble_client.cpp

