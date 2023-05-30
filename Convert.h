#ifndef			_IOS_AUTH_RUNTIME_CONVERT_H_
#define			_IOS_AUTH_RUNTIME_CONVERT_H_

#include "Header.h"


// 转换字符数组至HEX
IOS_AUTH_RUNTIME_API std::string auth_convert_bytes_to_hex(const void* _Bytes, size_t _Size) noexcept;

// 转换HEX至字符
IOS_AUTH_RUNTIME_API char auth_convert_hex_to_char(char _Hex) noexcept;

// 转换HEX至字符数组
IOS_AUTH_RUNTIME_API std::string auth_convert_hex_to_bytes(const void* _Hex, size_t _Size) noexcept;


#endif
