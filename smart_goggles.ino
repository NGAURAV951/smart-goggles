#include <Wire.h>
#include <WiFi.h>
#include <TinyGPS++.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

#include <Adafruit_Fingerprint.h>


// =====================================================
//                 WIFI SETTINGS
// =====================================================

const char* WIFI_SSID = "YOUR_WIFI_NAME";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";


// =====================================================
//                 OLED SETTINGS
// =====================================================

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(
  SCREEN_WIDTH,
  SCREEN_HEIGHT,
  &Wire,
  -1
);


// =====================================================
//                 GPS SETTINGS
// =====================================================

#define GPS_RX 16
#define GPS_TX 17

HardwareSerial GPS_Serial(1);
TinyGPSPlus gps;


// =====================================================
//              FINGERPRINT SETTINGS
// =====================================================

// Fingerprint sensor UART
#define FP_RX 26
#define FP_TX 27

HardwareSerial Fingerprint_Serial(2);

Adafruit_Fingerprint fingerprint =
    Adafruit_Fingerprint(&Fingerprint_Serial);


// =====================================================
//                 TOUCH SENSOR
// =====================================================

#define TOUCH_PIN 4


// =====================================================
//                 BATTERY
// =====================================================

#define BATTERY_PIN 34


// =====================================================
//                 MPU6050
// =====================================================

Adafruit_MPU6050 mpu;


// =====================================================
//                 VARIABLES
// =====================================================

bool authenticated = false;
bool showSensors = false;

unsigned long lastTouchTime = 0;


// =====================================================
//                 SETUP
// =====================================================

void setup()
{
  Serial.begin(115200);

  // ---------------- I2C ----------------

  Wire.begin(21, 22);


  // ---------------- OLED ----------------

  if (!display.begin(
        SSD1306_SWITCHCAPVCC,
        0x3C))
  {
    Serial.println("OLED ERROR");

    while (true);
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(1);
  display.setCursor(20, 20);

  display.println("AURORA GLASSES");

  display.setCursor(25, 35);
  display.println("Starting...");

  display.display();

  delay(2000);


  // ---------------- GPS ----------------

  GPS_Serial.begin(
    9600,
    SERIAL_8N1,
    GPS_RX,
    GPS_TX
  );


  // ---------------- TOUCH ----------------

  pinMode(TOUCH_PIN, INPUT);


  // ---------------- BATTERY ----------------

  pinMode(BATTERY_PIN, INPUT);


  // ---------------- MPU6050 ----------------

  if (!mpu.begin())
  {
    Serial.println("MPU6050 not detected");
  }
  else
  {
    Serial.println("MPU6050 connected");

    mpu.setAccelerometerRange(
      MPU6050_RANGE_8_G
    );

    mpu.setGyroRange(
      MPU6050_RANGE_500_DEG
    );
  }


  // ---------------- FINGERPRINT ----------------

  Fingerprint_Serial.begin(
    57600,
    SERIAL_8N1,
    FP_RX,
    FP_TX
  );

  fingerprint.begin(57600);


  if (fingerprint.verifyPassword())
  {
    Serial.println(
      "Fingerprint sensor detected"
    );
  }
  else
  {
    Serial.println(
      "Fingerprint sensor NOT detected"
    );
  }


  // ---------------- WIFI ----------------

  WiFi.begin(
    WIFI_SSID,
    WIFI_PASSWORD
  );

  display.clearDisplay();

  display.setCursor(0, 0);
  display.println("AURORA GLASSES");

  display.setCursor(0, 20);
  display.println("Connecting WiFi...");

  display.display();


  int attempts = 0;

  while (
    WiFi.status() != WL_CONNECTED &&
    attempts < 20
  )
  {
    delay(500);

    Serial.print(".");

    attempts++;
  }


  display.clearDisplay();

  if (WiFi.status() == WL_CONNECTED)
  {
    display.setCursor(0, 0);
    display.println("WiFi Connected");

    display.setCursor(0, 20);
    display.println(WiFi.localIP());
  }
  else
  {
    display.setCursor(0, 0);
    display.println("WiFi Offline");
  }

  display.display();

  delay(2000);


  // ---------------- AUTHENTICATION ----------------

  authenticateUser();
}


// =====================================================
//                 MAIN LOOP
// =====================================================

void loop()
{

  // Read GPS continuously
  while (GPS_Serial.available())
  {
    gps.encode(
      GPS_Serial.read()
    );
  }


  // Touch control
  if (
    digitalRead(TOUCH_PIN) == HIGH &&
    millis() - lastTouchTime > 500
  )
  {
    lastTouchTime = millis();

    showSensors = !showSensors;

    Serial.println("Touch detected");
  }


  // Only operate after authentication
  if (!authenticated)
  {
    authenticateUser();
    return;
  }


  if (showSensors)
  {
    showSensorScreen();
  }
  else
  {
    showMainScreen();
  }

  delay(200);
}


// =====================================================
//             FINGERPRINT AUTHENTICATION
// =====================================================

void authenticateUser()
{

  display.clearDisplay();

  display.setTextSize(1);

  display.setCursor(0, 0);
  display.println("AURORA SECURITY");

  display.setCursor(0, 20);
  display.println("Place finger...");

  display.display();


  Serial.println(
    "Waiting for fingerprint..."
  );


  int result = getFingerprintID();


  if (result >= 0)
  {

    authenticated = true;

    Serial.print(
      "Authenticated ID: "
    );

    Serial.println(result);


    display.clearDisplay();

    display.setCursor(0, 0);
    display.println("ACCESS GRANTED");

    display.setCursor(0, 20);
    display.print("User ID: ");

    display.println(result);

    display.display();

    delay(1500);
  }
}


// =====================================================
//             GET FINGERPRINT ID
// =====================================================

int getFingerprintID()
{

  uint8_t p =
    fingerprint.getImage();

  if (p != FINGERPRINT_OK)
  {
    return -1;
  }


  p =
    fingerprint.image2Tz();

  if (p != FINGERPRINT_OK)
  {
    return -1;
  }


  p =
    fingerprint.fingerSearch();

  if (p != FINGERPRINT_OK)
  {
    return -1;
  }

  return fingerprint.fingerID;
}


// =====================================================
//             MAIN INFORMATION SCREEN
// =====================================================

void showMainScreen()
{
  display.clearDisplay();

  display.setTextSize(1);

  display.setCursor(0, 0);
  display.println("AURORA");

  // Time from GPS
  display.setCursor(0, 12);

  if (gps.time.isValid())
  {
    if (gps.time.hour() < 10) display.print("0");
    display.print(gps.time.hour());
    display.print(":");
    if (gps.time.minute() < 10) display.print("0");
    display.print(gps.time.minute());
  }
  else
  {
    display.print("--:--");
  }

  // GPS location
  display.setCursor(0, 26);

  if (gps.location.isValid())
  {
    display.print("LAT:");
    display.println(gps.location.lat(), 4);

    display.print("LON:");
    display.println(gps.location.lng(), 4);
  }
  else
  {
    display.println("GPS searching...");
  }

  // WiFi status
  display.setCursor(0, 50);

  if (WiFi.status() == WL_CONNECTED)
    display.println("WiFi: CONNECTED");
  else
    display.println("WiFi: OFF");

  display.display();
}


// =====================================================
//             SENSOR DATA SCREEN
// =====================================================

void showSensorScreen()
{
  display.clearDisplay();

  display.setTextSize(1);

  display.setCursor(0, 0);
  display.println("SENSOR DATA");

  sensors_event_t a, g, temp;

  if (mpu.getEvent(&a, &g, &temp))
  {
    display.setCursor(0, 15);

    display.print("X:");
    display.println(a.acceleration.x, 1);

    display.print("Y:");
    display.println(a.acceleration.y, 1);

    display.print("Z:");
    display.println(a.acceleration.z, 1);
  }

  // Battery
  int batteryValue = analogRead(BATTERY_PIN);

  display.setCursor(0, 52);
  display.print("BAT:");
  display.print(batteryValue);

  display.display();
}
