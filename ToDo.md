#ToDo

- **Hardware**

- **Firmware**
  - alle Variablen in die customDevice/config.json aufnehmen
  - Clock: Zeitzone konfigurierbar machen
  - FTP-Server: falscher User oder falsches Passwort crasht ESP

- **OTA**
  - mit JQuery Mobile 1.3.2 kann /data auf unter 192kB gebracht werden,
    mit -Os kann auch die Firmware auf 407kB gebracht werden. Damit würde
    OTA im Layout 1MB mit 192kB SPIFFS funktionieren. Jedoch müsste dazu das
    Webinterface angepasst werden, da sich die ältere Version von JQuery Mobile
    etwas anders verhält/anders aussieht.

- **API**
  - erstellen der Funktionsübersicht (PF) -> doxygen

- **UI**
  - API Call Abfrage rootString sporadisch Absturz ggf. json zu lang
  - Abfrage ob AP UND STA auf OFF gesetzt werden!
  - Zugriff auf Websocket nur nach Authentisierung
  - nur SHA1 Hash des Passworts übertragen
  - FTP konfigurierbar

- **Devices**
  - SimpleSwitch
  - DEMOS
    - DEMO_GPIO
    - DEMO_I2C_OW

- **Modules**
  - GPIO
  - I2C
    - Sensoren in eigene Klassen
  - LCD
  - oneWire
  - QRE1113
  - WS2812
  - MCP23017
    - IRQ not working
