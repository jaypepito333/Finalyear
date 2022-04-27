#include <SPI.h>
#include <MFRC522.h>
#include <ArduinoJson.h>
#include <SoftwareSerial.h>

#define SS_PIN 10
#define RST_PIN 9
#define ACCESS_DELAY 2000
#define DENIED_DELAY 1000
SoftwareSerial sim800L(7, 6);
SoftwareSerial ArduinoUno(3, 2);
SoftwareSerial espSerial(5, 4);
int data;
const String num = "+254719189576";
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

void setup()
{
  Serial.begin(9600);   // Initiate a serial communication
  SPI.begin();          // Initiate  SPI bus
  ArduinoUno.begin(4800);
  espSerial.begin(4800);
  mfrc522.PCD_Init();   // Initiate MFRC522
  sim800L.begin(9600);
  Serial.println("Initializing...");
  //Once the handshake test is successful, it will back to OK
  sim800L.println("AT");
  delay(1000);
  sim800L.println("AT+CMGF=1");
  delay(1000);
  Serial.println("Put your card to the reader...");
  Serial.println();

}
void loop()
{

  updateSerial();
  // Look for new ca
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent())
  {
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial())
  {
    return;
  }
  //Show UID on serial monitor
  Serial.print("Tagging UID tag :");
  String content = "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
    //Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    //content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.print("Message : ");
  content.toUpperCase();
  // Create JSON Object to pass the string to node MCU using content.substring(1)
  String val = content.substring(1);

  StaticJsonDocument<1000> doc;
  doc["cardID"] = val;
  //Send data to NodeMCU
  serializeJson(doc, ArduinoUno);
  delay(500);
}
void updateSerial()
{
  delay(500);
  while (Serial.available())
  {
    sim800L.write(Serial.read());//Forward what Serial received to Software Serial Port
  }
  while (sim800L.available())
  {
    Serial.write(sim800L.read());//Forward what Software Serial received to Serial Port
  }
}
void receiveNo() {
  while (espSerial.available() > 0) {
    StaticJsonDocument<1000> doc;
    DeserializationError error = deserializeJson(doc, espSerial);

    // Test parsing
    while (error) {
      Serial.println("Invalid JSON Object");
      delay(500);
      DeserializationError error = deserializeJson(doc, espSerial);
      return;
    }
    String card = doc["Number"];
    Serial.print("Number received is: ");
    Serial.println(card);

    delay(5000);
  }
}
void send_sms()
{
  Serial.println("sending sms....");
  delay(50);
  sim800L.print("AT+CMGF=1\r");
  delay(1000);
  sim800L.print("AT+CMGS=\"" + num + "\"\r");
  delay(1000);
  sim800L.print("Your Bag is in CheckIn 2");
  delay(100);
  sim800L.write(0x1A); //ascii code for ctrl-26 //Serial2.println((char)26); //ascii code for ctrl-26
  delay(5000);
}
