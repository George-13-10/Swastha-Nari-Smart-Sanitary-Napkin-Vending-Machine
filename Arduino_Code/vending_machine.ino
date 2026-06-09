#include <SPI.h>
#include <MFRC522.h>

// RFID Pins
#define SS_PIN 10
#define RST_PIN 9

MFRC522 mfrc522(SS_PIN, RST_PIN);

// Motor Driver Pins
const int enA = 6;
const int in1 = 5;
const int in2 = 4;

// Authorized RFID UID
String authorizedUID = "90 89 C6 32";

void setup()
{
    Serial.begin(115200);

    SPI.begin();
    mfrc522.PCD_Init();

    pinMode(enA, OUTPUT);
    pinMode(in1, OUTPUT);
    pinMode(in2, OUTPUT);

    stop_motor();

    Serial.println("Sanitary Napkin Vending Machine Ready");
}

void loop()
{
    // ---------------- RFID SECTION ----------------

    if (mfrc522.PICC_IsNewCardPresent() &&
        mfrc522.PICC_ReadCardSerial())
    {
        String content = "";

        for (byte i = 0; i < mfrc522.uid.size; i++)
        {
            if (mfrc522.uid.uidByte[i] < 0x10)
                content += " 0";
            else
                content += " ";

            content += String(mfrc522.uid.uidByte[i], HEX);
        }

        content.toUpperCase();

        Serial.print("Card UID:");
        Serial.println(content);

        if (content.substring(1) == authorizedUID)
        {
            Serial.println("Authorized RFID Card");

            dispense_napkin();
        }
        else
        {
            Serial.println("Unauthorized RFID Card");
            delay(1000);
        }

        mfrc522.PICC_HaltA();
    }

    // ---------------- UPI SECTION ----------------

    if (Serial.available())
    {
        String data = Serial.readStringUntil('\n');
        data.trim();

        if (data == "1")
        {
            Serial.println("UPI Payment Success");

            dispense_napkin();
        }
    }
}

void dispense_napkin()
{
    turn_motor();

    delay(4000);   // Dispense duration

    stop_motor();

    Serial.println("Napkin Dispensed");
}

void turn_motor()
{
    analogWrite(enA, 120);

    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
}

void stop_motor()
{
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
}
