/**
 **************************************************
 *
 * @file        sketch_main.ino
 * @brief       Gesture detection with APDS-9960 and LCD display,
 *              including WiFi connectivity and HTTP POST requests.
 *
 * @author      Marko Toldi
 *
 **************************************************
 */
#include "APDS9960-SOLDERED.h"
#include "16x2-LCD-SOLDERED.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <map>

// Initialize gesture sensor and LCD display
APDS_9960 apds;
LCD lcd;

// WiFi credentials (replace with your own SSID and password)
const char* ssid = "vipermain";
const char* password = "88888888";

// Mapping gestures to corresponding text labels
std::map<int, String> gestureMap = {
    {GESTURE_UP, "UP"},
    {GESTURE_DOWN, "DOWN"},
    {GESTURE_LEFT, "LEFT"},
    {GESTURE_RIGHT, "RIGHT"}
};

/**
 * @brief Sets up WiFi connection, initializes LCD, and starts the gesture sensor.
 */
void setup() {
    Serial.begin(115200); // Start serial communication
    while (!Serial);      // Wait for Serial Monitor to open (for debugging)

    lcd.begin();     // Initialize the LCD
    lcd.backlight(); // Turn on the LCD backlight

    // Initialize the APDS-9960 gesture sensor
    if (!apds.begin()) {
        Serial.println("Error initializing APDS-9960 sensor!");
        while (1); // Halt execution if the sensor is not available
    }

    Serial.println("Detecting gestures...");

    // Connect to WiFi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi!");
}

/**
 * @brief Sends a POST request with a predefined number when "UP" gesture is detected.
 */
void sendPOSTRequest() {
    HTTPClient http;

    // Define the URL where the POST request will be sent
    String url = "https://webhook.site/45b37a03-6c14-45f9-b70c-4c079b092a9e"; // Replace with your webhook URL

    http.begin(url); // Start the HTTP request
    http.addHeader("Content-Type", "application/json"); // Set content type to JSON

    // Define the data payload to send
    int numberToPost = 58;
    String payload = "{\"gesture\": " + String(numberToPost) + "}";

    // Send the POST request
    int httpCode = http.POST(payload);

    // Check if the request was successful
    if (httpCode > 0) {
        Serial.printf("POST request sent, response code: %d\n", httpCode);
    } else {
        Serial.printf("Error sending POST request: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end(); // Close the HTTP connection
}

/**
 * @brief Continuously checks for gestures and processes them accordingly.
 */
void loop() {
    // Check if a gesture has been detected
    if (apds.gestureAvailable()) {
        int gesture = apds.readGesture(); // Read the detected gesture
        lcd.clear(); // Clear the LCD before displaying new data

        // Check if the detected gesture exists in the map
        if (gestureMap.count(gesture)) {
            Serial.println("Detected " + gestureMap[gesture]); // Print gesture to Serial Monitor
            lcd.print(gestureMap[gesture]); // Display gesture on LCD

            // If the detected gesture is "UP", send a POST request
            if (gesture == GESTURE_UP) {
                sendPOSTRequest();
            }
        } else {
            Serial.println("Unknown gesture"); // Handle unexpected gestures
        }

        delay(500); // Short delay to keep text visible on LCD
    }
}
