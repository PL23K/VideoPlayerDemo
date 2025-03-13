//
// Created by Yangzhiyong on 2025/3/13.
//

#include "LogMessage.h"
#include <cstdarg>

static std::function<void(LogLevel, const DString&)> sLogCallback = nullptr;

void LogSetCallback(const std::function<void(LogLevel, const DString&)> &_callback){
  sLogCallback = _callback;
}


inline int vsnprintfT(char* dst, size_t count, const char* format, va_list ap)
{
    return vsnprintf(dst, count, format, ap);
}

inline int vsnprintfT(wchar_t* dst, size_t count, const wchar_t* format, va_list ap)
{
#ifdef DUILIB_BUILD_FOR_WIN
    return _vsnwprintf_s(dst, count, count, format, ap);
#else
    return vswprintf(dst, count, format, ap);
#endif
}

template<typename CharType>
void StringAppendVT(const CharType* format, va_list ap, std::basic_string<CharType>& output)
{
    CharType stack_buffer[1024] = { 0, };

    /* first, we try to finish the task using a fixed-size buffer in the stack */
    va_list ap_copy;
    va_copy(ap_copy, ap);

    int result = vsnprintfT(stack_buffer, COUNT_OF(stack_buffer), format, ap_copy);
    va_end(ap_copy);
    if (result >= 0 && result < static_cast<int>(COUNT_OF(stack_buffer)))
    {
        /* It fits */
        output.append(stack_buffer, result);
        return;
    }

    /* then, we have to repeatedly increase buffer size until it fits. */
    int buffer_size = COUNT_OF(stack_buffer);
    std::basic_string<CharType> heap_buffer;
    for (;;)
    {
        if (result != -1)
        {
            ASSERT(0);
            return; /* not expected, result should be -1 here */
        }
        buffer_size <<= 1; /* try doubling the buffer size */
        if (buffer_size > 32 * 1024 * 1024)
        {
            ASSERT(0);
            return;    /* too long */
        }
        /* resize */
        heap_buffer.resize(buffer_size);
        /*
         * NOTE: You can only use a va_list once.  Since we're in a while loop, we
         * need to make a new copy each time so we don't use up the original.
         */
        va_copy(ap_copy, ap);
        result = vsnprintfT(&heap_buffer[0], buffer_size, format, ap_copy);
        va_end(ap_copy);

        if ((result >= 0) && (result < buffer_size)) {
            /* It fits */
            output.append(&heap_buffer[0], result);
            return;
        }
    }
}

void LogMessage(const char* _format, ...)
{
#ifdef UNICODE
    va_list args;
    va_start(args, _format);
    std::string message;
    StringAppendVT(_format, args, message);
    va_end(args);
    // 单字符 转 宽字符
    std::wstring messageW;
    messageW = ui::StringConvert::MBCSToUnicode(message);

    if(sLogCallback != nullptr){
      sLogCallback(LogLevel::Info, messageW);
    }

#else
    va_list args;
    va_start(args, _format);
    std::string message;
    StringAppendVT(_format, args, message);
    va_end(args);

    if(sLogCallback != nullptr){
        sLogCallback(LogLevel::Info, message);
    }
#endif
}

void LogMessage(const wchar_t* _format, ...)
{
#ifdef UNICODE
    va_list args;
    va_start(args, _format);
    std::wstring message;
    StringAppendVT(_format, args, message);
    va_end(args);

    if(sLogCallback != nullptr){
        sLogCallback(LogLevel::Info, message);
    }
#else
    va_list args;
    va_start(args, _format);
    std::wstring messageW;
    StringAppendVT(_format, args, messageW);
    va_end(args);

    // 宽字符 转 单字符
    std::string message;
    message = ui::StringConvert::UnicodeToMBCS(messageW);

    if(sLogCallback != nullptr){
        sLogCallback(LogLevel::Info, message);
    }
#endif
}



void LogMessageEx(LogLevel _logLevel, const char* _format, ...)
{
#ifndef _DEBUG
    if (_logLevel == LogLevel::Debug)
    {
        return;
    }
#endif
#ifdef UNICODE
    va_list args;
    va_start(args, _format);
    std::string message;
    StringAppendVT(_format, args, message);
    va_end(args);

    // 单字符 转 宽字符
    std::wstring messageW;
    messageW = ui::StringConvert::MBCSToUnicode(message);

    if(sLogCallback != nullptr){
        sLogCallback(_logLevel, messageW);
    }

#else
    va_list args;
    va_start(args, _format);
    std::string message;
    StringAppendVT(_format, args, message);
    va_end(args);

    if(sLogCallback != nullptr){
        sLogCallback(_logLevel, message);
    }
#endif
}

void LogMessageEx(LogLevel _logLevel, const wchar_t* _format, ...)
{
#ifndef _DEBUG
    if (_logLevel == LogLevel::Debug)
    {
        return;
    }
#endif

#ifdef UNICODE
    va_list args;
    va_start(args, _format);
    std::wstring message;
    StringAppendVT(_format, args, message);
    va_end(args);

    if(sLogCallback != nullptr){
        sLogCallback(_logLevel, message);
    }
#else
    va_list args;
    va_start(args, _format);
    std::wstring messageW;
    StringAppendVT(_format, args, messageW);
    va_end(args);

    // 宽字符 转 单字符
    std::string message;
    message = ui::StringConvert::UnicodeToMBCS(messageW);

    if(sLogCallback != nullptr){
        sLogCallback(_logLevel, message);
    }
#endif
}


DString GetFormatDate() {
    return GetFormatDate(_T("%Y-%m-%d %H:%M:%S"));
}

DString GetFormatDate(const DString& _strDateFormat) {
    const std::time_t nowTimeT = std::time(nullptr);
    std::tm nowTime = {};
    localtime_s(&nowTime, &nowTimeT);
    std::stringstream ss;
    ss<<std::put_time(&nowTime,ui::StringConvert::TToMBCS(_strDateFormat).c_str());
    return ui::StringConvert::MBCSToT(ss.str());
}