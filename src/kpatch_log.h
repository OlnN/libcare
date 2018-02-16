#ifndef __KP_LOG_H__
#define __KP_LOG_H__

#include <stdio.h>

#define ERROR_CODE(return_code)	(return_code - 256)

#define	ERROR_SUCCESS			0
#define	ERROR_GENERAL			ERROR_CODE(1)
// ERROR_FATAL: libcare-ctl encountered fatal error and terminated execution
#define	ERROR_FATAL			ERROR_CODE(2)
// ERROR_ARGUMENTS: libcare-ctl was invoked with wrong arguments
#define	ERROR_ARGUMENTS			ERROR_CODE(3)
// ERROR_PROCESS_NOT_FOUND: process with specified pid does not exist or have already exited
#define	ERROR_PROCESS_NOT_FOUND		ERROR_CODE(4)
// ERROR_PATCH_NOT_FOUND: patch file not found, or patch of same or higher patch level is already applied to the process
#define	ERROR_PATCH_NOT_FOUND		ERROR_CODE(5)
// ERROR_PATCH_FAILURE: libcare-ctl failed to apply patch
#define	ERROR_PATCH_FAILURE		ERROR_CODE(6)
// ERROR_UNPATCH_FAILURE: libcare-ctl failed to cancel patch
// ERROR_UNPATCH_FAILURE can also be returned in case of error during unpatch after unsuccessful patching
#define	ERROR_UNPATCH_FAILURE		ERROR_CODE(8)
// ERROR_RESOURCE_ACCESS: libcare-ctl was not able to attach to process(PTRACE_ATTACH, write access to /proc/PID/mem or stack unwind)
#define	ERROR_RESOURCE_ACCESS		ERROR_CODE(9)
// ERROR_RESOURCE_BUSY: process is currently executing code that must be modified and can't be patched at this moment
#define	ERROR_RESOURCE_BUSY		ERROR_CODE(10)

extern int log_level, log_indent;

void kplog(int level, const char *fmt, ...) __attribute__((format(printf, 2, 3)));
void kpfatal(const char *fmt, ...) __attribute__((noreturn,format(printf,1,2)));

#define kpdebug(fmt...)		kplog(LOG_DEBUG, fmt)
#define kpwarn(fmt...)		kplog(LOG_WARN, fmt)
#define kpinfo(fmt...)		kplog(LOG_INFO, fmt)

void _kpfatalerror(const char *filename, int line, const char *fmt, ...)
	__attribute__((noreturn,format(printf,3,4)));
void _kplogerror(const char *filename, int line, const char *fmt, ...)
	__attribute__((format(printf,3,4)));

#define kpfatalerror(fmt...)	_kpfatalerror(__FILE__, __LINE__, fmt)
#define kplogerror(fmt...)	_kplogerror(__FILE__, __LINE__, fmt)

#define kperr(fmt...)		do {		\
	int errsv = errno;			\
	errno = 0;				\
	_kplogerror(__FILE__, __LINE__, fmt);	\
	errno = errsv;				\
} while (0)

#define LOG_ERR		0
#define LOG_WARN	1
#define LOG_INFO	2
#define LOG_DEBUG	3
#define LOG_TRACE	5

#endif
