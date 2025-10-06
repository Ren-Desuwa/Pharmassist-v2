// #include "Database.h"

// // Global instance
// CSVDatabase DB;

// CSVDatabase::CSVDatabase() {
//   patientCount = 0;
//   userCount = 0;
//   notificationCount = 0;
//   prescriptionCount = 0;
// }

// bool CSVDatabase::begin(const String& patientsPath, const String& usersPath, 
//                         const String& notificationsPath, const String& prescriptionsPath) {
//   patientsFile = patientsPath;
//   usersFile = usersPath;
//   notificationsFile = notificationsPath;
//   prescriptionsFile = prescriptionsPath;
  
//   Serial.println("CSVDatabase: Initialized with file paths");
//   return true;
// }

// String CSVDatabase::trim(String str) {
//   str.trim();
//   return str;
// }

// String CSVDatabase::escapeCSV(String field) {
//   if (field.indexOf(',') >= 0 || field.indexOf('"') >= 0 || field.indexOf('\n') >= 0) {
//     field.replace("\"", "\"\"");
//     return "\"" + field + "\"";
//   }
//   return field;
// }

// std::vector<String> CSVDatabase::parseCSVLine(String line) {
//   std::vector<String> fields;
//   String field = "";
//   bool inQuotes = false;
  
//   for (int i = 0; i < line.length(); i++) {
//     char c = line[i];
    
//     if (c == '"') {
//       if (inQuotes && i + 1 < line.length() && line[i + 1] == '"') {
//         field += '"';
//         i++;
//       } else {
//         inQuotes = !inQuotes;
//       }
//     } else if (c == ',' && !inQuotes) {
//       fields.push_back(trim(field));
//       field = "";
//     } else {
//       field += c;
//     }
//   }
//   fields.push_back(trim(field));
  
//   return fields;
// }

// void CSVDatabase::parseMedicines(String medicineStr, MedicineEntry* medicines, int& count) {
//   count = 0;
//   if (medicineStr.length() == 0) return;
  
//   int start = 0;
//   int pos = 0;
  
//   while (pos <= medicineStr.length() && count < MAX_MEDICINES_PER_PRESCRIPTION) {
//     if (pos == medicineStr.length() || medicineStr[pos] == '|') {
//       String entry = medicineStr.substring(start, pos);
//       entry.trim();
      
//       if (entry.length() > 0) {
//         int colon1 = entry.indexOf(':');
//         int colon2 = entry.indexOf(':', colon1 + 1);
//         int colon3 = entry.indexOf(':', colon2 + 1);
        
//         if (colon1 > 0 && colon2 > colon1 && colon3 > colon2) {
//           medicines[count].name = entry.substring(0, colon1);
//           medicines[count].dosage = entry.substring(colon1 + 1, colon2);
//           medicines[count].form = entry.substring(colon2 + 1, colon3);
//           medicines[count].frequency = entry.substring(colon3 + 1);
//           medicines[count].name.trim();
//           medicines[count].dosage.trim();
//           medicines[count].form.trim();
//           medicines[count].frequency.trim();
//           count++;
//         }
//       }
      
//       start = pos + 1;
//     }
//     pos++;
//   }
// }

// String CSVDatabase::serializeMedicines(MedicineEntry* medicines, int count) {
//   String result = "";
//   for (int i = 0; i < count; i++) {
//     if (i > 0) result += " | ";
//     result += medicines[i].name + ":" + medicines[i].dosage + ":" + 
//               medicines[i].form + ":" + medicines[i].frequency;
//   }
//   return result;
// }

// bool CSVDatabase::loadAll() {
//   bool success = true;
//   success &= loadPatients();
//   success &= loadUsers();
//   success &= loadNotifications();
//   success &= loadPrescriptions();
//   return success;
// }

// bool CSVDatabase::saveAll() {
//   bool success = true;
//   success &= savePatients();
//   success &= saveUsers();
//   success &= saveNotifications();
//   success &= savePrescriptions();
//   return success;
// }

// // ============ PATIENT OPERATIONS ============

// bool CSVDatabase::loadPatients() {
//   patientCount = 0;
  
//   if (!Storage.exists(patientsFile)) {
//     Serial.println("CSVDatabase: Patients file not found");
//     return false;
//   }
  
//   String content = Storage.readFile(patientsFile);
//   if (content.length() == 0) {
//     Serial.println("CSVDatabase: Patients file is empty");
//     return false;
//   }
  
//   int lineStart = 0;
//   int lineEnd = 0;
//   bool firstLine = true;
  
//   while (lineEnd < content.length()) {
//     lineEnd = content.indexOf('\n', lineStart);
//     if (lineEnd == -1) lineEnd = content.length();
    
//     String line = content.substring(lineStart, lineEnd);
//     line.trim();
    
//     if (line.length() > 0 && !firstLine && patientCount < MAX_PATIENTS) {
//       std::vector<String> fields = parseCSVLine(line);
//       if (fields.size() >= 5) {
//         patients[patientCount].PatientID = fields[0].toInt();
//         patients[patientCount].Name = fields[1];
//         patients[patientCount].MRN = fields[2];
//         patients[patientCount].Department = fields[3];
//         patients[patientCount].Location = fields[4];
//         patientCount++;
//       }
//     }
    
//     firstLine = false;
//     lineStart = lineEnd + 1;
//   }
  
//   Serial.printf("CSVDatabase: Loaded %d patients\n", patientCount);
//   return true;
// }

// bool CSVDatabase::savePatients() {
//   String content = "PatientID,Name,MRN,Department,Location\n";
  
//   for (int i = 0; i < patientCount; i++) {
//     content += String(patients[i].PatientID) + ",";
//     content += escapeCSV(patients[i].Name) + ",";
//     content += escapeCSV(patients[i].MRN) + ",";
//     content += escapeCSV(patients[i].Department) + ",";
//     content += escapeCSV(patients[i].Location) + "\n";
//   }
  
//   bool success = Storage.writeFile(patientsFile, content);
//   if (success) {
//     Serial.printf("CSVDatabase: Saved %d patients\n", patientCount);
//   }
//   return success;
// }

// Patient* CSVDatabase::getPatient(int id) {
//   for (int i = 0; i < patientCount; i++) {
//     if (patients[i].PatientID == id) {
//       return &patients[i];
//     }
//   }
//   return nullptr;
// }

// Patient* CSVDatabase::getPatientByMRN(const String& mrn) {
//   for (int i = 0; i < patientCount; i++) {
//     if (patients[i].MRN == mrn) {
//       return &patients[i];
//     }
//   }
//   return nullptr;
// }

// int CSVDatabase::addPatient(const Patient& patient) {
//   if (patientCount >= MAX_PATIENTS) {
//     Serial.println("CSVDatabase: Patient limit reached");
//     return -1;
//   }
  
//   // Find max ID
//   int maxId = 0;
//   for (int i = 0; i < patientCount; i++) {
//     if (patients[i].PatientID > maxId) maxId = patients[i].PatientID;
//   }
  
//   patients[patientCount] = patient;
//   patients[patientCount].PatientID = maxId + 1;
//   patientCount++;
  
//   savePatients();
//   return patients[patientCount - 1].PatientID;
// }

// bool CSVDatabase::updatePatient(int id, const Patient& patient) {
//   for (int i = 0; i < patientCount; i++) {
//     if (patients[i].PatientID == id) {
//       patients[i] = patient;
//       patients[i].PatientID = id; // Preserve ID
//       savePatients();
//       return true;
//     }
//   }
//   return false;
// }

// bool CSVDatabase::deletePatient(int id) {
//   for (int i = 0; i < patientCount; i++) {
//     if (patients[i].PatientID == id) {
//       // Shift remaining patients
//       for (int j = i; j < patientCount - 1; j++) {
//         patients[j] = patients[j + 1];
//       }
//       patientCount--;
//       savePatients();
//       return true;
//     }
//   }
//   return false;
// }

// // ============ USER OPERATIONS ============

// bool CSVDatabase::loadUsers() {
//   userCount = 0;
  
//   if (!Storage.exists(usersFile)) {
//     Serial.println("CSVDatabase: Users file not found");
//     return false;
//   }
  
//   String content = Storage.readFile(usersFile);
//   if (content.length() == 0) {
//     Serial.println("CSVDatabase: Users file is empty");
//     return false;
//   }
  
//   int lineStart = 0;
//   int lineEnd = 0;
//   bool firstLine = true;
  
//   while (lineEnd < content.length()) {
//     lineEnd = content.indexOf('\n', lineStart);
//     if (lineEnd == -1) lineEnd = content.length();
    
//     String line = content.substring(lineStart, lineEnd);
//     line.trim();
    
//     if (line.length() > 0 && !firstLine && userCount < MAX_USERS) {
//       std::vector<String> fields = parseCSVLine(line);
//       if (fields.size() >= 7) {
//         users[userCount].UserID = fields[0].toInt();
//         users[userCount].Username = fields[1];
//         users[userCount].Password = fields[2];
//         users[userCount].FullName = fields[3];
//         users[userCount].Email = fields[4];
//         users[userCount].LicenseNumber = fields[5];
//         users[userCount].Specialty = fields[6];
//         userCount++;
//       }
//     }
    
//     firstLine = false;
//     lineStart = lineEnd + 1;
//   }
  
//   Serial.printf("CSVDatabase: Loaded %d users\n", userCount);
//   return true;
// }

// bool CSVDatabase::saveUsers() {
//   String content = "UserID,Username,Password,FullName,Email,LicenseNumber,Specialty\n";
  
//   for (int i = 0; i < userCount; i++) {
//     content += String(users[i].UserID) + ",";
//     content += escapeCSV(users[i].Username) + ",";
//     content += escapeCSV(users[i].Password) + ",";
//     content += escapeCSV(users[i].FullName) + ",";
//     content += escapeCSV(users[i].Email) + ",";
//     content += escapeCSV(users[i].LicenseNumber) + ",";
//     content += escapeCSV(users[i].Specialty) + "\n";
//   }
  
//   bool success = Storage.writeFile(usersFile, content);
//   if (success) {
//     Serial.printf("CSVDatabase: Saved %d users\n", userCount);
//   }
//   return success;
// }

// User* CSVDatabase::getUser(int id) {
//   for (int i = 0; i < userCount; i++) {
//     if (users[i].UserID == id) {
//       return &users[i];
//     }
//   }
//   return nullptr;
// }

// User* CSVDatabase::getUserByUsername(const String& username) {
//   for (int i = 0; i < userCount; i++) {
//     if (users[i].Username == username) {
//       return &users[i];
//     }
//   }
//   return nullptr;
// }

// int CSVDatabase::addUser(const User& user) {
//   if (userCount >= MAX_USERS) {
//     Serial.println("CSVDatabase: User limit reached");
//     return -1;
//   }
  
//   int maxId = 0;
//   for (int i = 0; i < userCount; i++) {
//     if (users[i].UserID > maxId) maxId = users[i].UserID;
//   }
  
//   users[userCount] = user;
//   users[userCount].UserID = maxId + 1;
//   userCount++;
  
//   saveUsers();
//   return users[userCount - 1].UserID;
// }

// bool CSVDatabase::updateUser(int id, const User& user) {
//   for (int i = 0; i < userCount; i++) {
//     if (users[i].UserID == id) {
//       users[i] = user;
//       users[i].UserID = id;
//       saveUsers();
//       return true;
//     }
//   }
//   return false;
// }

// bool CSVDatabase::deleteUser(int id) {
//   for (int i = 0; i < userCount; i++) {
//     if (users[i].UserID == id) {
//       for (int j = i; j < userCount - 1; j++) {
//         users[j] = users[j + 1];
//       }
//       userCount--;
//       saveUsers();
//       return true;
//     }
//   }
//   return false;
// }

// // ============ NOTIFICATION OPERATIONS ============

// bool CSVDatabase::loadNotifications() {
//   notificationCount = 0;
  
//   if (!Storage.exists(notificationsFile)) {
//     Serial.println("CSVDatabase: Notifications file not found");
//     return false;
//   }
  
//   String content = Storage.readFile(notificationsFile);
//   if (content.length() == 0) {
//     Serial.println("CSVDatabase: Notifications file is empty");
//     return false;
//   }
  
//   int lineStart = 0;
//   int lineEnd = 0;
//   bool firstLine = true;
  
//   while (lineEnd < content.length()) {
//     lineEnd = content.indexOf('\n', lineStart);
//     if (lineEnd == -1) lineEnd = content.length();
    
//     String line = content.substring(lineStart, lineEnd);
//     line.trim();
    
//     if (line.length() > 0 && !firstLine && notificationCount < MAX_NOTIFICATIONS) {
//       std::vector<String> fields = parseCSVLine(line);
//       if (fields.size() >= 9) {
//         notifications[notificationCount].NotifID = fields[0].toInt();
//         notifications[notificationCount].Title = fields[1];
//         notifications[notificationCount].Message = fields[2];
//         notifications[notificationCount].Type = fields[3];
//         notifications[notificationCount].Timestamp = fields[4];
//         notifications[notificationCount].Read = (fields[5] == "True" || fields[5] == "true" || fields[5] == "1");
//         notifications[notificationCount].ActionRequired = (fields[6] == "True" || fields[6] == "true" || fields[6] == "1");
//         notifications[notificationCount].PrescriptionID = fields[7];
//         notifications[notificationCount].UserID = fields[8];
//         notificationCount++;
//       }
//     }
    
//     firstLine = false;
//     lineStart = lineEnd + 1;
//   }
  
//   Serial.printf("CSVDatabase: Loaded %d notifications\n", notificationCount);
//   return true;
// }

// bool CSVDatabase::saveNotifications() {
//   String content = "NotifID,Title,Message,Type,Timestamp,Read,ActionRequired,PrescriptionID,UserID\n";
  
//   for (int i = 0; i < notificationCount; i++) {
//     content += String(notifications[i].NotifID) + ",";
//     content += escapeCSV(notifications[i].Title) + ",";
//     content += escapeCSV(notifications[i].Message) + ",";
//     content += escapeCSV(notifications[i].Type) + ",";
//     content += escapeCSV(notifications[i].Timestamp) + ",";
//     content += (notifications[i].Read ? "True" : "False") + String(",");
//     content += (notifications[i].ActionRequired ? "True" : "False") + String(",");
//     content += escapeCSV(notifications[i].PrescriptionID) + ",";
//     content += escapeCSV(notifications[i].UserID) + "\n";
//   }
  
//   bool success = Storage.writeFile(notificationsFile, content);
//   if (success) {
//     Serial.printf("CSVDatabase: Saved %d notifications\n", notificationCount);
//   }
//   return success;
// }

// Notification* CSVDatabase::getNotification(int id) {
//   for (int i = 0; i < notificationCount; i++) {
//     if (notifications[i].NotifID == id) {
//       return &notifications[i];
//     }
//   }
//   return nullptr;
// }

// int CSVDatabase::addNotification(const Notification& notification) {
//   if (notificationCount >= MAX_NOTIFICATIONS) {
//     Serial.println("CSVDatabase: Notification limit reached");
//     return -1;
//   }
  
//   int maxId = 0;
//   for (int i = 0; i < notificationCount; i++) {
//     if (notifications[i].NotifID > maxId) maxId = notifications[i].NotifID;
//   }
  
//   notifications[notificationCount] = notification;
//   notifications[notificationCount].NotifID = maxId + 1;
//   notificationCount++;
  
//   saveNotifications();
//   return notifications[notificationCount - 1].NotifID;
// }

// bool CSVDatabase::updateNotification(int id, const Notification& notification) {
//   for (int i = 0; i < notificationCount; i++) {
//     if (notifications[i].NotifID == id) {
//       notifications[i] = notification;
//       notifications[i].NotifID = id;
//       saveNotifications();
//       return true;
//     }
//   }
//   return false;
// }

// bool CSVDatabase::markAsRead(int id) {
//   for (int i = 0; i < notificationCount; i++) {
//     if (notifications[i].NotifID == id) {
//       notifications[i].Read = true;
//       saveNotifications();
//       return true;
//     }
//   }
//   return false;
// }

// bool CSVDatabase::deleteNotification(int id) {
//   for (int i = 0; i < notificationCount; i++) {
//     if (notifications[i].NotifID == id) {
//       for (int j = i; j < notificationCount - 1; j++) {
//         notifications[j] = notifications[j + 1];
//       }
//       notificationCount--;
//       saveNotifications();
//       return true;
//     }
//   }
//   return false;
// }

// // ============ PRESCRIPTION OPERATIONS ============

// bool CSVDatabase::loadPrescriptions() {
//   prescriptionCount = 0;
  
//   if (!Storage.exists(prescriptionsFile)) {
//     Serial.println("CSVDatabase: Prescriptions file not found");
//     return false;
//   }
  
//   String content = Storage.readFile(prescriptionsFile);
//   if (content.length() == 0) {
//     Serial.println("CSVDatabase: Prescriptions file is empty");
//     return false;
//   }
  
//   int lineStart = 0;
//   int lineEnd = 0;
//   bool firstLine = true;
  
//   while (lineEnd < content.length()) {
//     lineEnd = content.indexOf('\n', lineStart);
//     if (lineEnd == -1) lineEnd = content.length();
    
//     String line = content.substring(lineStart, lineEnd);
//     line.trim();
    
//     if (line.length() > 0 && !firstLine && prescriptionCount < MAX_PRESCRIPTIONS) {
//       std::vector<String> fields = parseCSVLine(line);
//       if (fields.size() >= 11) {
//         prescriptions[prescriptionCount].PrescriptionID = fields[0].toInt();
//         prescriptions[prescriptionCount].PrescriptionCode = fields[1];
//         prescriptions[prescriptionCount].PatientName = fields[2];
//         prescriptions[prescriptionCount].MRN = fields[3];
//         prescriptions[prescriptionCount].Department = fields[4];
//         prescriptions[prescriptionCount].Location = fields[5];
//         parseMedicines(fields[6], prescriptions[prescriptionCount].Medicines, 
//                       prescriptions[prescriptionCount].MedicineCount);
//         prescriptions[prescriptionCount].Status = fields[7];
//         prescriptions[prescriptionCount].Date = fields[8];
//         prescriptions[prescriptionCount].DoctorName = fields[9];
//         prescriptions[prescriptionCount].UserID = fields[10];
//         prescriptionCount++;
//       }
//     }
    
//     firstLine = false;
//     lineStart = lineEnd + 1;
//   }
  
//   Serial.printf("CSVDatabase: Loaded %d prescriptions\n", prescriptionCount);
//   return true;
// }

// bool CSVDatabase::savePrescriptions() {
//   String content = "PrescriptionID,PrescriptionCode,PatientName,MRN,Department,Location,Medicines,Status,Date,DoctorName,UserID\n";
  
//   for (int i = 0; i < prescriptionCount; i++) {
//     content += String(prescriptions[i].PrescriptionID) + ",";
//     content += escapeCSV(prescriptions[i].PrescriptionCode) + ",";
//     content += escapeCSV(prescriptions[i].PatientName) + ",";
//     content += escapeCSV(prescriptions[i].MRN) + ",";
//     content += escapeCSV(prescriptions[i].Department) + ",";
//     content += escapeCSV(prescriptions[i].Location) + ",";
//     content += escapeCSV(serializeMedicines(prescriptions[i].Medicines, prescriptions[i].MedicineCount)) + ",";
//     content += escapeCSV(prescriptions[i].Status) + ",";
//     content += escapeCSV(prescriptions[i].Date) + ",";
//     content += escapeCSV(prescriptions[i].DoctorName) + ",";
//     content += escapeCSV(prescriptions[i].UserID) + "\n";
//   }
  
//   bool success = Storage.writeFile(prescriptionsFile, content);
//   if (success) {
//     Serial.printf("CSVDatabase: Saved %d prescriptions\n", prescriptionCount);
//   }
//   return success;
// }

// Prescription* CSVDatabase::getPrescription(int id) {
//   for (int i = 0; i < prescriptionCount; i++) {
//     if (prescriptions[i].PrescriptionID == id) {
//       return &prescriptions[i];
//     }
//   }
//   return nullptr;
// }

// Prescription* CSVDatabase::getPrescriptionByCode(const String& code) {
//   for (int i = 0; i < prescriptionCount; i++) {
//     if (prescriptions[i].PrescriptionCode == code) {
//       return &prescriptions[i];
//     }
//   }
//   return nullptr;
// }

// int CSVDatabase::addPrescription(const Prescription& prescription) {
//   if (prescriptionCount >= MAX_PRESCRIPTIONS) {
//     Serial.println("CSVDatabase: Prescription limit reached");
//     return -1;
//   }
  
//   int maxId = 0;
//   for (int i = 0; i < prescriptionCount; i++) {
//     if (prescriptions[i].PrescriptionID > maxId) maxId = prescriptions[i].PrescriptionID;
//   }
  
//   prescriptions[prescriptionCount] = prescription;
//   prescriptions[prescriptionCount].PrescriptionID = maxId + 1;
//   prescriptionCount++;
  
//   savePrescriptions();
//   return prescriptions[prescriptionCount - 1].PrescriptionID;
// }

// bool CSVDatabase::updatePrescription(int id, const Prescription& prescription) {
//   for (int i = 0; i < prescriptionCount; i++) {
//     if (prescriptions[i].PrescriptionID == id) {
//       prescriptions[i] = prescription;
//       prescriptions[i].PrescriptionID = id;
//       savePrescriptions();
//       return true;
//     }
//   }
//   return false;
// }

// bool CSVDatabase::deletePrescription(int id) {
//   for (int i = 0; i < prescriptionCount; i++) {
//     if (prescriptions[i].PrescriptionID == id) {
//       for (int j = i; j < prescriptionCount - 1; j++) {
//         prescriptions[j] = prescriptions[j + 1];
//       }
//       prescriptionCount--;
//       savePrescriptions();
//       return true;
//     }
//   }
//   return false;
// }

// // ============ UTILITY FUNCTIONS ============

// void CSVDatabase::clearAll() {
//   patientCount = 0;
//   userCount = 0;
//   notificationCount = 0;
//   prescriptionCount = 0;
//   Serial.println("CSVDatabase: All data cleared from memory");
// }

// void CSVDatabase::printStats() {
//   Serial.println("===== CSV Database Statistics =====");
//   Serial.printf("Patients: %d / %d\n", patientCount, MAX_PATIENTS);
//   Serial.printf("Users: %d / %d\n", userCount, MAX_USERS);
//   Serial.printf("Notifications: %d / %d\n", notificationCount, MAX_NOTIFICATIONS);
//   Serial.printf("Prescriptions: %d / %d\n", prescriptionCount, MAX_PRESCRIPTIONS);
//   Serial.println("===================================");
// }