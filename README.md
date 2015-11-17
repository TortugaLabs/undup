# undup

Track duplicate files and merge them as hardlinks

* * *

index file:




*.udb

<inode> 0
	ino_t	inode;
	mode_t	mode;
	uid_t	uid;
	gid_t	gid;
	off_t		size;
	time_t	mtime;

<inode> p <cnt>
rpath

<inode> h
digest

<data>



 scan-fs
 - open db
 
 while count dirs
   if (dev 
