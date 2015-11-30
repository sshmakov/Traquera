CREATE TABLE "connections"(id integer primary key autoincrement,
    project varchar(255), dbClass varchar(255), dbType varchar(255), dbServer varchar(255), user varchar(255), password varchar(255), dbOsUser boolean, dbmsUser varchar(255), dbmsPass varchar (255), autoLogin boolean);
CREATE TABLE "folders"(id integer primary key autoincrement, parentId integer, title char(255), records clob, project char(255));
CREATE TABLE user(id integer primary key autoincrement, name char(255));
CREATE INDEX folders_by_project on folders (project);
