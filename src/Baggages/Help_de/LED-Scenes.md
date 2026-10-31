### Szenen

Jeder Kanal kann auf bis zu fünf Szenen (A bis E) reagieren. Für jede Szene wird die **Szenennummer** (1 bis 64) und die **Aktion** festgelegt. Empfängt das Szenenobjekt eine passende Szenennummer, führt der Kanal die Aktion mit der Ein-/Ausschaltgeschwindigkeit aus.

Aktionen:

* **nicht aktiv** – die Szene wird ignoriert
* **Einschaltwert (Tag/Nacht)** – wie Einschalten über das Schaltobjekt
* **Helligkeitswert** (EK, TW) – auf die eingetragene Helligkeit in Prozent
* **Farbwert** (RGB) – auf die eingetragene Farbe
* **Farbtemperatur** (TW) – nur die Farbtemperatur ändern, die Helligkeit bleibt
* **Helligkeit und Temperatur** (TW) – Helligkeit und Farbtemperatur setzen
* **ausschalten** – wie Ausschalten über das Schaltobjekt

Das Speichern (Lernen) von Szenen über den Bus wird nicht unterstützt. Ein Lerntelegramm ruft die Szene stattdessen auf.

