#include "arduino_secrets.h"

// Temperature sensor library
#include "DHT.h"

// Firebase Arduino based on WiFiNINA - Version: Latest
#include "Firebase_Arduino_WiFiNINA.h"


char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;
int status = WL_IDLE_STATUS;

char db_url[] = SECRET_DB_URL;
char db_sec[] = SECRET_DB_SEC;

// define Firebase data object
FirebaseData fbdo;

// firebase endpoint
String path = "/";

// define sensor type and data pin
#define DHTTYPE DHT11
#define DHTPIN 2

// initialize temp sensor
DHT dht(DHTPIN, DHTTYPE);

// led pin variable
int led = 2;

// time flag used for temperature readings
int timeFlag = 0;

void setup() {
  // initialize pins
  pinMode(LED_BUILTIN, OUTPUT);

  // initialize temp sensor
  dht.begin();
  
  // initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial);

  // attempt to connect to Wi-Fi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to network: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
    delay(10000);
  }
  
  Serial.println("You're connected to the network");

  Serial.println("---------------------------------------");
  Serial.println("Board Information:");
  // print your board's IP address:
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  // print your network's SSID:
  Serial.println();
  Serial.println("Network Information:");
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  Serial.println("---------------------------------------");

  // establish connection with firebase
  Firebase.begin(db_url, db_sec, ssid, pass);
  Firebase.reconnectWiFi(true);

  // initial temperature reading
  handleTempStatus();
}

void loop() {
  handleLedStatus();
  if (timeFlag == 60) {
    handleTempStatus();
    timeFlag = 0;
  }
  delay(1000);
  timeFlag++;
}

void handleLedStatus() {
  Firebase.getBool(fbdo, path + "led/status");
  bool status = fbdo.boolData();
  
  if (status == true) {
    Serial.print("Firebase status: ");
    Serial.println(status);
    digitalWrite(LED_BUILTIN, HIGH);
  } else {
    Serial.print("Firebase status: ");
    Serial.println(status);
    digitalWrite(LED_BUILTIN, LOW);
  }
}

void handleTempStatus() {
  float c = dht.readTemperature();
  float f = dht.readTemperature(true);
  float h = dht.readHumidity();

  String celcius = "";
  celcius.concat(c);

  String fahrenheit = "";
  fahrenheit.concat(f);

  String humidity = "";
  humidity.concat(h);

  String updateData =  "{\"c\":" + celcius + ", \"f\":" + fahrenheit + ", \"h\":" + humidity + "}";

  if (Firebase.updateNode(fbdo, "/temp", updateData)) {
    Serial.print("Returned data: ");
    Serial.println(fbdo.jsonData());

    Serial.print("Data sent: ");
    Serial.println(updateData);
  } else {
    Serial.println(fbdo.errorReason());
  }
}
