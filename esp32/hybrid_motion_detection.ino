#include <WiFi.h>
#include <HTTPClient.h>

// ============================================================
// EXPERIMENT MODE
// ============================================================
// true  = HYBRID
// false = CLOUD-ONLY
// ============================================================

#define HYBRID_MODE false


// ============================================================
// WIFI
// ============================================================

const char* ssid = "Ranjitha Y Gowda";
const char* password = "Ranjitha@16";


// ============================================================
// AWS API GATEWAY
// ============================================================

const char* apiUrl =
  "https://6kl27knga8.execute-api.us-east-1.amazonaws.com/motion";


// ============================================================
// HARDWARE
// ============================================================

const int PIR_PIN = 27;
const int LED_PIN = 26;


// ============================================================
// MOTION STATE
// ============================================================

bool previousMotionState = false;


// ============================================================
// SETUP
// ============================================================

void setup() {

  Serial.begin(115200);

  pinMode(PIR_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);

  digitalWrite(LED_PIN, LOW);

  Serial.println();
  Serial.println("================================================");
  Serial.println("       HYBRID EDGE-SERVERLESS SYSTEM");
  Serial.println("================================================");

  if (HYBRID_MODE) {
    Serial.println("MODE: HYBRID / EDGE");
  }
  else {
    Serial.println("MODE: CLOUD-ONLY");
  }

  Serial.println("================================================");


  // ----------------------------------------------------------
  // Wi-Fi connection
  // ----------------------------------------------------------

  Serial.println("Connecting to Wi-Fi...");

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {

    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Wi-Fi Connected!");

  Serial.print("ESP32 IP Address: ");
  Serial.println(WiFi.localIP());

  Serial.println("System Ready.");
  Serial.println();
}


// ============================================================
// MAIN LOOP
// ============================================================

void loop() {

  int motion = digitalRead(PIR_PIN);


  // ==========================================================
  // NEW MOTION DETECTED
  // ==========================================================

  if (motion == HIGH && !previousMotionState) {

    Serial.println();
    Serial.println("================================================");
    Serial.println("              MOTION DETECTED");
    Serial.println("================================================");

    // Start high-resolution timer
    unsigned long startTime = micros();


    // ========================================================
    // HYBRID MODE
    // ========================================================

    if (HYBRID_MODE) {

      Serial.println("Architecture : HYBRID");
      Serial.println("Decision     : EDGE");
      Serial.println();


      // ------------------------------------------------------
      // LOCAL EDGE DECISION - LED ON IMMEDIATELY
      // ------------------------------------------------------

      digitalWrite(LED_PIN, HIGH);

      unsigned long ledOnTime = micros();

      unsigned long hybridResponseTime =
        ledOnTime - startTime;


      Serial.println("Edge Decision: LED ON");

      Serial.print("Hybrid LED Response Time: ");
      Serial.print(hybridResponseTime);
      Serial.println(" microseconds");


      // ------------------------------------------------------
      // SEND EVENT TO AWS AFTER LOCAL ACTION
      // ------------------------------------------------------

      Serial.println();
      Serial.println("Sending event to AWS...");

      sendEventToAWS("HYBRID");
    }


    // ========================================================
    // CLOUD-ONLY MODE
    // ========================================================

    else {

      Serial.println("Architecture : CLOUD-ONLY");
      Serial.println("Decision     : CLOUD");
      Serial.println();


      // ------------------------------------------------------
      // LED STAYS OFF UNTIL CLOUD RESPONDS
      // ------------------------------------------------------

      digitalWrite(LED_PIN, LOW);

      Serial.println("LED is waiting for cloud decision...");
      Serial.println();
      Serial.println("Sending motion to AWS...");


      // ------------------------------------------------------
      // SEND MOTION EVENT TO CLOUD
      // ------------------------------------------------------

      String response = sendEventToAWS("CLOUD_ONLY");

      unsigned long responseTime = micros();

      unsigned long cloudRoundTripTime =
        responseTime - startTime;


      Serial.println();

      Serial.print("Cloud Round Trip Time: ");
      Serial.print(cloudRoundTripTime);
      Serial.println(" microseconds");

      Serial.print("Cloud Round Trip Time: ");
      Serial.print(cloudRoundTripTime / 1000.0);
      Serial.println(" ms");


      // ------------------------------------------------------
      // CHECK CLOUD DECISION
      // ------------------------------------------------------

      if (response.length() > 0 &&
          response.indexOf("ledCommand") >= 0 &&
          response.indexOf("ON") >= 0) {

        Serial.println("Cloud Decision: LED ON");


        // LED turns ON only after AWS response
        digitalWrite(LED_PIN, HIGH);

        unsigned long ledOnTime = micros();

        unsigned long cloudLedResponseTime =
          ledOnTime - startTime;


        Serial.print("Cloud-Only LED Response Time: ");
        Serial.print(cloudLedResponseTime);
        Serial.println(" microseconds");

        Serial.print("Cloud-Only LED Response Time: ");
        Serial.print(cloudLedResponseTime / 1000.0);
        Serial.println(" ms");

      }
      else {

        Serial.println(
          "ERROR: Cloud did not return LED ON command."
        );

        digitalWrite(LED_PIN, LOW);
      }
    }


    previousMotionState = true;
  }


  // ==========================================================
  // NO MOTION
  // ==========================================================

  if (motion == LOW && previousMotionState) {

    digitalWrite(LED_PIN, LOW);

    Serial.println();
    Serial.println("No Motion - LED OFF");
    Serial.println("================================================");
  }


  previousMotionState = motion;

  delay(100);
}


// ============================================================
// SEND EVENT TO AWS
// ============================================================

String sendEventToAWS(String mode) {


  // ----------------------------------------------------------
  // CHECK WI-FI CONNECTION
  // ----------------------------------------------------------

  if (WiFi.status() != WL_CONNECTED) {

    Serial.println("ERROR: Wi-Fi disconnected.");

    return "";
  }


  // ----------------------------------------------------------
  // CREATE HTTP CLIENT
  // ----------------------------------------------------------

  HTTPClient http;

  Serial.println("Connecting to API Gateway...");

  http.begin(apiUrl);


  // ----------------------------------------------------------
  // TIMEOUT SETTINGS
  // ----------------------------------------------------------

  http.setConnectTimeout(10000);
  http.setTimeout(15000);

  http.addHeader(
    "Content-Type",
    "application/json"
  );


  // ----------------------------------------------------------
  // CREATE JSON DATA
  // ----------------------------------------------------------

  String jsonData =
    "{\"deviceId\":\"ESP32-Test-Device\","
    "\"eventType\":\"MOTION_DETECTED\","
    "\"mode\":\"" + mode + "\"}";


  Serial.print("Request: ");
  Serial.println(jsonData);


  // ----------------------------------------------------------
  // SEND HTTP POST REQUEST
  // ----------------------------------------------------------

  unsigned long requestStart = micros();

  int httpResponseCode = http.POST(jsonData);

  unsigned long requestEnd = micros();

  unsigned long httpRequestTime =
    requestEnd - requestStart;


  // ----------------------------------------------------------
  // PRINT HTTP TIMING
  // ----------------------------------------------------------

  Serial.print("HTTP Request Time: ");
  Serial.print(httpRequestTime);
  Serial.println(" microseconds");

  Serial.print("HTTP Request Time: ");
  Serial.print(httpRequestTime / 1000.0);
  Serial.println(" ms");


  Serial.print("HTTP Response Code: ");
  Serial.println(httpResponseCode);


  // ----------------------------------------------------------
  // SUCCESSFUL RESPONSE
  // ----------------------------------------------------------

  if (httpResponseCode > 0) {

    String response = http.getString();

    Serial.print("AWS Response: ");
    Serial.println(response);

    http.end();

    return response;
  }


  // ----------------------------------------------------------
  // HTTP ERROR
  // ----------------------------------------------------------

  else {

    Serial.print("HTTP Error: ");
    Serial.println(http.errorToString(httpResponseCode));

    http.end();

    return "";
  }
}