// from google3/base/stringprintf.h
//
// Printf variants that place their output in a C++ string.
//
// Usage:
//      string result = StringPrintf("%d %s\n", 10, "hello");
//      SStringPrintf(&result, "%d %s\n", 10, "hello");
//      StringAppendF(&result, "%d %s\n", 20, "there");

#ifndef GOOGLE_PROTOBUF_STUBS_STRINGPRINTF_H
#define GOOGLE_PROTOBUF_STUBS_STRINGPRINTF_H

#include <string>

namespace google {

// Return a C++ string given printf-like input.
 std::string StringPrintf(const char* format, ...);

 std::wstring StringPrintf(const wchar_t* format, ...);

// Return a C++ string given vprintf-like input.
 std::string StringPrintV(const char* format, va_list ap);

// Store result into a supplied string and return it.
 const std::string& SStringPrintf(std::string* dst, const char* format, ...);

 const std::wstring& SStringPrintf(std::wstring* dst, const wchar_t* format, ...);

// Append result to a supplied string.
 void StringAppendF(std::string* dst, const char* format, ...);

 void StringAppendF(std::wstring* dst, const wchar_t* format, ...);

}  // namespace google

#endif  // GOOGLE_PROTOBUF_STUBS_STRINGPRINTF_H
