#define MyAppName "VRAgent"
#define MyAppVersion "1.0.0"
#define MyAppPublisher "HXDF Cop Ltd."
#define MyAppURL "http://"
#define MyAppId="4475443D-382C-4ACC-88FC-576FC57BE338"

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{{#MyAppId}}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppVerName={#MyAppName} {#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={pf}\VRSystem\VRAgent
DisableDirPage=yes
DefaultGroupName=VRSystem\VRAgent
DisableProgramGroupPage=yes
OutputDir=.\VRClientAgentInstall
OutputBaseFilename={#MyAppName} {#MyAppVersion}
Compression=lzma
SolidCompression=yes

[Languages]
Name: "english"; MessagesFile: "compiler:default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
Source: ".\NetEventService.dll"; DestDir: "{app}"; CopyMode: alwaysoverwrite; Flags: ignoreversion  
Source: ".\VRClientAgent.exe"; DestDir: "{app}"; CopyMode: alwaysoverwrite;Flags: ignoreversion
Source: ".\VRAgentConfig.ini"; DestDir: "{app}"; Flags: onlyifdoesntexist uninsneveruninstall
//Source: ".\InstallUtil.exe"; DestDir: "{app}"; Flags: ignoreversion


[Icons]
Name: "{group}\VRClientAgent"; Filename: "{app}\VRClientAgent.exe"
Name: "{commondesktop}\VRClientAgent"; Filename: "{app}\VRClientAgent.exe"; Tasks: desktopicon

;===============Please do not remove it, they are used to add regedit item====
[Registry]
Root: HKLM; Subkey: "SOFTWARE\VRSystem\VRAgent"; ValueType: string; ValueName:"VRAgentConfig"; ValueData:"{app}\VRAgentConfig.ini"; Flags:createvalueifdoesntexist;

[Run]
Filename: "sc.exe"; Parameters: "create VRClientAgent binPath= ""{app}\VRClientAgent.exe""";Flags:runhidden; 
//Filename: "{app}\InstallUtil.exe"; Parameters: """{app}\VRClientAgent.exe""";Flags:runhidden; 
Filename: "sc.exe"; Parameters: "config VRClientAgent start= AUTO";Flags:runhidden;
Filename: "sc.exe"; Parameters: "start VRClientAgent";Flags:runhidden;  

[UninstallRun]
;Filename: "sc.exe"; Parameters: "config VRClientAgent start= Manual";Flags:runhidden; 
Filename: "sc.exe"; Parameters: "stop VRClientAgent";Flags:runhidden;
Filename: "sc.exe"; Parameters: "delete VRClientAgent";Flags:runhidden; 
//Filename: "{app}\InstallUtil.exe /u"; Parameters: """{app}\VRClientAgent.exe""";Flags:runhidden; 

