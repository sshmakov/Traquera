  ; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{A55FF037-34CA-4879-A4C3-14D36230A800}
AppName={code:GetAppName}
AppVersion=2.7
;AppVerName=TrackTasks 2.0
AppPublisher={code:GetAppPublisher}
DefaultDirName={pf}\{code:GetAppDir}
DefaultGroupName={code:GetAppGroup}
AllowNoIcons=yes
;OutputDir=C:\Share\Distrib\TrackTasks
OutputDir=.
OutputBaseFilename={#OutputBaseFilename}
;setupTT2.7
Compression=lzma
SolidCompression=yes

[Languages]
Name: "russian"; MessagesFile: "compiler:Languages\Russian.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked
Name: "quicklaunchicon"; Description: "{cm:CreateQuickLaunchIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked; OnlyBelowVersion: 0,6.1

[Files]
Source: "..\..\build\traquera-main-Desktop-Release\client\release\traquera.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "redistribute\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "..\client\data\*"; DestDir: "{userappdata}\{code:GetAppName}"; Flags: ignoreversion recursesubdirs createallsubdirs onlyifdoesntexist uninsneveruninstall
; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[Icons]
Name: "{group}\{code:GetAppName}"; Filename: "{app}\tracks2.exe"
Name: "{group}\{cm:UninstallProgram,{code:GetAppName}}"; Filename: "{uninstallexe}"
Name: "{commondesktop}\{code:GetAppName}"; Filename: "{app}\tracks2.exe"; Tasks: desktopicon
Name: "{userappdata}\Microsoft\Internet Explorer\Quick Launch\{code:GetAppName}"; Filename: "{app}\tracks2.exe"; Tasks: quicklaunchicon

[Registry]
Root: HKCU; Subkey: "{code:GetRegPath}"; Flags: uninsdeletekeyifempty
Root: HKCU; Subkey: "{code:GetRegPath}"; ValueType: string; ValueName: "UserDBType"; ValueData: "QSQLITE"; Flags: createvalueifdoesntexist
Root: HKCU; Subkey: "{code:GetRegPath}"; ValueType: string; ValueName: "UserDBPath"; ValueData: "{userappdata}\{code:GetAppName}\user.db"; Flags: createvalueifdoesntexist
Root: HKCU; Subkey: "{code:GetRegPath}"; ValueType: string; ValueName: "UserDBUser"; ValueData: ""; Flags: createvalueifdoesntexist
Root: HKCU; Subkey: "{code:GetRegPath}"; ValueType: string; ValueName: "UserDBPassword"; ValueData: ""; Flags: createvalueifdoesntexist

[Run]
Filename: "{app}\tracks2.exe"; Description: "{cm:LaunchProgram,TrackTasks}"; Flags: nowait postinstall skipifsilent

[Code]
const AppName = 'TraQuera';
const AppPublisher = 'Sergey Shmakov';

function GetAppName(Param: String): String;
begin
  Result := AppName;
end;

function GetAppPublisher(Param: String): String;
begin
  Result := AppPublisher;
end;

function GetAppDir(Param: String): String;
begin
  Result := { GetAppPublisher(Param)+'\'+} GetAppName(Param);
end;

function GetAppGroup(Param: String): String;
begin
  Result := GetAppName(Param);
end;

function GetRegPath(Param: String): String;
begin
  //Result := ExpandConstant('Software\{code:GetAppPublisher}\{code:GetAppName}');
  Result := ExpandConstant('Software\R-Style Softlab\TrackTasks');
end;


