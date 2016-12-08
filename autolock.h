#ifndef _CRITICAL_SECTION_UTIL_H_
#define _CRITICAL_SECTION_UTIL_H_

#include <Windows.h>

class CriticalSection {
public:
    CriticalSection() { InitializeCriticalSection(&m_cs); }
    ~CriticalSection() { DeleteCriticalSection(&m_cs); }
    void lock() { EnterCriticalSection(&m_cs); }
    void unlock() { LeaveCriticalSection(&m_cs); }
private:
    CRITICAL_SECTION m_cs;
};

class CriticalSection2 {
public:
    CriticalSection2(CRITICAL_SECTION& cs)
        :m_cs(cs)
    {}
    ~CriticalSection2() { }
    void lock() { EnterCriticalSection(&m_cs); }
    void unlock() { LeaveCriticalSection(&m_cs); }
private:
    CRITICAL_SECTION& m_cs;
};

template <typename T>
class autolock {
public:
    autolock(T& lockable) : m_lockable(lockable) { m_lockable.lock(); }
    ~autolock() { m_lockable.unlock(); }
private:
    T& m_lockable;
};

#endif