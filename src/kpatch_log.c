#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>

#include "kpatch_log.h"

#ifdef STRESS_TEST
#include "kpatch_user.h"
extern int parent_pid;
#endif

int log_level = LOG_INFO;
int log_indent;
static FILE *log_file;

static void __valog(int level, const char *prefix, const char *fmt, va_list va)
{
	FILE *f = level <= LOG_WARN ? stderr : stdout;
	if (prefix)
		fprintf(f, "%s", prefix);

	if (log_file) {
		va_list vaf;
		va_copy(vaf, va);
		if (prefix)
			fprintf(log_file, "%s", prefix);
		vfprintf(log_file, fmt, vaf);
		fflush(log_file);
	}
	vfprintf(f, fmt, va);
}

void kplog(int level, const char *fmt, ...)
{
	int errno_sv;
	va_list va;
	char indent[8];

	if (level > log_level)
		return;

	errno_sv = errno;

	va_start(va, fmt);
	memset(indent, ' ', sizeof(indent));
	indent[log_indent] = 0;
	__valog(level, indent, fmt, va);
#if 0
	if (fmt[0] == '+')
		log_indent++;
	else if (fmt[0] == '-')
		log_indent--;
#endif
	va_end(va);

	errno = errno_sv;
}

void kpfatal(const char *fmt, ...)
{
	va_list va;

	va_start(va, fmt);
	__valog(LOG_ERR, "FATAL! ", fmt, va);
	va_end(va);

#ifdef STRESS_TEST
	if (parent_pid >= 0)
		stress_test_notify_parent();
#endif
	exit(1);
}

extern int elf_errno(void) __attribute__((weak));
extern const char *elf_errmsg(int err) __attribute((weak));

void __valogerror(const char *file, int line, const char *fmt, va_list va)
{
	int errno_sv = errno;

	fprintf(stderr, "%s(%d): ", file, line);
	__valog(LOG_ERR, NULL, fmt, va);
	if (errno_sv) {
		fprintf(stderr, "errno = %d, msg = '%s'\n",
			errno, strerror(errno));
	}

#ifndef __MACH__
	if (elf_errno && elf_errmsg) {
		int errno_elf = elf_errno();
		if (errno_elf) {
			fprintf(stderr, "errno_elf = %d, msg = '%s'\n",
				errno_elf, elf_errmsg(errno_elf));
		}
	}
#endif
	errno = errno_sv;
}

void _kplogerror(const char *file, int line, const char *fmt, ...)
{
	va_list va;

	va_start(va, fmt);
	__valogerror(file, line, fmt, va);
	va_end(va);
}

void _kpfatalerror(const char *file, int line, const char *fmt, ...)
{
	va_list va;

	va_start(va, fmt);
	__valogerror(file, line, fmt, va);
	va_end(va);

	exit(EXIT_FAILURE);
}

int log_file_init(char *fname)
{
	if (!fname)
		return -1;
	log_file = fopen(fname, "a");
	if (!log_file)
		return -1;
	return 0;
}

void log_file_free()
{
	if (log_file)
		fclose(log_file);
	log_file = NULL;
}
