# orimer project

- 通信構成
controller <-> hostPC <-> M5atom

controller からの入力を hostPCがbluetooth通信で受け取り、その情報を M5 atom にbluetooth通信で送る。

- hironori dir
M5 atom 側のプログラム

- kanae dir
host PC 側のプログラム
host PC には

# Python を使って platformio をインストール
py -3 -m pip install -U platformio

# プロジェクトルートでビルド (env名: m5stack-atom)
py -3 -m platformio run -e m5stack-atom

# ビルド
py -3 -m platformio run -e m5stack-atom

# クリーン
py -3 -m platformio run --target clean

# アップロード
py -3 -m platformio run -e m5stack-atom -t upload

# シリアルモニタ
py -3 -m platformio device monitor -b 115200

# from project root (c:\Users\Administrator\PP\orimer-prj)
pio run -e m5stack-atom
# or just
pio run
ysyo8usyopfasu8syouysdaoyuodaswdfyduofydsufyussdyzut
クリーン　再ビルド
python -m platformio run -e m5stack-atom --target clean
python -m platformio run -e m5stack-atom
python -m platformio run -e m5stack-atom -t upload


# platform.ini で　メモリ領域を増やす
// ...existing code...
[env:m5stack-atom]
platform = espressif32
board = m5stack-atom
framework = arduino
lib_deps = 
    m5stack/M5Atom@^0.1.3
    fastled/FastLED@^3.9.20
monitor_speed = 115200

; increase app partition (sacrifice OTA/spiffs) — try huge_app
board_build.partitions = huge_app.csv
; ensure toolchain knows flash size (optional)
board_build.flash_size = 4MB
; reduce debug/log size
build_flags = 
  -DCORE_DEBUG_LEVEL=0
  -Wl,--gc-sections
  -fdata-sections
  -ffunction-sections
; strip symbols to reduce size at link
; (PlatformIO/ESP32 usually strips, but keep this if needed)
; build_unflags = -g
JKL;

# ポート指定でアップロード
pio run -e m5stack-atom --target upload --upload-port COM3

# シリアルモニタ
pio device monitor --port COM3 --baud 115200

COM PORT が見つからない場合は platformio.ini で指定して
pio run -e m5stack-atom --target upload --upload-port COM3
でできる
USB のコードによってはできないものがある

tera term で COM を開いていてはいけない

# これで一発
python -m platformio run -e m5stack-atom --target upload --upload-port COM3

# PlatformIO で複数の Core がインストールされていて、古いバージョンが検出されている．
# # 現在の PIO Core 情報を確認
pio system info

# システム全体の不要な PIO を削除
pio system prune --dry-run   # 削除対象の確認（重要）
pio system prune             # 実際に削除


