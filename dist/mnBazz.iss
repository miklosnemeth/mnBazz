; mnBazz Windows installer. Build the Release artefacts first, then:
;   & "$env:LOCALAPPDATA\Programs\Inno Setup 6\ISCC.exe" dist\mnBazz.iss

#define MyAppName "mnBazz"
#define MyAppVersion "1.0.1"
#define MyAppPublisher "MN"
#define MyAppURL "https://miklosnemeth.github.io/mnBazz/"
#define Root ".."
#define Artefacts Root + "\build\mnBazz_artefacts\Release"

[Setup]
AppId={{7E4B1C2A-9F30-4C6E-8B11-0A6D5E2F91C4}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppVerName={#MyAppName} {#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
VersionInfoVersion={#MyAppVersion}
DefaultDirName={autopf}\{#MyAppName}
DisableProgramGroupPage=yes
DisableDirPage=yes
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible
PrivilegesRequired=admin
OutputDir=.
OutputBaseFilename=mnBazz-{#MyAppVersion}-win64-setup
Compression=lzma2
SolidCompression=yes
WizardStyle=modern
SetupLogging=yes
UninstallDisplayName={#MyAppName} {#MyAppVersion}

[Languages]
Name: "hungarian"; MessagesFile: "compiler:Languages\Hungarian.isl"
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "standalone"; Description: "Standalone alkalmazás és Start menü parancsikon"; GroupDescription: "További elemek:"; Flags: unchecked

[Files]
Source: "{#Artefacts}\VST3\mnBazz.vst3\*"; DestDir: "{commoncf64}\VST3\mnBazz.vst3"; Flags: recursesubdirs createallsubdirs ignoreversion
Source: "{#Artefacts}\Standalone\mnBazz.exe"; DestDir: "{app}"; Tasks: standalone; Flags: ignoreversion
Source: "INSTALL.txt"; DestDir: "{app}"; Tasks: standalone; Flags: ignoreversion isreadme

[Icons]
Name: "{autoprograms}\{#MyAppName}"; Filename: "{app}\mnBazz.exe"; Comment: "mnBazz standalone"; Tasks: standalone

[UninstallDelete]
Type: filesandordirs; Name: "{commoncf64}\VST3\mnBazz.vst3"
