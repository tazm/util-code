
#include "stringprintf.h"
#include <stdarg.h> // For va_list and related operations
#include <stdio.h> // MSVC requires this for _vsnprintf
#include <vector>

namespace google {

inline int vsnprintfT(char* buffer,
	size_t buf_size,
	const char* format,
	va_list argptr) {
		return vsnprintf(buffer, buf_size, format, argptr);
}

inline int vsnprintfT(wchar_t* buffer,
	size_t buf_size,
	const wchar_t* format,
	va_list argptr) {
		return vswprintf(buffer, buf_size, format, argptr);
}


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

// Templatized backend for StringPrintF/StringAppendF. This does not finalize
// the va_list, the caller is expected to do that.
template <class StringType>
static void StringAppendVT(StringType* dst,
						   const typename StringType::value_type* format,
						   va_list ap)
{

	typename StringType::value_type space[kSpaceLength];


  // It's possible for methods that use a va_list to invalidate
  // the data in it upon use.  The fix is to make a copy
  // of the structure before using it and use that copy instead.
  va_list backup_ap;
  va_copy(backup_ap, ap);
  int result = vsnprintfT(space, kSpaceLength, format, backup_ap);
  va_end(backup_ap);

  if (result < kSpaceLength) {
    if (result >= 0) {
      // Normal case -- everything fit.
      dst->append(space, result);
      return;
    }

    if (IS_COMPILER_MSVC) {
      // Error or MSVC running out of space.  MSVC 8.0 and higher
      // can be asked about space needed with the special idiom below:
      va_copy(backup_ap, ap);
      result = vsnprintfT(NULL, 0, format, backup_ap);
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
  std::vector<typename StringType::value_type> mem_buf(length);

  // Restore the va_list before we use it again
  va_copy(backup_ap, ap);
  result = vsnprintfT(&mem_buf[0], length, format, backup_ap);
  va_end(backup_ap);

  if (result >= 0 && result < length) {
    // It fit
    dst->append(&mem_buf[0], result);
  }
}

void StringAppendV(std::string* dst, const char* format, va_list ap) {
	StringAppendVT(dst, format, ap);
}


void StringAppendV(std::wstring* dst, const wchar_t* format, va_list ap) {
	StringAppendVT(dst, format, ap);
}

std::string StringPrintf(const char* format, ...) {
	va_list ap;
	va_start(ap, format);
	std::string result;
	StringAppendV(&result, format, ap);
	va_end(ap);
	return result;
}

std::wstring StringPrintf(const wchar_t* format, ...) {
	va_list ap;
	va_start(ap, format);
	std::wstring result;
	StringAppendV(&result, format, ap);
	va_end(ap);
	return result;
}

std::string StringPrintV(const char* format, va_list ap) {
	std::string result;
	StringAppendV(&result, format, ap);
	return result;
}

const std::string& SStringPrintf(std::string* dst, const char* format, ...) {
	va_list ap;
	va_start(ap, format);
	dst->clear();
	StringAppendV(dst, format, ap);
	va_end(ap);
	return *dst;
}

const std::wstring& SStringPrintf(std::wstring* dst,
								  const wchar_t* format, ...) 
{
	  va_list ap;
	  va_start(ap, format);
	  dst->clear();
	  StringAppendV(dst, format, ap);
	  va_end(ap);
	  return *dst;
}

void StringAppendF(std::string* dst, const char* format, ...) {
	va_list ap;
	va_start(ap, format);
	StringAppendV(dst, format, ap);
	va_end(ap);
}


void StringAppendF(std::wstring* dst, const wchar_t* format, ...) {
	va_list ap;
	va_start(ap, format);
	StringAppendV(dst, format, ap);
	va_end(ap);
}

}  // namespace google
