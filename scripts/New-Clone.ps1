$repo = "OAM-LEDModule"
$oldDir = Get-Location

Write-Host "Klone das Repo $repo" -ForegroundColor Yellow

git clone https://github.com/gosanman/$repo

Write-Host "Führe das Skript 'Restore-Project.ps1' aus" -ForegroundColor Yellow

Set-Location $repo\
.\scripts\Restore-Project.ps1 -wait $False

Write-Host "Fertig" -ForegroundColor Yellow
Set-Location $oldDir
timeout /T 20