// #include <Arduino.h>
// #include <WiFi.h>
// #include <ESPmDNS.h>
// #include <ESPAsyncWebServer.h>
// #include <DNSServer.h>
// #include <SPIFFS.h>
// #include <SD.h>
// #include <SPI.h>
// #include <ArduinoJson.h>
// #include <map>
// #include <vector>
// #include "AccelStepper.h"

// #define stepperXStep_pin 26 // pin for stepper X axis' driver
// #define stepperXDir_pin 16 // pin for stepper X axis' direction
// #define XRightEndStop 4 // pin for X axis right stopper
// #define XLeftEndStop 35 // pin for X axis left stopper

// #define stepperYStep_pin 25 // pin for stepper Y axis' driver
// #define stepperYDir_pin 27 // pin for stepper Y axis' direction
// #define YTopStop 34  // pin for Y acis top stopper (from 36)
// #define YBottomStop 2 // pin for Y axis bottom stopper

// // #define stepperYRightStep_pin 25
// // #define stepperYRightDir_pin 27
//  #define ZStop 36 // pin for Z axis stopper (from 39)
// // #define YRightBottomStop 2

// #define stepperZStep_pin 17
// #define stepperZDir_pin 14
// // Storage configuration
// #define SD_CS_PIN 5   // SD Card Chip Select pin

// // RF ID READER PINS
// // #define SS_PIN   15   // SDA
// // #define RST_PIN  27   // RST
// // #define HSPI_SCK 14
// // #define HSPI_MISO 12
// // #define HSPI_MOSI 13

// // SPIClass hspi(HSPI);
// // MFRC522 mfrc522(SS_PIN, RST_PIN);


// const char* AP_ssid = "PharmaAssist";

// AccelStepper stepperX(AccelStepper::DRIVER, stepperXStep_pin, stepperXDir_pin);
// AccelStepper stepperY(AccelStepper::DRIVER, stepperYStep_pin, stepperYDir_pin); 
// AccelStepper stepperZ(AccelStepper::DRIVER, stepperZStep_pin, stepperZDir_pin); 

// AsyncWebServer webServer(80);
// DNSServer dnsServer;

// const byte DNS_port = 53;
// IPAddress apIP(192, 168, 4, 1);
// IPAddress netMsk(255, 255, 255, 0);

// // Storage interface variables
// bool useSDCard = false;
// bool storageInitialized = false;
// String storageType = "";

// // Session management
// struct Session {
//   String token;
//   String username;
//   String fullName;
//   unsigned long createdAt;
//   unsigned long lastAccessed;
// };

// std::map<String, Session> activeSessions;
// const unsigned long SESSION_TIMEOUT = 3600000; // 1 hour in milliseconds

// // User database structure
// struct User {
//   int id;
//   String username;
//   String password;
//   String fullName;
//   String email;
//   String license;
//   String department;
// };

// // Reduced sample user accounts (3 established doctors)
// std::vector<User> users = {
//   {1, "test", "test123", "Test User", "test@example.com", "MD-00001", "General Practice"},
//   {2, "admin", "admin123", "Dr. John Smith", "j.smith@hospital.com", "MD-12345", "Internal Medicine"},
//   {3, "doctor1", "pass123", "Dr. Sarah Johnson", "s.johnson@hospital.com", "MD-23456", "Cardiology"},
//   {4, "doctor2", "med456", "Dr. Michael Chen", "m.chen@hospital.com", "MD-34567", "Emergency Medicine"}
// };

// // Prescription struct for storing submitted prescriptions
// struct Medication {
//   String medicationName;
//   String strength;
//   String dosageForm;
//   String frequency;
// };

// struct Prescription {
//   String id;
//   String patientName;
//   String patientMRN;
//   String ward;
//   String bedNumber;
//   std::vector<Medication> medications;
//   String status;
//   String date;
//   String prescribingPhysician;
//   String prescribingUsername; // New field to link to user
// };

// // Sample patient data
// struct Patient {
//   String name;
//   String mrn;
//   String ward;
//   String bed;
// };
// std::vector<Patient> patients = {
//   {"Sarah Wilson", "MRN-78901234", "cardiology", "Ward-A-12"},
//   {"Michael Rodriguez", "MRN-56789012", "internal", "Ward-B-08"},
//   {"Emma Thompson", "MRN-34567890", "emergency", "ER-03"},
//   {"Robert Chen", "MRN-23456789", "outpatient", ""},
//   {"Lisa Williams", "MRN-12345678", "cardiology", "Ward-A-25"},
//   {"James Anderson", "MRN-11223344", "internal", "Ward-B-15"},
//   {"Maria Garcia", "MRN-55667788", "emergency", "ER-07"}
// };

// // Notification structure - now linked to specific users
// struct Notification {
//   String id;
//   String title;
//   String content;
//   String type;
//   String time;
//   bool read;
//   bool actionRequired;
//   String relatedOrderId;
//   String assignedToUsername; // New field to link notification to specific user
// };

// // Notifications linked to specific users
// std::vector<Notification> notifications = {
//   // Notifications for Dr. John Smith (admin)
//   {"NOTIF-001", "Stock Alert: Medicine 5", "Limited stock remaining. Your prescription RX-2024-002 may experience delays. Alternative formulation available.","success",   "30 minutes ago", false, true, "RX-2024-002", "admin"},
//   {"NOTIF-002", "Prescription Approved", "Medicine 2 prescription for Sarah Wilson has been approved by pharmacy. Ready for dispensing.", "success",  "2 hours ago", true, false, "RX-2024-001", "admin"},
  
//   // Notifications for Dr. Sarah Johnson (doctor1)
//   {"NOTIF-003", "Patient Update Required", "Medicine 7 dosage for Lisa Williams requires adjustment based on latest INR results. Please review.",   "warning", "1 hour ago", false, true, "RX-2024-006", "doctor1"},
//   {"NOTIF-004", "Prescription Dispensed", "Medicine 4 for James Anderson has been successfully dispensed. Patient notified for collection.", "success",  "4 hours ago", true, false, "RX-2024-007", "doctor1"},
  
//   // Notifications for Dr. Michael Chen (doctor2)
//   {"NOTIF-005", "Emergency Medication Available", "Medicine 6 for Emma Thompson is now ready for immediate collection from emergency pharmacy.", "success",  "15 minutes ago", false, true, "RX-2024-003", "doctor2"},
//   {"NOTIF-006", "Drug Interaction Alert", "Potential interaction detected between prescribed Medicine 9 and patient's existing Medicine 7 therapy for Maria Garcia. Review recommended.",   "warning", "45 minutes ago", false, true, "RX-2024-008", "doctor2"}
// };

// std::vector<Prescription> prescriptions = {
//   // ---------------- Test User ----------------
//   {"RX-2024-100", "Maria Garcia", "MRN-55667788", "emergency", "ER-07",
//    {{"Medicine 1","500mg","capsule","tid"}, {"Medicine 8","500mg","tablet","bid"}},
//    "pending",  "2024-01-20", "Test User", "test"},
//   {"RX-2024-101", "Robert Chen", "MRN-23456789", "outpatient", "",
//    {{"Medicine 3","500mg","tablet","tid"}, {"Medicine 2","10mg","tablet","bid"}},
//    "dispensing", "2024-01-21", "Test User", "test"},
//   {"RX-2024-102", "James Anderson", "MRN-11223344", "internal", "Ward-B-15",
//    {{"Medicine 8","500mg","tablet","tid"}},
//    "ready", "2024-01-22", "Test User", "test"},

//   // ---------------- Dr. John Smith ----------------
//   {"RX-2024-001", "Sarah Wilson", "MRN-78901234", "cardiology", "Ward-A-12",
//    {{"Medicine 2","10mg","tablet","tid"}},
//    "dispensing", "2024-01-16", "Dr. John Smith", "admin"},
//   {"RX-2024-002", "Michael Rodriguez", "MRN-56789012", "internal", "Ward-B-08",
//    {{"Medicine 5","100IU/ml","injection","bid"}},
//    "pending", "2024-01-16", "Dr. John Smith", "admin"},
//   {"RX-2024-009", "Robert Chen", "MRN-23456789", "outpatient", "",
//    {{"Medicine 3","500mg","tablet","tid"}},
//    "ready", "2024-01-15", "Dr. John Smith", "admin"},
//   // Multi-medication example
//   {"RX-2024-012", "James Anderson", "MRN-11223344", "internal", "Ward-B-15",
//    {{"Medicine 4","20mg","tablet","tid"}, {"Medicine 8","500mg","tablet","tid"}},
//   "pending", "2024-01-18", "Dr. John Smith", "admin"},

//   // ---------------- Dr. Sarah Johnson ----------------
//   {"RX-2024-006", "Lisa Williams", "MRN-12345678", "cardiology", "Ward-A-25",
//    {{"Medicine 7","5mg","tablet","tid"}},
//    "partially-dispensed", "2024-01-14", "Dr. Sarah Johnson", "doctor1"},
//   {"RX-2024-007", "James Anderson", "MRN-11223344", "internal", "Ward-B-15",
//    {{"Medicine 4","20mg","tablet","bid"}},
//    "dispensed", "2024-01-13", "Dr. Sarah Johnson", "doctor1"},
//   {"RX-2024-010", "Sarah Wilson", "MRN-78901234", "cardiology", "Ward-A-12",
//    {{"Medicine 8","81mg","tablet","tid"}}, // Aspirin replaced with Medicine 8 from the static list
//    "ready", "2024-01-16", "Dr. Sarah Johnson", "doctor1"},

//   // ---------------- Dr. Michael Chen ----------------
//   {"RX-2024-003", "Emma Thompson", "MRN-34567890", "emergency", "ER-03",
//    {{"Medicine 6","1mg/ml","injection","once"}},
//    "ready", "2024-01-16", "Dr. Michael Chen", "doctor2"},
//   {"RX-2024-008", "Maria Garcia", "MRN-55667788", "emergency", "ER-07",
//    {{"Medicine 9","250mg","tablet","once"}},
//     "dispensing", "2024-01-16", "Dr. Michael Chen", "doctor2"},
//   {"RX-2024-011", "Emma Thompson", "MRN-34567890", "emergency", "ER-03",
//    {{"Medicine 9","20mg","tablet","once"}}, // replaced with Medicine 8 to stay inside static 9
//   "pending", "2024-01-16", "Dr. Michael Chen", "doctor2"}
// };

// long initial_homing=-1; // to make the direction go counterclockwise
// long maxSpeed = 5000.0; // max speed possible for stepper motor
// long accel = 5000.0; // how fast does the stepper motor moves
// long xInitial_max=12500; // standard steps before slowing down for x axis and prevent collision with the stopper
// // long yRInitial_max=18000; 
// long yLInitial_max=18000; // standard steps before slowing down for y axis and prevent collision with the stopper
// long xMax = xInitial_max; // identified max steps for x axis
// long yMax = yLInitial_max; // identified max steps for y axis
// int row = 0; // variable for the row of a dispenser
// int column = 0; // variable for the column of a dispenser

// // Medication master list (1-9)
// const char* medicationList[9] = {
//   "Medicine 1",
//   "Medicine 2",
//   "Medicine 3",
//   "Medicine 4",
//   "Medicine 5",
//   "Medicine 6",
//   "Medicine 7",
//   "Medicine 8",
//   "Medicine 9"
// };

// const char* frequencyList[3] = {
//   "once",
//   "bid",
//   "tid"
// };

// int getMedicationFrequency(const String& name) {
//   for (int i = 0; i < 3; ++i) {
//     if (name.equalsIgnoreCase(frequencyList[i])) return i + 1;
//   }
//   return 0;
// }
// int getMedicationIndex(const String& name) {
//   for (int i = 0; i < 9; ++i) {
//     if (name.equalsIgnoreCase(medicationList[i])) return i + 1;
//   }
//   return 0;
// }
// String getMedicationName(int idx) {
//   if (idx >= 1 && idx <= 9) return String(medicationList[idx - 1]);
//   return "";
// }

// // Session utility functions
// String generateSessionToken() {
//   String token = "";
//   const char charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
//   for (int i = 0; i < 32; i++) {
//     token += charset[random(0, sizeof(charset) - 1)];
//   }
//   return token;
// }

// void cleanupExpiredSessions() {
//   unsigned long currentTime = millis();
//   auto it = activeSessions.begin();
//   while (it != activeSessions.end()) {
//     if (currentTime - it->second.lastAccessed > SESSION_TIMEOUT) {
//       Serial.printf("Cleaning up expired session: %s\n", it->first.c_str());
//       it = activeSessions.erase(it);
//     } else {
//       ++it;
//     }
//   }
// }

// bool validateSession(const String& token) {
//   if (token.isEmpty()) return false;
  
//   auto it = activeSessions.find(token);
//   if (it != activeSessions.end()) {
//     unsigned long currentTime = millis();
//     if (currentTime - it->second.lastAccessed < SESSION_TIMEOUT) {
//       it->second.lastAccessed = currentTime; // Update last accessed time
//       return true;
//     } else {
//       activeSessions.erase(it); // Remove expired session
//     }
//   }
//   return false;
// }

// String getSessionToken(AsyncWebServerRequest *request) {
//   // Check for session token in cookies
//   if (request->hasHeader("Cookie")) {
//     String cookies = request->getHeader("Cookie")->value();
//     int tokenStart = cookies.indexOf("session_token=");
//     if (tokenStart != -1) {
//       tokenStart += 14; // Length of "session_token="
//       int tokenEnd = cookies.indexOf(";", tokenStart);
//       if (tokenEnd == -1) tokenEnd = cookies.length();
//       return cookies.substring(tokenStart, tokenEnd);
//     }
//   }
  
//   // Check for Authorization header
//   if (request->hasHeader("Authorization")) {
//     String auth = request->getHeader("Authorization")->value();
//     if (auth.startsWith("Bearer ")) {
//       return auth.substring(7);
//     }
//   }
  
//   return "";
// }

// String getCurrentUsername(const String& token) {
//   auto it = activeSessions.find(token);
//   if (it != activeSessions.end()) {
//     return it->second.username;
//   }
//   return "";
// }

// // Storage interface functions
// bool initStorage() {
//   Serial.println("Trying SD Card...");
//   int retries = 0;
//   const int maxRetries = 100;
//   while (retries < maxRetries) {
//     if (SD.begin(SD_CS_PIN)) {
//       uint8_t cardType = SD.cardType();
//       if (cardType != CARD_NONE) {
//         useSDCard = true;
//         storageType = "SD Card";
//         Serial.print("SD Card Type: ");
//         if (cardType == CARD_MMC) {
//           Serial.println("MMC");
//         } else if (cardType == CARD_SD) {
//           Serial.println("SDSC");
//         } else if (cardType == CARD_SDHC) {
//           Serial.println("SDHC");
//         } else {
//           Serial.println("UNKNOWN");
//         }
//         uint64_t cardSize = SD.cardSize() / (1024 * 1024);
//         Serial.printf("SD Card Size: %lluMB\n", cardSize);
//         Serial.println("SD Card mounted successfully");
//         return true;  
//       }
//     }
//     Serial.println("SD Card not detected, retrying in 2 seconds...");
//     delay(2000);
//     retries++;
//   }
//   Serial.println("SD Card failed to initialize after multiple attempts.");
//   return false;
// }

// bool fileExists(const char* path) {
//   // Only SD card supported
//   return SD.exists(path);
// }

// File openFile(const char* path, const char* mode = "r") {
//   // Only SD card supported
//   return SD.open(path, mode);
// }

// void serveFile(AsyncWebServerRequest *request, const char* filename, const char* contentType) {
//   Serial.printf("[LOG] serveFile: Request for %s (%s)\n", filename, contentType);
//   if(fileExists(filename)) {
//     Serial.printf("[LOG] serveFile: Found %s, sending file.\n", filename);
//     request->send(SD, filename, contentType);
//   } else {
//     Serial.printf("[LOG] serveFile: %s not found in %s\n", filename, storageType.c_str());
//     request->send(404, "text/plain", String(filename) + " not found in " + storageType);
//   }
// }

// // Authentication function
// User* authenticateUser(const String& username, const String& password) {
//   for (auto& user : users) {
//     if (user.username == username && user.password == password) {
//       return &user;
//     }
//   }
//   return nullptr;
// }

// void printHelp() {
//   Serial.println("Available Serial Commands:");
//   Serial.println("  help, h           - Show this help menu");
//   Serial.println("  status, s         - Show system status");
//   Serial.println("  users, u          - List all users");
//   Serial.println("  sessions, sess    - Show active sessions");
//   Serial.println("  prescriptions, rx, p - List all prescriptions");
//   Serial.println("  patients, pat     - List all patients");
//   Serial.println("  notifications, notif, n - List all notifications");
//   Serial.println("  medications, meds, m - List available medications");
//   Serial.println("  storage, stor     - Show storage information");
//   Serial.println("  memory, mem       - Show memory usage");
//   Serial.println("  wifi              - Show WiFi information");
//   Serial.println("  rx <id>           - Show prescription details");
//   Serial.println("  user <username>   - Show user details");
//   Serial.println("  session <token>   - Show session details");
//   Serial.println("  clear, cls        - Clear screen");
//   Serial.println("  reset             - Restart ESP32");
//   Serial.println("  cleanup           - Clean expired sessions");
//   Serial.println("  all, dump         - Dump all data");
//   Serial.println("  notif <username>  - Show notifications for specific user");
// }

// void printSystemStatus() {
//   Serial.println("=== SYSTEM STATUS ===");
//   Serial.printf("Uptime: %lu seconds\n", millis() / 1000);
//   Serial.printf("Free Heap: %u bytes\n", ESP.getFreeHeap());
//   Serial.printf("Chip Model: %s\n", ESP.getChipModel());
//   Serial.printf("CPU Frequency: %u MHz\n", ESP.getCpuFreqMHz());
//   Serial.printf("Flash Size: %u bytes\n", ESP.getFlashChipSize());
//   Serial.printf("Storage Type: %s\n", storageType.c_str());
//   Serial.printf("Storage Initialized: %s\n", storageInitialized ? "YES" : "NO");
//   Serial.printf("Active Users: %d\n", users.size());
//   Serial.printf("Active Sessions: %d\n", activeSessions.size());
//   Serial.printf("Total Prescriptions: %d\n", prescriptions.size());
//   Serial.printf("Total Notifications: %d\n", notifications.size());
//   Serial.printf("Total Patients: %d\n", patients.size());
// }

// void printUsers() {
//   Serial.println("=== USER DATABASE ===");
//   Serial.printf("Total Users: %d\n\n", users.size());
  
//   for (const auto& user : users) {
//     Serial.printf("ID: %d\n", user.id);
//     Serial.printf("  Username: %s\n", user.username.c_str());
//     Serial.printf("  Password: %s\n", user.password.c_str());
//     Serial.printf("  Full Name: %s\n", user.fullName.c_str());
//     Serial.printf("  Email: %s\n", user.email.c_str());
//     Serial.printf("  License: %s\n", user.license.c_str());
//     Serial.printf("  Department: %s\n", user.department.c_str());
    
//     // Count prescriptions and notifications for this user
//     int prescriptionCount = 0;
//     int notificationCount = 0;
//     for (const auto& rx : prescriptions) {
//       if (rx.prescribingUsername == user.username) prescriptionCount++;
//     }
//     for (const auto& notif : notifications) {
//       if (notif.assignedToUsername == user.username) notificationCount++;
//     }
//     Serial.printf("  Prescriptions: %d\n", prescriptionCount);
//     Serial.printf("  Notifications: %d\n", notificationCount);
//     Serial.println();
//   }
// }

// void printActiveSessions() {
//   Serial.println("=== ACTIVE SESSIONS ===");
//   Serial.printf("Total Active Sessions: %d\n", activeSessions.size());
//   Serial.printf("Session Timeout: %lu ms (%lu minutes)\n", SESSION_TIMEOUT, SESSION_TIMEOUT / 60000);
//   Serial.println();
  
//   if (activeSessions.empty()) {
//     Serial.println("No active sessions.");
//     return;
//   }
  
//   unsigned long currentTime = millis();
//   for (const auto& pair : activeSessions) {
//     const Session& session = pair.second;
//     unsigned long ageMs = currentTime - session.createdAt;
//     unsigned long lastAccessMs = currentTime - session.lastAccessed;
    
//     Serial.printf("Token: %s\n", session.token.c_str());
//     Serial.printf("  Username: %s\n", session.username.c_str());
//     Serial.printf("  Full Name: %s\n", session.fullName.c_str());
//     Serial.printf("  Age: %lu seconds\n", ageMs / 1000);
//     Serial.printf("  Last Access: %lu seconds ago\n", lastAccessMs / 1000);
//     Serial.printf("  Expires in: %lu seconds\n", (SESSION_TIMEOUT - lastAccessMs) / 1000);
//     Serial.println();
//   }
// }

// void printPrescriptions() {
//   Serial.println("=== PRESCRIPTIONS DATABASE ===");
//   Serial.printf("Total Prescriptions: %d\n\n", prescriptions.size());
  
//   if (prescriptions.empty()) {
//     Serial.println("No prescriptions found.");
//     return;
//   }
  
//   for (const auto& rx : prescriptions) {
//     Serial.printf("ID: %s\n", rx.id.c_str());
//     Serial.printf("  Patient: %s (MRN: %s)\n", rx.patientName.c_str(), rx.patientMRN.c_str());
//     Serial.printf("  Ward: %s, Bed: %s\n", rx.ward.c_str(), rx.bedNumber.c_str());
//     Serial.printf("  Status: %s\n", rx.status.c_str());
//     Serial.printf("  Date: %s\n", rx.date.c_str());
//     Serial.printf("  Physician: %s (%s)\n", rx.prescribingPhysician.c_str(), rx.prescribingUsername.c_str());
    

//     Serial.printf("  Medications (%d):\n", rx.medications.size());
    
//     for (size_t i = 0; i < rx.medications.size(); i++) {
//       const auto& med = rx.medications[i];
//       Serial.printf("    %d. %s (%d) %s (%s) %s\n", 
//                    (int)i+1, med.medicationName.c_str(), getMedicationIndex(med.medicationName), med.dosageForm.c_str(), med.frequency.c_str());
//     }
//     Serial.println();
//   }
// }

// void printPatients() {
//   Serial.println("=== PATIENTS DATABASE ===");
//   Serial.printf("Total Patients: %d\n\n", patients.size());
  
//   for (const auto& patient : patients) {
//     Serial.printf("Name: %s\n", patient.name.c_str());
//     Serial.printf("  MRN: %s\n", patient.mrn.c_str());
//     Serial.printf("  Ward: %s\n", patient.ward.c_str());
//     Serial.printf("  Bed: %s\n", patient.bed.isEmpty() ? "N/A" : patient.bed.c_str());
//     Serial.println();
//   }
// }

// void printNotifications() {
//   Serial.println("=== NOTIFICATIONS ===");
//   Serial.printf("Total Notifications: %d\n\n", notifications.size());
  
//   int unreadCount = 0;
//   int actionRequiredCount = 0;
//   for (const auto& n : notifications) {
//     if (!n.read) unreadCount++;
//     if (n.actionRequired) actionRequiredCount++;
//   }
  
//   Serial.printf("Unread: %d, Action Required: %d\n\n", unreadCount, actionRequiredCount);
  
//   for (const auto& notif : notifications) {
//     Serial.printf("ID: %s [%s]\n", notif.id.c_str(), notif.read ? "READ" : "UNREAD");
//     Serial.printf("  Title: %s\n", notif.title.c_str());
//     Serial.printf("  Assigned to: %s\n", notif.assignedToUsername.c_str());
//     Serial.printf("  Type: %s\n", notif.type.c_str());
//     Serial.printf("  Time: %s\n", notif.time.c_str());
//     Serial.printf("  Action Required: %s\n", notif.actionRequired ? "YES" : "NO");
//     if (!notif.relatedOrderId.isEmpty()) {
//       Serial.printf("  Related Order: %s\n", notif.relatedOrderId.c_str());
//     }
//     Serial.printf("  Content: %s\n", notif.content.c_str());
//     Serial.println();
//   }
// }

// void printNotificationsForUser(String username) {
//   Serial.printf("=== NOTIFICATIONS FOR USER: %s ===\n", username.c_str());
  
//   std::vector<Notification> userNotifications;
//   for (const auto& n : notifications) {
//     if (n.assignedToUsername.equalsIgnoreCase(username)) {
//       userNotifications.push_back(n);
//     }
//   }
  
//   Serial.printf("Total Notifications for %s: %d\n\n", username.c_str(), userNotifications.size());
  
//   if (userNotifications.empty()) {
//     Serial.printf("No notifications found for user '%s'.\n", username.c_str());
//     return;
//   }
  
//   int unreadCount = 0;
//   int actionRequiredCount = 0;
//   for (const auto& n : userNotifications) {
//     if (!n.read) unreadCount++;
//     if (n.actionRequired) actionRequiredCount++;
//   }
  
//   Serial.printf("Unread: %d, Action Required: %d\n\n", unreadCount, actionRequiredCount);
  
//   for (const auto& notif : userNotifications) {
//     Serial.printf("ID: %s [%s]\n", notif.id.c_str(), notif.read ? "READ" : "UNREAD");
//     Serial.printf("  Title: %s\n", notif.title.c_str());
//     Serial.printf("  Type: %s\n", notif.type.c_str());
//     Serial.printf("  Time: %s\n", notif.time.c_str());
//     Serial.printf("  Action Required: %s\n", notif.actionRequired ? "YES" : "NO");
//     if (!notif.relatedOrderId.isEmpty()) {
//       Serial.printf("  Related Order: %s\n", notif.relatedOrderId.c_str());
//     }
//     Serial.printf("  Content: %s\n", notif.content.c_str());
//     Serial.println();
//   }
// }

// void printMedications() {
//   Serial.println("=== MEDICATION MASTER LIST ===");
//   Serial.println("Available Medications:");
  
//   for (int i = 0; i < 9; i++) {
//     Serial.printf("  %d. %s\n", i+1, medicationList[i]);
//   }
//   Serial.println();
// }

// void printStorageInfo() {
//   Serial.println("=== STORAGE INFORMATION ===");
//   Serial.printf("Storage Type: %s\n", storageType.c_str());
//   Serial.printf("Storage Initialized: %s\n", storageInitialized ? "YES" : "NO");
//   Serial.printf("Using SD Card: %s\n", useSDCard ? "YES" : "NO");
//   Serial.printf("SD CS Pin: %d\n", SD_CS_PIN);
  
//   if (useSDCard && storageInitialized) {
//     uint64_t cardSize = SD.cardSize() / (1024 * 1024);
//     uint64_t usedBytes = SD.usedBytes();
//     uint64_t totalBytes = SD.totalBytes();
    
//     Serial.printf("Card Size: %llu MB\n", cardSize);
//     Serial.printf("Used Space: %llu bytes\n", usedBytes);
//     Serial.printf("Total Space: %llu bytes\n", totalBytes);
//     Serial.printf("Free Space: %llu bytes\n", totalBytes - usedBytes);
    
//     uint8_t cardType = SD.cardType();
//     Serial.print("Card Type: ");
//     if (cardType == CARD_MMC) {
//       Serial.println("MMC");
//     } else if (cardType == CARD_SD) {
//       Serial.println("SDSC");
//     } else if (cardType == CARD_SDHC) {
//       Serial.println("SDHC");
//     } else {
//       Serial.println("UNKNOWN");
//     }
//   }
// }

// void printMemoryInfo() {
//   Serial.println("=== MEMORY INFORMATION ===");
//   Serial.printf("Free Heap: %u bytes\n", ESP.getFreeHeap());
//   Serial.printf("Largest Free Block: %u bytes\n", ESP.getMaxAllocHeap());
//   Serial.printf("Min Free Heap: %u bytes\n", ESP.getMinFreeHeap());
//   Serial.printf("Free PSRAM: %u bytes\n", ESP.getFreePsram());
  
//   // Calculate approximate memory usage by data structures
//   size_t userMemory = users.size() * sizeof(User);
//   size_t sessionMemory = activeSessions.size() * sizeof(Session);
//   size_t prescriptionMemory = prescriptions.size() * sizeof(Prescription);
//   size_t patientMemory = patients.size() * sizeof(Patient);
//   size_t notificationMemory = notifications.size() * sizeof(Notification);
  
//   Serial.println("\nApproximate Data Structure Memory Usage:");
//   Serial.printf("  Users: %u bytes (%d entries)\n", userMemory, users.size());
//   Serial.printf("  Sessions: %u bytes (%d entries)\n", sessionMemory, activeSessions.size());
//   Serial.printf("  Prescriptions: %u bytes (%d entries)\n", prescriptionMemory, prescriptions.size());
//   Serial.printf("  Patients: %u bytes (%d entries)\n", patientMemory, patients.size());
//   Serial.printf("  Notifications: %u bytes (%d entries)\n", notificationMemory, notifications.size());
//   Serial.printf("  Total Data: ~%u bytes\n", 
//                userMemory + sessionMemory + prescriptionMemory + patientMemory + notificationMemory);
// }

// void printWiFiInfo() {
//   Serial.println("=== WIFI INFORMATION ===");
//   Serial.printf("Mode: %s\n", WiFi.getMode() == WIFI_AP ? "Access Point" : "Station");
//   Serial.printf("AP SSID: %s\n", AP_ssid);
//   Serial.printf("AP IP Address: %s\n", WiFi.softAPIP().toString().c_str());
//   Serial.printf("Connected Clients: %d\n", WiFi.softAPgetStationNum());
//   Serial.printf("Hostname: %s\n", WiFi.getHostname());
//   Serial.printf("MAC Address: %s\n", WiFi.macAddress().c_str());
// }

// void printPrescriptionDetails(String rxId) {
//   Serial.printf("=== PRESCRIPTION DETAILS: %s ===\n", rxId.c_str());
  
//   bool found = false;
//   for (const auto& rx : prescriptions) {
//     if (rx.id.equalsIgnoreCase(rxId)) {
//       found = true;
//       Serial.printf("Prescription ID: %s\n", rx.id.c_str());
//       Serial.printf("Patient Name: %s\n", rx.patientName.c_str());
//       Serial.printf("Patient MRN: %s\n", rx.patientMRN.c_str());
//       Serial.printf("Ward: %s\n", rx.ward.c_str());
//       Serial.printf("Bed Number: %s\n", rx.bedNumber.c_str());
//       Serial.printf("Status: %s\n", rx.status.c_str());
//       Serial.printf("Date: %s\n", rx.date.c_str());
//       Serial.printf("Prescribing Physician: %s (%s)\n", rx.prescribingPhysician.c_str(), rx.prescribingUsername.c_str());

      
      
//       Serial.printf("\nMedications (%d):\n", rx.medications.size());
//       for (size_t i = 0; i < rx.medications.size(); i++) {
//         const auto& med = rx.medications[i];
//         Serial.printf("  %d. (%d) Medication: %s\n", (int)i+1, getMedicationIndex(med.medicationName), med.medicationName.c_str());
//         Serial.printf("     Strength: %s\n", med.strength.c_str());
//         Serial.printf("     Dosage Form: %s\n", med.dosageForm.c_str());
//         Serial.printf("     Frequency: %s\n", med.frequency.c_str());
//         Serial.println();
//       }
//       break;
//     }
//   }
  
//   if (!found) {
//     Serial.printf("Prescription with ID '%s' not found.\n", rxId.c_str());
//   }
// }

// void printUserDetails(String username) {
//   Serial.printf("=== USER DETAILS: %s ===\n", username.c_str());
  
//   bool found = false;
//   for (const auto& user : users) {
//     if (user.username.equalsIgnoreCase(username)) {
//       found = true;
//       Serial.printf("User ID: %d\n", user.id);
//       Serial.printf("Username: %s\n", user.username.c_str());
//       Serial.printf("Password: %s\n", user.password.c_str());
//       Serial.printf("Full Name: %s\n", user.fullName.c_str());
//       Serial.printf("Email: %s\n", user.email.c_str());
//       Serial.printf("License: %s\n", user.license.c_str());
//       Serial.printf("Department: %s\n", user.department.c_str());
      
//       // Count prescriptions and notifications for this user
//       int prescriptionCount = 0;
//       int notificationCount = 0;
//       int unreadNotifications = 0;
//       for (const auto& rx : prescriptions) {
//         if (rx.prescribingUsername == user.username) prescriptionCount++;
//       }
//       for (const auto& notif : notifications) {
//         if (notif.assignedToUsername == user.username) {
//           notificationCount++;
//           if (!notif.read) unreadNotifications++;
//         }
//       }
//       Serial.printf("Prescriptions: %d\n", prescriptionCount);
//       Serial.printf("Notifications: %d (%d unread)\n", notificationCount, unreadNotifications);
      
//       // Check if user has active sessions
//       Serial.println("\nActive Sessions:");
//       bool hasSession = false;
//       for (const auto& pair : activeSessions) {
//         if (pair.second.username.equalsIgnoreCase(username)) {
//           hasSession = true;
//           Serial.printf("  Token: %s\n", pair.second.token.c_str());
//           Serial.printf("  Created: %lu seconds ago\n", (millis() - pair.second.createdAt) / 1000);
//           Serial.printf("  Last Access: %lu seconds ago\n", (millis() - pair.second.lastAccessed) / 1000);
//         }
//       }
//       if (!hasSession) {
//         Serial.println("  No active sessions");
//       }
//       break;
//     }
//   }
  
//   if (!found) {
//     Serial.printf("User '%s' not found.\n", username.c_str());
//   }
// }

// void printSessionDetails(String token) {
//   Serial.printf("=== SESSION DETAILS: %s ===\n", token.c_str());
  
//   auto it = activeSessions.find(token);
//   if (it != activeSessions.end()) {
//     const Session& session = it->second;
//     unsigned long currentTime = millis();
//     unsigned long ageMs = currentTime - session.createdAt;
//     unsigned long lastAccessMs = currentTime - session.lastAccessed;
    
//     Serial.printf("Token: %s\n", session.token.c_str());
//     Serial.printf("Username: %s\n", session.username.c_str());
//     Serial.printf("Full Name: %s\n", session.fullName.c_str());
//     Serial.printf("Created At: %lu ms (system time)\n", session.createdAt);
//     Serial.printf("Last Accessed: %lu ms (system time)\n", session.lastAccessed);
//     Serial.printf("Session Age: %lu seconds\n", ageMs / 1000);
//     Serial.printf("Time Since Last Access: %lu seconds\n", lastAccessMs / 1000);
//     Serial.printf("Expires In: %lu seconds\n", (SESSION_TIMEOUT - lastAccessMs) / 1000);
//     Serial.printf("Valid: %s\n", (lastAccessMs < SESSION_TIMEOUT) ? "YES" : "NO (EXPIRED)");
//   } else {
//     Serial.printf("Session with token '%s' not found.\n", token.c_str());
//   }
// }

// void printAllData() {
//   Serial.println("=== COMPLETE DATA DUMP ===");
//   Serial.println();
  
//   printSystemStatus();
//   Serial.println();
  
//   printUsers();
//   Serial.println();
  
//   printActiveSessions();
//   Serial.println();
  
//   printPrescriptions();
//   Serial.println();
  
//   printPatients();
//   Serial.println();
  
//   printNotifications();
//   Serial.println();
  
//   printMedications();
//   Serial.println();
  
//   printStorageInfo();
//   Serial.println();
  
//   printMemoryInfo();
//   Serial.println();
  
//   printWiFiInfo();
//   Serial.println();
  
//   Serial.println("=== END DATA DUMP ===");
// }


// void PushStepper(){
//     Serial.println("pushing stepper Z axis");
//     stepperZ.setMaxSpeed(2000.0); 
//     stepperZ.setAcceleration(2000.0);
//     stepperZ.moveTo(1300);
//     while(stepperZ.distanceToGo()!=0){
//         stepperZ.run();
//     }
//     Serial.println("retracting stepper Z axis");
//    stepperZ.moveTo(0);
//       while(stepperZ.distanceToGo()!=0){
//        stepperZ.run();
//     }

// }


// void homing(){
//   stepperX.setMaxSpeed(500.0);
//   stepperX.setAcceleration(10000.0);
//   stepperY.setMaxSpeed(500.0);
//   stepperY.setAcceleration(10000.0);
//   stepperZ.setMaxSpeed(200.0);
//   stepperZ.setAcceleration(500.0);
//   Serial.println("");
//   Serial.println("Stepper X and Y axis is Homing...............");

//   while(digitalRead(XLeftEndStop) + digitalRead(YBottomStop)+digitalRead(ZStop)<3){
//   stepperX.moveTo(initial_homing);
//   stepperY.moveTo(initial_homing);
//   stepperZ.moveTo(initial_homing);
//    //The value of stopper motor is either 0 or 1. 1 means the motor pressed the stopper, otherwise 0
//   // stepperYRight.moveTo(initial_homing);
//     if(digitalRead(XLeftEndStop)==LOW){
//       stepperX.run(); //if the stopper is not pressed, the motor will keep running
//     }
//     if(digitalRead(YBottomStop)==LOW){
//       stepperY.run(); //if the stopper is not pressed, the motor will keep running
//     }
//     if(digitalRead(ZStop)==LOW){
//       stepperZ.run(); //if the stopper is not pressed, the motor will keep running
//     }
//   initial_homing--; // to command the stepper motor to move clockwise or negative direction 
//   }

//   stepperX.setMaxSpeed(100.0);
//   stepperX.setAcceleration(100.0);
//   stepperY.setMaxSpeed(100.0);
//   stepperY.setAcceleration(100.0);
//   stepperZ.setMaxSpeed(100.0);
//   stepperZ.setAcceleration(100.0);
//   initial_homing=1; // to command the stepper motor to move counter-clockwise or positive direction so that the stopper is not pressed

//   while ((digitalRead(XLeftEndStop) + digitalRead(YBottomStop) + digitalRead(ZStop))>0){
//   stepperX.moveTo(initial_homing);
//   stepperY.moveTo(initial_homing);
//   stepperZ.moveTo(initial_homing);
//    //The value of stopper motor is either 0 or 1. 1 means the motor pressed the stopper, otherwise 0

//   // stepperYRight.moveTo(initial_homing);
//     if(digitalRead(XLeftEndStop)==HIGH){
//       stepperX.run();
//     }
//     if(digitalRead(YBottomStop)==HIGH){
//       stepperY.run();
//     }
//     if(digitalRead(ZStop)==HIGH){
//       stepperZ.run();
//     }
//      initial_homing++; // to command the stepper motor to move counter-clockwise or positive direction until the stopper is not pressed
//   }
//   stepperX.setCurrentPosition(0); // indicates that the stepper X is at position 0
//   stepperY.setCurrentPosition(0); // indicates that the stepper Y is at position 0
//   stepperZ.setCurrentPosition(0); // indicates that the stepper Y is at position 0
//   Serial.println("Homing Done!");
// }

// void moveToMax(){
//    stepperX.setMaxSpeed(5000);
//    stepperX.setAcceleration(5000);
//    stepperY.setMaxSpeed(5000);
//    stepperY.setAcceleration(5000);


//     stepperX.moveTo(xInitial_max);
//     stepperY.moveTo(yLInitial_max);

//   while(stepperX.currentPosition()<xInitial_max || stepperY.currentPosition()<yLInitial_max)
//             // || stepperYRight.currentPosition()<yRInitial_max) 
//             {
//     if(digitalRead(XRightEndStop)==LOW){
//       stepperX.run();
//     }
//     if(digitalRead(YTopStop)==LOW){
//       stepperY.run();
//     }
//   }

//   Serial.println("Stepper X and Y is locating Max Distance");
//   stepperX.setMaxSpeed(500.0);
//   stepperX.setAcceleration(200.0);
//   stepperY.setMaxSpeed(500.0);
//   stepperY.setAcceleration(200.0);
//   while ((digitalRead(XRightEndStop) + digitalRead(YTopStop))<2){
//     if(digitalRead(YTopStop)==LOW){
//       stepperY.moveTo(++yLInitial_max);
//       stepperY.run();
//     }
//     if(digitalRead(XRightEndStop)==LOW){
//       stepperX.moveTo(++xInitial_max);
//       stepperX.run();
//     }
//   }

//   Serial.print("Stepper X and Y Max Distance located!");
//   stepperX.setMaxSpeed(50.0);
//   stepperX.setAcceleration(50.0);
//   stepperY.setMaxSpeed(50.0);
//   stepperY.setAcceleration(50.0);

//   xInitial_max = stepperX.currentPosition();
//   yLInitial_max = stepperY.currentPosition();

//   while (digitalRead(XRightEndStop) + digitalRead(YTopStop)>0){
//     if(digitalRead(YTopStop)==HIGH){
//       stepperY.moveTo(--yLInitial_max);
//       stepperY.run();
//     }

//     if(digitalRead(XRightEndStop)==HIGH){
//       stepperX.moveTo(--xInitial_max);
//       stepperX.run();
//     }
//   }

//   xMax = stepperX.currentPosition();
//   yMax = stepperY.currentPosition();
//   Serial.print("Stepper X axis max location is : ");
//   Serial.print(xMax);
//   Serial.print("Stepper YLeft max: ");
//   Serial.print(yMax);
//   Serial.print(" |  Stepper Y max: ");
//   Serial.print(yMax);
//   delay(100);
// }

// //the set value is based on the position of the cabinet which has to be adjusted according to the actual position

// void goTo(int x, int y){
//   Serial.print("Done Going to  X: ");
//   Serial.print(x);
//   Serial.print(" , ");
//   Serial.println(y);
//   stepperX.moveTo(x);
//   stepperY.moveTo(y);
//   while(stepperX.distanceToGo()!=0 || stepperY.distanceToGo()!=0){
//     if(digitalRead(YBottomStop)==LOW && digitalRead(YTopStop)==LOW){
//       stepperY.run();
//     }
//     if(digitalRead(XLeftEndStop)==LOW && digitalRead(XRightEndStop)==LOW){
//       stepperX.run();
//     }
//   }
//   Serial.print("Done Going to  X: ");
//   Serial.print(stepperX.currentPosition());
//   Serial.print(" , Y: ");
//   Serial.println(stepperY.currentPosition());
//   delayMicroseconds(1000);
// }

// void cabinetNumber(int n){
//   if(n==1){
//   goTo(2000, 1800); 
//   }else if(n==2){
//  goTo(7500, 1800); 
//   }else if(n==3){
//    goTo(13000, 1800); 
//   }else if(n==4){
//   goTo(2000, 9400); 
//   }else if(n==5){
//   goTo(7500, 9400); 
//   }else if(n==6){
//   goTo(13000, 9400); 
//   }else if(n==7){
//   goTo(2000, 17100); 
//   }else if(n==8){
//   goTo(7400, 17100);
//   }else if(n==9){
//   goTo(13000, 17100); 
//   }
// }

// // Serial command handler function
// void handleSerialCommand(String command) {
//   command.trim();
//   String originalCommand = command;
//   command.toLowerCase();
  
//   Serial.println("========================================");
//   Serial.printf("Command received: '%s'\n", command.c_str());
//   Serial.println("========================================");
  
//   if (command == "help" || command == "h") {
//     printHelp();
//   }
//   else if (command == "status" || command == "s") {
//     printSystemStatus();
//   }
//   else if (command == "users" || command == "u") {
//     printUsers();
//   }
//   else if (command == "sessions" || command == "sess") {
//     printActiveSessions();
//   }
//   else if (command == "prescriptions" || command == "rx" || command == "p") {
//     printPrescriptions();
//   }
//   else if (command == "patients" || command == "pat") {
//     printPatients();
//   }
//   else if (command == "notifications" || command == "notif" || command == "n") {
//     printNotifications();
//   }
//   else if (command == "medications" || command == "meds" || command == "m") {
//     printMedications();
//   }
//   else if (command == "storage" || command == "stor") {
//     printStorageInfo();
//   }
//   else if (command == "memory" || command == "mem") {
//     printMemoryInfo();
//   }
//   else if (command == "wifi") {
//     printWiFiInfo();
//   }
//   else if (command.startsWith("rx ")) {
//     // Show specific prescription details
//     String rxId = originalCommand.substring(3);
//     rxId.trim();
//     printPrescriptionDetails(rxId);
//   }
//   else if (command.startsWith("user ")) {
//     // Show specific user details
//     String username = originalCommand.substring(5);
//     username.trim();
//     printUserDetails(username);
//   }
//   else if (command.startsWith("session ")) {
//     // Show specific session details
//     String token = originalCommand.substring(8);
//     token.trim();
//     printSessionDetails(token);
//   }
//   else if (command.startsWith("notif ")) {
//     // Show notifications for specific user
//     String username = originalCommand.substring(6);
//     username.trim();
//     printNotificationsForUser(username);
//   }
//   else if (command == "clear" || command == "cls") {
//     // Clear screen (send multiple newlines)
//     for(int i = 0; i < 50; i++) {
//       Serial.println();
//     }
//     Serial.println("Screen cleared.");
//   }
//   else if (command == "reset") {
//     Serial.println("Restarting ESP32 in 3 seconds...");
//     delay(3000);
//     ESP.restart();
//   }
//   else if (command == "cleanup") {
//     cleanupExpiredSessions();
//     Serial.println("Session cleanup completed.");
//   }
//   else if (command == "all" || command == "dump") {
//     printAllData();
//   } 
//   else if (command == "pushstepper") {
//     PushStepper();
//   }
//   else if (command == "home" || command == "homing") {
//     homing();
//   }
//   else if (command == "movemax" || command == "max" || command == "move to max") {
//     moveToMax();
//   } else if (command.startsWith("cabinetnumber ") || command.startsWith("cbn ")) {
//     int cabinetNum = command.substring(command.indexOf(' ') + 1).toInt();
//     Serial.printf("Cabinet number set to: %d\n", cabinetNum);
//     cabinetNumber(cabinetNum);
//   } else if (command == "goto" || command == "go to") {
//     Serial.println("Enter row (0-4): ");
//     int r = Serial.readStringUntil('\n').toInt();
//     Serial.println("Enter column (0-4): ");
//     int c = Serial.readStringUntil('\n').toInt();
//     Serial.printf("Moving to Row: %d, Column: %d\n", r, c);
//     goTo(r, c);
//   } else {
//     Serial.println("Unknown command. Type 'help' for available commands.");
//   }
  
//   Serial.println("========================================");
//   Serial.println();
// }

// void dispenseMedications(int medications[3]) {
//     Serial.println("Starting medication dispensing...");
//     for (int i = 0; i < 3; i++) {
//         Serial.println("Starting medication dispensing for medication index: " + String(i));

//         Serial.printf("Preparing to dispense medication index %d from cabinet %d\n", i, medications[i]);
//         cabinetNumber(medications[i]);
//         Serial.printf("Dispensing medication from cabinet %d\n", medications[i]);
//         Serial.printf("Dispensing medication index %d from cabinet %d\n", i, medications[i]);
//         PushStepper();
//         Serial.printf("Dispensed pill medication index %d from cabinet %d\n", i, medications[i]);
//         delay(500); // wait for half a second before dispensing the next pill
//         Serial.println("Medication dispensed.");
//     }
// }


// void setup() {
//   Serial.begin(115200);
//   delay(1000);
//   Serial.println("ESP32 Started");
//   Serial.println("ESP32 Initializing motors");
//   pinMode(XRightEndStop, INPUT); // set left stopper of the X axis as input
//   pinMode(XLeftEndStop, INPUT); // set right stopper of the X axis as input
//   pinMode(YTopStop, INPUT); // set top stopper of the  Y axis as input
//   pinMode(YBottomStop, INPUT); // set bottom stopper of the  Y axis as input
//   pinMode(ZStop, INPUT); // set bottom stopper of the  Z axis as input

//   pinMode(stepperXStep_pin, OUTPUT); // set the stepper of the X axis as output
//   pinMode(stepperYStep_pin, OUTPUT); // set the stepper of the Y axis as output
//   pinMode(stepperZStep_pin, OUTPUT); // set the stepper of the Z axis as output
//   pinMode(stepperXDir_pin, OUTPUT); // set the stepper of the X axis' direction as output
//   pinMode(stepperYDir_pin, OUTPUT); // set the stepper of the Y axis' direction as output
//   pinMode(stepperZDir_pin, OUTPUT); // set the stepper of the Z axis' direction as output

 

//   // Initialize random seed
//   randomSeed(analogRead(0));

//   // Initialize storage (SD Card with SPIFFS fallback)
//   storageInitialized = initStorage();
//   if(!storageInitialized) {
//     Serial.println("Failed to initialize any storage. Web server will not serve files.");
//   }
// // RFID reader initialization
// //   SPI.begin(HSPI_SCK, HSPI_MISO, HSPI_MOSI, SS_PIN); // Start SPI bus
// //   hspi.begin(HSPI_SCK, HSPI_MISO, HSPI_MOSI, SS_PIN);
// //   mfrc522.PCD_Init();

//   // Configure AP with static IP
//   WiFi.mode(WIFI_AP);
//   WiFi.softAPConfig(apIP, apIP, netMsk);
//   WiFi.softAP(AP_ssid, NULL);
//   Serial.println("WiFi AP Started");
//   WiFi.setHostname("PharmaAssist");
//   Serial.print("AP IP: ");
//   Serial.println(WiFi.softAPIP());

//   // DNS server (for captive portal)
//   dnsServer.start(DNS_port, "*", apIP);
//   Serial.println("DNS Server started for captive portal");

//   // mDNS
//   if (!MDNS.begin("PharmaAssist")) {
//     Serial.println("Error starting mDNS");
//   } else {
//     Serial.println("mDNS Started: http://PharmaAssist.local");
//     MDNS.addService("http", "tcp", 80);
//   }

//   // Enable CORS globally
//   DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
//   DefaultHeaders::Instance().addHeader("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
//   DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers", "Content-Type, Authorization");

//   // Handle OPTIONS requests for CORS
//   webServer.onNotFound([](AsyncWebServerRequest *request) {
//     Serial.printf("[LOG] onNotFound: %s %s\n", (request->method() == HTTP_OPTIONS ? "OPTIONS" : "NOTFOUND"), request->url().c_str());
//     if (request->method() == HTTP_OPTIONS) {
//       request->send(200);
//       Serial.println("[LOG] Sent CORS preflight response.");
//       return;
//     }
//     Serial.print("Request for: ");
//     Serial.println(request->url());
    
//     // For captive portal, redirect to login page
//     if (!request->url().startsWith("/api/")) {
//       String redirectURL = "http://PharmaAssist.com/login.html";
//       request->redirect(redirectURL);
//     } else {
//       request->send(404, "text/plain", "Not Found");
//     }
//   });

//   // Root route - redirect to login or main page based on session
//   webServer.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
//     Serial.printf("[LOG] GET /\n");
//     if(!storageInitialized) {
//       request->send(500, "text/plain", String("Storage (") + storageType + ") not initialized");
//       return;
//     }
    
//     String token = getSessionToken(request);
//     if (validateSession(token)) {
//       Serial.println("[LOG] Valid session, redirecting to /index.html");
//       String redirectURL = "http://" + WiFi.softAPIP().toString() + "/index.html";
//       request->redirect(redirectURL);
//     } else {
//       Serial.println("[LOG] No valid session, redirecting to /login.html");
//       String redirectURL = "http://" + WiFi.softAPIP().toString() + "/login.html";
//       request->redirect(redirectURL);
//     }
//   });

//   // Login page (public)
//   webServer.on("/login.html", HTTP_GET, [](AsyncWebServerRequest *request) {
//     Serial.println("[LOG] GET /login.html");
//     if(!storageInitialized) {
//       request->send(500, "text/plain", String("Storage (") + storageType + ") not initialized");
//       return;
//     }
//     serveFile(request, "/login.html", "text/html");
//   });

//   // CSS file (public)
//   webServer.on("/login-styles.css", HTTP_GET, [](AsyncWebServerRequest *request) {
//     Serial.println("[LOG] GET /login-styles.css");
//     if(!storageInitialized) {
//       request->send(500, "text/plain", String("Storage (") + storageType + ") not initialized");
//       return;
//     }
//     serveFile(request, "/login-styles.css", "text/css");
//   });

//   // JavaScript file (public)
//   webServer.on("/login-script.js", HTTP_GET, [](AsyncWebServerRequest *request) {
//     Serial.println("[LOG] GET /login-script.js");
//     if(!storageInitialized) {
//       request->send(500, "text/plain", String("Storage (") + storageType + ") not initialized");
//       return;
//     }
//     serveFile(request, "/login-script.js", "application/javascript");
//   });

//   // Protected routes - require valid session
//   webServer.on("/index.html", HTTP_GET, [](AsyncWebServerRequest *request) {
//     Serial.println("[LOG] GET /index.html");
//     if(!storageInitialized) {
//       request->send(500, "text/plain", String("Storage (") + storageType + ") not initialized");
//       return;
//     }
    
//     String token = getSessionToken(request);
//     if (!validateSession(token)) {
//       Serial.println("[LOG] Unauthorized access to /index.html, redirecting to /login.html");
//       String redirectURL = "http://" + WiFi.softAPIP().toString() + "/login.html";
//       request->redirect(redirectURL);
//       return;
//     }
    
//     Serial.println("[LOG] Authorized access to /index.html, serving file.");
//     serveFile(request, "/index.html", "text/html");
//   });

//   webServer.on("/styles.css", HTTP_GET, [](AsyncWebServerRequest *request) {
//     Serial.println("[LOG] GET /styles.css");
//     if(!storageInitialized) {
//       request->send(500, "text/plain", String("Storage (") + storageType + ") not initialized");
//       return;
//     }
    
//     String token = getSessionToken(request);
//     if (!validateSession(token)) {
//       Serial.println("[LOG] Unauthorized access to /styles.css");
//       request->send(401, "text/plain", "Unauthorized");
//       return;
//     }
    
//     Serial.println("[LOG] Authorized access to /styles.css, serving file.");
//     serveFile(request, "/styles.css", "text/css");
//   });

//   webServer.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request) {
//     Serial.println("[LOG] GET /script.js");
//     if(!storageInitialized) {
//       request->send(500, "text/plain", String("Storage (") + storageType + ") not initialized");
//       return;
//     }
    
//     String token = getSessionToken(request);
//     if (!validateSession(token)) {
//       Serial.println("[LOG] Unauthorized access to /script.js");
//       request->send(401, "text/plain", "Unauthorized");
//       return;
//     }
    
//     Serial.println("[LOG] Authorized access to /script.js, serving file.");
//     serveFile(request, "/script.js", "application/javascript");
//   });

//   // Authentication endpoint
//   webServer.on("/api/login", HTTP_POST, [](AsyncWebServerRequest *request) {
//     Serial.println("[LOG] POST /api/login (headers received)");
//     }, NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
//     Serial.println("[LOG] POST /api/login (body received)");
//     String body = String((char*)data).substring(0, len);
//     Serial.printf("[LOG] Received body: %s\n", body.c_str());
//     Serial.printf("[DEBUG] Body length: %u, Index: %u, Total: %u\n", (unsigned)len, (unsigned)index, (unsigned)total);
    
//     StaticJsonDocument<200> doc;
//     DeserializationError error = deserializeJson(doc, body);
    
//     if (error) {
//       Serial.println("[LOG] JSON parsing failed");
//       Serial.printf("[DEBUG] JSON error: %s\n", error.c_str());
//       request->send(400, "application/json", "{\"success\":false,\"message\":\"Invalid JSON\"}");
//       return;
//     }
    
//     String type = doc["type"].as<String>(); // e.g., "username" or "email"
//     String username = "";
//     String email = "";
//     String password = doc["password"].as<String>();

//     if (type != "username" && type != "email") {
//       Serial.println("[LOG] Invalid login type");
//       request->send(400, "application/json", "{\"success\":false,\"message\":\"Invalid login type\"}");
//       return;
//     } else {
//       Serial.printf("[LOG] Login type: %s\n", type.c_str());
//     }

//     User* user = nullptr;
//     if (type == "username") {
//       username = doc["username"].as<String>();
//       Serial.println("[LOG] Processing login by username");
//       Serial.printf("[DEBUG] Parsed username: %s\n", username.c_str());
//       Serial.printf("[DEBUG] Parsed password: %s\n", password.c_str());
//       Serial.printf("[LOG] Login attempt - User: %s, Pass: %s\n", username.c_str(), password.c_str());
//       user = authenticateUser(username, password);
//     } else {
//       email = doc["email"].as<String>();
//       Serial.println("[LOG] Processing login by email");
//       Serial.printf("[DEBUG] Parsed email: %s\n", email.c_str());
//       Serial.printf("[DEBUG] Parsed password: %s\n", password.c_str());
//       Serial.printf("[LOG] Login attempt - Email: %s, Pass: %s\n", email.c_str(), password.c_str());
//       // Find user by email
//       for (auto& u : users) {
//         if (u.email.equalsIgnoreCase(email) && u.password == password) {
//           user = &u;
//           username = u.username;
//           break;
//         }
//       }
//     }
//     if (user != nullptr) {
//       Serial.printf("[LOG] Authentication successful for %s\n", user->fullName.c_str());
//       Serial.printf("[DEBUG] User struct: username=%s, email=%s, fullName=%s\n", user->username.c_str(), user->email.c_str(), user->fullName.c_str());
//       // Create new session
//       String sessionToken = generateSessionToken();
//       Serial.printf("[LOG] Session generated: %s for user: %s\n", sessionToken.c_str(), user->username.c_str());
//       Session newSession;
//       newSession.token = sessionToken;
//       newSession.username = user->username;
//       newSession.fullName = user->fullName;
//       newSession.createdAt = millis();
//       newSession.lastAccessed = millis();
      
//       activeSessions[sessionToken] = newSession;
      
//       // Send success response with session info
//       StaticJsonDocument<300> response;
//       response["success"] = true;
//       response["message"] = "Authentication successful";
//       response["session_token"] = sessionToken;
//       response["user"]["name"] = user->fullName;
//       response["user"]["email"] = user->email;
//       response["user"]["license"] = user->license;
//       response["user"]["department"] = user->department;
      
//       String responseStr;
//       serializeJson(response, responseStr);
      
//       // Set session cookie
//       AsyncWebServerResponse* resp = request->beginResponse(200, "application/json", responseStr);
//       resp->addHeader("Set-Cookie", "session_token=" + sessionToken + "; Path=/; Max-Age=3600");
//       request->send(resp);
      
//       Serial.printf("[LOG] Sending authentication success response for %s\n", user->fullName.c_str());
//     } else {
//       Serial.println("[LOG] Authentication failed");
//       Serial.println("[DEBUG] No matching user found for given credentials.");
//       request->send(401, "application/json", "{\"success\":false,\"message\":\"Invalid credentials\"}");
//     }
//   });

//   // Session validation endpoint
//   webServer.on("/api/validate-session", HTTP_GET, [](AsyncWebServerRequest *request) {
//     Serial.println("[LOG] GET /api/validate-session");
//     String token = getSessionToken(request);
    
//     if (validateSession(token)) {
//       Serial.println("[LOG] Session valid");
//       auto it = activeSessions.find(token);
//       if (it != activeSessions.end()) {
//         StaticJsonDocument<200> response;
//         response["valid"] = true;
//         response["username"] = it->second.username;
//         response["fullName"] = it->second.fullName;
        
//         String responseStr;
//         serializeJson(response, responseStr);
//         request->send(200, "application/json", responseStr);
//       } else {
//         request->send(200, "application/json", "{\"valid\":false}");
//       }
//     } else {
//       Serial.println("[LOG] Session invalid");
//       request->send(200, "application/json", "{\"valid\":false}");
//     }
//   });

//   // Logout endpoint
//   webServer.on("/api/logout", HTTP_POST, [](AsyncWebServerRequest *request) {
//     Serial.println("[LOG] POST /api/logout");
//     String token = getSessionToken(request);
    
//     if (!token.isEmpty()) {
//       auto it = activeSessions.find(token);
//       if (it != activeSessions.end()) {
//         Serial.printf("[LOG] Logging out user: %s\n", it->second.fullName.c_str());
//         activeSessions.erase(it);
//       }
//     }
    
//     Serial.println("[LOG] Sending logout response");
//     // Clear session cookie
//     AsyncWebServerResponse* resp = request->beginResponse(200, "application/json", "{\"success\":true,\"message\":\"Logged out successfully\"}");
//     resp->addHeader("Set-Cookie", "session_token=; Path=/; Max-Age=0");
//     request->send(resp);
//   });

//   // Session info endpoint (protected)
//   webServer.on("/api/session-info", HTTP_GET, [](AsyncWebServerRequest *request) {
//     Serial.println("[LOG] GET /api/session-info");
//     String token = getSessionToken(request);
    
//     if (!validateSession(token)) {
//       Serial.println("[LOG] Unauthorized session-info request");
//       request->send(401, "application/json", "{\"error\":\"Unauthorized\"}");
//       return;
//     }
    
//     auto it = activeSessions.find(token);
//     if (it != activeSessions.end()) {
//       Serial.printf("[LOG] Session info for user: %s\n", it->second.fullName.c_str());
//       StaticJsonDocument<300> response;
//       response["username"] = it->second.username;
//       response["fullName"] = it->second.fullName;
//       response["sessionAge"] = (millis() - it->second.createdAt) / 1000; // in seconds
//       response["activeSessions"] = activeSessions.size();
      
//       String responseStr;
//       serializeJson(response, responseStr);
//       request->send(200, "application/json", responseStr);
//     } else {
//       Serial.println("[LOG] Session not found");
//       request->send(401, "application/json", "{\"error\":\"Session not found\"}");
//     }
//   });

//   // Handle favicon requests
//   webServer.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request) {
//     Serial.println("[LOG] GET /favicon.ico");
//     if(!storageInitialized) {
//       request->send(404, "text/plain", "Storage not available");
//       return;
//     }
//     serveFile(request, "/favicon.ico", "image/x-icon");
//   });

//   // Prescription submission endpoint
//   webServer.on("/api/prescription", HTTP_POST, [](AsyncWebServerRequest *request) {
//     Serial.println("[LOG] POST /api/prescription (headers received)");
//     String token = getSessionToken(request);
//     if (!validateSession(token)) {
//       request->send(401, "application/json", "{\"success\":false,\"message\":\"Unauthorized\"}");
//       return;
//     }
// }, NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
//   static String bodyBuffer;
//   if (index == 0) bodyBuffer = "";
//   bodyBuffer += String((char*)data).substring(0, len);
//   if (index + len == total) {
//     Serial.println("[LOG] POST /api/prescription (body received, final chunk)");
//     Serial.printf("[LOG] Received prescription body: %s\n", bodyBuffer.c_str());

//     String token = getSessionToken(request);
//     String currentUsername = getCurrentUsername(token);

//     StaticJsonDocument<2048> doc;
//     DeserializationError error = deserializeJson(doc, bodyBuffer);
//     if (error) {
//       Serial.println("[LOG] JSON parsing failed for prescription");
//       request->send(400, "application/json", "{\"success\":false,\"message\":\"Invalid JSON\"}");
//       return;
//     }

//     Prescription rx;
//     rx.id = doc["id"] | "";
//     rx.patientName = doc["patientName"] | "";
//     rx.patientMRN = doc["patientMRN"] | "";
//     rx.ward = doc["ward"] | "";
//     rx.bedNumber = doc["bedNumber"] | "";
//     rx.status = doc["status"] | "pending";
//     rx.date = doc["date"] | "";
//     rx.prescribingPhysician = doc["prescribingPhysician"] | "";
//     rx.prescribingUsername = currentUsername;

//     // Parse medications array (frontend format)
//     if (doc["medications"].is<JsonArray>()) {
//       for (JsonObject med : doc["medications"].as<JsonArray>()) {
//         Medication m;
//         m.medicationName = med["medicationName"] | "";
//         m.strength = med["strength"] | "";
//         m.dosageForm = med["dosageForm"] | "";
//         m.frequency = med["frequency"] | "";
//         rx.medications.push_back(m);
//       }
//     }

//     prescriptions.push_back(rx);
//     Serial.printf("[LOG] Prescription saved by %s. Total prescriptions: %d\n", currentUsername.c_str(), (int)prescriptions.size());

//     // Dispense logic (if needed)
//     int medications[rx.medications.size()];
//     for (size_t i = 0; i < rx.medications.size(); i++) {
//       medications[i] = getMedicationIndex(rx.medications[i].medicationName);
//       Serial.printf("Medication %d: %s\n", (int)i+1, rx.medications[i].medicationName.c_str());
//     }
//     dispenseMedications(medications);

//     request->send(200, "application/json", "{\"success\":true,\"message\":\"Prescription received and saved.\"}");
//   }
// });

//   // Logging endpoint: POST /api/log
//   webServer.on("/api/log", HTTP_POST, [](AsyncWebServerRequest *request) {
//     // No headers to process
//   }, NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
//     Serial.println("[LOG] POST /api/log (body received)");
//     String body = String((char*)data).substring(0, len);
//     StaticJsonDocument<512> doc;
//     DeserializationError error = deserializeJson(doc, body);
//     if (error) {
//       Serial.println("[LOG] /api/log: Invalid JSON");
//       request->send(400, "application/json", "{\"success\":false,\"message\":\"Invalid JSON\"}");
//       return;
//     }
//     String context = doc["context"] | "unknown";
//     String details = doc["details"].isNull() ? "" : doc["details"].as<String>();
//     Serial.printf("[CLIENT LOG] %s : %s\n", context.c_str(), details.c_str());
//     request->send(200, "application/json", "{\"success\":true}");
//   });

//   // --- API: Register ---
//   webServer.on("/api/register", HTTP_POST, [](AsyncWebServerRequest *request) {
//     // No headers to process
//   }, NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
//     String body = String((char*)data).substring(0, len);
//     StaticJsonDocument<256> doc;
//     DeserializationError error = deserializeJson(doc, body);
//     if (error) {
//       request->send(400, "application/json", "{\"success\":false,\"message\":\"Invalid JSON\"}");
//       return;
//     }
//     String email = doc["email"] | "";
//     String password = doc["password"] | "";
//     if (email.isEmpty() || password.isEmpty()) {
//       request->send(400, "application/json", "{\"success\":false,\"message\":\"Missing email or password\"}");
//       return;
//     }
//     // Extract username from email
//     String username = email;
//     int atIndex = username.indexOf('@');
//     if (atIndex > 0) username = username.substring(0, atIndex);

//     // Check if user exists
//     for (const auto& user : users) {
//       if (user.username == username) {
//         request->send(409, "application/json", "{\"success\":false,\"message\":\"User already exists\"}");
//         return;
//       }
//     }
//     // Create new user with empty data (newly registered accounts should be empty)
//     User newUser;
//     newUser.id = users.size() + 1;
//     newUser.username = username;
//     newUser.password = password;
//     newUser.fullName = username;
//     newUser.email = email;
//     newUser.license = "MD-NEW";
//     newUser.department = "General Practice";
//     users.push_back(newUser);
    
//     Serial.printf("[LOG] New user registered: %s (%s)\n", username.c_str(), email.c_str());

//     // Create session
//     String sessionToken = generateSessionToken();
//     Session newSession;
//     newSession.token = sessionToken;
//     newSession.username = newUser.username;
//     newSession.fullName = newUser.fullName;
//     newSession.createdAt = millis();
//     newSession.lastAccessed = millis();
//     activeSessions[sessionToken] = newSession;

//     StaticJsonDocument<256> response;
//     response["success"] = true;
//     response["message"] = "Registration successful";
//     response["session_token"] = sessionToken;
//     response["user"]["name"] = newUser.fullName;
//     response["user"]["email"] = newUser.email;
//     response["user"]["license"] = newUser.license;
//     response["user"]["department"] = newUser.department;
//     String responseStr;
//     serializeJson(response, responseStr);

//     AsyncWebServerResponse* resp = request->beginResponse(200, "application/json", responseStr);
//     resp->addHeader("Set-Cookie", "session_token=" + sessionToken + "; Path=/; Max-Age=3600");
//     request->send(resp);
//   });

//   // --- API: Prescriptions (filtered by current user) ---
//   webServer.on("/api/prescriptions", HTTP_GET, [](AsyncWebServerRequest *request) {
//     String token = getSessionToken(request);
//     if (!validateSession(token)) {
//       request->send(401, "application/json", "{\"error\":\"Unauthorized\"}");
//       return;
//     }
    
//     String currentUsername = getCurrentUsername(token);
    
//     StaticJsonDocument<4096> doc;
//     JsonArray arr = doc.createNestedArray("data");
    
//     // Only return prescriptions for the current user
//     for (const auto& rx : prescriptions) {
//       if (rx.prescribingUsername == currentUsername) {
//         JsonObject o = arr.createNestedObject();
//         o["id"] = rx.id;
//         o["patientName"] = rx.patientName;
//         o["patientMRN"] = rx.patientMRN;
//         o["ward"] = rx.ward;
//         o["bedNumber"] = rx.bedNumber;
//         o["status"] = rx.status;
//         o["date"] = rx.date;
//         o["prescribingPhysician"] = rx.prescribingPhysician;
//         // Only send first medication for summary
//         if (!rx.medications.empty()) {
//           o["medicationName"] = rx.medications[0].medicationName;
//           o["strength"] = rx.medications[0].strength;
//           o["dosageForm"] = rx.medications[0].dosageForm;
//           o["frequency"] = rx.medications[0].frequency;
//         }
//       }
//     }
//     doc["success"] = true;
//     String out;
//     serializeJson(doc, out);
//     request->send(200, "application/json", out);
//   });

//   // --- API: Notifications (filtered by current user) ---
//   webServer.on("/api/notifications", HTTP_GET, [](AsyncWebServerRequest *request) {
//     String token = getSessionToken(request);
//     if (!validateSession(token)) {
//       request->send(401, "application/json", "{\"error\":\"Unauthorized\"}");
//       return;
//     }
    
//     String currentUsername = getCurrentUsername(token);
    
//     StaticJsonDocument<4096> doc;
//     JsonArray arr = doc.createNestedArray("data");
    
//     // Only return notifications for the current user
//     for (const auto& n : notifications) {
//       if (n.assignedToUsername == currentUsername) {
//         JsonObject o = arr.createNestedObject();
//         o["id"] = n.id;
//         o["title"] = n.title;
//         o["content"] = n.content;
//         o["type"] = n.type;
//         o["time"] = n.time;
//         o["read"] = n.read;
//         o["actionRequired"] = n.actionRequired;
//         o["relatedOrderId"] = n.relatedOrderId;
//       }
//     }
//     doc["success"] = true;
//     String out;
//     serializeJson(doc, out);
//     request->send(200, "application/json", out);
//   });

//   // --- API: Mark notification as read ---
//   webServer.on("^\\/api\\/notifications\\/([\\w\\-]+)/read$", HTTP_POST, [](AsyncWebServerRequest *request) {
//     String token = getSessionToken(request);
//     if (!validateSession(token)) {
//       request->send(401, "application/json", "{\"error\":\"Unauthorized\"}");
//       return;
//     }
    
//     String currentUsername = getCurrentUsername(token);
//     String notifId = request->pathArg(0);
    
//     for (auto& n : notifications) {
//       if (n.id == notifId && n.assignedToUsername == currentUsername) {
//         n.read = true;
//         break;
//       }
//     }
//     request->send(200, "application/json", "{\"success\":true}");
//   });

//   // --- API: Mark all notifications as read ---
//   webServer.on("/api/notifications/mark-all-read", HTTP_POST, [](AsyncWebServerRequest *request) {
//     String token = getSessionToken(request);
//     if (!validateSession(token)) {
//       request->send(401, "application/json", "{\"error\":\"Unauthorized\"}");
//       return;
//     }
    
//     String currentUsername = getCurrentUsername(token);
    
//     for (auto& n : notifications) {
//       if (n.assignedToUsername == currentUsername) {
//         n.read = true;
//       }
//     }
//     request->send(200, "application/json", "{\"success\":true}");
//   });

//   // --- API: Patients ---
//   webServer.on("/api/patients", HTTP_GET, [](AsyncWebServerRequest *request) {
//     String token = getSessionToken(request);
//     if (!validateSession(token)) {
//       request->send(401, "application/json", "{\"error\":\"Unauthorized\"}");
//       return;
//     }
    
//     StaticJsonDocument<2048> doc;
//     JsonArray arr = doc.createNestedArray("data");
//     for (const auto& p : patients) {
//       JsonObject o = arr.createNestedObject();
//       o["name"] = p.name;
//       o["mrn"] = p.mrn;
//       o["ward"] = p.ward;
//       o["bed"] = p.bed;
//     }
//     doc["success"] = true;
//     String out;
//     serializeJson(doc, out);
//     request->send(200, "application/json", out);
//   });

//   // --- API: Prescription actions (collect/cancel) ---
//   webServer.on("^\\/api\\/prescriptions\\/([\\w\\-]+)/collect$", HTTP_POST, [](AsyncWebServerRequest *request) {
//     String token = getSessionToken(request);
//     if (!validateSession(token)) {
//       request->send(401, "application/json", "{\"error\":\"Unauthorized\"}");
//       return;
//     }
    
//     String currentUsername = getCurrentUsername(token);
//     String rxId = request->pathArg(0);
    
//     for (auto& rx : prescriptions) {
//       if (rx.id == rxId && rx.prescribingUsername == currentUsername) {
//         rx.status = "dispensed";
//         Serial.printf("[LOG] Prescription %s marked as collected by %s\n", rxId.c_str(), currentUsername.c_str());
//         break;
//       }
//     }
//     request->send(200, "application/json", "{\"success\":true}");
//   });
  
//   webServer.on("^\\/api\\/prescriptions\\/([\\w\\-]+)/cancel$", HTTP_POST, [](AsyncWebServerRequest *request) {
//     String token = getSessionToken(request);
//     if (!validateSession(token)) {
//       request->send(401, "application/json", "{\"error\":\"Unauthorized\"}");
//       return;
//     }
    
//     String currentUsername = getCurrentUsername(token);
//     String rxId = request->pathArg(0);
    
//     for (auto& rx : prescriptions) {
//       if (rx.id == rxId && rx.prescribingUsername == currentUsername) {
//         rx.status = "cancelled";
//         Serial.printf("[LOG] Prescription %s cancelled by %s\n", rxId.c_str(), currentUsername.c_str());
//         break;
//       }
//     }
//     request->send(200, "application/json", "{\"success\":true}");
//   });

//   webServer.begin();
//   Serial.println("Web Server started on port 80");
//   Serial.printf("Using %s for file storage\n", storageType.c_str());
//   Serial.print("Access the web interface at: http://");
//   Serial.println(WiFi.softAPIP());
//   Serial.println("\nSample user accounts:");
//   for (const auto& user : users) {
//     // Count prescriptions and notifications for each user
//     int prescriptionCount = 0;
//     int notificationCount = 0;
//     for (const auto& rx : prescriptions) {
//       if (rx.prescribingUsername == user.username) prescriptionCount++;
//     }
//     for (const auto& notif : notifications) {
//       if (notif.assignedToUsername == user.username) notificationCount++;
//     }
//     Serial.printf("  Username: %s, Password: %s, Name: %s\n", 
//                   user.username.c_str(), user.password.c_str(), user.fullName.c_str());
//     Serial.printf("    Prescriptions: %d, Notifications: %d\n", prescriptionCount, notificationCount);
//   }
//   Serial.println("\nAPI endpoints:");
//   Serial.println("  POST /api/login - Authentication");
//   Serial.println("  POST /api/register - User registration (new users start with empty data)");
//   Serial.println("  GET /api/validate-session - Session validation");
//   Serial.println("  POST /api/logout - Logout");
//   Serial.println("  GET /api/session-info - Session information (protected)");
//   Serial.println("  GET /api/prescriptions - User's prescriptions (protected, filtered)");
//   Serial.println("  GET /api/notifications - User's notifications (protected, filtered)");
//   Serial.println("  GET /api/patients - Patient database (protected)");
//   Serial.println("  POST /api/prescription - Submit prescription data (protected)");
//   Serial.println("  POST /api/log - Client logging");
//   Serial.println("  POST /api/notifications/{id}/read - Mark notification as read");
//   Serial.println("  POST /api/notifications/mark-all-read - Mark all notifications as read");
//   Serial.println("  POST /api/prescriptions/{id}/collect - Mark prescription as collected");
//   Serial.println("  POST /api/prescriptions/{id}/cancel - Cancel prescription");
  
//   Serial.println("\nData Structure Summary:");
//   Serial.printf("  Total Users: %d (3 sample + new registrations)\n", users.size());
//   Serial.printf("  Total Patients: %d\n", patients.size());
//   Serial.printf("  Total Prescriptions: %d\n", prescriptions.size());
//   Serial.printf("  Total Notifications: %d\n", notifications.size());
//   Serial.println("  - Each notification is linked to a specific doctor");
//   Serial.println("  - Each prescription is linked to its prescribing doctor");
//   Serial.println("  - New registered users start with empty prescriptions and notifications");
//   Serial.println("  - All API endpoints are now user-specific and protected");

//   printHelp();

//    Serial.println("Stepper X and Y axis is Homing..............."); //Indicates that the steppers are in the homing process through serial monitor
//   homing(); // call the function for homing
//   moveToMax(); // call the function for moving at the max steps
//   stepperX.setMaxSpeed(maxSpeed); // set the speed of the X axis stepper to maximum value
//   stepperX.setAcceleration(accel); // set the acceleration value of the X axis stepper 
//   stepperY.setMaxSpeed(maxSpeed); // set the speed of the Y axis stepper to maximum value
//   stepperY.setAcceleration(accel); // set the acceleration value of the Y axis stepper 
//   row = yMax/5; // to determine the initial value for each row
//   column = xMax/4; // to determine the initial value for each column
// }

// void loop() {
//   dnsServer.processNextRequest();
  
//   // Clean up expired sessions every 60 seconds
//   static unsigned long lastCleanup = 0;
//   if (millis() - lastCleanup > 60000) {
//     cleanupExpiredSessions();
//     lastCleanup = millis();
//   }

//   // Serial command parser
//   static String serialBuffer;
//   while (Serial.available()) {
//     char c = Serial.read();
//     if (c == '\n' || c == '\r') {
//       if (serialBuffer.length() > 0) {
//         handleSerialCommand(serialBuffer);
//         serialBuffer = "";
//       }
//     } else if (isPrintable(c)) {
//       serialBuffer += c;
//     }
//   }


//   delay(10);
// }