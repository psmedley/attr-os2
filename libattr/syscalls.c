/*
  Copyright (C) 2015  Dmitry V. Levin <ldv@altlinux.org>

  This program is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by
  the Free Software Foundation, either version 2.1 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * These dumb wrappers are for backwards compatibility only.
 * Actual syscall wrappers are long gone to libc.
 */

#include "config.h"

#include <sys/xattr.h>

#ifdef HAVE_VISIBILITY_ATTRIBUTE
# pragma GCC visibility push(default)
#endif

int libattr_setxattr(const char *path, const char *name,
		     void *value, size_t size, int flags)
{
	return setxattr(path, name, value, size, flags);
}

int libattr_lsetxattr(const char *path, const char *name,
		      void *value, size_t size, int flags)
{
	return lsetxattr(path, name, value, size, flags);
}

int libattr_fsetxattr(int filedes, const char *name,
		      void *value, size_t size, int flags)
{
	return fsetxattr(filedes, name, value, size, flags);
}

ssize_t libattr_getxattr(const char *path, const char *name,
			 void *value, size_t size)
{
	return getxattr(path, name, value, size);
}

ssize_t libattr_lgetxattr(const char *path, const char *name,
			  void *value, size_t size)
{
	return lgetxattr(path, name, value, size);
}

ssize_t libattr_fgetxattr(int filedes, const char *name,
			  void *value, size_t size)
{
	return fgetxattr(filedes, name, value, size);
}

ssize_t libattr_listxattr(const char *path, char *list, size_t size)
{
	return listxattr(path, list, size);
}

ssize_t libattr_llistxattr(const char *path, char *list, size_t size)
{
	return llistxattr(path, list, size);
}

ssize_t libattr_flistxattr(int filedes, char *list, size_t size)
{
	return flistxattr(filedes, list, size);
}

int libattr_removexattr(const char *path, const char *name)
{
	return removexattr(path, name);
}

int libattr_lremovexattr(const char *path, const char *name)
{
	return lremovexattr(path, name);
}

int libattr_fremovexattr(int filedes, const char *name)
{
	return fremovexattr(filedes, name);
}

#ifdef __OS2__
#define INCL_LONGLONG
#define INCL_DOS
#define INCL_DOSPROCESS
#define INCL_DOSPROFILE
#define INCL_DOSMISC
#define INCL_DOSMODULEMGR
#define INCL_DOSDATETIME
#define INCL_DOSERRORS

#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <types.h>
#include <string.h>
#include <sys/socket.h>
#include <malloc.h>
#include <netinet/in.h>

ssize_t getxattr (const char *path, const char *name, void *value, size_t size)
{
        return os2getxattr(path, 0, name, value, size);
}


ssize_t lgetxattr(const char *path, const char *name, void *value, size_t size)
{
	return os2getxattr(path, 0, name, value, size);
}

ssize_t fgetxattr(int filedes, const char *name, void *value, size_t size)
{
	return os2getxattr(0, filedes, name, value, size);
}

ssize_t listxattr (const char *path, char *list, size_t size)
{
	return os2listxattr(path, 0, list, size);
}

ssize_t llistxattr(const char *path, char *list, size_t size)
{
	return os2listxattr(path, 0, list, size);
}

ssize_t flistxattr (int filedes, char *list, size_t size)
{
	return os2listxattr(0, filedes, list, size);
}


int lsetxattr (const char *path, const char *name, const void *value, size_t size, int flags)
{
	return os2setxattr (path, 0, name, value, size, flags);
}

int setxattr (const char *path, const char *name, const void *value, size_t size, int flags)
{
	return os2setxattr (path, 0, name, value, size, flags);
}

int fsetxattr (int filedes, const char *name, const void *value, size_t size, int flags)
{
	return os2setxattr (0, filedes, name, value, size, flags);
}

int removexattr (const char *path, const char *name)
{
	return os2removexattr (path, 0, name);
}

int lremovexattr(const char *path, const char *name)
{
	return os2removexattr (path, 0, name);
}

int fremovexattr (int filedes, const char *name)
{
	return os2removexattr (0, filedes, name);
}

#if 1
void maperrno(int rc)
{
	switch (rc)
	{
		case ERROR_PATH_NOT_FOUND    : 
		case ERROR_FILE_NOT_FOUND    : errno = ENOENT; break;
		case ERROR_INVALID_HANDLE    : errno = EBADF; break;
		case ERROR_ACCESS_DENIED     : errno = EACCES; break;
		case ERROR_BUFFER_OVERFLOW   :
		case ERROR_NOT_ENOUGH_MEMORY : errno = ERANGE; break;
		case ERROR_INVALID_EA_NAME   : errno = ENOATTR; break;
		case ERROR_INVALID_LEVEL     :
		case ERROR_INVALID_PARAMETER : errno = EINVAL; break;
		case ERROR_SHARING_VIOLATION : errno = EACCES; break;
		default : errno = EINVAL;
	}
}

ssize_t os2getxattr (const char *path, int file, const char *name, void *value, size_t size)
{
	int rc, namelen;
	EAOP2       eaop2 = {0};
	PGEA2LIST   pgea2list = NULL;
	PFEA2LIST   pfea2list = NULL;
	char * p;
	
	if ((!path && !file) || !name)
	{
		errno = EINVAL;
		return -1;
	}
	namelen = strlen(name);
	if (namelen > 0xFF)
	{
		errno = EINVAL;
		return -1;
	}
	pgea2list = (PGEA2LIST)calloc(sizeof(GEA2LIST) + namelen + 1, 1);
	pgea2list->list[0].oNextEntryOffset = 0;
	pgea2list->list[0].cbName = namelen;
	strcpy(pgea2list->list[0].szName, name);
	pgea2list->cbList = sizeof(GEA2LIST) + namelen;

	// max ea is 64kb
	pfea2list = (PFEA2LIST)calloc(sizeof(FEA2LIST) + 0x10000, 1);
	pfea2list->cbList = sizeof(FEA2LIST) + 0x10000;

	eaop2.fpGEA2List = pgea2list;
	eaop2.fpFEA2List = pfea2list;
	eaop2.oError = 0;	
	do
	{
		if (path)
		{
			char npath[CCHMAXPATH + 1] = {0};
			strncpy(npath, path, CCHMAXPATH);
			for (p = npath; *p; p++)
			{
				if (*p == '/') *p = '\\';
			}
			rc = DosQueryPathInfo(npath, FIL_QUERYEASFROMLIST, &eaop2, sizeof(eaop2));
		}
		else
		{
			rc = DosQueryFileInfo( file, FIL_QUERYEASFROMLIST, &eaop2, sizeof(eaop2));
		}
		if (rc)
		{
			maperrno(rc);
			rc = -1;
			break;
		}
		if (strnicmp(pfea2list->list[0].szName, name, namelen) || pfea2list->list[0].cbValue == 0)
		{
			errno = ENOATTR;
			rc = -1;
			break;
		}
		rc = pfea2list->list[0].cbValue;
		if (value)
		{
			if (size < rc)
			{
				errno = ERANGE;
				rc = -1;
			}
			else
			{
				p = pfea2list->list[0].szName + pfea2list->list[0].cbName + 1;
				memcpy(value, p, rc);
			}
		}
	} while (0);
	if (pgea2list)
	{
		free(pgea2list);
	}
	if (pfea2list)
	{
		free(pfea2list);
	}

	fprintf(stderr,"os2getxattr : (%s:%d) %s %d\n", path ? path : "null", file, name, rc);

	return rc;
}

ssize_t os2listxattr (const char *path, int file, char *list, size_t size)
{
	fprintf(stderr, "os2listxattr: path %s, file %u at %s(%u)\n", path, file, __FILE__, __LINE__);
	ssize_t gotsize = 0;
	unsigned long ulCount = -1;
	int rc;
	char * buf, *p = list;
	PFEA2 pfea;
	FILESTATUS4 stat = {0};
	char npath[CCHMAXPATH + 1] = {0};
	if (!path && !file)
	{
		errno = EINVAL;
		return -1;
	}
	if (path)
	{
		char * p;
		strncpy(npath, path, CCHMAXPATH);
		for (p = npath; *p; p++)
		{
			if (*p == '/') *p = '\\';
		}
		rc = DosQueryPathInfo(npath, FIL_QUERYEASIZE, &stat, sizeof(stat));
	}
	else
	{
		rc = DosQueryFileInfo( file, FIL_QUERYEASIZE, &stat, sizeof(stat));
	}
	if (rc)
	{
		fprintf(stderr,"os2listxattr1 : (%s:%d) rc=%d\n", path ? npath : "null", file, rc);
		maperrno(rc);
		return -1;		
	}
	if (stat.cbList <= 4)
	{
		// NO ea
		return 0;
	}
	//YD DosEnumAttribute doesn't like high-mem buffers, get a low one.
	buf = (char *)_tmalloc(stat.cbList * 2);
	rc = DosEnumAttribute(path ? 1 : 0, path ? (PVOID)npath : (PVOID)&file, 1, (PVOID)buf, stat.cbList * 2, &ulCount, 1);
	if (rc)
	{
		fprintf(stderr,"os2listxattr2 : (%s:%d) rc=%d\n", path ? npath : "null", file, rc);
		maperrno(rc);
		_tfree(buf);
		return -1;
	}
	if (ulCount > 0)
	for (pfea = (PFEA2)buf;;pfea = (PFEA2)((char *)pfea + pfea->oNextEntryOffset))
	{
		if (pfea->cbName > 0)
		{
			gotsize += pfea->cbName + 1;
			if (p && size >= gotsize)
			{
				pfea->szName[pfea->cbName] = 0;
				strcpy(p, pfea->szName);
				p += strlen(p) + 1;
			}
		}
		// had to be added to avoid crash in case of broken extended attributes 
		if (pfea->oNextEntryOffset > 0x10000)
		{
			fprintf(stderr,"Broken Extended Attributes detected for: %s:%d, Last EA:%s\n", path ? path : "null", file, pfea->szName);
			break;
		}
		if (!pfea->oNextEntryOffset)
		{
			break;
		}
	}
	_tfree(buf);

	fprintf(stderr,"os2listxattr : (%s:%d) %d\n", path ? path : "null", file, gotsize);

	if (gotsize > size)
	{
		errno = ERANGE;
		return list ? -1 : gotsize;
	}
	return gotsize;
}

int os2removexattr (const char *path, int file, const char *name)
{
	int rc, namelen;
	EAOP2 eaop2 = {0};
	PFEA2LIST pfea2list = NULL;
	char buf[300] = {0};

	if ((!path && !file) || !name)
	{
		errno = EINVAL;
		return -1;
	}

	namelen = strlen(name);
	if (namelen > 0xFF)
	{
		errno = EINVAL;
		return -1;
	}
	
	pfea2list = (PFEA2LIST)buf;
	pfea2list->list[0].cbName = namelen;
	pfea2list->list[0].cbValue = 0;
	pfea2list->list[0].fEA = 0;
	strcpy(pfea2list->list[0].szName, name);
	pfea2list->cbList = sizeof(FEA2LIST) + namelen;
	eaop2.fpFEA2List = pfea2list;

	if (path)
	{
		char npath[CCHMAXPATH + 1] = {0};
		char * p;
		strncpy(npath, path, CCHMAXPATH);
		for (p = npath; *p; p++)
		{
			if (*p == '/') *p = '\\';
		}
		rc = DosSetPathInfo(npath, FIL_QUERYEASIZE, &eaop2, sizeof(eaop2), DSPI_WRTTHRU);
	}
	else
	{
		rc = DosSetFileInfo( file, FIL_QUERYEASIZE, &eaop2, sizeof(eaop2));
	}
	if (rc)
	{
		maperrno(rc);
		return -1;
	}
	return 0;
}

#ifndef XATTR_CREATE
#define XATTR_CREATE  1
#endif
#ifndef XATTR_REPLACE
#define XATTR_REPLACE 2
#endif

int os2setxattr (const char *path, int file, const char *name, const void *value, size_t size, int flags)
{
	int rc, namelen, totalsize;
	EAOP2       eaop2 = {0};
	PFEA2LIST   pfea2list = NULL;
	char * p;
	
	fprintf(stderr,"os2setxattr : (%s:%d) name=%s size=%d, flags = %d\n", path ? path : "null", file, name, size, flags);

	if ((!path && !file) || !name || (!value && size))
	{
		errno = EINVAL;
		return -1;
	}
	namelen = strlen(name);
	if (namelen > 0xFF)
	{
		errno = EINVAL;
		return -1;
	}

	if (flags & (XATTR_CREATE | XATTR_REPLACE))
	{
		ssize_t esize = os2getxattr(path, file, name, 0, 0);
		if (flags & XATTR_CREATE && esize > 0)
		{
			errno = EEXIST;
			return -1;
		}
		if (flags & XATTR_REPLACE && esize < 0)
		{
			errno = ENOATTR;
			return -1;
		}
	}

	totalsize = sizeof(FEA2LIST) + size + namelen + 1;

	pfea2list = (PFEA2LIST)calloc(totalsize, 1);
	pfea2list->cbList = totalsize;
	pfea2list->list[0].oNextEntryOffset = 0;
	pfea2list->list[0].cbName = namelen;
	pfea2list->list[0].cbValue = size;
	strcpy(pfea2list->list[0].szName, name);
	if (value)
	{
		memcpy(pfea2list->list[0].szName + namelen + 1, value, size);
	}
	eaop2.fpFEA2List = pfea2list;	

	if (path)
	{
		char npath[CCHMAXPATH + 1] = {0};
		char * p;
		strncpy(npath, path, CCHMAXPATH);
		for (p = npath; *p; p++)
		{
			if (*p == '/') *p = '\\';
		}
		rc = DosSetPathInfo(npath, FIL_QUERYEASIZE, &eaop2, sizeof(eaop2), DSPI_WRTTHRU);
		if (rc == ERROR_SHARING_VIOLATION)
		{
                        ULONG action;
			APIRET apiret2 = DosOpenL(
				(PCSZ)path,
				(PHFILE)&file,
				&action,
				0,
				0,
				OPEN_ACTION_CREATE_IF_NEW | OPEN_ACTION_OPEN_IF_EXISTS,
				OPEN_ACCESS_READWRITE | OPEN_SHARE_DENYNONE, 0L);
			if (apiret2)
				fprintf(stderr, "os2setxattr: DosOpenL(%s) failed with error %lu at %s(%u)\n", path, apiret2, __FILE__, __LINE__);
			else {
				rc = DosSetFileInfo(file, FIL_QUERYEASIZE, &eaop2, sizeof(eaop2));

				apiret2 = DosClose(file);
				if (apiret2)
					fprintf(stderr, "os2getxattr: DosClose(%u) failed with error %lu at %s(%u)\n", file, apiret2, __FILE__, __LINE__);	// 2017-10-25 SHL
			}
		}

	}
	else
	{
		rc = DosSetFileInfo( file, FIL_QUERYEASIZE, &eaop2, sizeof(eaop2));
	}
	free(pfea2list);
	if (rc)
	{
		fprintf(stderr,"os2setxattr : (%s:%d) %s rc=%d\n", path ? path : "null", file, name, rc);
		maperrno(rc);
		return -1;
	}
	return 0;
}

#else
static char *out_of_mem_msg = "out of memory allocating %lu at %s(%u)\n";

/**
 * Map OS/2 apiret to errno
 */

static void map_os2_apiret(APIRET apiret)
{
	switch (apiret) {
	case ERROR_DISK_FULL:		// 2015-04-12 SHL
		errno = ENOSPC;
		break;
	case ERROR_SHARING_VIOLATION:
		errno = EACCES;
		break;
	case 0:
		// Should not have been called - try to do no harm
		break;
	default:
		// 2009-07-31 SHL FIXME to map better
		fprintf(stderr, "can not map OS/2 error %lu to errno\n", apiret);	// 2009-07-31 SHL
		if (!errno)
			errno = EINVAL;	// Ensure non-zero
	} // switch
}

/**
 * Return EA value given pathname or file handle and EA name
 * @param path is pointer to file name or NULL
 * @param file is file handle or 0
 * @param name is pointer to EA name
 * @param value is optional pointer to EA value return buffer
 * @param size is value buffer size in bytes
 * @note Only path or file may be passed, not both
 * @note Sets errno as needed
 * @returns EA size in bytes or -1 if error
 */

// static
ssize_t os2getxattr(const char *path, int file, const char *name, void *value, size_t size)
{
	int easize;
	int namelen;
	size_t reqsize;
	EAOP2 eaop2 = {0,0,0};
	PGEA2LIST pgea2list = NULL;
	PFEA2LIST pfea2list = NULL;
	char * p;

	if ((!path && !file) || !name) {
		errno = EINVAL;
		return -1;
	}
	if (file && path) {
		fprintf(stderr, "os2getxattr: path %p not NULL and file %u not 0 at %s(%u)\n", path, file, __FILE__, __LINE__);
		errno = EINVAL;
		return -1;
	}
	namelen = strlen(name);
	if (namelen > 0xFF) {
		errno = EINVAL;
		return -1;
	}
	reqsize = sizeof(GEA2LIST) + namelen + 1;
	pgea2list = (PGEA2LIST)calloc(reqsize, 1);
	// 2010-11-30 SHL
	if (pgea2list == NULL) {
		fprintf(stderr, out_of_mem_msg, reqsize, __FILE__, __LINE__);
		errno = ENOMEM;
		return -1;
	}
	pgea2list->list[0].oNextEntryOffset = 0;
	pgea2list->list[0].cbName = namelen;
	strcpy(pgea2list->list[0].szName, name);
	pgea2list->cbList = sizeof(GEA2LIST) + namelen;

	// Max EA set size is 64KB
	reqsize = sizeof(FEA2LIST) + 0x10000;
	pfea2list = (PFEA2LIST)calloc(reqsize, 1);
	// 2010-11-30 SHL
	if (pfea2list == NULL) {
		fprintf(stderr, out_of_mem_msg, reqsize, __FILE__, __LINE__);
		errno = ENOMEM;
		return -1;
	}

	pfea2list->cbList = sizeof(FEA2LIST) + 0x10000;
	eaop2.fpGEA2List = pgea2list;
	eaop2.fpFEA2List = pfea2list;
	eaop2.oError = 0;

	do {
		APIRET apiret = NO_ERROR;	// 2017-10-26 SHL Use
		APIRET apiret2;			// 2017-10-26 SHL Use
		int needclose = 0;		// 2017-10-25 SHL Use
		if (path) {
			/* DosQueryPathInfo fails if file open for write elsewhere
			   Try to access via handle when this occurs
			   2017-10-25 SHL
			*/
			apiret = DosQueryPathInfo((PCSZ)path, FIL_QUERYEASFROMLIST, &eaop2, sizeof(eaop2));
			if (apiret == ERROR_SHARING_VIOLATION) {
                                ULONG action;
				APIRET apiret2 = DosOpenL(
					(PCSZ)path,
					(PHFILE)&file,
					&action,
					0,
					0,
					OPEN_ACTION_FAIL_IF_NEW |
					OPEN_ACTION_OPEN_IF_EXISTS,
					OPEN_FLAGS_FAIL_ON_ERROR |
					OPEN_FLAGS_NOINHERIT |
					OPEN_FLAGS_RANDOMSEQUENTIAL |
					OPEN_SHARE_DENYNONE |
					OPEN_ACCESS_READONLY, 0);
				if (apiret2)
					fprintf(stderr, "os2getxattr: DosOpenL(%s) failed with error %lu at %s(%u)\n", path, apiret2, __FILE__, __LINE__);
				else
					needclose = 1;
			} // if sharing
			else if (apiret)
				fprintf(stderr, "os2getxattr: DosQueryPathInfo(%s, FIL_QUERYEASFROMLIST) for %s failed with error %lu at %s(%u)\n", path, name, apiret, __FILE__, __LINE__);	// 2011-12-30 SHL
//			else if (verbose > 2)
//				 fprintf(stderr, "DosQueryPathInfo(%s, FIL_QUERYEASFROMLIST, %s) returning %lu bytes\n", path, name, eaop2.fpFEA2List->cbList);
		} // if path

		if (file) {
			apiret = DosQueryFileInfo( file, FIL_QUERYEASFROMLIST, &eaop2, sizeof(eaop2));
			if (apiret)
				fprintf(stderr, "os2getxattr: DosQueryFileInfo(%u, FIL_QUERYEASFROMLIST) for %s failed with error %lu at %s(%u)\n", file, name, apiret, __FILE__, __LINE__);	// 2011-12-30 SHL
//			else if (verbose > 2)
//				fprintf(stderr, "DosQueryFileInfo(%d, FIL_QUERYEASFROMLIST, %s) returning %lu bytes\n", file, name, eaop2.fpFEA2List->cbList);
		} // if file

		// If local open, close here ignoring errors
		if (needclose) {
			apiret2 = DosClose(file);
			if (apiret2)
				fprintf(stderr, "os2getxattr: DosClose(%u) failed with error %lu at %s(%u)\n", file, apiret2, __FILE__, __LINE__);	// 2017-10-25 SHL
			file = NULL;
		}

		if (apiret) {
			map_os2_apiret(apiret);
			easize = -1;
			break;
		}

		if (strnicmp(pfea2list->list[0].szName, name, namelen) || pfea2list->list[0].cbValue == 0) {
			errno = ENOATTR;
			easize = -1;
			break;
		}

		easize = pfea2list->list[0].cbValue;	// EA value size in bytes
		// If want EA value returned
		if (value) {
			if ((size_t)easize > size) {
				errno = ERANGE;
				easize = -1;
			}
			else {
				// Point at EA value - name is nul terminated
				p = pfea2list->list[0].szName + pfea2list->list[0].cbName + 1;
				memcpy(value, p, easize);	// Copy EA value - not nul terminated
			}
		}
	} while (0);

	if (pgea2list)
		free(pgea2list);
	if (pgea2list)
		free(pfea2list);

	return easize;
}

/**
 * Return EA name list given pathname or file handle
 * @param path is pointer to file name or NULL
 * @param file is file handle or 0
 * @param list points to optional return buffer for nul terminated list of EA names
 * @param size is list size limit in bytes
 * @note Only path or file may be passed, not both
 * @return calculated list size in bytes or -1 if error
 */

// static
ssize_t os2listxattr(const char *path, int file, char *list, size_t size)
{
	ssize_t calcsize = 0;
	size_t reqsize;
	unsigned long ulCount = -1;	// Request all
	APIRET apiret = NO_ERROR;	// 2017-10-26 SHL Use
	APIRET apiret2;			// 2017-10-26 SHL Use
	int needclose = 0;		// 2017-10-25 SHL Use
	char *buf = NULL;
	char *p = list;
	PFEA2 pfea;
	FILESTATUS4 fs4;
	fprintf(stderr, "os2listattr: path %p, file %u at %s(%u)\n", path, file, __FILE__, __LINE__);
	if (!path && !file) {
		errno = EINVAL;
		return -1;
	}
	if (file && path) {
		fprintf(stderr, "os2listxattr: path %p not NULL and file %u not 0 at %s(%u)\n", path, file, __FILE__, __LINE__);
		errno = EINVAL;
		return -1;
	}

	do {
		// Get EA set size
		if (path) {
			/* DosQueryPathInfo fails if file open for write elsewhere
			   Try to access via handle when this occurs
			   2017-10-25 SHL
			*/
			apiret = DosQueryPathInfo((PCSZ)path, FIL_QUERYEASIZE, &fs4, sizeof(fs4));
			if (apiret == ERROR_SHARING_VIOLATION) {
                                ULONG action;
				APIRET apiret2 = DosOpenL(
					(PCSZ)path,
					(PHFILE)&file,
					&action,
					0,
					0,
					OPEN_ACTION_FAIL_IF_NEW |
					OPEN_ACTION_OPEN_IF_EXISTS,
					OPEN_FLAGS_FAIL_ON_ERROR |
					OPEN_FLAGS_NOINHERIT |
					OPEN_FLAGS_RANDOMSEQUENTIAL |
					OPEN_SHARE_DENYNONE |
					OPEN_ACCESS_READONLY, 0);
				if (apiret2)
					fprintf(stderr, "os2getxattr: DosOpenL(%s) failed with error %lu at %s(%u)\n", path, apiret2, __FILE__, __LINE__);
				else
					needclose = 1;
			} // if sharing
			else if (apiret)
				fprintf(stderr, "os2listxattr DosQueryPathInfo(%s, FIL_QUERYEASIZE) failed with error %lu %s(%u)\n", path, apiret, __FILE__, __LINE__);
//			else if (verbose > 2)
//				fprintf(stderr, "DosQueryPathInfo(%s, FIL_QUERYEASIZE) returning %lu bytes\n", path, fs4.cbList);
		} // if path

		if (file) {
			apiret = DosQueryFileInfo( file, FIL_QUERYEASIZE, &fs4, sizeof(fs4));
			if (apiret)
				fprintf(stderr, "os2listxattr DosQueryFilInfo(%u, FIL_QUERYEASIZE) failed with error %lu at %s(%u)\n", file, apiret, __FILE__, __LINE__);
//			else if (verbose > 2)
//				fprintf(stderr, "DosQueryFilInfo(%d, FIL_QUERYEASIZE) returning %lu bytes\n", file, fs4.cbList);
		} // if file

		if (apiret) {
			map_os2_apiret(apiret);
			calcsize = -1;
			break;
		}

	        if (fs4.cbList <= 4) {
		        // No EAs
		        calcsize = 0;
			break;
	        }

	        // YD DosEnumAttribute doesn't like high-mem buffers, get a low one.
	        reqsize = fs4.cbList * 2;
	        buf = (char *)_tmalloc(reqsize);
	        // 2009-09-19 SHL was trapping on large copies
	        if (!buf) {
		        fprintf(stderr, out_of_mem_msg, reqsize, __FILE__, __LINE__);	// 2009-09-19 SHL
		        calcsize = -1;
			break;
	        }
		// Get EA names, without values
	        apiret = DosEnumAttribute(file ? ENUMEA_REFTYPE_FHANDLE : ENUMEA_REFTYPE_PATH,
					  file ? (PVOID)&file : (PVOID)path,
					  1,				// ulEntry
					  (PVOID)buf,
					  fs4.cbList * 2,
					  &ulCount,
					  ENUMEA_LEVEL_NO_VALUE);
	        if (apiret) {
		        fprintf(stderr, "os2listxattr DosEnumAttribute(%p, %d) failed with error %lu at %s(%u)\n", path, file, apiret, __FILE__, __LINE__);
			break;
	        }
//	        else if (verbose > 2) {
//		        if (file > 0)
//			        fprintf(stderr, "DosEnumAttribute(%d) reports %lu EAs\n", file, ulCount);
//		        else
//			        fprintf(stderr, "DosEnumAttribute(%s) reports %lu EAs\n", path, ulCount);
//	        }

	        if (ulCount > 0) {
		        /* 2015-11-03 SHL FIXME to avoid crash if DosEnumAttribute returns corrupted
		           FEA2 with bogus oNextEntryOffset
		        */
		        for (pfea = (PFEA2)buf;;pfea = (PFEA2)((char *)pfea + pfea->oNextEntryOffset)) {
#if 1 // 2017-10-26 SHL	FIXME to be gone unless really needed
			        if ((UINT)pfea < (UINT)buf || (UINT)pfea > (UINT)buf + reqsize) {
				        fprintf(stderr, "os2listxattr DosEnumAttribute returned corrupted FEA2 list at %s(%u)\n", __FILE__, __LINE__);
				        errno = EFAULT;
				        calcsize = -1;
					break;
			        }
#endif
			        if (pfea->cbName > 0) {
				        calcsize += pfea->cbName + 1;	// Accumulate size plus 1 for trailing nul
					// If list passed, fill avoiding overflow
				        if (p && (size_t)calcsize <= size) {
					        pfea->szName[pfea->cbName] = 0;	// FIXME to be gone
					        strcpy(p, pfea->szName);
					        p += strlen(p) + 1;
				        }
			        }
			        if (!pfea->oNextEntryOffset)
				        break;	// Done
			} // for pfea
		} // if ulCount
	} while (0);

	if (apiret) {
		map_os2_apiret(apiret);
		calcsize = -1;
	}

	// If local open, close here ignoring errors
	if (needclose) {
		apiret2 = DosClose(file);
		if (apiret2)
			fprintf(stderr, "os2getxattr: DosClose(%u) failed with error %lu at %s(%u)\n", file, apiret2, __FILE__, __LINE__);	// 2017-10-25 SHL
		file = NULL;
	}

	if (buf)
		_tfree(buf);

	if (calcsize != -1 && (size_t)calcsize > size) {
		errno = ERANGE;
		calcsize = list ? -1 : calcsize;
	}

//	if (verbose > 2)
//		fprintf(stderr, "os2listxattr returning %ld EA list bytes\n", (unsigned long)calcsize);

	return calcsize;
}

/**
 * Delete EA given pathname or file handle and EA name
 * @param path is pointer to file name or NULL
 * @param file is file handle or 0
 * @param name is pointer to EA name
 * @note Only path or file may be passed, not both
 * @returns 0 if ok or -1 if error
 */

// static
int os2removexattr(const char *path, int file, const char *name)
{
	APIRET apiret;
	int namelen;
	EAOP2 eaop2 = {0,0,0};
	PFEA2LIST pfea2list = NULL;
	char buf[300] = {0};

	if ((!path && !file) || !name) {
		errno = EINVAL;
		return -1;
	}

	namelen = strlen(name);
	if (namelen > 0xFF) {
		errno = EINVAL;
		return -1;
	}

	pfea2list = (PFEA2LIST)buf;
	pfea2list->list[0].cbName = namelen;
	pfea2list->list[0].cbValue = 0;
	pfea2list->list[0].fEA = 0;
	strcpy(pfea2list->list[0].szName, name);
	pfea2list->cbList = sizeof(FEA2LIST) + namelen;
	eaop2.fpFEA2List = pfea2list;

	if (path) {
		apiret = DosSetPathInfo((PCSZ)path, FIL_QUERYEASIZE, &eaop2, sizeof(eaop2), DSPI_WRTTHRU);
		if (apiret)
			fprintf(stderr, "os2removexattr: DosSetPathInfo(%s, FIL_QUERYEASIZE) for %s failed with error %lu at %s(%u)\n", path, name, apiret, __FILE__, __LINE__);	// 2011-12-30 SHL
//		else if (verbose > 1)
//			fprintf(stderr, "DosSetPathInfo(%s) deleted EA %s\n", path, name);
	}
	else {
		apiret = DosSetFileInfo( file, FIL_QUERYEASIZE, &eaop2, sizeof(eaop2));
		if (apiret)
			fprintf(stderr, "os2removexattr: DosFileInfo(%u, FIL_QUERYEASIZE) for %s failed with error %lu %s(%u)\n", file, name, apiret, __FILE__, __LINE__);	// 2011-12-30 SHL
//		else if (verbose > 1)
//			fprintf(stderr, "DosSetFileInfo(%d) deleted EA %s\n", file, name);
	}

	if (apiret) {
		map_os2_apiret(apiret);
		return -1;
	}

	return 0;
}

#ifndef XATTR_CREATE
#define XATTR_CREATE  1
#endif
#ifndef XATTR_REPLACE
#define XATTR_REPLACE 2
#endif

/**
 * Set EA value given pathname or file handle and EA name and value
 * @param path is pointer to file name or NULL
 * @param file is file handle or 0
 * @param name is pointer to EA name
 * @param value is pointer to EA value
 * @param size is value buffer size in bytes
 * @param flags is action (XATTR_CREATE, XATTR_REPLACE, ...)
 * @note Only path or file may be passed, not both
 * @note Sets errno as needed
 * @returns 0 if ok or -1 if error
 */

// static
int os2setxattr (const char *path, int file, const char *name, const void *value, size_t size, int flags)
{
	APIRET apiret;
	int namelen;
	int totalsize;
	EAOP2       eaop2 = {0,0,0};
	PFEA2LIST   pfea2list = NULL;

	if ((!path && !file) || !name || (!value && size)) {
		errno = EINVAL;
		return -1;
	}
	namelen = strlen(name);
	if (namelen > 0xFF) {
		errno = EINVAL;
		return -1;
	}

	if (flags & (XATTR_CREATE | XATTR_REPLACE)) {
		ssize_t esize = os2getxattr(path, file, name, 0, 0);
		if (flags & XATTR_CREATE && esize > 0) {
			errno = EEXIST;
			return -1;
		}
		if (flags & XATTR_REPLACE && esize < 0) {
			errno = ENOATTR;
			return -1;
		}
	}

	totalsize = sizeof(FEA2LIST) + size + namelen + 1;

	pfea2list = (PFEA2LIST)calloc(totalsize, 1);
	// 2010-11-30 SHL
	if (pfea2list == NULL) {
		fprintf(stderr, out_of_mem_msg, totalsize, __FILE__, __LINE__);
		errno = ENOMEM;
		return -1;
	}
	pfea2list->cbList = totalsize;
	pfea2list->list[0].oNextEntryOffset = 0;
	pfea2list->list[0].cbName = namelen;
	pfea2list->list[0].cbValue = size;
	strcpy(pfea2list->list[0].szName, name);
	if (value)
		memcpy(pfea2list->list[0].szName + namelen + 1, value, size);
	eaop2.fpFEA2List = pfea2list;

	if (path) {
		apiret = DosSetPathInfo((PCSZ)path, FIL_QUERYEASIZE, &eaop2, sizeof(eaop2), DSPI_WRTTHRU);
		if (apiret)
			fprintf(stderr, "os2setxattr: DosSetPathInfo(%s, FIL_QUERYEASIZE) for %s failed with error %lu at %s(%u)\n", path, name, apiret, __FILE__, __LINE__);	// 2011-12-30 SHL
//		else if (verbose > 1)
//			fprintf(stderr, "DosSetPathInfo(%s) set EA %s\n", path, name);
	}
	else {
		apiret = DosSetFileInfo( file, FIL_QUERYEASIZE, &eaop2, sizeof(eaop2));
		if (apiret)
			fprintf(stderr, "os2setxattr: DosSetFileInfo(%u, FIL_QUERYEASIZE) for %s failed with error %lu at %s(%u)\n", file, name, apiret, __FILE__, __LINE__);	// 2011-12-30 SHL
//		else if (verbose > 1)
//			fprintf(stderr, "DosSetFileInfo(%d, FIL_QUERYEASIZE) set EA %s\n", file, name);
	}

	free(pfea2list);

	if (apiret) {
		map_os2_apiret(apiret);
		return -1;
	}

	return 0;
}
#endif //1
#endif // OS2
#ifdef HAVE_VISIBILITY_ATTRIBUTE
# pragma GCC visibility pop
#endif
