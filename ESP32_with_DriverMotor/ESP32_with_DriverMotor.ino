#define pin_MOTOR_DIRR 22
#define pin_MOTOR_PWMR 23

#define pin_MOTOR_DIRL 32
#define pin_MOTOR_PWML 33

#define PWM_FREQUENCY   1000
#define PWM_RESOLUTION  8
#define M1A_PWM_CHANNEL 6
#define M1B_PWM_CHANNEL 7
#define M2A_PWM_CHANNEL 8
#define M2B_PWM_CHANNEL 9

//#define merah 19
//#define kuning 18
//#define hijau 5

#include <Arduino.h>
#if defined(ESP32)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif
#include <Firebase_ESP_Client.h>

//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

// Insert your network credentials
#define WIFI_SSID "admin"
#define WIFI_PASSWORD "12345678"

// Insert Firebase project API Key
#define API_KEY "AIzaSyCe0X-3HTFcK9Vu6fS3Mtn4RPPVsRk9Vkg"

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "https://test-465fb-default-rtdb.asia-southeast1.firebasedatabase.app/"

//Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
String stringValue;
float floatValue;
bool signupOK = false;

void setup() {
  ledcSetup(M1A_PWM_CHANNEL, PWM_FREQUENCY, PWM_RESOLUTION);
  ledcSetup(M1B_PWM_CHANNEL, PWM_FREQUENCY, PWM_RESOLUTION);
  ledcSetup(M2A_PWM_CHANNEL, PWM_FREQUENCY, PWM_RESOLUTION);
  ledcSetup(M2B_PWM_CHANNEL, PWM_FREQUENCY, PWM_RESOLUTION);

  ledcAttachPin(pin_MOTOR_DIRL, M1A_PWM_CHANNEL);
  ledcAttachPin(pin_MOTOR_PWML, M1B_PWM_CHANNEL);
  ledcAttachPin(pin_MOTOR_DIRR, M2A_PWM_CHANNEL);
  ledcAttachPin(pin_MOTOR_PWMR, M2B_PWM_CHANNEL);

  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("ok");
    signupOK = true;
  } else {
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback;  //see addons/TokenHelper.h

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop() {
  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 100 || sendDataPrevMillis == 0)) {
    sendDataPrevMillis = millis();
    if (Firebase.RTDB.getString(&fbdo, "/LED/Merah")) {
      Serial.println(fbdo.stringData());

      if (fbdo.stringData() == "1") {
        setMotor(80, 80);
        Serial.println("Nyala");
      } else {
        setMotor(0, 0);
        Serial.println("Mati");
      }

    } else {
      Serial.println(fbdo.errorReason());
    }
//    if (Firebase.RTDB.getString(&fbdo, "/LED/Kuning")) {
//      Serial.println(fbdo.stringData());
//
//      if (fbdo.stringData() == "1") {
//        digitalWrite(kuning, HIGH);
//        Serial.println("Nyala");
//      } else {
//        digitalWrite(kuning, LOW);
//        Serial.println("Mati");
//      }
//
//    } else {
//      Serial.println(fbdo.errorReason());
//    }
//    if (Firebase.RTDB.getInt(&fbdo, "/LED/Hijau")) {
//      Serial.println(fbdo.intData());
//
//      if (fbdo.intData() == 1) {
//        digitalWrite(hijau, HIGH);
//        Serial.println("Nyala");
//      } else {
//        digitalWrite(hijau, LOW);
//        Serial.println("Mati");
//      }
//
//    } else {
//      Serial.println(fbdo.errorReason());
//    }
  }
}

void setMotor(int speedLeft, int speedRight)
{
  speedLeft = constrain(speedLeft, -255, 255);
  speedRight = constrain(speedRight, -255, 255);

  if (speedLeft > 0) {
    int speedL = map(speedLeft, 0, 255, 255, 0);
    ledcWrite(M1A_PWM_CHANNEL, speedL);
    ledcWrite(M1B_PWM_CHANNEL, 255);
  }
  else {
    int speedL = map(speedLeft, -255, 0, 0, 255);
    ledcWrite(M1A_PWM_CHANNEL, 255);
    ledcWrite(M1B_PWM_CHANNEL, speedL);
  }

  if (speedRight > 0) {
    int speedR = map(speedRight, 0, 255, 255, 0);
    ledcWrite(M2A_PWM_CHANNEL, speedR);
    ledcWrite(M2B_PWM_CHANNEL, 255);
  }
  else {
    int speedR = map(speedRight, -255, 0, 0, 255);
    ledcWrite(M2A_PWM_CHANNEL, 255);
    ledcWrite(M2B_PWM_CHANNEL, speedR);
  }
}
