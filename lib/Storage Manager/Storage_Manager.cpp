#include "Storage_Manager.h"

// Global instance
StorageManager Storage;

StorageManager::StorageManager() : initialized(false), sdCSPin(5) {}

fs::FS* StorageManager::getFileSystem() {
  if (!initialized) return nullptr;
  
  switch (currentStorage) {
    case STORAGE_SPIFFS:
      return &SPIFFS;
    case STORAGE_SD:
      return &SD;
    default:
      return nullptr;
  }
}

bool StorageManager::begin(StorageType type, int csPin) {
  currentStorage = type;
  sdCSPin = csPin;
  initialized = false;
  
  switch (type) {
    case STORAGE_SPIFFS:
      storageTypeName = "SPIFFS";
      Serial.println("StorageManager: Initializing SPIFFS...");
      if (SPIFFS.begin(true)) {
        initialized = true;
        Serial.println("StorageManager: SPIFFS mounted successfully");
      } else {
        Serial.println("StorageManager: Failed to mount SPIFFS");
      }
      break;
      
    case STORAGE_SD:
      storageTypeName = "SD Card";
      Serial.printf("StorageManager: Initializing SD Card (CS: %d)...\n", csPin);
      if (SD.begin(csPin)) {
        uint8_t cardType = SD.cardType();
        if (cardType != CARD_NONE) {
          initialized = true;
          Serial.print("StorageManager: SD Card Type: ");
          switch (cardType) {
            case CARD_MMC: Serial.println("MMC"); break;
            case CARD_SD: Serial.println("SDSC"); break;
            case CARD_SDHC: Serial.println("SDHC"); break;
            default: Serial.println("UNKNOWN"); break;
          }
          uint64_t cardSize = SD.cardSize() / (1024 * 1024);
          Serial.printf("StorageManager: SD Card Size: %lluMB\n", cardSize);
          Serial.println("StorageManager: SD Card mounted successfully");
        } else {
          Serial.println("StorageManager: No SD card attached");
        }
      } else {
        Serial.println("StorageManager: SD Card mount failed");
      }
      break;
  }
  
  return initialized;
}

void StorageManager::end() {
  if (!initialized) return;
  
  switch (currentStorage) {
    case STORAGE_SPIFFS:
      SPIFFS.end();
      break;
    case STORAGE_SD:
      SD.end();
      break;
  }
  
  initialized = false;
  Serial.println("StorageManager: Storage unmounted");
}

bool StorageManager::exists(const String& path) {
  if (!initialized) return false;
  
  fs::FS* fs = getFileSystem();
  return fs ? fs->exists(path) : false;
}

File StorageManager::open(const String& path, const String& mode) {
  if (!initialized) return File();
  
  fs::FS* fs = getFileSystem();
  return fs ? fs->open(path, mode.c_str()) : File();
}

bool StorageManager::remove(const String& path) {
  if (!initialized) return false;
  
  fs::FS* fs = getFileSystem();
  return fs ? fs->remove(path) : false;
}

bool StorageManager::rename(const String& pathFrom, const String& pathTo) {
  if (!initialized) return false;
  
  fs::FS* fs = getFileSystem();
  return fs ? fs->rename(pathFrom, pathTo) : false;
}

bool StorageManager::mkdir(const String& path) {
  if (!initialized) return false;
  
  fs::FS* fs = getFileSystem();
  return fs ? fs->mkdir(path) : false;
}

bool StorageManager::rmdir(const String& path) {
  if (!initialized) return false;
  
  fs::FS* fs = getFileSystem();
  return fs ? fs->rmdir(path) : false;
}

File StorageManager::openDir(const String& path) {
  if (!initialized) return File();
  
  fs::FS* fs = getFileSystem();
  return fs ? fs->open(path) : File();
}

bool StorageManager::writeFile(const String& path, const String& content) {
  File file = open(path, "w");
  if (!file) {
    Serial.println("StorageManager: Failed to open file for writing: " + path);
    return false;
  }
  
  size_t written = file.print(content);
  file.close();
  
  bool success = (written == content.length());
  if (success) {
    Serial.println("StorageManager: File written successfully: " + path);
  } else {
    Serial.println("StorageManager: Failed to write file: " + path);
  }
  
  return success;
}

bool StorageManager::appendFile(const String& path, const String& content) {
  File file = open(path, "a");
  if (!file) {
    Serial.println("StorageManager: Failed to open file for appending: " + path);
    return false;
  }
  
  size_t written = file.print(content);
  file.close();
  
  bool success = (written == content.length());
  if (success) {
    Serial.println("StorageManager: Content appended successfully: " + path);
  } else {
    Serial.println("StorageManager: Failed to append to file: " + path);
  }
  
  return success;
}

String StorageManager::readFile(const String& path) {
  File file = open(path, "r");
  if (!file) {
    Serial.println("StorageManager: Failed to open file for reading: " + path);
    return String();
  }
  
  String content = file.readString();
  file.close();
  
  Serial.println("StorageManager: File read successfully: " + path + " (" + String(content.length()) + " bytes)");
  return content;
}

StorageInfo StorageManager::getInfo() {
  StorageInfo info;
  info.type = storageTypeName;
  info.initialized = initialized;
  info.totalBytes = 0;
  info.usedBytes = 0;
  info.freeBytes = 0;
  info.cardType = "";
  
  if (!initialized) return info;
  
  switch (currentStorage) {
    case STORAGE_SPIFFS:
      info.totalBytes = SPIFFS.totalBytes();
      info.usedBytes = SPIFFS.usedBytes();
      info.freeBytes = info.totalBytes - info.usedBytes;
      break;
      
    case STORAGE_SD:
      info.totalBytes = SD.totalBytes();
      info.usedBytes = SD.usedBytes();
      info.freeBytes = info.totalBytes - info.usedBytes;
      
      uint8_t cardType = SD.cardType();
      switch (cardType) {
        case CARD_MMC: info.cardType = "MMC"; break;
        case CARD_SD: info.cardType = "SDSC"; break;
        case CARD_SDHC: info.cardType = "SDHC"; break;
        default: info.cardType = "UNKNOWN"; break;
      }
      break;
  }
  
  return info;
}

fs::FS* StorageManager::getFS() {
  return getFileSystem();
}

void StorageManager::listDir(const String& dirname, uint8_t levels) {
  if (!initialized) {
    Serial.println("StorageManager: Storage not initialized");
    return;
  }
  
  File root = openDir(dirname);
  if (!root) {
    Serial.println("StorageManager: Failed to open directory: " + dirname);
    return;
  }
  
  if (!root.isDirectory()) {
    Serial.println("StorageManager: Not a directory: " + dirname);
    root.close();
    return;
  }
  
  Serial.println("StorageManager: Listing directory: " + dirname);
  
  File file = root.openNextFile();
  while (file) {
    for (uint8_t i = 0; i < levels - 1; i++) {
      Serial.print("  ");
    }
    
    if (file.isDirectory()) {
      Serial.println("DIR : " + String(file.name()));
      if (levels > 1) {
        listDir(file.path(), levels - 1);
      }
    } else {
      Serial.println("FILE: " + String(file.name()) + " (" + String(file.size()) + " bytes)");
    }
    file = root.openNextFile();
  }
  root.close();
}

size_t StorageManager::getFileSize(const String& path) {
  File file = open(path, "r");
  if (!file) return 0;
  
  size_t size = file.size();
  file.close();
  return size;
}

String StorageManager::getContentType(const String& filename) {
  if (filename.endsWith(".html")) return "text/html";
  else if (filename.endsWith(".css")) return "text/css";
  else if (filename.endsWith(".js")) return "application/javascript";
  else if (filename.endsWith(".json")) return "application/json";
  else if (filename.endsWith(".png")) return "image/png";
  else if (filename.endsWith(".gif")) return "image/gif";
  else if (filename.endsWith(".jpg")) return "image/jpeg";
  else if (filename.endsWith(".ico")) return "image/x-icon";
  else if (filename.endsWith(".xml")) return "text/xml";
  else if (filename.endsWith(".pdf")) return "application/pdf";
  else if (filename.endsWith(".zip")) return "application/zip";
  else if (filename.endsWith(".gz")) return "application/x-gzip";
  return "text/plain";
}