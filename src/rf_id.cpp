// #include <SPI.h>
// #include <SD.h>

// // ---------------- HSPI (SD card) ----------------
// #define SD_CS_PIN     15
// #define HSPI_SCK      14
// #define HSPI_MISO     12
// #define HSPI_MOSI     13

// SPIClass hspi(HSPI);  // Create HSPI bus

// void setup() {
//   Serial.begin(115200);
//   delay(500);

//   // --- Init HSPI for SD card ---
//   hspi.begin(HSPI_SCK, HSPI_MISO, HSPI_MOSI, SD_CS_PIN);
  
// }

// void loop() {
//   if (!SD.begin(SD_CS_PIN, hspi)) {
//     Serial.println("❌ SD Card mount failed!");
//   } else {
//     Serial.println("✅ SD Card mounted on HSPI");
//     File file = SD.open("/test.txt", FILE_WRITE);
//     if (file) {
//       file.println("Hello from HSPI SD!");
//       file.close();
//       Serial.println("Wrote /test.txt");
//     }
//   }
//   delay(3000); // wait 3 seconds before next attempt
// }
