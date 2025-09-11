#ifndef STORAGE_MANAGER_H
#define STORAGE_MANAGER_H

#include <Arduino.h>
#include <SPIFFS.h>
#include <SD.h>
#include <SPI.h>
#include <FS.h>

enum StorageType {
  STORAGE_SPIFFS,
  STORAGE_SD
};

struct StorageInfo {
  String type;
  bool initialized;
  uint64_t totalBytes;
  uint64_t usedBytes;
  uint64_t freeBytes;
  String cardType;  // Only for SD cards
};

class StorageManager {
private:
  StorageType currentStorage;
  bool initialized;
  int sdCSPin;
  String storageTypeName;
  
  fs::FS* getFileSystem();

public:
  StorageManager();
  
  // Initialization
  bool begin(StorageType type, int csPin = 5);
  void end();
  
  // File operations
  bool exists(const String& path);
  File open(const String& path, const String& mode = "r");
  bool remove(const String& path);
  bool rename(const String& pathFrom, const String& pathTo);
  bool mkdir(const String& path);
  bool rmdir(const String& path);
  
  // Directory operations
  File openDir(const String& path);
  
  // Write operations
  bool writeFile(const String& path, const String& content);
  bool appendFile(const String& path, const String& content);
  String readFile(const String& path);
  
  // Storage info
  StorageInfo getInfo();
  bool isInitialized() { return initialized; }
  String getStorageType() { return storageTypeName; }
  StorageType getCurrentStorage() { return currentStorage; }
  
  // For web server integration
  fs::FS* getFS();
  
  // Utility functions
  void listDir(const String& dirname, uint8_t levels = 1);
  size_t getFileSize(const String& path);
  String getContentType(const String& filename);
};

// Global instance
extern StorageManager Storage;

#endif