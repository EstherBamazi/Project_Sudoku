# SDL2 officiel pour CLion / MinGW 64 bits. Aucune installation systeme.
$ErrorActionPreference = "Stop"
$racineProjet = Split-Path $PSScriptRoot -Parent
$dossierTiers = Join-Path $racineProjet "third_party"
$archiveSDL = Join-Path $dossierTiers "SDL2-devel-2.32.10-mingw.zip"
New-Item -ItemType Directory -Force -Path $dossierTiers | Out-Null
Invoke-WebRequest -Uri "https://github.com/libsdl-org/SDL/releases/download/release-2.32.10/SDL2-devel-2.32.10-mingw.zip" -OutFile $archiveSDL
$empreinte = (Get-FileHash -LiteralPath $archiveSDL -Algorithm SHA256).Hash
if ($empreinte -ne "F15CFF5FCA62EC9381A016EF1D42A95C638CD72D2F226BA5781C76FE43DBD1AC") {
    throw "Archive SDL2 inattendue : empreinte SHA256 incorrecte."
}
Expand-Archive -LiteralPath $archiveSDL -DestinationPath $dossierTiers -Force
$configurationSDL = Join-Path $dossierTiers "SDL2-2.32.10/x86_64-w64-mingw32/lib/cmake/SDL2"
Write-Host "SDL2 prete. Dans les options CMake de CLion, ajouter :"
Write-Host ('-DSUDOKU_AVEC_SDL=ON -DSDL2_DIR="{0}"' -f $configurationSDL)
