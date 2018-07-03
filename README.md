# **OmniESP**&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<img src="https://github.com/Pfannex/OmniESP/wiki/_pics/Wemos_D1_mini.jpg" width="20%"/>
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

[OmniESP WiKi](https://github.com/Pfannex/OmniESP/wiki)
