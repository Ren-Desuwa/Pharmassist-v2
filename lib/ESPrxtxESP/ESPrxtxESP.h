#ifndef ESPRXTXESP_H
#define ESPRXTXESP_H

#include <Arduino.h>
#include <HardwareSerial.h>

class ESPReader {
private:
    HardwareSerial* serial;
    String buffer;
    bool dataAvailable;
    
public:
    ESPReader(HardwareSerial* hwSerial) {
        serial = hwSerial;
        buffer = "";
        dataAvailable = false;
    }
    
    void begin(long baudRate = 9600) {
        serial->begin(baudRate);
    }
    
    ~ESPReader() {
        // Don't delete HardwareSerial as it's not dynamically allocated
    }
    
    void update() {
        while (serial->available()) {
            char c = serial->read();
            if (c == '\n') {
                dataAvailable = true;
                return;
            }
            buffer += c;
        }
    }
    
    bool available() {
        update();
        return dataAvailable;
    }
    
    String read() {
        if (dataAvailable) {
            String data = buffer;
            buffer = "";
            dataAvailable = false;
            return data;
        }
        return "";
    }
    
    String peek() {
        return buffer;
    }
    
    void flush() {
        buffer = "";
        dataAvailable = false;
        serial->flush();
    }
};

class ESPSender {
private:
    HardwareSerial* serial;
    
public:
    ESPSender(HardwareSerial* hwSerial) {
        serial = hwSerial;
    }
    
    void begin(long baudRate = 9600) {
        serial->begin(baudRate);
    }
    
    ~ESPSender() {
        // Don't delete HardwareSerial as it's not dynamically allocated
    }
    
    void send(String message) {
        serial->print(message);
        serial->print('\n');
        serial->flush();
    }
    
    void send(const char* message) {
        serial->print(message);
        serial->print('\n');
        serial->flush();
    }
    
    void send(int value) {
        serial->print(value);
        serial->print('\n');
        serial->flush();
    }
    
    void send(float value) {
        serial->print(value);
        serial->print('\n');
        serial->flush();
    }

    
    bool isReady() {
        return serial != nullptr;
    }
};

class ESPrxtxESP {
private:
    ESPReader* reader;
    ESPSender* sender;
    unsigned long lastPingTime;
    unsigned long pingInterval;
    bool autoPingEnabled;
    
public:
    ESPrxtxESP(HardwareSerial* hwSerial) {
        reader = new ESPReader(hwSerial);
        sender = new ESPSender(hwSerial);
        lastPingTime = 0;
        pingInterval = 10000; // Default 10 seconds
        autoPingEnabled = false;
    }
    
    void begin(long baudRate = 9600) {
        reader->begin(baudRate);
        sender->begin(baudRate);
    }
    
    ~ESPrxtxESP() {
        delete reader;
        delete sender;
    }
    
    // Reader methods
    bool available() {
        return reader->available();
    }
    
    String read() {
        return reader->read();
    }
    
    String peek() {
        return reader->peek();
    }
    
    void flushReader() {
        reader->flush();
    }
    
    // Sender methods
    void send(String message) {
        sender->send(message);
    }
    
    void send(const char* message) {
        sender->send(message);
    }
    
    void send(int value) {
        sender->send(value);
    }
    
    void send(float value) {
        sender->send(value);
    }
    
    bool isReady() {
        return sender->isReady();
    }
    
    // Combined methods
    void update() {
        reader->update();
    }
    
    String readIfAvailable() {
        if (available()) {
            return read();
        }
        return "";
    }
    
    bool sendAndWaitForReply(String message, String& reply, unsigned long timeout = 5000) {
        send(message);
        unsigned long startTime = millis();
        
        while (millis() - startTime < timeout) {
            if (available()) {
                reply = read();
                return true;
            }
            delay(10);
        }
        return false;
    }
    
    // Ping/Pong/Ack functionality
    void sendPing() {
        send("PING");
    }
    
    void sendPong() {
        send("PONG");
    }
    
    void sendAck() {
        send("ACK");
    }
    
    bool isPing(String message) {
        message.toUpperCase();
        message.trim();
        return message == "PING";
    }
    
    bool isPong(String message) {
        message.toUpperCase();
        message.trim();
        return message == "PONG";
    }
    
    bool isAck(String message) {
        message.toUpperCase();
        message.trim();
        return message == "ACK";
    }
    
    // Auto-ping functionality
    void enableAutoPing(unsigned long intervalMs = 10000) {
        autoPingEnabled = true;
        pingInterval = intervalMs;
        lastPingTime = millis();
    }
    
    void disableAutoPing() {
        autoPingEnabled = false;
    }
    
    void handleAutoPing() {
        if (autoPingEnabled && (millis() - lastPingTime >= pingInterval)) {
            sendPing();
            lastPingTime = millis();
        }
    }
    
    // Command parsing helper
    String parseCommand(String message) {
        message.toUpperCase();
        message.trim();
        
        if (isPing(message)) {
            sendPong();
            return "PING_RECEIVED";
        }
        else if (isPong(message)) {
            return "PONG_RECEIVED";
        }
        else if (isAck(message)) {
            return "ACK_RECEIVED";
        }
        else if (message.startsWith("DISPENSE:")) {
            return "DISPENSE:" + message.substring(9);
        }
        else {
            send("ERROR:UNKNOWN_COMMAND");
            return "ERROR:UNKNOWN_COMMAND";
        }
    }
};

#endif