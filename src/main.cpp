#include <Arduino.h>
#include "esp_camera.h"
#include <WiFi.h>//make esp connect to wifi
#include <Firebase.h>//make esp connect to firebase
#include <Servo.h>//make esp connect to servo
#include <ArduinoWebsockets.h>//let esp connect to websocket
#include <addons/TokenHelper.h>// Provide the token generation process info.
#include <addons/RTDBHelper.h> // Provide the RTDB payload printing info and other helper functions.
#include "credential.h"
#include "camera_pins.h"

#define CAMERA_MODEL_AI_THINKER

const char* ssid = "ALVIN";
const char* password = "Alvin1004";
const char* websocket_server_host = "192.168.100.10";
const uint16_t websocket_server_port = 6969;

using namespace websockets;
WebsocketsClient client;

Servo Servo1;
Servo Servo2;
FirebaseData stream;
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;


String parentPath = "/";
String childPath[6] = {"/servo1","/servo2","/up","/down","/left","/right"};

void streamCallback(MultiPathStreamData stream) {
  size_t numChild = sizeof(childPath) / sizeof(childPath[0]);
  Serial.println("Connected to rtdb");


  for (size_t i = 0; i < numChild; i++) {
      // Serial.println("masok loop");
      Serial.println(childPath[i]);
      if (stream.get(childPath[i])) {
        Serial.printf("path: %s, event: %s, type: %s, value: %s%s", stream.dataPath.c_str(), stream.eventType.c_str(), stream.type.c_str(), stream.value.c_str(), i < numChild - 1 ? "\n" : "");
        Serial.printf("\n");

        // Serial.println("masok if");


        const char* childPath = stream.dataPath.c_str();
        String value = stream.value.c_str();

        if (strcmp(childPath, "/servo1") == 0) {
          int servo1 = value.toInt();
          Servo1.write(servo1);
        } 

        // else if(strcmp(childPath, "/servo2") == 0) {
        //   int servo2 = value.toInt();
        //   Servo2.write(servo2);
        // }

        else if (strcmp(childPath, "/up") == 0)
      {
        if (value == "true")
        {
          digitalWrite(in1, LOW);
          digitalWrite(in2, HIGH);
          digitalWrite(in3, LOW);
          digitalWrite(in4, HIGH);
          // Serial.printf("helloimunderthewater");
        }
        else
        {
          digitalWrite(in1, LOW);
          digitalWrite(in2, LOW);
          digitalWrite(in3, LOW);
          digitalWrite(in4, LOW);
        }
      }
      else if (strcmp(childPath, "/down") == 0)
      {
        if (value == "true")
        {
          digitalWrite(in1, HIGH);
          digitalWrite(in2, LOW);
          digitalWrite(in3, HIGH);
          digitalWrite(in4, LOW);
        }
        else
        {
          digitalWrite(in1, LOW);
          digitalWrite(in2, LOW);
          digitalWrite(in3, LOW);
          digitalWrite(in4, LOW);
        }
      }
      else if (strcmp(childPath, "/right") == 0)
      {
        if (value == "true")
        {
          digitalWrite(in1, HIGH);
          digitalWrite(in2, LOW);
          digitalWrite(in3, LOW);
          digitalWrite(in4, HIGH);
        }
        else
        {
          digitalWrite(in1, LOW);
          digitalWrite(in2, LOW);
          digitalWrite(in3, LOW);
          digitalWrite(in4, LOW);
        }
      }
      else if (strcmp(childPath, "/left") == 0)
      {
        if (value == "true")
        {
          digitalWrite(in1, LOW);
          digitalWrite(in2, HIGH);
          digitalWrite(in3, HIGH);
          digitalWrite(in4, LOW);
        }
        else
        {
          digitalWrite(in1, LOW);
          digitalWrite(in2, LOW);
          digitalWrite(in3, LOW);
          digitalWrite(in4, LOW);
        }
      }
    }
  };
}

void streamTimeoutCallback(bool timeout) {
  if (timeout)
    Serial.println("stream timed out, resuming...\n");

  if (!stream.httpConnected())
    Serial.printf("error code: %d, reason: %s\n\n", stream.httpCode(), stream.errorReason().c_str());
}


// put function declarations here:
void setup(){
  pinMode(in1,OUTPUT);
  pinMode(in2,OUTPUT);
  pinMode(in3,OUTPUT);
  pinMode(in4,OUTPUT);
  digitalWrite(in1,LOW);
  digitalWrite(in2,LOW);
  digitalWrite(in3,LOW);
  digitalWrite(in4,LOW);
  // pinMode(LED_BUILTIN,OUTPUT);
  // pinMode(Verservo, OUTPUT);
  pinMode(Horservo,OUTPUT);
  Servo1.attach(Horservo);
  // Servo2.attach(Verservo);
  // Servo1.write(90);
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();

  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  // digitalWrite(LED_BUILTIN,HIGH);
  Serial.println("WiFi connected");

    /* Assign the api key (required) */
  config.api_key = API_KEY;

  //   /* Assign the user sign in credentials */
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; // see addons/TokenHelper.h

  Firebase.begin(&config, &auth);

  Firebase.reconnectWiFi(true);

  if (!Firebase.beginMultiPathStream(stream, parentPath))
  Serial.printf("stream begin error, %s\n\n", stream.errorReason().c_str());

  Firebase.setMultiPathStreamCallback(stream, streamCallback, streamTimeoutCallback);

  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 10000000;
  config.pixel_format = PIXFORMAT_JPEG;
  // init with high specs to pre-allocate larger buffers
  if (psramFound())
  {
    config.frame_size = FRAMESIZE_VGA;
    config.jpeg_quality = 40;
    config.fb_count = 2;
  }
  else
  {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK)
  {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  Serial.print("Camera Ready!");

  while (!client.connect(websocket_server_host, websocket_server_port, "/"))
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Websocket Connected!");
}


void loop()
{
  // if (Firebase.ready()) {

  // }
  camera_fb_t *fb = esp_camera_fb_get();
  if(!fb){
    // Serial.println("Camera capture failed");
    esp_camera_fb_return(fb);
    return;
  }

  if(fb->format != PIXFORMAT_JPEG){
    Serial.println("Non-JPEG data not implemented");
    return;
  }

  client.sendBinary((const char*) fb->buf, fb->len);
  esp_camera_fb_return(fb);
  // put your main code here, to run repeatedly:
}

