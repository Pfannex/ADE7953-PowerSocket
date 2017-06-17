#changeLOG
Stand: 05.03.2017 / 16:43h
letzte Änderung: Pf@nne

- **Hardware**
  - 
  
- **Firmware**
  - Topic /getWave hinzugefügt, V_INST, IA_INST und IB_INST werden nacheinader published
  - SoftRegister Period und SampleRate zur Steuerung der INST-Werte hinzugefügt
  - INST-Routinen überarbeitet, 1000 Samples mit 7kHz möglich
  - INST mit max 4kHz, Samples auf max. 500 begrenzt
  - INST nach dem publish der INST-Values erfolgt ein INSTready publish zum Refresh des SVG-Plots
  - Energy /resetEnergy mit Vorgabewert
  - SoftRegister PERIODGAIN hinzugefügt (223.75 + (read(PERIODGAIN)/1000)) obwohl die Korrektur Datasheet 223kHz -> 223.75kHz (3.58 MHz/16= 223.750 kHz clock) schon reichen würde
  - Ansteuerung des Relais über MQTT
  - MQTT LastWill, QoS und retain hinzugefügt
  - getState liefert den aktuellen Status des RelaisOutputs
  - MQTT subcribe bereinigt, kein FeedbackLoop mehr
  - getWave wird erst ab 1% Spannungsmesswert angestoßen (endlos while, wait for ZeroCross)
  - NTP.timeUpdate vorläufig entfernt

- **FHEM**
  - dummys für INST-Werte ergänzt
  - Pfanne_fhem.cfg abgelegt
  - INST funktionalitäten erweitert
  - INST-Samples in Frames möglich [tt,vv;tt,vv;-tt,-vv]
  - Ansteuerung des Relais mit Rückmeldestruktur
  - ESP8266 StatusWatcher hinzugefügt
  - Nach dem FHEM-Start wir der aktuelle RelaisState automatisch geholt
  
