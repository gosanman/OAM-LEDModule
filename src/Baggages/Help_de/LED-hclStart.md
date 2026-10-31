### HCL anwenden

* **Nur beim Einschalten** – der Kanal schaltet mit den aktuellen HCL-Werten ein. Danach kann er frei gedimmt werden; die HCL-Werte werden erst beim nächsten Einschalten wieder übernommen.
* **Auch wenn eingeschaltet ist** – die HCL-Werte werden zusätzlich jede Minute auf den eingeschalteten Kanal übertragen. Manuelles Dimmen wird dabei bei der nächsten Aktualisierung überschrieben.

Ausgeschaltete Kanäle werden durch HCL nicht eingeschaltet. Außerhalb des Zeitfensters liefert der HCL-Kanal seine Minimalwerte. Solange noch keine HCL-Werte vorliegen (etwa in der ersten Minute nach einem Neustart oder ohne gültige Uhrzeit), schaltet der Kanal mit seinem normalen Einschaltwert ein.

