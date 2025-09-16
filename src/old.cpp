// #include <WiFi.h>
// #include <WebServer.h>
// #include "AccelStepper.h"
// #include <ArduinoJson.h>
// #include <esp_now.h>
// #include <U8g2lib.h>
// #include <Adafruit_PWMServoDriver.h>
// #include <Wire.h>

// #ifdef U8X8_HAVE_HW_SPI
// #include <SPI.h>
// #endif

// #define stepperXStep_pin 27
// #define stepperXDir_pin 12
// #define XRightEndStop 35
// #define XLeftEndStop 34

// #define stepperYLeftStep_pin 32
// #define stepperYLeftDir_pin 33
// #define YLeftButtomStop 39

// #define stepperYRightStep_pin 25
// #define stepperYRightDir_pin 26
// #define YRightButtomStop 36

// #define SERVOMIN  80  // Minimum value
// #define SERVOMAX  600  // Maximum value

// // Define servo motor connections (expand as required)
// #define SER0  0   //Servo Motor 0 on connector 0

// AccelStepper stepperX(AccelStepper::DRIVER, stepperXStep_pin, stepperXDir_pin);
// AccelStepper stepperYLeft(AccelStepper::DRIVER, stepperYLeftStep_pin, stepperYLeftDir_pin); 
// AccelStepper stepperYRight(AccelStepper::DRIVER, stepperYRightStep_pin, stepperYRightDir_pin); 

// Adafruit_PWMServoDriver pca9685 = Adafruit_PWMServoDriver(0x40);

// // Set your Wi-Fi access point credentials
// const char *ssid = "PharmaAssist";
// const char *password = "CZSHSRobotics";
// const char *routerSSID = "PharmaAssist_Wifi";
// const char *routerPassword = "1qaz2wsx3edc";

// // REPLACE WITH YOUR RECEIVER MAC Address
// uint8_t broadcastAddress[] = {0x5c, 0x01, 0x3b, 0x6b, 0xb0, 0x94};
// //uint8_t broadcastAddress[] = {0x3c, 0x8a, 0x1f, 0x5e, 0xc8, 0xf8};

// U8G2_ST7920_128X64_F_SW_SPI u8g2(U8G2_R0, /* clock=*/ 14, /* data=*/ 13, /* CS=*/ 15, /* reset=*/ 0);

// // Static IP configuration
// IPAddress staticIP(192, 168, 0, 100); // ESP32 static IP
// IPAddress gateway(192, 168, 0, 1);    // IP Address of your network gateway (router)
// IPAddress subnet(255, 255, 255, 0);   // Subnet mask
// IPAddress primaryDNS(192, 168, 0, 1); // Primary DNS (optional)
// IPAddress secondaryDNS(0, 0, 0, 0);   // Secondary DNS (optional)

// const int ledPin = 27;
// long initial_homing=-1;
// long xInitial_max=15000;
// long yRInitial_max=20000;
// long yLInitial_max=20000;
// long xMax = xInitial_max;
// long yLeftMax = yLInitial_max;
// long yRightMax = yRInitial_max;
// long yMax = (yLeftMax + yRightMax)/2;
// long maxSpeed = 25000.0;
// long accel = 5000.0;
// int pwm0;

// // Create an instance of the WebServer on port 80
// WebServer server(80);

// // Structure example to send data
// // Must match the receiver structure
// typedef struct struct_message {
//   char pName[32];
//   char med1[8];
//   char dose1[8];
//   char freq1[16];
//   char med2[8];
//   char dose2[8];
//   char freq2[16];
//   char med3[8];
//   char dose3[8];
//   char freq3[16];
// } struct_message;

// // Create a struct_message called myData
// struct_message PMedData;

// esp_now_peer_info_t peerInfo;

// // callback when data is sent
// void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
//   Serial.print("\r\nLast Packet Send Status:\t");
//   Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
// }

// // HTML page with 9 buttons in a 3x3 grid
// String pharmaAsisstWebSite = R"rawliteral(
// <!DOCTYPE html>
// <html lang="en">
// <head>
//     <meta charset="UTF-8">
//     <meta name="viewport" content="width=device-width, initial-scale=1.0">
//     <title>PharmaAssist</title>
//     <style>
//         body {font-family: Arial, sans-serif;text-align: center;background-image: url('bg.jpeg');background-size: cover;margin: 0;padding: 0;}
//         .container {width: 85%;max-width: 350px;margin: auto;margin-top: 15px;background-image: url('bg.jpeg');padding: 20px;border-radius: 10px;box-shadow: 0px 4px 10px gray;}
//         select, input, button {margin: 10px;padding: 10px;width: 90%;border-radius: 5px;border: 1px solid #ccc;}
//         button {background-color: #0288d1;color: white;border: none;cursor: pointer;}
//         button:hover {background-color: #0277bd;}
//         #dataButton {background-color: #0288d1;color: white;border: none;cursor: pointer;}
//         #dataButton:hover {background-color: #0277bd;}
//         ul {list-style-type: none;padding: 0;}
//         .title {color: #01579b;}
//         #loginContainer {display: block;}
//         #mainContainer {display: none;}
//         #patientName {display: none;}
//         #finalizeData {display: none;}
//     </style>
//         <script>
//         let medNum = 0;
//         let list = '';
    
//             window.onload = function() {
//                 document.getElementById("frequency").addEventListener("change", function() {
//                     let hoursInput = document.getElementById("hours");
//                     hoursInput.style.display = (this.value === "every") ? "block" : "none";
//                 });
//             };
    
//             function login() {
//                 let username = document.getElementById("username").value;
//                 let password = document.getElementById("password").value;
    
//                 if (username && password) {
//                     document.getElementById("loginContainer").style.display = "none";
//                     document.getElementById("mainContainer").style.display = "block";
//                 } else {
//                     alert("Please enter valid credentials.");
//                 }
//             }
    
//             function addPrescription() {
//                 medNum++
//                 if (medNum > 3) {
//                     alert("Maximum of 3 prescriptions allowed.");
//                     return;
//                 }
    
//                 let patient = document.getElementById("patient").value;
//                 let medicine = document.getElementById("medicine").value;
//                 let dose = document.getElementById("dose").value;
//                 let frequency = document.getElementById("frequency").value;
//                 let hours = document.getElementById("hours").value;
    
//                 if (frequency === "every" && hours === "") {
//                     alert("Please specify the number of hours.");
//                     return;
//                 }
    
//                 let frequencyText = (frequency === "every") ? Every ${hours} hours : ${frequency}/day;

//                 if(medNum==1){
//                     list +=`"${patient}","${medicine}","${dose}","${frequencyText}"`;
//                     document.getElementById("patientName").style.display = "block";
//                     document.getElementById("patientName").innerHTML = "Patient Name: " + ${patient};
//                 }else{
//                     list +=`,"${medicine}","${dose}","${frequencyText}"`;
//                 }
//                 let listItem = document.createElement("li");
//                 listItem.textContent = ${medicine} - ${dose} - ${frequencyText};
//                 document.getElementById("prescriptionList").appendChild(listItem);

//                 document.getElementById("finalizeData").value = "["+list+"]";
//             }
//         </script>
// </head>
// <body>

//     <!-- Login Form -->
//     <div class="container" id="loginContainer">
//         <h2 class="title">PramaAssist Login</h2>
//         <label for="username">Username:</label>
//         <input type="text" id="username" placeholder="Enter username">
//         <label for="password">Password:</label>
//         <input type="password" id="password" placeholder="Enter password">
//         <button onclick="login()">Login</button>
//     </div>
    
//     <!-- Prescription Form -->
//     <div class="container" id="mainContainer">
//         <h2 class="title">PramaAssist Prescription</h2>
        
//         <label for="patient">Select Patient:</label>
//         <select id="patient">
//             <option value="Ireneo D. Seraspe Jr">Ireneo D. Seraspe Jr</option>
//             <option value="Rodolfo T. Pomida Jr.">Rodolfo T. Pomida Jr.</option>
//             <option value="Allen Carl BArtolome">Allen Carl Bartolome</option>
//         </select>
        
//         <label for="medicine">Select Medicine:</label>
//         <select id="medicine">
//             <option value="Med1">Medicine 1</option>
//             <option value="Med2">Medicine 2</option>
//             <option value="Med3">Medicine 3</option>
//             <option value="Med4">Medicine 4</option>
//             <option value="Med5">Medicine 5</option>
//             <option value="Med6">Medicine 6</option>
//             <option value="Med7">Medicine 7</option>
//             <option value="Med8">Medicine 8</option>
//             <option value="Med9">Medicine 9</option>
//         </select>

//         <label for="dose">Dose:</label>
//         <select id="dose">
//             <option value="50">50mg</option>
//             <option value="100">100mg</option>
//             <option value="250">250mg</option>
//             <option value="500">500mg</option>
//             <option value="1">1g</option>
//         </select>

//         <label for="frequency">Times per day:</label>
//         <select id="frequency">
//             <option value="1">Once</option>
//             <option value="2">Twice</option>
//             <option value="3">Three times</option>
//             <option value="every">Every X hours</option>
//         </select>
        
//         <input type="number" id="hours" placeholder="Enter hours" style="display: none;">
        
//         <button onclick="addPrescription()">Add Prescription</button>
        
//         <h3 class="title">Prescription List</h3>
//         <h4 id="patientName">Patient Name</h4>
//         <ul id="prescriptionList"></ul>
//           <form action="/sendData" method="POST">
//           <textarea id="finalizeData" name="data" rows="3" cols="15"></textarea><br>
//             <input id="dataButton" type="submit" value="Finalize Prescription">
//             </form>
//     </div>
// </body>
// </html>

// )rawliteral";

// // Function to handle data sent from the client
// void handleSendData() {

//   if (server.hasArg("data")) {
//     String jsonData = server.arg("data");
//     Serial.println("Received data: " + jsonData);

//     // Parse JSON array
//     DynamicJsonDocument doc(1024);
//     DeserializationError error = deserializeJson(doc, jsonData);

//     if (error) {
//       Serial.print(F("deserializeJson() failed: "));
//       Serial.println(error.f_str());
//       server.send(400, "text/plain", "Invalid JSON");
//       return;
//     }
//     // Process the JSON array
//     JsonArray array = doc.as<JsonArray>();
//     for (byte i = 0; i<array.size(); i++) {
//       if(i==0){
//         strcpy(PMedData.pName, array[i]);
//       }else if(i==1){
//         cabinetNumber(medNum(array[i].as<String>()));
//         delay(500);
//         strcpy(PMedData.med1, array[i]);
//       }else if(i==2){
//         strcpy(PMedData.dose1, array[i]);
//       }else if(i==3){
//         strcpy(PMedData.freq1, array[i]);
//       }else if(i==4){
//         cabinetNumber(medNum(array[i].as<String>()));
//         delay(500);
//         strcpy(PMedData.med2, array[i]);
//       }else if(i==5){
//         strcpy(PMedData.dose2, array[i]);
//       }else if(i==6){
//         strcpy(PMedData.freq2, array[i]);
//       }else if(i==7){
//         cabinetNumber(medNum(array[i].as<String>()));
//         delay(500);
//         strcpy(PMedData.med3, array[i]);
//       }else if(i==8){
//         strcpy(PMedData.dose3, array[i]);
//       }else if(i==9){
//         strcpy(PMedData.freq3, array[i]);
//       }
//     }
  
//   // Send message via ESP-NOW
//   esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &PMedData, sizeof(PMedData));
   
//   if (result == ESP_OK) {
//     Serial.println("Sent with success");
//   }
//   else {
//     Serial.println("Error sending the data");
//   }

//     server.send(200, "text/html", pharmaAsisstWebSite);
//   } else {
//     server.send(400, "text/plain", "No data received");
//   }

// }

// int medNum(String m){
//   if(m.equals("Med1")){
//     return 1;
//   }else if(m.equals("Med2")){
//     return 2;
//   }else if(m.equals("Med3")){
//     return 3;
//   }else if(m.equals("Med4")){
//     return 4;
//   }else if(m.equals("Med5")){
//     return 5;
//   }else if(m.equals("Med6")){
//     return 6;
//   }else if(m.equals("Med7")){
//     return 7;
//   }else if(m.equals("Med8")){
//     return 8;
//   }else if(m.equals("Med9")){
//     return 9;
//   }else{
//     return 0;
//   }
// }

// // Setup Wi-Fi and Web Server
// void setup() {
//   // Start serial communication
//   Serial.begin(115200);

//   pinMode(XRightEndStop, INPUT);  // put your setup code here, to run once:
//   pinMode(XLeftEndStop, INPUT);
//   pinMode(YRightButtomStop, INPUT);
//   pinMode(YLeftButtomStop, INPUT);

//   pinMode(stepperXStep_pin, OUTPUT);
//   pinMode(stepperYLeftStep_pin, OUTPUT);
//   pinMode(stepperYRightStep_pin, OUTPUT);
//   pinMode(stepperXDir_pin, OUTPUT);
//   pinMode(stepperYLeftDir_pin, OUTPUT);
//   pinMode(stepperYRightDir_pin, OUTPUT);
//   delay(5);

//     // Print to monitor
//   Serial.println("PCA9685 Servo Test");
//   // Initialize PCA9685
//   pca9685.begin();
//   // Set PWM Frequency to 50Hz
//   pca9685.setPWMFreq(50);
//   Serial.println("PCA9685 Servo Set to home");
//   pca9685.setPWM(SER0, 0, 0);
//   delay(500);

//   u8g2.begin();
//   u8g2.clearBuffer();					// clear the internal memory
//   u8g2.setFont(u8g2_font_ncenB08_tr);	// choose a suitable font
//   u8g2.drawStr(30,13,"PharmaAssist");	// write something to the internal memory
//   u8g2.sendBuffer();	

// //  Start the Wi-Fi access point
//   WiFi.softAP(ssid, password);
//   Serial.println("ESP32 Access Point Started");
//   Serial.print("IP Address: ");
//   Serial.println(WiFi.softAPIP());

//   // Configuring static IP
// //   if(!WiFi.config(staticIP, gateway, subnet, primaryDNS, secondaryDNS)) {
// //     Serial.println("Failed to configure Static IP");
// //   } else {
// //     Serial.println("Static IP configured!");
// //     u8g2.clearBuffer();					// clear the internal memory
// //     u8g2.drawStr(30,13,"PharmaAssist");
// //     u8g2.drawStr(0,30,"Static IP configured!");
// //     u8g2.sendBuffer();
// //         delay(500);
// //   }

// // //This is to connect to a wifi
// //     WiFi.mode(WIFI_STA); //Optional
// //     Serial.print("Connecting to ");
// //     Serial.println(routerSSID);
// //         u8g2.clearBuffer();					// clear the internal memory
// //     u8g2.drawStr(30,13,"PharmaAssist");
// //     u8g2.drawStr(0,30,"Connecting to Wifi");
// //     u8g2.sendBuffer();
// //         delay(500);
// //     WiFi.begin(routerSSID, routerPassword);
// //     Serial.println("\nConnecting");
// //     while(WiFi.status() != WL_CONNECTED){
// //         Serial.print(".");
// //         delay(100);
// //     }
// //     Serial.println("\nConnected to the WiFi network");
// //     Serial.print("Local ESP32 IP: ");
// //     Serial.println(WiFi.localIP());

//   // Init ESP-NOW
//   if (esp_now_init() != ESP_OK) {
//     Serial.println("Error initializing ESP-NOW");
//     return;
//   }else{
//     Serial.println("ESP-NOW Acticated");
//         u8g2.clearBuffer();					// clear the internal memory
//     u8g2.drawStr(30,13,"PharmaAssist");
//     u8g2.drawStr(0,30,"ESP-NOW Acticated");
//     u8g2.sendBuffer();
//     delay(500);
//   }

//   // Once ESPNow is successfully Init, we will register for Send CB to
//   // get the status of Trasnmitted packet
//   esp_now_register_send_cb(OnDataSent);
  
//   // Register peer
//   memcpy(peerInfo.peer_addr, broadcastAddress, 6);
//   peerInfo.channel = 0;  
//   peerInfo.encrypt = false;
  
//   // Add peer        
//   if (esp_now_add_peer(&peerInfo) != ESP_OK){
//     Serial.println("Failed to add peer");
//     return;
//   }else{
//     Serial.println("Peer Added!!!");
//         u8g2.clearBuffer();					// clear the internal memory
//     u8g2.drawStr(30,13,"PharmaAssist");
//     u8g2.drawStr(0,30,"Peer Added!!!");
//     u8g2.sendBuffer();
//     delay(500);
//   }

// // Define routes
//   server.on("/", HTTP_GET, []() {server.send(200, "text/html", pharmaAsisstWebSite);  });
//   server.on("/sendData", HTTP_POST, handleSendData);
  
//   Serial.print("Start the server");// Start the server
//   server.begin();
//       u8g2.clearBuffer();					// clear the internal memory
//     u8g2.drawStr(30,13,"PharmaAssist");
//     u8g2.drawStr(0,30,"Start the server!!!");
//   u8g2.sendBuffer();
//   delay(1000);
//       u8g2.clearBuffer();					// clear the internal memory
//     u8g2.drawStr(30,13,"PharmaAssist");
//     u8g2.drawStr(0,30,"Homing");
//   u8g2.sendBuffer();
//   homing();
//   stepperX.setMaxSpeed(maxSpeed);
//   stepperX.setAcceleration(accel);
//   stepperYLeft.setMaxSpeed(maxSpeed);
//   stepperYLeft.setAcceleration(accel);
//   stepperYRight.setMaxSpeed(maxSpeed);
//   stepperYRight.setAcceleration(accel);

//       u8g2.clearBuffer();					// clear the internal memory
//     u8g2.drawStr(30,13,"PharmaAssist");
//     u8g2.drawStr(0,30,"Let's Start!!!");
//   u8g2.sendBuffer();
// //  goTo(15000, 20000);
//     goTo(8000,8500);
// }

// void loop() {
//   server.handleClient();
//   // Set values to send
// }

// void homing(){
//   stepperX.setMaxSpeed(500.0);
//   stepperX.setAcceleration(10000.0);
//   stepperYLeft.setMaxSpeed(500.0);
//   stepperYLeft.setAcceleration(10000.0);
//   stepperYRight.setMaxSpeed(500.0);
//   stepperYRight.setAcceleration(10000.0);
//   Serial.println("");
//   Serial.println("Stepper X and Y axis is Homing...............");

//   while((digitalRead(XLeftEndStop) + digitalRead(YLeftButtomStop) + digitalRead(YRightButtomStop))<3){
//   stepperX.moveTo(initial_homing);
//   stepperYLeft.moveTo(initial_homing);
//   stepperYRight.moveTo(initial_homing);
//     if(digitalRead(XLeftEndStop)==LOW){
//       stepperX.run();
//     }
//     if(digitalRead(YLeftButtomStop)==LOW){
//       stepperYLeft.run();
//     }
//     if(digitalRead(YRightButtomStop)==LOW){
//       stepperYRight.run();
//     }
//   initial_homing--;
//   }

//   stepperX.setMaxSpeed(100.0);
//   stepperX.setAcceleration(100.0);
//   stepperYLeft.setMaxSpeed(100.0);
//   stepperYLeft.setAcceleration(100.0);
//   stepperYRight.setMaxSpeed(100.0);
//   stepperYRight.setAcceleration(100.0);
//   initial_homing=1;

//   while ((digitalRead(XLeftEndStop) + digitalRead(YLeftButtomStop) + digitalRead(YRightButtomStop))>0){
//   stepperX.moveTo(initial_homing);
//   stepperYLeft.moveTo(initial_homing);
//   stepperYRight.moveTo(initial_homing);
//     if(digitalRead(XLeftEndStop)==HIGH){
//       stepperX.run();
//     }
//     if(digitalRead(YLeftButtomStop)==HIGH){
//       stepperYLeft.run();
//     }
//     if(digitalRead(YRightButtomStop)==HIGH){
//       stepperYRight.run();
//     }
//      initial_homing++;
//   }
//   stepperX.setCurrentPosition(0);
//   stepperYLeft.setCurrentPosition(0);
//   stepperYRight.setCurrentPosition(0);
//   Serial.println("Homing Done!");

// }

// void goTo(int x, int y){
//   stepperX.moveTo(x);
//   stepperYLeft.moveTo(y);
//   stepperYRight.moveTo(y);
//   while(stepperX.distanceToGo()!=0 || stepperYLeft.distanceToGo()!=0 || stepperYRight.distanceToGo()!=0){
//     if(digitalRead(YLeftButtomStop)==LOW && digitalRead(YRightButtomStop)==LOW ){
//       stepperYLeft.run();
//       stepperYRight.run();
//     }
//     if(digitalRead(XLeftEndStop)==LOW && digitalRead(XRightEndStop)==LOW){
//       stepperX.run();
//     }
//   }
//   Serial.print("Done Going to  X: ");
//   Serial.print(stepperX.currentPosition());
//   Serial.print(" , Y: ");
//   Serial.println((stepperYLeft.currentPosition()+stepperYRight.currentPosition())/2);
//   delayMicroseconds(1000);
// }

// void cabinetNumber(int n){
//   if(n==1){
//     goTo(2000,17100);
//     getMed();
//   }else if(n==2){
//     goTo(8000,17100);
//     getMed();
//   }else if(n==3){
//     goTo(13500,17100);
//     getMed();
//   }else if(n==4){
//     goTo(2000,9100);
//     getMed();
//   }else if(n==5){
//     goTo(8000,9100);
//     getMed();
//   }else if(n==6){
//     goTo(13500,9100);
//     getMed();
//   }else if(n==7){
//     goTo(2000,700);
//     getMed();
//   }else if(n==8){
//     goTo(8000,700);
//     getMed();
//   }else if(n==9){
//     goTo(13500,700);
//     getMed();
//   }
// }

// void getMed(){
//     // Move Motor 0 from 180 to 0 degrees
//   for (int posDegrees = 180; posDegrees >= 0; posDegrees--) {
//     // Determine PWM pulse width
//     pwm0 = map(posDegrees, 0, 180, SERVOMIN, SERVOMAX);
//     // Write to PCA9685
//     pca9685.setPWM(SER0, 0, pwm0);
//     // Print to serial monitor
//     Serial.print("Motor 0 = ");
//     Serial.println(posDegrees);
//     delay(30);
//   }
//   // Move Motor 0 from 0 to 180 degrees
//   for (int posDegrees = 0; posDegrees <= 180; posDegrees++) {
//     // Determine PWM pulse width
//     pwm0 = map(posDegrees, 0, 180, SERVOMIN, SERVOMAX);
//     // Write to PCA9685
//     pca9685.setPWM(SER0, 0, pwm0);
//     // Print to serial monitor
//     Serial.print("Motor 0 = ");
//     Serial.println(posDegrees);
//     delay(30);
//   }
// }