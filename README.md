# windows-kernel-file-protector
## TL;DR
Protect a file from being deleted or corrupted using windows kernel file system mini-filter driver.

Tested on Windows 10 1909 x64.

## How it works
Using file system mini-filter driver we intercept every PreCreateFile operation, PreWriteFile operation and PreSetFileInformation.

Delete protector:

If create file was called to one of our protected files with FILE_DELETE_ON_CLOSE the driver will fail it with STATUS_ACCESS_DENIED.

If set file information was called to one of our protected files with FileDispositionInformation and DeleteFile is true the driver will fail it with STATUS_ACCESS_DENIED.

Rename protector:

If set file information was called to one of our protected files with FileRenameInformation the driver will fail it with STATUS_ACCESS_DENIED.

Write protector:

If write file was called to one of our protected files the driver will fail it with STATUS_ACCESS_DENIED.

## Usage

Open cmd as Administrator:

rundll32 syssetup.dll,SetupInfObjectInstallAction DefaultInstall 128 {path to the driver inf file}

sc start FileProtector

Now any file that you want to be unchangeable and protected (like backups) add to its name "_protected".

For example: I got file backup.txt which contains an important backup.
In order to protect it, I will change its name to backup_protected.txt and now, the file is protected by our driver.

DONE!!!