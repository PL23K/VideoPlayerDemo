//
// Created by Yangzhiyong on 2025/3/13.
//

#ifndef LOGMESSAGE_H
#define LOGMESSAGE_H

#include "duilib/duilib.h"
#include <functional>
#include <iomanip>

#define COUNT_OF(array)            (sizeof(array)/sizeof(array[0]))

enum LogLevel {
    Error = 0,
    Debug,
    Info
};


void LogSetCallback(const std::function<void(LogLevel, const DString&)> &_callback);

void LogMessage(const char* _format, ...);
void LogMessage(const wchar_t* _format, ...);
void LogMessageEx(LogLevel _logLevel, const char* _format, ...);
void LogMessageEx(LogLevel _logLevel, const wchar_t* _format, ...);

DString GetFormatDate();
DString GetFormatDate(const DString& _strDateFormat);

#endif //LOGMESSAGE_H
