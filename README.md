# M5Core2 CO2 Monitor

M5Stack Core2とSensirion SCD4xセンサーを使用したCO2濃度モニターです。リアルタイムでCO2濃度、温度、湿度を測定・表示します。

## 機能

- CO2濃度の測定・表示（ppm）
- 温度測定・表示（℃）
- 湿度測定・表示（%RH）
- シリアル出力による詳細ログ
- LCD画面への見やすい表示

## 必要なハードウェア

- **M5Stack Core2** - メインコントローラー
- **Sensirion SCD40/SCD41センサー** - CO2/温湿度センサー
- **接続ケーブル** - I2C接続用

## 配線

| M5Core2 | SCD4x センサー |
|---------|---------------|
| 3.3V    | VDD           |
| GND     | GND           |
| GPIO32  | SDA           |
| GPIO33  | SCL           |

## ソフトウェア要件

### 開発環境
- **PlatformIO** (推奨) または Arduino IDE
- **ESP32 ボードパッケージ**

### 依存ライブラリ
- `m5stack/M5Core2@^0.1.7` - M5Stack Core2用ライブラリ
- `sensirion/Sensirion I2C SCD4x@^1.0.0` - SCD4xセンサー用ライブラリ

## インストール・セットアップ

### PlatformIOを使用する場合

1. リポジトリをクローン
```bash
git clone https://github.com/mmddrrr/m5core2-co2-monitor.git
cd m5core2-co2-monitor
```

2. 依存関係をインストール
```bash
pio lib install
```

3. ビルド・アップロード
```bash
pio run --target upload
```

### Arduino IDEを使用する場合

1. 必要なライブラリをインストール
   - M5Core2 ライブラリ
   - Sensirion I2C SCD4x ライブラリ

2. `main.cpp` の内容を Arduino IDE にコピー

3. ボード設定
   - ボード: "M5Stack-Core2"
   - Upload Speed: 115200

## 使用方法

1. M5Core2の電源を入れる
2. センサーの初期化が完了するまで待機（約1-2秒）
3. LCD画面に以下の情報が表示されます：
   - CO2濃度（ppm）
   - 温度（℃）
   - 湿度（%RH）
4. シリアルモニター（115200 baud）でより詳細な情報を確認可能

## 表示例

```
M5Core2 CO2 Monitor
SCD41 initialized.

CO2:  412 ppm
Temp: 23.45 C
Hum: 45.67 %RH
```

## トラブルシューティング

### 電源が入らない場合
- USB-Cケーブルで充電してください
- 電源ボタンを3-5秒間長押ししてください

### センサーが初期化できない場合
- 配線を再確認してください
- センサーのI2Cアドレス（0x62）が正しいか確認してください
- シリアルモニターでエラーメッセージを確認してください

### 測定値が表示されない場合
- センサーが正しく接続されているか確認してください
- 初期化後、最初の測定まで数秒かかる場合があります
- "Invalid sample detected" が表示される場合は、センサーのウォームアップを待ってください

## 技術仕様

- **測定範囲**: 
  - CO2: 400-5000 ppm
  - 温度: -10～60℃
  - 湿度: 0～100%RH
- **測定精度**:
  - CO2: ±(50 ppm + 5% of reading)
  - 温度: ±0.8℃
  - 湿度: ±6%RH
- **更新間隔**: 1秒

## ファイル構成

```
├── src/
│   └── main.cpp          # メインプログラム
├── platformio.ini        # PlatformIO設定ファイル
└── README.md            # このファイル
```

## ライセンス

このプロジェクトはMITライセンスの下で公開されています。

## 貢献

バグ報告や機能改善の提案はIssuesでお知らせください。プルリクエストも歓迎します。

## 更新履歴

- **v1.0.0** - 初期リリース
  - CO2濃度、温度、湿度の基本測定機能
  - LCD表示とシリアル出力
  - M5Core2対応

## 参考資料

- [M5Stack Core2 公式ドキュメント](https://docs.m5stack.com/en/core/core2)
- [Sensirion SCD4x データシート](https://sensirion.com/products/catalog/SCD40/)
- [PlatformIO Documentation](https://docs.platformio.org/)