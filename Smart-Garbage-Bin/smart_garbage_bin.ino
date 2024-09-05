#include <WiFi.h>
#include <WiFiAP.h>
#include <WiFiClient.h>
#include <ESP32Servo.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <Adafruit_TCS34725.h>

Servo myServo;

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define TRIGGER_PIN 2
#define ECHO_PIN 4
#define RGB_SENSOR_SDA 21
#define RGB_SENSOR_SCL 22

const int MAX_DISTANCE = 60; // Maximum distance in centimeters

const String SETUP_INIT = "SETUP: Initializing ESP32 dev board";
const String SETUP_ERROR = "!!ERROR!! SETUP: Unable to start SoftAP mode";
const String SETUP_SERVER_START = "SETUP: HTTP server started --> IP addr: ";
const String SETUP_SERVER_PORT = " on port: ";
const String INFO_NEW_CLIENT = "New client connected";
const String INFO_DISCONNECT_CLIENT = "Client disconnected";
const String HTTP_HEADER = "HTTP/1.1 200 OK\r\nContent-type:text/html\r\n\r\n";
const String HTML_WELCOME = "<h1>Welcome to your Smart Bin application!</h1>";

const char *SSID = "Smart Bin";
const char *PASS = "1234abcd";
const int HTTP_PORT_NO = 80;

WiFiServer HttpServer(HTTP_PORT_NO);

Adafruit_TCS34725 rgbSensor = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

bool isLidOpen = false;

void setup()
{
  Serial.begin(9600);

  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
  {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;
  }

  display.clearDisplay();
  display.display();

  myServo.attach(5);
  myServo.write(0);

  if (!WiFi.softAP(SSID, PASS))
  {
    Serial.println(SETUP_ERROR);
    while (1)
      ;
  }
  const IPAddress accessPointIP = WiFi.softAPIP();
  const String webServerInfoMessage = SETUP_SERVER_START + accessPointIP.toString() + SETUP_SERVER_PORT + HTTP_PORT_NO;
  HttpServer.begin();
  Serial.println(webServerInfoMessage);

  // Initialize RGB sensor
  if (!rgbSensor.begin())
  {
    Serial.println(F("Could not find a valid TCS34725 sensor, check wiring!"));
    while (1)
      ;
  }

  // Initialize lid status
  isLidOpen = false;
}

void loop()
{
  WiFiClient client = HttpServer.available();
  if (client)
  {
    Serial.println(INFO_NEW_CLIENT);
    String currentLine = "";
    while (client.connected())
    {
      if (client.available())
      {
        char c = client.read();
        Serial.write(c);
        if (c == '\n')
        {
          if (currentLine.length() == 0)
          {
            printWelcomePage(client, updateAndDisplayMessage());
            break;
          }
          else if (currentLine.startsWith("GET /classifyMaterial"))
          {
            // Handle material classification request
            String materialRequest = currentLine.substring(22);
            classifyMaterial(client, materialRequest);
            break;
          }
          else if (currentLine.startsWith("GET /openlid"))
          {
            myServo.write(180);
            delay(2000);
            client.print("Lid opened");
            isLidOpen = true; // Set the lid status to open
            printWelcomePage(client, updateAndDisplayMessage());
            break;
          }
          else if (currentLine.startsWith("GET /closelid"))
          {
            closeLid();
            client.print("Lid closed");
            isLidOpen = false; // Set the lid status to closed
            printWelcomePage(client, updateAndDisplayMessage());
            break;
          }
          else if (currentLine.startsWith("GET /showFullness"))
          {
            myServo.write(0);
            client.print("Fullness shown");
            printWelcomePage(client, updateAndDisplayMessage());
            break;
          }
          else if (currentLine.startsWith("GET /getMessage"))
          {
            client.println(HTTP_HEADER);
            client.print(updateAndDisplayMessage());
            break;
          }
          else
          {
            currentLine = "";
          }
        }
        else if (c != '\r')
        {
          currentLine += c;
        }
      }
    }
    client.stop();
    Serial.println(INFO_DISCONNECT_CLIENT);
    Serial.println();
  }
}

// Modify the HTML buttons to pass the button label to the classifyMaterial function
void printWelcomePage(WiFiClient client, const char *message)
{
  if (client.connected())
  {
    client.println(HTTP_HEADER);
    client.print(HTML_WELCOME);

    // text box
    client.print("<div>");
    client.print("<textarea id=\"messageBox\" rows=\"4\" cols=\"50\">");
    client.print(message);
    client.print("</textarea>");
    client.print("</div>");

    // buttons for HTML response
    client.print("<button onclick=\"openLid()\">Open Lid</button>");
    client.print("<button onclick=\"closeLid()\">Close Lid</button>");
    client.print("<button onclick=\"showFullness()\">Show Fullness</button>");

    // buttons for trash types
    client.print("<button onclick=\"classifyMaterial('Plastic')\">Plastic</button>");
    client.print("<button onclick=\"classifyMaterial('Metal')\">Metal</button>");
    client.print("<button onclick=\"classifyMaterial('Food')\">Food</button>");

    client.print("<script>"
                 "function openLid() {"
                 "  fetch('/openlid').then(updateMessageBox);"
                 "}"
                 "function closeLid() {"
                 "  fetch('/closelid').then(updateMessageBox);"
                 "}"
                 "function showFullness() {"
                 "  fetch('/showFullness').then(updateMessageBox);"
                 "}"
                 "function classifyMaterial(material) {"
                 "  fetch('/classifyMaterial?' + material).then(updateMessageBox);"
                 "}"
                 "function updateMessageBox() {"
                 "  fetch('/getMessage').then(response => response.text()).then(message => {"
                 "    document.getElementById('messageBox').value = message;"
                 "  });"
                 "}"
                 "</script>");

    client.println();
  }
}

// Add a global variable to store the button label that was pressed
String pressedButtonLabel = "";

void classifyMaterial(WiFiClient client, String material)
{
  // Store the pressed button label
  pressedButtonLabel = material;
  // Remove " HTTP/1.1" from the pressedButtonLabel
  pressedButtonLabel.replace(" HTTP/1.1", "");

  // Check if the lid is already open
  if (!isLidOpen) {
    // Measure the distance
    int distance = getDistance();

    // Read color from RGB sensor
    uint16_t r, g, b, c;
    rgbSensor.getRawData(&r, &g, &b, &c);

    // Print RGB values for debugging
    Serial.print("RGB values: ");
    Serial.print("R: "); Serial.print(r);
    Serial.print(", G: "); Serial.print(g);
    Serial.print(", B: "); Serial.println(b);

    // Classify color
    String detectedMaterial;

    // Define thresholds for red and blue
    const int redThreshold = 150; // Adjust this threshold based on your sensor readings
    const int blueThreshold = 150; // Adjust this threshold based on your sensor readings

    if (r > redThreshold && r > g && r > b)
    {
      // Red is dominant, consider it as plastic
      detectedMaterial = "Plastic";
    }
    else if (b > blueThreshold && b > r && b > g)
    {
      // Blue is dominant, consider it as metal
      detectedMaterial = "Metal";
    }
    else
    {
      // Other colors, or no dominant color
      detectedMaterial = "Food";
    }

    // Print detected material and pressed button label for debugging
    Serial.print("Detected Material: "); Serial.println(detectedMaterial);
    Serial.print("Pressed Button Label: "); Serial.println(pressedButtonLabel);

    // Open the lid if detected material matches the pressed material button
    if (pressedButtonLabel == "Food" && detectedMaterial == "Food") {
      myServo.write(180);
      delay(2000);
      client.print("Lid opened for Food");
      myServo.write(0);
      isLidOpen = true; // Set the lid status to open
    } else if (pressedButtonLabel == "Metal" && detectedMaterial == "Metal") {
      myServo.write(180);
      delay(2000);
      client.print("Lid opened for Metal");
      myServo.write(0);
      isLidOpen = true; // Set the lid status to open
    } else if (pressedButtonLabel == "Plastic" && detectedMaterial == "Plastic") {
      myServo.write(180);
      delay(2000);
      client.print("Lid opened for Plastic");
      myServo.write(0);
      isLidOpen = true; // Set the lid status to open
    } else {
      // Lid closing logic for non-matching materials
      myServo.write(0);
      client.print("Requested material does not match detected material");
      isLidOpen = false; // Set the lid status to closed
    }
  } else {
    // Lid is already open, print a message or take appropriate action
    client.print("Lid is already open");
    isLidOpen = false; // Reset the lid status to closed
  }
}

void closeLid() {
  myServo.write(0);
  isLidOpen = false; // Set the lid status to closed
}

// Modify the updateAndDisplayMessage function to pass the pressed button label
const char *updateAndDisplayMessage()
{
  // Measure the distance
  int distance = getDistance();

  // Read color from RGB sensor
  uint16_t r, g, b, c;
  rgbSensor.getRawData(&r, &g, &b, &c);

  // Update and display the message
  static char message[60];
  snprintf(message, sizeof(message), "Fullness: %d\nRGB: (%d, %d, %d)", distance, r, g, b);

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(message);
  display.display();

  return message;
}

int getDistance()
{
  // Trigger the ultrasonic sensor
  digitalWrite(TRIGGER_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN, LOW);

  // Read the pulse duration from the echo pin
  long duration = pulseIn(ECHO_PIN, HIGH);

  // Calculate the distance in centimeters
  int distance = duration * 0.034 / 2;

  return distance;
}
