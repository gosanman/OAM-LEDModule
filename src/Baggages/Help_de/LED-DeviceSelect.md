### Gerät

Hier wählst Du das Gerät aus, das bei Dir verbaut ist, mit seiner Hardware-Version:

* **LED-DK-6x24V V1.0** und **V1.2** – 6-Kanal-Dimmer für den Deckeneinbau
* **LED-DK-12x24V V1.0** und **V1.2** – 12-Kanal-Dimmer für den Deckeneinbau
* **LED-APP-6x24V V0.1** – Applikationsplatine (6 Kanäle) für das Reg1

Die Auswahl bestimmt, welche Betriebsmodi und Anschlusspläne angeboten werden und welche Einstellungen (z. B. Bedienfeld) sichtbar sind.

Das Gerät vergleicht die Auswahl beim Start mit der Hardware, für die seine Firmware gebaut ist. Passt sie nicht oder ist kein Gerät gewählt, legt es keine Kanäle an und schaltet keinen Ausgang, auch nicht über Funktionstaste, Bedienfeld, Konsole oder Funktionstest. Es meldet den Fehler im Log und über das Diagnoseobjekt (`ER HW <Firmware><><ETS>`), auf dem Bedienfeld („Geraet in ETS passt nicht zur Firmware“) und bei der LED-APP mit der Fehler-LED. Abhilfe: das richtige Gerät wählen und neu programmieren.

Beim Aktualisieren einer Applikation vor Version 0.6 ist noch kein Gerät gewählt. Die Einstellungen bleiben erhalten, das Gerät muss aber einmal gewählt werden.

Hinweis: Die Taste [ ▲ ] bzw. [ Prog ] aktiviert den Programmiermodus. Die Taste [ ••• ] bzw. [ Func1 ] löst die Aktionen aus, die Du unter **Einstellungen → Funktionstaste konfigurieren** festlegst.

