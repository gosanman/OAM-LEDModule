### Dimmkurve

Die Dimmkurve legt fest, wie der Helligkeitswert in die Ansteuerung des Ports umgerechnet wird:

* **A - Linear** – Wert und Ausgang verhalten sich proportional
* **B - 2,8** – Gamma-Kurve 2,8
* **C - 3,8** – Gamma-Kurve 3,8
* **D - CIE** – nach CIE-Helligkeitsempfinden
* **E - DALI** – logarithmisch wie bei DALI

Das Auge empfindet Helligkeit nicht linear. Für Einzelkanäle empfehlen sich daher die Kurven B bis E, damit das Dimmen gleichmäßig wirkt. Für Tunable White und RGB sollte die Kurve auf **A - Linear** bleiben, da sonst Farbtemperatur bzw. Farbe beim Dimmen verfälscht werden; RGB hat dafür eine eigene Gamma-Korrektur.

Bei den Kurven B und C bleibt der Ausgang bei sehr kleinen Werten (unter etwa 4 % bzw. 9 %) dunkel, obwohl der Kanal als eingeschaltet gemeldet wird.


