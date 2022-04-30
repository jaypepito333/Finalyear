# RFID-tags-Arduino
To Start With:
# NodeMCU
 Steps:
    - Go to File -> Preferences ->Additional Boards Manager the Paste
        https://arduino.esp8266.com/stable/package_esp8266com_index.json
    - Go to Tools -> Manage Libraries -> Search mfrc522 by GithubCommunity only and Install it
    - Same in Manage Libraries -> Search ArduinoJson and install version 6.19.2
    - Check the *ssid and *password and enter your wifi or hostpot details.
    - start ngrok eg ./ngrok http 8000 -> port where laravel is running at.
    - Copy the URL generated at Forwording with eg http://ea26-197-156-190-211.ngrok.io.
    - Paste it in the code to replace the one that is There. eg *host.
 # Upload code
   - ** Go to Tools -> Board -> ESP8266 boards -> select NodeMcu1.0(ESP-12E Module).
   - ** In Tools -> Port -> select COM * without the name Arduino.
   
 # ARDUINO UNO CODE(GETRFID_ID_FROM ARDUINO)
  *UPLOAD CODE
      Go to Tools -> Board -> Arduino AVR boards -> select Arduino Uno.
      In Tools -> Port -> select COM * with the name Arduino.
 
