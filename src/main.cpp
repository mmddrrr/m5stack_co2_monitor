#include <M5Core2.h> 
#include <SensirionI2CScd4x.h>

SensirionI2cScd4x scd4x;

// CO2チェック用変数
const uint16_t co2Threshold = 1000; // CO2濃度閾値 (ppm)
bool wasOverThreshold = false; // 前回の測定で閾値を超えていたかのフラグ
unsigned long lastBeepTime = 0; // 最後にbeepした時刻
const unsigned long beepInterval = 60000; // beepの間隔 (1分)

// ビープ音用関数
void beep(int frequency = 1000, int duration = 200) {
  M5.Axp.SetSpkEnable(true);  // スピーカーを有効化
  
  // PWMチャンネル0を使用してトーンを生成
  ledcSetup(0, frequency, 8);
  ledcAttachPin(2, 0);  // Core2のスピーカーピン
  ledcWrite(0, 128);    // 50%デューティサイクル
  
  delay(duration);
  
  ledcWrite(0, 0);      // 音を停止
  M5.Axp.SetSpkEnable(false);  // スピーカーを無効化
}

// アラーム音（高低音の交互音）
void alarm() {
  M5.Axp.SetSpkEnable(true);
  ledcAttachPin(2, 0);
  for (int i = 0; i < 3; i++) {
    // 高音
    ledcSetup(0, 1500, 8);
    ledcWrite(0, 128);
    delay(200);
    // 低音
    ledcSetup(0, 800, 8);
    ledcWrite(0, 128);
    delay(200);
  }
  ledcWrite(0, 0);
  M5.Axp.SetSpkEnable(false);
}

void setup() {
  M5.begin(true, true, true);  // Core2では引数を明示的に指定
  M5.Lcd.setTextFont(2);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.fillScreen(BLACK);  // 画面クリア
  M5.Lcd.println("M5Core2 CO2 Monitor");

  Wire.begin(32, 33);  // Core2のI2Cピン指定 (SDA=32, SCL=33)
  Serial.begin(115200);
  
  delay(1000);  // 初期化待機時間

  // SCD41の初期化
  uint16_t error;
  
  scd4x.begin(Wire, 0x62);  // SCD40/SCD41のデフォルトI2Cアドレス

  // 測定停止
  error = scd4x.stopPeriodicMeasurement();
  if (error) {
    Serial.print("Error stopping periodic measurement: ");
    Serial.println(error, HEX);
  }
  
  delay(500);  // コマンド間の待機時間

  // 測定開始
  error = scd4x.startPeriodicMeasurement();
  if (error) {
    Serial.print("Error starting periodic measurement: ");
    Serial.println(error, HEX);
    M5.Lcd.println("SCD41 init failed!");
  } else {
    M5.Lcd.println("SCD41 initialized.");
    Serial.println("SCD41 initialized successfully");
  }
}

void loop() {
  M5.update();  // Core2では必須のアップデート

  uint16_t error;
  bool dataReady = false;
  
  // データ準備完了の確認
  error = scd4x.getDataReadyStatus(dataReady);
  if (error) {
    Serial.print("Error reading data ready status: ");
    Serial.println(error, HEX);
    delay(1000);
    return;
  }

  if (!dataReady) {
    delay(100);
    return;
  }

  // 測定値の読み取り
  uint16_t co2;
  float temperature;
  float humidity;

  error = scd4x.readMeasurement(co2, temperature, humidity);
  if (error) {
    Serial.print("Error reading measurement: ");
    Serial.println(error, HEX);
    delay(1000);
  } else if (co2 == 0) {
    M5.Lcd.fillRect(0, 50, 320, 190, BLACK);  // 表示エリアをクリア
    M5.Lcd.setCursor(0, 50);
    M5.Lcd.println("Invalid sample detected!");
    Serial.println("Invalid sample detected");
  } else {
    // CO2濃度による色の設定
    uint16_t bgColor = BLACK;
    uint16_t textColor = WHITE;
    
    if (co2 > co2Threshold) {
      bgColor = YELLOW;    // 背景を黄色に
      textColor = RED;     // 文字を赤に
    }
    
    // 正常な測定値の表示
    M5.Lcd.fillRect(0, 50, 320, 190, bgColor);  // 表示エリアを指定色でクリア
    M5.Lcd.setTextColor(textColor, bgColor);    // 文字色と背景色を設定
    M5.Lcd.setCursor(0, 50);
    M5.Lcd.printf("CO2: %4d ppm\n", co2);
    M5.Lcd.printf("Temp: %.2f C\n", temperature);
    M5.Lcd.printf("Hum: %.2f %%RH\n", humidity);
    
    // シリアル出力
    Serial.printf("CO2: %d ppm, Temp: %.2f C, Humidity: %.2f %%RH\n", 
                  co2, temperature, humidity);

    // 閾値を超えた時のアラーム音制御
    if (co2 > co2Threshold) {
      if (!wasOverThreshold) {
        // 閾値を超えた瞬間
        alarm();
        Serial.printf("CO2 concentration over %dppm! Alarm...\n", co2Threshold);
        wasOverThreshold = true;
        lastBeepTime = millis();
      } else if (millis() - lastBeepTime >= beepInterval) {
        // 閾値を超え続けている間は1分おきにアラーム音
        alarm();
        Serial.printf("CO2 still over %dppm! Alarm...\n", co2Threshold);
        lastBeepTime = millis();
      }
    } else {
      wasOverThreshold = false; // 閾値を下回ったらフラグをリセット
    }
  }

  delay(1000);
}