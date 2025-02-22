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
$filePath = Join-Path -Path $projectDir -ChildPath "kaenx" | Join-Path -ChildPath $files[0].Name
~/bin/Kaenx.Creator.Console publish $filePath
cp $projectDir/kaenx/knxprod.h $projectDir/include