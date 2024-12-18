#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

TwoWire I2Cbus = TwoWire(0);

// Display defines
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &I2Cbus, OLED_RESET);

// Wi-Fi credentials
const char* ssid = "CEMD";
const char* password = "cemd1811";

// Static IP configuration
IPAddress local_IP(192, 168, 1, 101);  // Replace with your desired static IP
IPAddress gateway(192, 168, 1, 1);     // Replace with your network gateway
IPAddress subnet(255, 255, 255, 0);    // Subnet mask

AsyncWebServer server(80);

// Function to display text on OLED
void printLineByLine(Adafruit_SSD1306 &display, String lines[], int lineCount) {
  display.clearDisplay();
  display.setTextSize(1);  // Set text size to normal
  display.setTextColor(WHITE);

  // Loop through the array of strings and print each line
  for (int i = 0; i < lineCount; i++) {
    display.setCursor(0, i * 10);  // Adjust the vertical spacing (10 pixels per line)
    display.println(lines[i]);
    delay(200);
  }

  display.display();  // Send the content to the OLED
}
void setup() {
  Serial.begin(115200);

  // Initialize I2C for the OLED display
  I2Cbus.begin(26, 27, 100000);

  // Initialize OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println("SSD1306 OLED display failed to initialize.");
    while (true);
  }

  display.clearDisplay();
  display.setTextSize(1);  // Set text size (1 is normal size)
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.print("Initializing...");
  display.display();
  delay(2000);
  display.clearDisplay();

  // Connect to Wi-Fi with static IP
  if (!WiFi.config(local_IP, gateway, subnet)) {
    Serial.println("Static IP configuration failed!");
  }
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("WiFi connected!");
  Serial.print("ESP32 IP Address: ");
  Serial.println(WiFi.localIP());

  // Display Wi-Fi connection info on OLED
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("WiFi Connected!");
  display.setCursor(0, 10);
  display.print("IP: ");
  display.print(WiFi.localIP());

  // Set up the HTTP POST endpoint
  server.on("/display", HTTP_POST, [](AsyncWebServerRequest * request) {}, NULL,
  [](AsyncWebServerRequest * request, uint8_t *data, size_t len, size_t index, size_t total) {
    // Parse JSON payload
    DynamicJsonDocument doc(512);
    DeserializationError error = deserializeJson(doc, data);
    if (error) {
      Serial.println("JSON parse failed!");
      request->send(400, "application/json", "{\"message\":\"Invalid JSON\"}");
      return;
    }

    String p_name = doc["name"].as<String>();
    String p_id = doc["id"].as<String>();
    String p_bed = doc["bed"].as<String>();
    String finger_ID = doc["fg"].as<String>();
    String p_date = doc["Date"].as<String>();
    String p_time = doc["Time"].as<String>();

    p_name.trim();
    p_id.trim();
    p_bed.trim();
    finger_ID.trim();
    p_date.trim();
    p_time.trim();

    int idLength = p_id.length();
    // Prepare an array of strings for line-by-line printing
    String dataLines[] = {
      "Name: " + p_name.substring(0, 5),
      "ID: " + p_id.substring(idLength - 5),
      "Bed: " + p_bed.substring(0, 3),
      "FingerID: " + finger_ID.substring(0, 5),
      "Date: " + p_date.substring(0, 10),
      "Time: " + p_time.substring(0, 9)
    };

    // Call the function to display the data
    printLineByLine(display, dataLines, 6);  // Pass the array and the number of lines
    //    // Clear the screen completely
    //    display.clearDisplay();
    //    display.display();       // Update the display to show a blank screen
    //    delay(100);
    //    displayValue(p_name, p_id, p_bed, finger_ID, p_date, p_time);

    Serial.println("Data received:");
    Serial.print(p_name.length());
    Serial.println("\tName: " + p_name);
    Serial.print(p_id.length());
    Serial.println("\tId: " + p_id);
    Serial.print(p_bed.length());
    Serial.println("\tBed no: " + p_bed);
    Serial.print(finger_ID.length());
    Serial.println("\tfinger id: " + finger_ID);
    Serial.print(p_date.length());
    Serial.println("\tDate: " + p_date);
    Serial.print(p_time.length());
    Serial.println("\ttime: " + p_time);

    request->send(200, "application/json", "{\"message\":\"Data displayed successfully\"}");
  });

  // Start the server
  server.begin();
}

void displayValue(String p_name, String p_id, String p_bed, String finger_ID, String p_date, String p_time) {
  // Clear the screen completely
  display.clearDisplay();
  display.display();       // Update the display to show a blank screen
  delay(100);

  // Now print the updated content line by line
  display.setTextSize(1);
  display.setTextColor(WHITE);

  display.setCursor(0, 0);
  display.print("Name: ");
  //if (p_name.length() > 10) p_name = p_name.substring(0, 10);  // Limit to 10 chars
  display.print(p_name.substring(0, 5));

  display.setCursor(0, 10);
  display.print("ID: ");
  int idLength = p_id.length();
  display.print(p_id.substring(idLength - 5));

  display.setCursor(0, 20);
  display.print("BED no : ");
  //if (p_name.length() > 10) p_name = p_name.substring(0, 10);  // Limit to 10 chars
  display.print(p_bed.substring(0, 3));

  //display.setCursor(0, 20);
  ////display.print("Bed: ");
  //display.print(p_bed.substring(0, 5));
  //
  display.setCursor(0, 30);
  display.print("FingerID: ");
  display.print(finger_ID.substring(0, 5));
  //
  delay(2000);
  display.setCursor(0, 40);
  display.print("Date: ");
  display.print(p_date.substring(0, 10));
  //
  display.setCursor(0, 50);
  display.print("Time: ");
  display.print(p_time.substring(0, 9));

  display.display();  // Send updated buffer to OLED

}

void loop() {
  // Nothing needed in the loop
}
