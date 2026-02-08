了解 👍
じゃあ **「Windows ⇄ M5Atom BLE 通信テスト」**について、

* **なぜこのテストをやるのか**
* **どうやってやるのか**
* **結果から何が分かるのか**
* **失敗時に何を疑うのか**

を **そのままリポジトリに置ける Markdown** でまとめるね。
以下を `ble_windows_test.md` とかで保存すればOK。

---

```md
# Windows ⇄ M5Atom BLE 通信テスト手順書

## 目的

本ドキュメントは、M5Atom（ESP32 + NimBLE）で実装した BLE 通信スタックが  
**外部デバイスと正しく通信できるか**を検証するためのテスト手順をまとめたものである。

対象は以下：

- BLE Advertise / Scan
- GATT Service / Characteristic
- Write（Client → Server）
- Notify（Server → Client）
- 接続 / 切断イベント

Windows をクライアントとして用いることで、  
**Atom 同士通信の前段階として BLE 実装の正当性を確認する**ことが目的。

---

## テスト構成

```

[ Windows (Python + bleak) ]
↓ Write
BLE GATT
↑ Notify
[ M5Atom (NimBLE-Arduino) ]

````

- Windows : BLE Client
- M5Atom  : BLE Server

---

## 使用ツール

### Windows 側
- OS : Windows 10 / 11
- Python : 3.9 以上
- ライブラリ : bleak

```bash
pip install bleak
````

### M5Atom 側

* Framework : Arduino
* BLE Stack : NimBLE-Arduino
* Board : M5Stack Atom

---

## 前提条件（BLE 定義）

M5Atom 側で以下の GATT 構成が存在すること。

| 種別                      | UUID                                   | 用途      |
| ----------------------- | -------------------------------------- | ------- |
| Service                 | `4fafc201-1fb5-459e-8fcc-c5c9c331914b` | 制御用サービス |
| Characteristic (Notify) | `beb5483e-36e1-4688-b7f5-ea07361b26a8` | 状態送信    |
| Characteristic (Write)  | `beb5483e-36e1-4688-b7f5-ea07361b26a8` | 操作受信    |

---

## テスト手順

1. M5Atom を起動し、BLE Advertise を開始する
2. Windows で Python スクリプトを実行
3. Scan に M5Atom が検出されることを確認
4. 自動的に接続されることを確認
5. Write が M5Atom 側で受信されることを確認
6. Notify が Windows 側で受信されることを確認
7. プログラム終了時に正常に切断されることを確認

---

## このテストで分かること

### 1. BLE Advertise が正しいか

* Scan に出ない場合、以下を疑う

  * Service UUID の advertise 忘れ
  * advertise 開始タイミング
  * UUID の typo

---

### 2. GATT 定義が正しいか

* 接続できない場合

  * Service UUID 不一致
  * BLE 初期化順序ミス

---

### 3. Write が正しく届くか

* Windows → M5Atom のデータ経路が正常
* Characteristic 権限（WRITE）が正しい

---

### 4. Notify が正しく飛ぶか

* M5Atom → Windows の非同期通信が正常
* CCCD（Notify 設定）が正しい
* NimBLE の callback 実装が正しい

---

### 5. 接続管理が正しいか

* onConnect / onDisconnect が想定通り呼ばれる
* 接続状態フラグの更新が正しい

---

## 失敗時のチェックリスト

| 症状         | 確認ポイント              |
| ---------- | ------------------- |
| Scan に出ない  | advertise UUID      |
| 接続できない     | Service UUID        |
| Write 失敗   | WRITE 権限            |
| Notify 来ない | start_notify / CCCD |
| 途中で切断      | 電源 / watchdog       |