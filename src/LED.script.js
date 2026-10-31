// Funktionstest: schaltet einen Kanal direkt am Geraet ein oder aus.
// Firmware: LEDModule::processFunctionProperty (objectIndex 160, propertyId 6, Funktion 1)
function APP_buttonOnOffAction(device, online, progress, context) {
    var parChannelID = device.getParameterByName("APP_ChannelIDs");
    var parAction = device.getParameterByName("APP_OnOffAction");

    progress.setText("LED: Aktion wurde gestartet...");

    var data = [1]; // interne Funktions-ID
    data = data.concat([parChannelID.value]);
    data = data.concat([parAction.value]);

    online.connect();
    var resp = online.invokeFunctionProperty(160, 6, data);
    online.disconnect();

    if (!resp || resp.length < 1 || resp[0] != 0) {
        throw new Error("LED: Es ist ein unbekannter Fehler aufgetreten!");
    }
    progress.setText("LED: Aktion wurde ausgeführt.");
}
