#ifndef __DBGVIEW_OUT_H__
#define __DBGVIEW_OUT_H__

#include <stdarg.h> 
#include <stdio.h>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN  /* We always want minimal includes */
#endif
#include <Windows.h>

#ifndef DBG_VIEW_OUT_DISABLE
#define dbgviewprint debugviewout
#else
#define dbgviewprint
#endif


#ifdef _MSC_VER
enum { IS_COMPILER_MSVC = 1 };
#ifndef va_copy
// Define va_copy for MSVC. This is a hack, assuming va_list is simply a
// pointer into the stack and is safe to copy.
#define va_copy(dest, src) ((dest) = (src))
#endif
#else
enum { IS_COMPILER_MSVC = 0 };
#endif

static const int kSpaceLength = 1024;

inline void __cdecl debugviewout(char* format, ...) 
{
	// First try with a small fixed size buffer
	
	char space[kSpaceLength];

	// It's possible for methods that use a va_list to invalidate
	// the data in it upon use.  The fix is to make a copy
	// of the structure before using it and use that copy instead.
	va_list ap;
	va_start(ap, format);

	va_list backup_ap;
	va_copy(backup_ap, ap);
	int result = vsnprintf(space, kSpaceLength, format, backup_ap);
	va_end(backup_ap);

	if (result < kSpaceLength) {
		if (result >= 0) {
			// Normal case -- everything fit.
			OutputDebugStringA(space);
			return;
		}

		if (IS_COMPILER_MSVC) {
			// Error or MSVC running out of space.  MSVC 8.0 and higher
			// can be asked about space needed with the special idiom below:
			va_copy(backup_ap, ap);
			result = vsnprintf(NULL, 0, format, backup_ap);
			va_end(backup_ap);
		}

		if (result < 0) {
			// Just an error.
			return;
		}
	}

	// Increase the buffer size to the size requested by vsnprintf,
	// plus one for the closing \0.
	int length = result+1;
	char* buf = new char[length];

	// Restore the va_list before we use it again
	va_copy(backup_ap, ap);
	result = vsnprintf(buf, length, format, backup_ap);
	va_end(backup_ap);

	if (result >= 0 && result < length) {
		// It fit
		OutputDebugStringA(buf);
	}
	delete[] buf;
}

inline void __cdecl debugviewout(wchar_t* format, ...) 
{
	// First try with a small fixed size buffer
	wchar_t space[kSpaceLength];

	// It's possible for methods that use a va_list to invalidate
	// the data in it upon use.  The fix is to make a copy
	// of the structure before using it and use that copy instead.
	va_list ap;
	va_start(ap, format);

	va_list backup_ap;
	va_copy(backup_ap, ap);
	int result = vswprintf(space, kSpaceLength, format, backup_ap);
	va_end(backup_ap);

	if (result < kSpaceLength) {
		if (result >= 0) {
			// Normal case -- everything fit.
			OutputDebugStringW(space);
			return;
		}

		if (IS_COMPILER_MSVC) {
			// Error or MSVC running out of space.  MSVC 8.0 and higher
			// can be asked about space needed with the special idiom below:
			va_copy(backup_ap, ap);
			result = vswprintf(NULL, 0, format, backup_ap);
			va_end(backup_ap);
		}

		if (result < 0) {
			// Just an error.
			return;
		}
	}

	// Increase the buffer size to the size requested by vsnprintf,
	// plus one for the closing \0.
	int length = result+1;
	wchar_t* buf = new wchar_t[length];

	// Restore the va_list before we use it again
	va_copy(backup_ap, ap);
	result = vswprintf(buf, length, format, backup_ap);
	va_end(backup_ap);

	if (result >= 0 && result < length) {
		// It fit
		OutputDebugStringW(buf);
	}
	delete[] buf;
}

#endif
