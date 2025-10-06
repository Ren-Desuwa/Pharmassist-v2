#ifndef DATABASE_H
#define DATABASE_H

#include <Arduino.h>
#include <vector>
#include <map>

// ============================================================================
// ENUMS AND CONSTANTS
// ============================================================================

enum UserRole {
  ROLE_ADMIN = 0,
  ROLE_DOCTOR = 1,
  ROLE_NURSE = 2,
  ROLE_PATIENT = 3
};

enum PrescriptionStatus {
  STATUS_PENDING = 0,
  STATUS_DISPENSING = 1,
  STATUS_READY = 2,
  STATUS_PARTIALLY_DISPENSED = 3,
  STATUS_DISPENSED = 4,
  STATUS_CANCELLED = 5
};

// ============================================================================
// STRUCTS
// ============================================================================

struct User {
  int id;
  String username;
  String password;
  String fullName;
  String email;
  String license;
  String department;
  UserRole role;
  int patientId; // Links to Patient table if role is PATIENT, -1 otherwise

  User() : id(-1), role(ROLE_DOCTOR), patientId(-1) {}
  User(String uname, String pwd, String fname, String mail, String lic, String dept, UserRole r, int pId = -1) :
       id(-1), username(uname), password(pwd), fullName(fname), email(mail), license(lic), 
       department(dept), role(r), patientId(pId) {}
};

struct Patient {
  int id;
  String name;
  String mrn;
  String ward;
  String bed;
  String dateOfBirth;
  int userId; // Links to User table if patient has login, -1 otherwise
  
  Patient() : id(0), userId(-1) {}
  Patient(int i = 0, String n, String m, String w, String b, String dob, int uId = -1) :
          id(i), name(n), mrn(m), ward(w), bed(b), dateOfBirth(dob), userId(uId) {}
};

struct Medication {
  String medicationName;
  String strength;
  String dosageForm;
  String frequency;
  int quantity; // Number of doses/pills
};

struct Prescription {
  int id;
  String prescriptionCode; // e.g., "RX-2024-001"
  int patientId; // Foreign key to Patient
  int doctorId;  // Foreign key to User (doctor)
  int nurseId;   // Foreign key to User (nurse), -1 if not assigned
  std::vector<Medication> medications;
  PrescriptionStatus status;
  String dateCreated;
  String dateDispensed;
  String notes;
  
  Prescription() : id(0), patientId(-1), doctorId(-1), nurseId(-1), 
                   status(STATUS_PENDING) {}
};

struct Notification {
  int id;
  String notificationCode; // e.g., "NOTIF-001"
  String title;
  String content;
  String type; // "info", "warning", "success", "error"
  String dateTime;
  bool read;
  bool actionRequired;
  int relatedPrescriptionId; // Foreign key to Prescription, -1 if not related
  int assignedUserId; // Foreign key to User
  
  Notification() : id(0), read(false), actionRequired(false), 
                   relatedPrescriptionId(-1), assignedUserId(-1) {}
};

struct DispenseRequest {
  int prescriptionId;
  int patientId;
  int doctorId;
  int nurseId; // Who is dispensing
  String prescriptionCode;
  String patientName;
  String patientMRN;
  String doctorName;
  String nurseName;
  std::vector<int> medicationIds;    // Index 1-9
  std::vector<int> frequencies;      // 1=once, 2=bid, 3=tid
  std::vector<int> quantities;       // Number of doses
  String dateTime;
  
  String toString() const {
    String result = "DISPENSE:";
    result += prescriptionCode + "|";
    result += "PAT:" + patientMRN + "|";
    result += "DOC:" + doctorName + "|";
    result += "NURSE:" + nurseName + "|";
    result += "MEDS:";
    
    for (size_t i = 0; i < medicationIds.size(); i++) {
      if (i > 0) result += ",";
      result += "M" + String(medicationIds[i]) + ":" + 
                String(frequencies[i]) + "x" + String(quantities[i]);
    }
    return result;
  }
};

// ============================================================================
// DATABASE CLASS
// ============================================================================

class Database {
private:
  std::vector<User> users;
  std::vector<Patient> patients;
  std::vector<Prescription> prescriptions;
  std::vector<Notification> notifications;
  
  int nextUserId;
  int nextPatientId;
  int nextPrescriptionId;
  int nextNotificationId;

public:
  Database() : nextUserId(1), nextPatientId(1), 
               nextPrescriptionId(1), nextNotificationId(1) {}
  
  // ========================================================================
  // USER CRUD OPERATIONS
  // ========================================================================
  
  int addUser(const User& user) {
    User newUser = user;
    newUser.id = nextUserId++;
    users.push_back(newUser);
    return newUser.id;
  }
  
  bool updateUser(int id, const User& updatedUser) {
    for (auto& user : users) {
      if (user.id == id) {
        user = updatedUser;
        user.id = id; // Preserve ID
        return true;
      }
    }
    return false;
  }
  
  bool deleteUser(int id) {
    for (auto it = users.begin(); it != users.end(); ++it) {
      if (it->id == id) {
        users.erase(it);
        return true;
      }
    }
    return false;
  }
  
  User* getUserById(int id) {
    for (auto& user : users) {
      if (user.id == id) return &user;
    }
    return nullptr;
  }
  
  User* getUserByUsername(const String& username) {
    for (auto& user : users) {
      if (user.username.equalsIgnoreCase(username)) return &user;
    }
    return nullptr;
  }
  
  User* getUserByEmail(const String& email) {
    for (auto& user : users) {
      if (user.email.equalsIgnoreCase(email)) return &user;
    }
    return nullptr;
  }
  
  std::vector<User*> getUsersByRole(UserRole role) {
    std::vector<User*> result;
    for (auto& user : users) {
      if (user.role == role) result.push_back(&user);
    }
    return result;
  }
  
  std::vector<User*> getAllUsers() {
    std::vector<User*> result;
    for (auto& user : users) {
      result.push_back(&user);
    }
    return result;
  }
  
  // ========================================================================
  // PATIENT CRUD OPERATIONS
  // ========================================================================
  
  int addPatient(const Patient& patient) {
    Patient newPatient = patient;
    newPatient.id = nextPatientId++;
    patients.push_back(newPatient);
    return newPatient.id;
  }
  
  bool updatePatient(int id, const Patient& updatedPatient) {
    for (auto& patient : patients) {
      if (patient.id == id) {
        patient = updatedPatient;
        patient.id = id; // Preserve ID
        return true;
      }
    }
    return false;
  }
  
  bool deletePatient(int id) {
    for (auto it = patients.begin(); it != patients.end(); ++it) {
      if (it->id == id) {
        patients.erase(it);
        return true;
      }
    }
    return false;
  }
  
  Patient* getPatientById(int id) {
    for (auto& patient : patients) {
      if (patient.id == id) return &patient;
    }
    return nullptr;
  }
  
  Patient* getPatientByMRN(const String& mrn) {
    for (auto& patient : patients) {
      if (patient.mrn.equalsIgnoreCase(mrn)) return &patient;
    }
    return nullptr;
  }
  
  std::vector<Patient*> getAllPatients() {
    std::vector<Patient*> result;
    for (auto& patient : patients) {
      result.push_back(&patient);
    }
    return result;
  }
  
  // ========================================================================
  // PRESCRIPTION CRUD OPERATIONS
  // ========================================================================
  
  int addPrescription(const Prescription& prescription) {
    Prescription newPrescription = prescription;
    newPrescription.id = nextPrescriptionId++;
    prescriptions.push_back(newPrescription);
    return newPrescription.id;
  }
  
  bool updatePrescription(int id, const Prescription& updatedPrescription) {
    for (auto& prescription : prescriptions) {
      if (prescription.id == id) {
        prescription = updatedPrescription;
        prescription.id = id; // Preserve ID
        return true;
      }
    }
    return false;
  }
  
  bool deletePrescription(int id) {
    for (auto it = prescriptions.begin(); it != prescriptions.end(); ++it) {
      if (it->id == id) {
        prescriptions.erase(it);
        return true;
      }
    }
    return false;
  }
  
  Prescription* getPrescriptionById(int id) {
    for (auto& prescription : prescriptions) {
      if (prescription.id == id) return &prescription;
    }
    return nullptr;
  }
  
  Prescription* getPrescriptionByCode(const String& code) {
    for (auto& prescription : prescriptions) {
      if (prescription.prescriptionCode.equalsIgnoreCase(code)) 
        return &prescription;
    }
    return nullptr;
  }
  
  std::vector<Prescription*> getPrescriptionsByPatient(int patientId) {
    std::vector<Prescription*> result;
    for (auto& prescription : prescriptions) {
      if (prescription.patientId == patientId) 
        result.push_back(&prescription);
    }
    return result;
  }
  
  std::vector<Prescription*> getPrescriptionsByDoctor(int doctorId) {
    std::vector<Prescription*> result;
    for (auto& prescription : prescriptions) {
      if (prescription.doctorId == doctorId) 
        result.push_back(&prescription);
    }
    return result;
  }
  
  std::vector<Prescription*> getPrescriptionsByStatus(PrescriptionStatus status) {
    std::vector<Prescription*> result;
    for (auto& prescription : prescriptions) {
      if (prescription.status == status) 
        result.push_back(&prescription);
    }
    return result;
  }
  
  std::vector<Prescription*> getAllPrescriptions() {
    std::vector<Prescription*> result;
    for (auto& prescription : prescriptions) {
      result.push_back(&prescription);
    }
    return result;
  }
  
  // ========================================================================
  // NOTIFICATION CRUD OPERATIONS
  // ========================================================================
  
  int addNotification(const Notification& notification) {
    Notification newNotification = notification;
    newNotification.id = nextNotificationId++;
    notifications.push_back(newNotification);
    return newNotification.id;
  }
  
  bool updateNotification(int id, const Notification& updatedNotification) {
    for (auto& notification : notifications) {
      if (notification.id == id) {
        notification = updatedNotification;
        notification.id = id; // Preserve ID
        return true;
      }
    }
    return false;
  }
  
  bool deleteNotification(int id) {
    for (auto it = notifications.begin(); it != notifications.end(); ++it) {
      if (it->id == id) {
        notifications.erase(it);
        return true;
      }
    }
    return false;
  }
  
  Notification* getNotificationById(int id) {
    for (auto& notification : notifications) {
      if (notification.id == id) return &notification;
    }
    return nullptr;
  }
  
  std::vector<Notification*> getNotificationsByUser(int userId) {
    std::vector<Notification*> result;
    for (auto& notification : notifications) {
      if (notification.assignedUserId == userId) 
        result.push_back(&notification);
    }
    return result;
  }
  
  std::vector<Notification*> getUnreadNotificationsByUser(int userId) {
    std::vector<Notification*> result;
    for (auto& notification : notifications) {
      if (notification.assignedUserId == userId && !notification.read) 
        result.push_back(&notification);
    }
    return result;
  }
  
  std::vector<Notification*> getAllNotifications() {
    std::vector<Notification*> result;
    for (auto& notification : notifications) {
      result.push_back(&notification);
    }
    return result;
  }
  
  // ========================================================================
  // HELPER FUNCTIONS
  // ========================================================================
  
  String getRoleName(UserRole role) {
    switch(role) {
      case ROLE_ADMIN: return "Admin";
      case ROLE_DOCTOR: return "Doctor";
      case ROLE_NURSE: return "Nurse";
      case ROLE_PATIENT: return "Patient";
      default: return "Unknown";
    }
  }
  
  String getStatusName(PrescriptionStatus status) {
    switch(status) {
      case STATUS_PENDING: return "pending";
      case STATUS_DISPENSING: return "dispensing";
      case STATUS_READY: return "ready";
      case STATUS_PARTIALLY_DISPENSED: return "partially-dispensed";
      case STATUS_DISPENSED: return "dispensed";
      case STATUS_CANCELLED: return "cancelled";
      default: return "unknown";
    }
  }
  
  PrescriptionStatus parseStatus(const String& status) {
    if (status.equalsIgnoreCase("pending")) return STATUS_PENDING;
    if (status.equalsIgnoreCase("dispensing")) return STATUS_DISPENSING;
    if (status.equalsIgnoreCase("ready")) return STATUS_READY;
    if (status.equalsIgnoreCase("partially-dispensed")) return STATUS_PARTIALLY_DISPENSED;
    if (status.equalsIgnoreCase("dispensed")) return STATUS_DISPENSED;
    if (status.equalsIgnoreCase("cancelled")) return STATUS_CANCELLED;
    return STATUS_PENDING;
  }
  
  DispenseRequest createDispenseRequest(int prescriptionId) {
    DispenseRequest req;
    Prescription* rx = getPrescriptionById(prescriptionId);
    
    if (!rx) return req;
    
    req.prescriptionId = prescriptionId;
    req.prescriptionCode = rx->prescriptionCode;
    req.patientId = rx->patientId;
    req.doctorId = rx->doctorId;
    req.nurseId = rx->nurseId;
    
    Patient* patient = getPatientById(rx->patientId);
    if (patient) {
      req.patientName = patient->name;
      req.patientMRN = patient->mrn;
    }
    
    User* doctor = getUserById(rx->doctorId);
    if (doctor) req.doctorName = doctor->fullName;
    
    User* nurse = getUserById(rx->nurseId);
    if (nurse) req.nurseName = nurse->fullName;
    else req.nurseName = "Unassigned";
    
    // Parse medications
    for (const auto& med : rx->medications) {
      int medIdx = getMedicationIndex(med.medicationName);
      int freq = getMedicationFrequency(med.frequency);
      
      req.medicationIds.push_back(medIdx);
      req.frequencies.push_back(freq);
      req.quantities.push_back(med.quantity);
    }
    
    return req;
  }
  
  // ========================================================================
  // MEDICATION HELPERS (from original code)
  // ========================================================================
  
  int getMedicationIndex(const String& name) {
    const char* medicationList[9] = {
      "Medicine 1", "Medicine 2", "Medicine 3",
      "Medicine 4", "Medicine 5", "Medicine 6",
      "Medicine 7", "Medicine 8", "Medicine 9"
    };
    
    for (int i = 0; i < 9; ++i) {
      if (name.equalsIgnoreCase(medicationList[i])) return i + 1;
    }
    return 0;
  }
  
  int getMedicationFrequency(const String& freq) {
    if (freq.equalsIgnoreCase("once")) return 1;
    if (freq.equalsIgnoreCase("bid")) return 2;
    if (freq.equalsIgnoreCase("tid")) return 3;
    return 0;
  }
  
  String getMedicationName(int idx) {
    const char* medicationList[9] = {
      "Medicine 1", "Medicine 2", "Medicine 3",
      "Medicine 4", "Medicine 5", "Medicine 6",
      "Medicine 7", "Medicine 8", "Medicine 9"
    };
    
    if (idx >= 1 && idx <= 9) return String(medicationList[idx - 1]);
    return "";
  }
  
  // ========================================================================
  // INITIALIZATION WITH SAMPLE DATA
  // ========================================================================
  
  void initializeSampleData() {
    // Add sample users
    User admin1 = {"admin", "admin123", "Dr. John Smith", "j.smith@hospital.com", 
                   "MD-12345", "Internal Medicine", ROLE_ADMIN, -1};
    addUser(admin1);
    
    User doctor1 = {"doctor1", "pass123", "Dr. Sarah Johnson", "s.johnson@hospital.com",
                    "MD-23456", "Cardiology", ROLE_DOCTOR, -1};
    addUser(doctor1);
    
    User doctor2 = {"doctor2", "med456", "Dr. Michael Chen", "m.chen@hospital.com",
                    "MD-34567", "Emergency Medicine", ROLE_DOCTOR, -1};
    addUser(doctor2);
    
    User nurse1 = {"nurse1", "nurse123", "Nurse Emily Davis", "e.davis@hospital.com",
                   "RN-11111", "General Ward", ROLE_NURSE, -1};
    addUser(nurse1);
    
    User nurse2 = {"nurse2", "nurse456", "Nurse Robert Wilson", "r.wilson@hospital.com",
                   "RN-22222", "Emergency", ROLE_NURSE, -1};
    addUser(nurse2);
    
    // Add sample patients
    Patient p1 = {0, "Sarah Wilson", "MRN-78901234", "cardiology", "Ward-A-12",
                  "1985-05-15", -1};
    int pid1 = addPatient(p1);
    
    Patient p2 = {0, "Michael Rodriguez", "MRN-56789012", "internal", "Ward-B-08",
                  "1978-11-22", -1};
    int pid2 = addPatient(p2);
    
    Patient p3 = {0, "Emma Thompson", "MRN-34567890", "emergency", "ER-03",
                  "1992-03-08", -1};
    int pid3 = addPatient(p3);
    
    Patient p4 = {0, "Robert Chen", "MRN-23456789", "outpatient", "",
                  "1960-07-19", -1};
    addPatient(p4);
    
    // Add sample prescriptions
    Prescription rx1;
    rx1.prescriptionCode = "RX-2024-001";
    rx1.patientId = pid1;
    rx1.doctorId = 1; // admin/Dr. Smith
    rx1.nurseId = 1;  // nurse1
    rx1.status = STATUS_DISPENSING;
    rx1.dateCreated = "2024-01-16";
    Medication med1 = {"Medicine 2", "10mg", "tablet", "tid", 30};
    rx1.medications.push_back(med1);
    addPrescription(rx1);
    
    Prescription rx2;
    rx2.prescriptionCode = "RX-2024-002";
    rx2.patientId = pid2;
    rx2.doctorId = 2; // doctor1/Dr. Johnson
    rx2.nurseId = -1;
    rx2.status = STATUS_PENDING;
    rx2.dateCreated = "2024-01-16";
    Medication med2 = {"Medicine 5", "100IU/ml", "injection", "bid", 10};
    rx2.medications.push_back(med2);
    addPrescription(rx2);
    
    // Add sample notifications
    Notification notif1;
    notif1.notificationCode = "NOTIF-001";
    notif1.title = "Stock Alert: Medicine 5";
    notif1.content = "Limited stock remaining. Prescription RX-2024-002 may experience delays.";
    notif1.type = "warning";
    notif1.dateTime = "30 minutes ago";
    notif1.read = false;
    notif1.actionRequired = true;
    notif1.relatedPrescriptionId = 2;
    notif1.assignedUserId = 1; // admin
    addNotification(notif1);
    
    Notification notif2;
    notif2.notificationCode = "NOTIF-002";
    notif2.title = "Prescription Ready";
    notif2.content = "Medicine 2 for Sarah Wilson is ready for collection.";
    notif2.type = "success";
    notif2.dateTime = "1 hour ago";
    notif2.read = false;
    notif2.actionRequired = true;
    notif2.relatedPrescriptionId = 1;
    notif2.assignedUserId = 4; // nurse1
    addNotification(notif2);
  }
};

#endif // DATABASE_H