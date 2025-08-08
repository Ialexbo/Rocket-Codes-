#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

// UUIDs
#define SERVICE_UUID        "0000FFE0-0000-1000-8000-00805F9B34FB"
#define CHARACTERISTIC_UUID "0000FFE1-0000-1000-8000-00805F9B34FB"

const int ledPin = 18;
bool deviceState = false;

BLECharacteristic *pCharacteristic;

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      // Método 1: Usar getData() para obtener los bytes directamente
      const uint8_t* data = pCharacteristic->getData();
      size_t length = pCharacteristic->getLength();
      
      if (length > 0) {
        deviceState = (data[0] == 1); // Comparar con valor numérico
        digitalWrite(ledPin, deviceState ? HIGH : LOW);
        Serial.printf("Datos recibidos: %d, Estado: %s\n", 
                     data[0], 
                     deviceState ? "ON" : "OFF");
      }
      
      /*
      // Método 2: Alternativa usando String de Arduino
      String valueStr = pCharacteristic->getValue().c_str();
      if (valueStr.length() > 0) {
        deviceState = (valueStr[0] == '1'); // Comparar con carácter '1'
        digitalWrite(ledPin, deviceState ? HIGH : LOW);
        Serial.println("Estado: " + String(deviceState ? "ON" : "OFF"));
      }
      */
    }
};

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  BLEDevice::init("ESP32_BLE_Control");
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);
  
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ |
                      BLECharacteristic::PROPERTY_WRITE
                    );

  pCharacteristic->setCallbacks(new MyCallbacks());
  pCharacteristic->setValue("0");
  
  pService->start();
  pServer->getAdvertising()->start();
  Serial.println("BLE listo para conexión");
}

void loop() {
  delay(2000);
}