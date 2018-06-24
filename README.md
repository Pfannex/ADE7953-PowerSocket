# **OmniESP** 
_Universelles Framework für ESP-Devices mit MQTT-Anbindung_
***


In diesem Framework stehen alle Grundfunktionen zum Erstellen eigenener Devices zur Verfügung.
Unterstütz werden bislang folgende Funktionen:

- ESP8266 WiFi-Funktionalitäten
  - Event getriggerte StateMachine zum Handling von STATION- und ACCESS-POINT-Mode
  - WiFi-Network-Scan
  - MQTT-Unterstützung
  - FTP-Server für SPIFFS Zugriff
- Webinterface (UI) 
  - Konfiguration
  - Authentifizierung
  - readings
  - logs
  - frei konfigurierbares Dashboard (auch zur Laufzeit!)
    - Button, Textbox, Checkbox, Radiobutton, Grid, ...
  - OTA Fileupload mit versioniertem Tarball
    - *.bin kompilierte Firmware
    - Webinterface
    - Konfigurationsfiles
  - ~OTA AUTO-Update über Web-Space~
- ApplicationInterface (API)
  - Topic gestützte set/get/event-Struktur
  - Voller zugriff auf alle internen Funktionen über:
    - MQTT-Topics
    - HTML-Set/Get 
- ~_LAN-Anbindung über WizNet W5x00_~
- SPIFFS Unterstüzung
  - vollständiges Handling der Configurations-Files
  - Unterstützung des JSON-File-Formates
- ESP-Tools für Logging-/ Debugging-Funktionalitäten
- Clocksystem mit NTP-Synchronisierung
- vollständig Modularer Aufbau des Core
  - Model
    - GPIO
    - oneWire
    - etc.
  - View
    - MQTT
    - WebServer
  - Controller
    - API-handler
    - Topic-Handler
- Device-Plug-In-System zum einfachen handling unterschiedlicher Devices  

[CodeStruktur](https://omniesp.4nv.eu/index.html)


***

[HOME](https://github.com/Pfannex/BasicTemplate/wiki)   

- **WiFi**
- **LAN**
- **UI**  
  - [Config](https://github.com/Pfannex/ADE7953-PowerSocket/wiki/UI_Config)  
  - [Readings](https://github.com/Pfannex/ADE7953-PowerSocket/wiki/UI_Readings)
- **API**  
  - [Übersicht](https://github.com/Pfannex/ADE7953-PowerSocket/wiki/API_%C3%9Cbersicht)

***

[ToDo](https://github.com/Pfannex/BasicTemplate/blob/master/ToDo.md)  
[changeLOG](https://github.com/Pfannex/BasicTemplate/blob/master/changeLOG.md)


