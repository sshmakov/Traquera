create table folders_n(id integer primary key autoincrement, parentId integer, title char(255), records clob);
insert into folders_n(id, parentId, title, records)  select f.id, f.parentId, f.title, group_concat(recordId,',')    from folders as f    left join recordInFolder as rf on rf.folderId = f.id   group by f.id, f.parentId, f.title   order by f.id;
drop table folders;
alter table folders_n rename to folders;
drop table recordInFolder;
