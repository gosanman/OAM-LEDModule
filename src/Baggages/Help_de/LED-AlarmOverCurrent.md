### Überstrom überwachen

Meldet über das Objekt **Überstrom**, wenn der Gesamtstrom den **Grenzwert Überstrom** übersteigt.

Zusätzlich ist im Gerät ein **Hardware-Überstromschutz** fest eingestellt. Er ist nicht abschaltbar und löst bei 6 A aus (mit Messwiderstand 0,010 Ohm). Er schaltet innerhalb von etwa 0,25 Sekunden **alle Ausgänge ab** und meldet dies über das Diagnoseobjekt. Die Abschaltung bleibt bestehen, bis frühestens 3 Sekunden danach ein Telegramm an einen Kanal empfangen wird. Dann werden die Ausgänge wieder eingeschaltet und der Strom nach 0,4 Sekunden erneut geprüft. Liegt der Überstrom weiter an, wird erneut abgeschaltet.

Ein Grenzwert über 6 A ist daher nicht wirksam.

