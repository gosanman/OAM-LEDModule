if (-Not (Test-Path -Path platformio.ini)) {
    Set-Location ..
}
if (-Not (Test-Path -Path platformio.ini)) {
    Write-Host "Skript aus falschem Pfad ausgeführt..."
    timeout /T 20
    exit 1
}
$projectDir = Get-Location
$files = Get-ChildItem -Path $projectDir/kaenx/*.ae-manu

if ($files.Count -eq 0) {
    Write-Host "Keine .ae-manu Dateien gefunden im Ordner kaenx."
    exit 1
}
Write-Host "Verfuegbare .ae-manu Dateien:"
for ($i = 0; $i -lt $files.Count; $i++) {
    Write-Host "$($i+1): $($files[$i].Name)"
}

$selection = Read-Host "Bitte waehlen eine Datei zur Verarbeitung aus (1-$($files.Count))"

if ($selection -match "^[1-9]\d*$") {
    $selectedIndex = [int]$selection - 1
    if ($selectedIndex -ge 0 -and $selectedIndex -lt $files.Count) {
        $selectedFile = $files[$selectedIndex]
        $filePath = Join-Path -Path $projectDir -ChildPath "kaenx" | Join-Path -ChildPath $selectedFile.Name
        & "$HOME/bin/Kaenx.Creator.Console" publish $filePath
    } else {
        Write-Host "Ungueltige Auswahl."
        exit 1
    }
} else {
    Write-Host "Ungueltige Eingabe."
    exit 1
}

Write-Host ""
$knxprod = Get-ChildItem -Path $projectDir/kaenx/*.knxprod
$knxprodname = $knxprod[0].Name
Write-Host "Die KNX-Produktdatei $knxprodname wurde erfolgreich erstellt."
$generatedFile = "$projectDir/kaenx/knxprod.h"
# Verschiebe die knxprod.h Datei nach include
$targetPath = Join-Path -Path $projectDir -ChildPath "include\knxprod.h"
if (Test-Path $targetPath) {
    Remove-Item $targetPath -Force
}
Move-Item $generatedFile $targetPath
Write-Host "Die knxprod.h wurde erstellt und nach include verschoben."
Write-Host ""