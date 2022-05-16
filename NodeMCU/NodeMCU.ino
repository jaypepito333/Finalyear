#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <MFRC522.h>
#include <ESP8266HTTPClient.h>
#include <Wire.h>
#define SS_PIN D4
#define RST_PIN D0


const char *ssid = "Robaranks"; //WIFI NAME OR HOTSPOT
const char *password = "1234567890"; //WIFI PASSWORD POR MOBILE HOTSPOT PASSWORD
const char* host = "http://334f-154-122-213-31.ngrok.io"; //LOCAL IPv4 ADDRESS...ON CMD WINDOW TYPE ipconfig/all
const uint16_t port = 80; //PORT OF THE LOCAL SERVER
HTTPClient http;

//GSM Module RX pin to NodeMCU D0
//GSM Module TX pin to NodeMCU D8
//#define rxPin D2
//#define txPin D3
SoftwareSerial sim800L(D2, D3);
SoftwareSerial uno(D1, D8);

MFRC522 mfrc522(SS_PIN, RST_PIN);

void setup() {
  Serial.begin(9600);
  uno.begin(4800);

  //  pinMode(D0, INPUT);
  //  pinMode(D8, OUTPUT);
  WiFi.mode(WIFI_OFF);
  delay(1000);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  SPI.begin();
  mfrc522.PCD_Init();
  //espS.begin(4800);
  sim800L.begin(9600);
  Serial.println("Initializing...");
  //Once the handshake test is successful, it will back to OK
  sim800L.println("AT");
  updateSerial();
  sim800L.println("AT+CMGF=1");
  updateSerial();

}

void GetData(String cardID) {
  WiFiClient client;
  //Connection to url failed
  if (!client.connect(host, port)) {
    Serial.println("connection failed");
    delay(5000);
    return;
  }
  //Connection successfully
  if (client.connected()) {

    String json;
    String dest = "Bag At CheckIn 2";
    String phone;

    //Location stored in JSON format
    DynamicJsonDocument doc(200);

    doc["cardID"] = cardID;
    doc["destination"] = dest;
    doc["phone"] = phone;

    String card = cardID;

    serializeJson(doc, json);
    //Replace the ngrok url here
    http.begin(client, "http://334f-154-122-213-31.ngrok.io/api/product/search/" + String(cardID));   //Specify request destination
    http.addHeader("Content-Type", "application/json");
    int httpCode = http.GET();   //Send the request
    String payload = http.getString();                                        //Get the response payload

    Serial.println(httpCode);   //Print HTTP  code
    Serial.println(payload);    //Print request response payload

    if (payload == "Creating Bag........") {
      return;
    }
    else {
      String Number = payload.substring(1, 10);
      String no = "+254" + Number;
      Serial.println(no);

      send_sms(no);
      delay(2000);
      String json;
      String dest = "Sort Area 1";
      //Location stored in JSON format
      DynamicJsonDocument doc(200);
      doc["bag_tagID"] = cardID;
      doc["Terminal_at"] = dest;
      serializeJson(doc, json);
      Serial.println(json);
      //Replace the ngrok url here
      http.begin(client, "http://334f-154-122-213-31.ngrok.io/api/status");     //Specify request destination
      http.addHeader("Content-Type", "application/json");
      int httpCode = http.POST(json);   //Send the request
      String payload = http.getString();                                        //Get the response payload

      Serial.println(httpCode);   //Print HTTP  code
    }

    http.end();
    delay(10000);

  }
}

void cardIDfromUno(String cardID) {
  WiFiClient client;
  //Connection to url failed
  if (!client.connect(host, port)) {
    Serial.println("connection failed");
    delay(5000);
    return;
  }
  //Connection successfully
  if (client.connected()) {

    String json;
    String dest = "Bag At Sort Area 2";
    String phone;

    //Location stored in JSON format
    DynamicJsonDocument doc(200);

    doc["cardID"] = cardID;

    String card = cardID;
    serializeJson(doc, json);
    Serial.println(json);
    //Replace the ngrok url here
    http.begin(client, "http://334f-154-122-213-31.ngrok.io/api/product/search/" + String(cardID));   //Specify request destination
    http.addHeader("Content-Type", "application/json");
    int httpCode = http.GET();   //Send the request
    String payload = http.getString();                                        //Get the response payload

    Serial.println(httpCode);   //Print HTTP  code
    Serial.println(payload);    //Print request response payload
    if (payload == "Creating Bag........") {
      return;
    }
    else {
      String Number = payload.substring(1, 10);
      String no = "+254" + Number;
      Serial.println(no);
      send_sms1(no);
      delay(2000);
      String json;
      String dest = "Sort Area 2";
      //Location stored in JSON format
      DynamicJsonDocument doc(200);
      doc["bag_tagID"] = cardID;
      doc["Terminal_at"] = dest;
      serializeJson(doc, json);
      Serial.println(json);
      //Replace the ngrok url here
      http.begin(client, "http://334f-154-122-213-31.ngrok.io/api/status");     //Specify request destination
      http.addHeader("Content-Type", "application/json");
      int httpCode = http.POST(json);   //Send the request
      String payload = http.getString();                                        //Get the response payload

      Serial.println(httpCode);   //Print HTTP  code
    }

    http.end();
    delay(10000);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  updateSerial();
  while (Serial.available())  {
    //String s = Serial.readString();
    //Serial.println(s);
    sim800L.println(Serial.readString());
  }
  while (uno.available() > 0) {
    StaticJsonDocument<1000> doc;
    DeserializationError error = deserializeJson(doc, uno);

    // Test parsing
    while (error) {
      Serial.println("Invalid JSON Object");
      delay(500);
      DeserializationError error = deserializeJson(doc, uno);
      return;
    }
    String card = doc["cardID"];
    Serial.print("CardID received is: ");
    Serial.println(card);
    cardIDfromUno(card);

    delay(5000);
  }
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
  Serial.print("UID tag :");
  String content = "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
    //Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    //content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  //SendCardID(content.substring(1));
  GetData(content.substring(1));
  Serial.println();
  delay(5000);
  String val = "Hello";
  StaticJsonDocument<1000> doc;
  doc["cardID"] = val;
  //Send data to NodeMCU
  serializeJson(doc, uno);
  delay(500);

}
void send_sms(String num)
{
  Serial.println("sending sms....");
  delay(50);
  sim800L.print("AT+CMGF=1\r");
  delay(1000);
  sim800L.print("AT+CMGS=\"" + num + "\"\r");
  delay(1000);
  sim800L.print("Your Bag is in Sort Area 1");
  delay(100);
  sim800L.write(0x1A); //ascii code for ctrl-26 //Serial2.println((char)26); //ascii code for ctrl-26
  delay(5000);
}
void send_sms1(String num)
{
  Serial.println("sending sms....");
  delay(50);
  sim800L.print("AT+CMGF=1\r");
  delay(1000);
  sim800L.print("AT+CMGS=\"" + num + "\"\r");
  delay(1000);
  sim800L.print("Your Bag is in Sort Area 2");
  delay(100);
  sim800L.write(0x1A); //ascii code for ctrl-26 //Serial2.println((char)26); //ascii code for ctrl-26
  delay(5000);
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
