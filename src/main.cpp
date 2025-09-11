#include <Arduino.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <ESPAsyncWebServer.h>
#include <DNSServer.h>
#include <SPIFFS.h>
#include <SD.h>
#include <SPI.h>

// Storage configuration - set which storage to use
#define USE_SPIFFS 0  // Set to 1 for SPIFFS, 0 for SD Card
#define SD_CS_PIN 5   // SD Card Chip Select pin

const char* AP_ssid = "Pharmassist v1";

AsyncWebServer webServer(80);
DNSServer dnsServer;

const byte DNS_port = 53;
IPAddress apIP(192, 168, 4, 1);
IPAddress netMsk(255, 255, 255, 0);

// Storage interface variables
bool storageInitialized = false;
String storageType = "";

// Storage interface functions
bool initStorage() {
#if USE_SPIFFS
  storageType = "SPIFFS";
  Serial.println("Initializing SPIFFS...");
  if(!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return false;
  }
  Serial.println("SPIFFS mounted successfully");
  return true;
#else
  storageType = "SD Card";
  Serial.println("Initializing SD Card...");
  if(!SD.begin(SD_CS_PIN)) {
    Serial.println("Card Mount Failed");
    return false;
  }
  
  uint8_t cardType = SD.cardType();
  if(cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    return false;
  }
  
  Serial.print("SD Card Type: ");
  if(cardType == CARD_MMC) {
    Serial.println("MMC");
  } else if(cardType == CARD_SD) {
    Serial.println("SDSC");
  } else if(cardType == CARD_SDHC) {
    Serial.println("SDHC");
  } else {
    Serial.println("UNKNOWN");
  }
  
  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);
  Serial.println("SD Card mounted successfully");
  return true;
#endif
}

bool fileExists(const char* path) {
#if USE_SPIFFS
  return SPIFFS.exists(path);
#else
  return SD.exists(path);
#endif
}

File openFile(const char* path, const char* mode = "r") {
#if USE_SPIFFS
  return SPIFFS.open(path, mode);
#else
  return SD.open(path, mode);
#endif
}

void serveFile(AsyncWebServerRequest *request, const char* filename, const char* contentType) {
  Serial.printf("Serving %s from %s\n", filename, storageType.c_str());
  if(fileExists(filename)) {
#if USE_SPIFFS
    request->send(SPIFFS, filename, contentType);
#else
    request->send(SD, filename, contentType);
#endif
  } else {
    Serial.printf("%s not found in %s\n", filename, storageType.c_str());
    request->send(404, "text/plain", String(filename) + " not found in " + storageType);
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("ESP32 Started");

  // Initialize storage
  storageInitialized = initStorage();
  if(!storageInitialized) {
    Serial.printf("Failed to initialize %s. Web server will not serve files.\n", storageType.c_str());
    // Continue anyway - the device can still create an AP, just won't serve files
  }

  // Configure AP with static IP
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, netMsk);
  WiFi.softAP(AP_ssid);
  Serial.println("WiFi AP Started");
  Serial.print("AP IP: ");
  Serial.println(WiFi.softAPIP());

  // DNS server (for captive portal)
  dnsServer.start(DNS_port, "*", apIP);
  Serial.println("DNS Server started for captive portal");

  // mDNS
  if (!MDNS.begin("pharmassist")) {
    Serial.println("Error starting mDNS");
  } else {
    Serial.println("mDNS Started: http://pharmassist.local");
  }

  // Enable CORS globally
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers", "Content-Type, Authorization");

  // Main routes - serving from storage
  webServer.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    if(!storageInitialized) {
      request->send(500, "text/plain", String("Storage (") + storageType + ") not initialized");
      return;
    }
    serveFile(request, "/index.html", "text/html");
  });

  // Main Page
  webServer.on("/index.html", HTTP_GET, [](AsyncWebServerRequest *request) {
    if(!storageInitialized) {
      request->send(500, "text/plain", String("Storage (") + storageType + ") not initialized");
      return;
    }
    serveFile(request, "/index.html", "text/html");
  });

  // CSS file
  webServer.on("/styles.css", HTTP_GET, [](AsyncWebServerRequest *request) {
    if(!storageInitialized) {
      request->send(500, "text/plain", String("Storage (") + storageType + ") not initialized");
      return;
    }
    serveFile(request, "/styles.css", "text/css");
  });

  // JavaScript file
  webServer.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    if(!storageInitialized) {
      request->send(500, "text/plain", String("Storage (") + storageType + ") not initialized");
      return;
    }
    serveFile(request, "/script.js", "application/javascript");
  });

  // Storage info endpoint
  webServer.on("/storage-info", HTTP_GET, [](AsyncWebServerRequest *request) {
    String info = "Storage Type: " + storageType + "\n";
    info += "Status: " + String(storageInitialized ? "Initialized" : "Failed") + "\n";
    
    if(storageInitialized) {
#if USE_SPIFFS
      info += "Total Bytes: " + String(SPIFFS.totalBytes()) + "\n";
      info += "Used Bytes: " + String(SPIFFS.usedBytes()) + "\n";
      info += "Free Bytes: " + String(SPIFFS.totalBytes() - SPIFFS.usedBytes()) + "\n";
#else
      uint64_t cardSize = SD.cardSize() / (1024 * 1024);
      uint64_t totalBytes = SD.totalBytes() / (1024 * 1024);
      uint64_t usedBytes = SD.usedBytes() / (1024 * 1024);
      info += "Card Size: " + String((unsigned long)cardSize) + " MB\n";
      info += "Total: " + String((unsigned long)totalBytes) + " MB\n";
      info += "Used: " + String((unsigned long)usedBytes) + " MB\n";
      info += "Free: " + String((unsigned long)(totalBytes - usedBytes)) + " MB\n";
#endif
    }
    
    request->send(200, "text/plain", info);
  });

  // Generic static file handler
  if(storageInitialized) {
#if USE_SPIFFS
    webServer.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");
#else
    webServer.serveStatic("/", SD, "/").setDefaultFile("index.html");
#endif
  }

  // Handle favicon requests
  webServer.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request) {
    if(!storageInitialized) {
      request->send(404, "text/plain", "Storage not available");
      return;
    }
    serveFile(request, "/favicon.ico", "image/x-icon");
  });

  // Captive portal handler
  webServer.onNotFound([](AsyncWebServerRequest *request) {
    Serial.print("Request for: ");
    Serial.println(request->url());
    
    // For captive portal, redirect to main page
    if (!request->url().startsWith("/api/")) {
      String redirectURL = "http://" + WiFi.softAPIP().toString() + "/";
      request->redirect(redirectURL);
    } else {
      request->send(404, "text/plain", "Not Found");
    }
  });

  webServer.begin();
  Serial.println("Web Server started on port 80");
  Serial.printf("Using %s for file storage\n", storageType.c_str());
  Serial.print("Access the web interface at: http://");
  Serial.println(WiFi.softAPIP());
  Serial.println("Storage info available at: http://" + WiFi.softAPIP().toString() + "/storage-info");
}

String inputString = "";       // stores user input from Serial

void loop() {
  dnsServer.processNextRequest();
  delay(10);
}