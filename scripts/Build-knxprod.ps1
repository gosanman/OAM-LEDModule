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

Write-Host ""
$knxprod = Get-ChildItem -Path $projectDir/kaenx/*.knxprod
$knxprodname = $knxprod[0].Name
Write-Host "Die KNX-Produktdatei $knxprodname wurde erfolgreich erstellt."

# KO-Definitionen in der generierten Datei anpassen
# BASE_Ko und KoBASE_ Definitionen anpassen
$generatedFile = "$projectDir/kaenx/knxprod.h"
Copy-Item -Path $generatedFile -Destination $projectDir/kaenx/knxprod_org.h_ -Force
Write-Host "Sicherungskopie von knxprod.h erstellt: knxprod_org.h_"
$content = Get-Content $generatedFile -Raw

# Regex pattern für BASE_Ko und KoBASE_ Definitionen
$pattern = '(?m)^(//!<[^\n]+\n)#define\s+(BASE_Ko\w+)\s+(\d+)\s*\n#define\s+(Ko\w+)\s+knx\.getGroupObject\([^)]+\)'
$replacement = {
    param($match)
    $comment = $match.Groups[1].Value
    $baseName = $match.Groups[2].Value
    $number = $match.Groups[3].Value
    $koName = $match.Groups[4].Value
    
    "${comment}#define $baseName $number + BASE_Share_KoOffset`n#define $koName knx.getGroupObject($baseName)"
}
$newContent = [regex]::Replace($content, $pattern, $replacement)
$newContent | Set-Content $generatedFile -NoNewline

# Verschiebe die angepasste Datei nach include
$targetPath = Join-Path -Path $projectDir -ChildPath "include\knxprod.h"
if (Test-Path $targetPath) {
    Remove-Item $targetPath -Force
}
Move-Item $generatedFile $targetPath
Write-Host "Die knxprod.h wurde erstellt und nach include verschoben."