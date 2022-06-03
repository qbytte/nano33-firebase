#include "arduino_secrets.h"

// Firebase Arduino based on WiFiNINA - Version: Latest
#include "Firebase_Arduino_WiFiNINA.h"


char ssid[] = SECRET_SSID;      // your network SSID (name)
char pass[] = SECRET_PASS;      // your network password (use for WPA, or use as key for WEP)
int status = WL_IDLE_STATUS;    // the Wi-Fi status

char db_url[] = SECRET_DB_URL;  // firebase project url
char db_sec[] = SECRET_DB_SEC;  // firebase secret to get access

// define Firebase data object
FirebaseData fbdo;

// firebase endpoint
String path = "/test";

// led pin variable
int led = 2;

void setup() {
  // initialize pins
  pinMode(LED_BUILTIN, OUTPUT);
  
  // initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial);

  // attempt to connect to Wi-Fi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to network: ");
    Serial.println(ssid);
    // connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }

  // you're connected now, so print out the data:
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
}

void loop() {
  Firebase.getBool(fbdo, path + "/led/status");
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
  
  delay(1000);
}
