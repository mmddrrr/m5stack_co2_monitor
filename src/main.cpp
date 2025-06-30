#include <M5Core2.h> 
#include <SensirionI2CScd4x.h>

SensirionI2cScd4x scd4x;

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
    // 正常な測定値の表示
    M5.Lcd.fillRect(0, 50, 320, 190, BLACK);  // 表示エリアをクリア
    M5.Lcd.setCursor(0, 50);
    M5.Lcd.printf("CO2: %4d ppm\n", co2);
    M5.Lcd.printf("Temp: %.2f C\n", temperature);
    M5.Lcd.printf("Hum: %.2f %%RH\n", humidity);
    
    // シリアル出力
    Serial.printf("CO2: %d ppm, Temp: %.2f C, Humidity: %.2f %%RH\n", 
                  co2, temperature, humidity);
  }

  delay(1000);
}