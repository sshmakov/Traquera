  ; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!
#ifndef AppName
#define AppName "TraQuera"
#endif

#ifndef AppVer
#define AppVer "3.3"
#endif

#ifndef Publisher
#define Publisher ("Sergey Shmakov")
#endif

#ifndef OutputBaseFilename
#define OutputBaseFilename (AppName + "_setup")
#endif

#ifndef BuildPath
#define BuildPath "..\..\build\traquera-main-Desktop-Release"
#endif

#ifndef QTDIR
#define QTDIR = GetEnv("QTDIR")
#endif

#if QTDIR == ""
#define QTDIR "C:\Qt\4.8.4"
#endif


[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{A55FF037-34CA-4879-A4C3-14D36230A800}
AppName={#AppName}
AppVersion={#AppVer}
;AppVerName=TrackTasks 2.0
AppPublisher={#Publisher}
DefaultDirName={pf}\{#AppName}
DefaultGroupName={#AppName}
AllowNoIcons=yes
OutputDir=.
OutputBaseFilename={#OutputBaseFilename}
Compression=lzma
SolidCompression=yes

[Components]
Name: "main"; Description: "{cm:NameAndVersion,{#AppName},{#AppVer}}"; Types: full custom compact; Flags: fixed
Name: "plugins"; Description: "{cm:Plugins}"; Types: full custom
Name: "plugins/jira"; Description: "{cm:PluginFor,Atlassian JIRA}"; Types: full custom
Name: "plugins/tracker"; Description: "{cm:PluginFor,PVCS Tracker 7.x}"; Types: full custom
Name: "plugins/msplans"; Description: "{cm:PluginFor,Microsoft Project}"; Types: full custom

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked
Name: "quicklaunchicon"; Description: "{cm:CreateQuickLaunchIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked; OnlyBelowVersion: 0,6.1

[Files]
Source: "{#BuildPath}\client\release\traquera-protect.exe"; DestName: "traquera.exe"; DestDir: "{app}"; Components: main; Flags: ignoreversion
Source: "{#BuildPath}\lib\*.dll"; DestDir: "{app}"; Components: main; Flags: ignoreversion 
Source: "redistribute\*"; DestDir: "{app}"; Components: main; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#QTDIR}\plugins\imageformats\*"; DestDir: "{app}\imageformats"; Components: main; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#QTDIR}\plugins\sqldrivers\*"; DestDir: "{app}\sqldrivers"; Components: main; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "..\client\data\*"; DestDir: "{app}\data"; Components: main; Flags: ignoreversion recursesubdirs createallsubdirs 
; Jira
Source: "{#BuildPath}\plugins\jira\jira.dll"; DestDir: "{app}\plugins\jira"; Components: "plugins/jira"
; Tracker
Source: "{#BuildPath}\plugins\tracker\tracker.dll"; DestDir: "{app}\plugins\tracker"; Components: "plugins/tracker"
Source: "{#BuildPath}\plugins\tracker\plugin.ini"; DestDir: "{app}\plugins\tracker"; Components: "plugins/tracker"
Source: "{#BuildPath}\plugins\tracker\data\*"; DestDir: "{app}\plugins\tracker\data"; Components: "plugins/tracker"
; MSProject
Source: "{#BuildPath}\plugins\msplans\msplans.dll"; DestDir: "{app}\plugins\msplans"; Components: "plugins/msplans"

; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[Icons]
Name: "{group}\{#AppName}"; Filename: "{app}\traquera.exe"
Name: "{group}\{cm:UninstallProgram,{#AppName}}"; Filename: "{uninstallexe}"
Name: "{commondesktop}\{#AppName}"; Filename: "{app}\traquera.exe"; Tasks: desktopicon
Name: "{userappdata}\Microsoft\Internet Explorer\Quick Launch\{#AppName}"; Filename: "{app}\traquera.exe"; Tasks: quicklaunchicon

[Registry]
Root: HKCU; Subkey: "{code:GetRegPath}"; Flags: uninsdeletekeyifempty
Root: HKCU; Subkey: "{code:GetRegPath}"; ValueType: string; ValueName: "UserDBType"; ValueData: "QSQLITE"; Flags: createvalueifdoesntexist
Root: HKCU; Subkey: "{code:GetRegPath}"; ValueType: string; ValueName: "UserDBPath"; ValueData: "{userappdata}\{#AppName}\user.db"; Flags: createvalueifdoesntexist
Root: HKCU; Subkey: "{code:GetRegPath}"; ValueType: string; ValueName: "UserDBUser"; ValueData: ""; Flags: createvalueifdoesntexist
Root: HKCU; Subkey: "{code:GetRegPath}"; ValueType: string; ValueName: "UserDBPassword"; ValueData: ""; Flags: createvalueifdoesntexist

[Ini]
Filename: "{app}\plugins\tracker\plugin.ini"; Section: "Init"; Key: "LibPath"; String: "{code:GetTrackerPath}"; Components: "plugins/tracker"

[Run]
Filename: "{app}\traquera.exe"; Description: "{cm:LaunchProgram,Traquera}"; Flags: nowait postinstall skipifsilent

[Languages]
Name: "ru"; MessagesFile: "compiler:Languages\Russian.isl"
Name: "en"; MessagesFile: "compiler:Default.isl"

[CustomMessages]
ru.SelectTrkFolder=������� ����� PVCS Tracker
ru.Plugins=����������
ru.PluginFor=���������� ��� %1

en.SelectTrkFolder=Select folder with PVCS Tracker
en.Plugins=Plugins
en.PluginFor=Plugin for %1

[Code]
function cm(Message: String): String;
begin
  Result := ExpandConstant('{cm:' + Message + '}');
end;


function GetRegPath(Param: String): String;
begin
  Result := ExpandConstant('Software\R-Style Softlab\TrackTasks');
end;


function GetTrackerPath(Param: String): String;
var
  Path : String;
begin
  Result := ExpandConstant('{pf32}\PVCS\Tracker\nt');
  Path := FileSearch('trktooln.dll', Result);
  if Path <> '' then
    Exit;
  Path := Result;
  if BrowseForFolder(cm('SelectTrkFolder'), Path, false) then
    Result := Path;
  
end;

(*
var
  Page: TInputDirWizardPage;
  DataDir: String;


procedure InitializeWizard();
begin
...

// Create the page
Page := CreateInputDirPage(wpWelcome,
  'Select Personal Data Location', 'Where should personal data files be stored?',
  'Personal data files will be stored in the following folder.'#13#10#13#10 +
  'To continue, click Next. If you would like to select a different folder, click Browse.',
  False, 'New Folder');

// Add item (with an empty caption)
Page.Add('');

// Set initial value (optional)
Page.Values[0] := ExpandConstant('{userappdata}\My Company\My Program');

...

// Read value into variable
DataDir := Page.Values[0];
  
end;
*)