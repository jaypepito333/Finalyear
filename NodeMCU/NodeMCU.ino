#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <MFRC522.h>
#include <ESP8266HTTPClient.h>

#define SS_PIN D4
#define RST_PIN D1


const char *ssid = "Robaranks"; //WIFI NAME OR HOTSPOT
const char *password = "1234567890"; //WIFI PASSWORD POR MOBILE HOTSPOT PASSWORD
const char* host = "http://4eec-197-156-190-154.ngrok.io"; //LOCAL IPv4 ADDRESS...ON CMD WINDOW TYPE ipconfig/all
const uint16_t port = 80; //PORT OF THE LOCAL SERVER
int rank = 4; //THE RANK WHOSE DATA YOU WANT TO FETCH
HTTPClient http;

//GSM Module RX pin to NodeMCU D3
//GSM Module TX pin to NodeMCU D4
#define rxPin D0
#define txPin D8
SoftwareSerial sim800L(rxPin, txPin);
SoftwareSerial uno(D2, D3);

MFRC522 mfrc522(SS_PIN, RST_PIN);

void setup() {
  Serial.begin(9600);
  uno.begin(4800);
  pinMode(D2, INPUT);
  pinMode(D3, OUTPUT);
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

  sim800L.begin(9600);
  Serial.println("Initializing...");
  //Once the handshake test is successful, it will back to OK
  sim800L.println("AT");
  delay(1000);
  sim800L.println("AT+CMGF=1");
  delay(1000);

}
void SendCardID(String cardID) {
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
    String dest = "Terminal 1";
    String phone = "0718189576";
    //Location stored in JSON format
    DynamicJsonDocument doc(200);
    doc["cardID"] = cardID;
    doc["destination"] = dest;
    doc["phone"] = phone;
    serializeJson(doc, json);
    Serial.println(json);
    //Replace the ngrok url here
    http.begin(client, "http://4eec-197-156-190-154.ngrok.io/api/postdata");     //Specify request destination
    http.addHeader("Content-Type", "application/json");
    int httpCode = http.POST(json);   //Send the request
    String payload = http.getString();                                        //Get the response payload

    Serial.println(httpCode);   //Print HTTP  code
    Serial.println(payload);    //Print request response payload
    if (httpCode == 201) {
      Serial.println("Data Saved");
    }

    http.end();
    delay(10000);

  }
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
    Serial.println(json);
    //Replace the ngrok url here
    http.begin(client, "http://4eec-197-156-190-154.ngrok.io/api/product/search/" + String(cardID));   //Specify request destination
    http.addHeader("Content-Type", "application/json");
    int httpCode = http.GET();   //Send the request
    String payload = http.getString();                                        //Get the response payload

    Serial.println(httpCode);   //Print HTTP  code
    Serial.println(payload);    //Print request response payload
    if (payload != "") {

      String Number = payload.substring(1, 10);
      String no = "+254" + Number;
      Serial.println(no);
      send_sms(no);
      String json;
      String dest = "CheckIn 2";
      //Location stored in JSON format
      DynamicJsonDocument doc(200);
      doc["bag_tagID"] = cardID;
      doc["Terminal_at"] = dest;
      serializeJson(doc, json);
      Serial.println(json);
      //Replace the ngrok url here
      http.begin(client, "http://4eec-197-156-190-154.ngrok.io/api/status");     //Specify request destination
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
    String dest = "Bag At CheckIn 2";
    String phone;

    //Location stored in JSON format
    DynamicJsonDocument doc(200);

    doc["cardID"] = cardID;

    String card = cardID;
    serializeJson(doc, json);
    Serial.println(json);
    //Replace the ngrok url here
    http.begin(client, "http://4eec-197-156-190-154.ngrok.io/api/product/search/" + String(cardID));   //Specify request destination
    http.addHeader("Content-Type", "application/json");
    int httpCode = http.GET();   //Send the request
    String payload = http.getString();                                        //Get the response payload

    Serial.println(httpCode);   //Print HTTP  code
    Serial.println(payload);    //Print request response payload
    if (payload == "success") {
      String json;
      String dest = "CheckIn 2";
      //Location stored in JSON format
      DynamicJsonDocument doc(200);
      doc["bag_tagID"] = cardID;
      doc["Terminal_at"] = dest;
      serializeJson(doc, json);
      Serial.println(json);
      //Replace the ngrok url here
      http.begin(client, "http://4eec-197-156-190-154.ngrok.io/api/status");     //Specify request destination
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

  while (sim800L.available()) {
    Serial.println(sim800L.readString());
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

}
void send_sms(String Number)
{
    Serial.println("sending sms....");
    delay(50);
    sim800L.print("AT+CMGF=1\r");
    delay(1000);
    sim800L.print("AT+CMGS=\""+Number+"\"\r");
    delay(1000);
    sim800L.print("Your Bag is in CheckIn 2");
    delay(100);
    sim800L.write(0x1A); //ascii code for ctrl-26 //Serial2.println((char)26); //ascii code for ctrl-26
    delay(5000);
}
