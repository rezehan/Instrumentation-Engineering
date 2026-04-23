#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <MPU6050.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
MPU6050 mpu;

void setup() {
  Serial.begin(9600);
  Wire.begin();

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("MPU6050 I2C");
  lcd.setCursor(0, 1);
  lcd.print("Initializing...");
  delay(2000);
  lcd.clear();

  mpu.initialize();

  if (mpu.testConnection()) {
    Serial.println("MPU6050 BERHASIL TERHUBUNG!");
    lcd.setCursor(0, 0);
    lcd.print("Sensor OK!");
    delay(1000);
    lcd.clear();
  } else {
    Serial.println("MPU6050 GAGAL TERHUBUNG!");
    lcd.setCursor(0, 0);
    lcd.print("Sensor ERROR!");
    while (1);
  }

  // Header tabel — hanya tampil sekali
  Serial.println("+-------+----------------+-------------+");
  Serial.println("| Sumbu |     Nilai (g)  |   Arah      |");
  Serial.println("+-------+----------------+-------------+");
}

void loop() {
  int16_t ax, ay, az;
  int16_t gx, gy, gz;

  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  // Konversi ke nilai g
  float accelX = ax / 16384.0;
  float accelY = ay / 16384.0;
  float accelZ = az / 16384.0;

  // Tampilkan di LCD
  lcd.setCursor(0, 0);
  lcd.print("X:");
  lcd.print(accelX, 1);
  lcd.print(" Y:");
  lcd.print(accelY, 1);
  lcd.setCursor(0, 1);
  lcd.print("Z:");
  lcd.print(accelZ, 1);
  lcd.print("g        ");

  // Baris tabel X
  Serial.print("|   X   |     ");
  if (accelX >= 0) Serial.print(" ");
  Serial.print(accelX, 4);
  Serial.print(" g    | Kiri/Kanan  |");
  Serial.println();

  // Baris tabel Y
  Serial.print("|   Y   |     ");
  if (accelY >= 0) Serial.print(" ");
  Serial.print(accelY, 4);
  Serial.print(" g    | Maju/Mundur |");
  Serial.println();

  // Baris tabel Z
  Serial.print("|   Z   |     ");
  if (accelZ >= 0) Serial.print(" ");
  Serial.print(accelZ, 4);
  Serial.print(" g    | Atas/Bawah  |");
  Serial.println();

  Serial.println("+-------+----------------+-------------+");
  Serial.println();

  delay(1000);
}
