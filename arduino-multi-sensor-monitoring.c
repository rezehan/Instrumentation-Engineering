#include <DHT.h>
#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <MPU6050.h>

// ==================== PIN DEFINITION ====================
#define DHT_PIN     2
#define DHT_TYPE    DHT22
#define MQ_PIN      A0

// ==================== OBJECT INIT =======================
DHT dht(DHT_PIN, DHT_TYPE);
Adafruit_BMP085 bmp;
MPU6050 mpu;

unsigned long lastRead = 0;
const unsigned long interval = 2000;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  dht.begin();

  Serial.println(F("============================================"));
  Serial.println(F("     SISTEM MONITORING MULTI-SENSOR         "));
  Serial.println(F("============================================"));
  Serial.println();

  // Init BMP180
  if (!bmp.begin()) {
    Serial.println(F("[ERROR] BMP180 tidak terdeteksi!"));
  } else {
    Serial.println(F("[OK] BMP180 siap."));
  }

  // Init MPU6050
  mpu.initialize();
  if (mpu.testConnection()) {
    Serial.println(F("[OK] MPU6050 siap."));
  } else {
    Serial.println(F("[ERROR] MPU6050 tidak terdeteksi!"));
  }

  Serial.println(F("[OK] DHT22 siap."));
  Serial.println(F("[OK] Sensor MQ siap."));
  Serial.println();
  delay(1500);
}

void loop() {
  unsigned long now = millis();
  if (now - lastRead >= interval) {
    lastRead = now;
    bacaSemua();
  }
}

// ==================== FUNGSI BACA SEMUA ====================
void bacaSemua() {

  // -------- DHT22 --------
  float suhu_dht  = dht.readTemperature();
  float humidity  = dht.readHumidity();
  float heat_index = (!isnan(suhu_dht) && !isnan(humidity))
                     ? dht.computeHeatIndex(suhu_dht, humidity, false)
                     : NAN;

  // -------- BMP180 --------
  float suhu_bmp = bmp.readTemperature();
  long  tekanan  = bmp.readPressure();
  float altitude = bmp.readAltitude();

  // -------- MQ Sensor --------
  int   mq_raw  = analogRead(MQ_PIN);
  float mq_volt = mq_raw * (5.0 / 1023.0);

  // -------- MPU6050 --------
  int16_t ax, ay, az;
  int16_t gx, gy, gz;
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  // Konversi ke satuan nyata
  float axG  = ax / 16384.0;   // g
  float ayG  = ay / 16384.0;
  float azG  = az / 16384.0;
  float gxDs = gx / 131.0;     // °/s
  float gyDs = gy / 131.0;
  float gzDs = gz / 131.0;

  // Hitung sudut kemiringan (tilt)
  float roll  = atan2(ayG, azG) * 180.0 / PI;
  float pitch = atan2(-axG, sqrt(ayG * ayG + azG * azG)) * 180.0 / PI;

  // ==================== TAMPILAN SERIAL ====================
  Serial.println(F("╔══════════════════════════════════════════╗"));
  Serial.print  (F("║  "));
  Serial.print  (F("Waktu: "));
  Serial.print  (millis() / 1000);
  Serial.println(F(" detik"));
  Serial.println(F("╚══════════════════════════════════════════╝"));
  Serial.println();

  // ------- DHT22 -------
  Serial.println(F("┌─[ DHT22 - Suhu & Kelembaban ]───────────┐"));
  Serial.print  (F("│  Suhu       : "));
  if (isnan(suhu_dht)) Serial.println(F("Gagal membaca!          │"));
  else { Serial.print(suhu_dht, 1); Serial.println(F(" °C                    │")); }

  Serial.print  (F("│  Kelembaban : "));
  if (isnan(humidity)) Serial.println(F("Gagal membaca!          │"));
  else { Serial.print(humidity, 1); Serial.println(F(" %                     │")); }

  Serial.print  (F("│  Heat Index : "));
  if (isnan(heat_index)) Serial.println(F("N/A                     │"));
  else { Serial.print(heat_index, 1); Serial.println(F(" °C                    │")); }
  Serial.println(F("└─────────────────────────────────────────┘"));
  Serial.println();

  // ------- BMP180 -------
  Serial.println(F("┌─[ BMP180 - Tekanan & Ketinggian ]───────┐"));
  Serial.print  (F("│  Suhu       : ")); Serial.print(suhu_bmp, 1);
  Serial.println(F(" °C                    │"));
  Serial.print  (F("│  Tekanan    : ")); Serial.print(tekanan / 100.0, 2);
  Serial.println(F(" hPa                  │"));
  Serial.print  (F("│  Ketinggian : ")); Serial.print(altitude, 1);
  Serial.println(F(" m (estimasi)          │"));
  Serial.println(F("└─────────────────────────────────────────┘"));
  Serial.println();

  // ------- MQ Sensor -------
  Serial.println(F("┌─[ MQ Sensor - Kualitas Udara ]──────────┐"));
  Serial.print  (F("│  ADC Raw    : ")); Serial.println(mq_raw);
  Serial.print  (F("│  Tegangan   : ")); Serial.print(mq_volt, 3);
  Serial.println(F(" V"));
  Serial.print  (F("│  Status     : "));
  if      (mq_raw < 200) Serial.println(F("UDARA BERSIH       ✓  │"));
  else if (mq_raw < 500) Serial.println(F("SEDANG             ~  │"));
  else                   Serial.println(F("GAS TERDETEKSI     ⚠  │"));
  Serial.println(F("└─────────────────────────────────────────┘"));
  Serial.println();

  // ------- MPU6050 -------
  Serial.println(F("┌─[ MPU6050 - Akselerometer & Giroskop ]──┐"));
  Serial.println(F("│  -- Akselerometer --                     │"));
  Serial.print  (F("│  X : ")); Serial.print(axG, 3); Serial.println(F(" g"));
  Serial.print  (F("│  Y : ")); Serial.print(ayG, 3); Serial.println(F(" g"));
  Serial.print  (F("│  Z : ")); Serial.print(azG, 3); Serial.println(F(" g"));
  Serial.println(F("│                                          │"));
  Serial.println(F("│  -- Giroskop --                          │"));
  Serial.print  (F("│  X : ")); Serial.print(gxDs, 2); Serial.println(F(" °/s"));
  Serial.print  (F("│  Y : ")); Serial.print(gyDs, 2); Serial.println(F(" °/s"));
  Serial.print  (F("│  Z : ")); Serial.print(gzDs, 2); Serial.println(F(" °/s"));
  Serial.println(F("│                                          │"));
  Serial.println(F("│  -- Sudut Kemiringan --                  │"));
  Serial.print  (F("│  Roll  : ")); Serial.print(roll, 2);  Serial.println(F(" °"));
  Serial.print  (F("│  Pitch : ")); Serial.print(pitch, 2); Serial.println(F(" °"));
  Serial.println(F("└─────────────────────────────────────────┘"));
  Serial.println();
  Serial.println(F("══════════════════════════════════════════"));
  Serial.println();
}
