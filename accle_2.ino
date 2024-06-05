#include <Arduino_LSM9DS1.h>
#include <ArduinoBLE.h>

BLEService accelerometerService("19B10000-E8F2-537E-4F6C-D104768A1214");
BLECharacteristic accelerationCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify, 12);

float x, y, z;
int degreesX = 0;
int degreesY = 0;

void setup() {
  Serial.begin(9600);
  while (!Serial);
  Serial.println("Started");

  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }

  if (!BLE.begin()) {
    Serial.println("Failed to initialize BLE!");
    while (1);
  }

  BLE.setLocalName("Arduino Nano 33 BLE");
  BLE.setAdvertisedService(accelerometerService);
  accelerometerService.addCharacteristic(accelerationCharacteristic);
  BLE.addService(accelerometerService);
  accelerometerService.addCharacteristic(accelerationCharacteristic);
  accelerationCharacteristic.setValue("Hello, Central!");

  BLE.advertise();
  
  Serial.print("Accelerometer sample rate = ");
  Serial.print(IMU.accelerationSampleRate());
  Serial.println("Hz");
}

void loop() {
  BLEDevice central = BLE.central();

  if (central) {
    Serial.print("Connected to central: ");
    Serial.println(central.address());

    while (central.connected()) {
      if (IMU.accelerationAvailable()) {
        IMU.readAcceleration(x, y, z);

        // Update the BLE characteristic with acceleration values
        String accelerationData = String(x) + "," + String(y) + "," + String(z);
        accelerationCharacteristic.writeValue(accelerationData.c_str());

        // Print tilt information to Serial Monitor
        if (x > 0.1) {
          x = 100 * x;
          degreesX = map(x, 0, 97, 0, 90);
          Serial.print("Tilting up ");
          Serial.print(degreesX);
          Serial.println(" degrees");
        }
        if (x < -0.1) {
          x = 100 * x;
          degreesX = map(x, 0, -100, 0, 90);
          Serial.print("Tilting down ");
          Serial.print(degreesX);
          Serial.println(" degrees");
        }
        if (y > 0.1) {
          y = 100 * y;
          degreesY = map(y, 0, 97, 0, 90);
          Serial.print("Tilting left ");
          Serial.print(degreesY);
          Serial.println(" degrees");
        }
        if (y < -0.1) {
          y = 100 * y;
          degreesY = map(y, 0, -100, 0, 90);
          Serial.print("Tilting right ");
          Serial.print(degreesY);
          Serial.println(" degrees");
        }
        delay(1000);
      }
    }

    Serial.println("Central disconnected");
  }
}
