#include <Arduino.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <ESPAsyncWebServer.h>
#include <DNSServer.h>
#include "Storage_Manager.h"

// Configuration - Choose storage type
#define STORAGE_TYPE STORAGE_SD    // or STORAGE_SPIFFS
#define SD_CS_PIN 5                // SD Card Chip Select pin (only used for SD)

const char* AP_ssid = "Pharmassist v1";

AsyncWebServer webServer(80);
DNSServer dnsServer;

const byte DNS_port = 53;
IPAddress apIP(192, 168, 4, 1);
IPAddress netMsk(255, 255, 255, 0);

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("ESP32 Started");

  // Initialize storage using the StorageManager
  bool storageOK = Storage.begin(STORAGE_TYPE, SD_CS_PIN);
  if (!storageOK) {
    Serial.println("Storage initialization failed. Web server will have limited functionality.");
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

  // Helper function to serve files
  auto serveFile = [](AsyncWebServerRequest *request, const String& filename) {
    if (!Storage.isInitialized()) {
      request->send(500, "text/plain", "Storage not initialized");
      return;
    }
    
    if (Storage.exists(filename)) {
      String contentType = Storage.getContentType(filename);
      request->send(*Storage.getFS(), filename, contentType);
      Serial.println("Served: " + filename + " (" + contentType + ")");
    } else {
      request->send(404, "text/plain", filename + " not found in " + Storage.getStorageType());
      Serial.println("File not found: " + filename);
    }
  };

  // Main routes
  webServer.on("/", HTTP_GET, [serveFile](AsyncWebServerRequest *request) {
    serveFile(request, "/index.html");
  });

  webServer.on("/index.html", HTTP_GET, [serveFile](AsyncWebServerRequest *request) {
    serveFile(request, "/index.html");
  });

  webServer.on("/styles.css", HTTP_GET, [serveFile](AsyncWebServerRequest *request) {
    serveFile(request, "/styles.css");
  });

  webServer.on("/script.js", HTTP_GET, [serveFile](AsyncWebServerRequest *request) {
    serveFile(request, "/script.js");
  });

  webServer.on("/favicon.ico", HTTP_GET, [serveFile](AsyncWebServerRequest *request) {
    serveFile(request, "/favicon.ico");
  });

  // Storage info endpoint
  webServer.on("/storage-info", HTTP_GET, [](AsyncWebServerRequest *request) {
    StorageInfo info = Storage.getInfo();
    
    String response = "Storage Type: " + info.type + "\n";
    response += "Status: " + String(info.initialized ? "Initialized" : "Failed") + "\n";
    
    if (info.initialized) {
      if (Storage.getCurrentStorage() == STORAGE_SD && !info.cardType.isEmpty()) {
        response += "Card Type: " + info.cardType + "\n";
      }
      
      // Convert bytes to MB for better readability
      float totalMB = info.totalBytes / (1024.0 * 1024.0);
      float usedMB = info.usedBytes / (1024.0 * 1024.0);
      float freeMB = info.freeBytes / (1024.0 * 1024.0);
      
      response += "Total: " + String(totalMB, 2) + " MB\n";
      response += "Used: " + String(usedMB, 2) + " MB\n";
      response += "Free: " + String(freeMB, 2) + " MB\n";
    }
    
    request->send(200, "text/plain", response);
  });

  // File management endpoints
  webServer.on("/api/files", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (!Storage.isInitialized()) {
      request->send(500, "application/json", "{\"error\":\"Storage not initialized\"}");
      return;
    }
    
    // List files in root directory
    String json = "{\"files\":[";
    File root = Storage.openDir("/");
    if (root) {
      bool first = true;
      File file = root.openNextFile();
      while (file) {
        if (!first) json += ",";
        json += "{";
        json += "\"name\":\"" + String(file.name()) + "\",";
        json += "\"size\":" + String(file.size()) + ",";
        json += "\"isDir\":" + String(file.isDirectory() ? "true" : "false");
        json += "}";
        first = false;
        file = root.openNextFile();
      }
      root.close();
    }
    json += "]}";
    
    request->send(200, "application/json", json);
  });

  // File upload endpoint
  webServer.on("/api/upload", HTTP_POST, [](AsyncWebServerRequest *request) {
    request->send(200);
  }, [](AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final) {
    if (!Storage.isInitialized()) {
      return;
    }
    
    static File uploadFile;
    
    if (index == 0) {
      // Start of file
      String path = "/" + filename;
      uploadFile = Storage.open(path, "w");
      if (!uploadFile) {
        Serial.println("Failed to create file: " + path);
        return;
      }
      Serial.println("Starting upload: " + path);
    }
    
    if (len) {
      uploadFile.write(data, len);
    }
    
    if (final) {
      uploadFile.close();
      Serial.println("Upload completed: " + filename + " (" + String(index + len) + " bytes)");
    }
  });

  // Generic static file handler
  if (Storage.isInitialized()) {
    webServer.serveStatic("/", *Storage.getFS(), "/").setDefaultFile("index.html");
  }

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
  Serial.println("Using " + Storage.getStorageType() + " for file storage");
  Serial.print("Access the web interface at: http://");
  Serial.println(WiFi.softAPIP());
  Serial.println("Endpoints:");
  Serial.println("  /storage-info - Storage information");
  Serial.println("  /api/files - List files");
  Serial.println("  /api/upload - File upload (POST)");
}

String inputString = "";

void loop() {
  dnsServer.processNextRequest();
  delay(10);
}