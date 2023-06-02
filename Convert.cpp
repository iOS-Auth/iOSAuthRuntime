#include "Convert.h"



// 转换字符数组至HEX
IOS_AUTH_RUNTIME_API std::string auth_convert_bytes_to_hex(const void* _Bytes, size_t _Size) noexcept
{
	if(_Bytes == nullptr || _Size == 0)
	{
		return {};
	}

	auto		vBuffer = (const char*)_Bytes;
	auto		vConvert = std::string();
	char 		vHex[3] = { 0 };
	for(auto vIndex = 0U; vIndex < _Size; ++vIndex)
	{
		std::sprintf(vHex, "%02X", (uint8_t)vBuffer[vIndex]);
		vConvert.push_back(vHex[0]);
		vConvert.push_back(vHex[1]);
	}
	return vConvert;
}

// 转换HEX至字符
IOS_AUTH_RUNTIME_API char auth_convert_hex_to_char(char _Hex) noexcept
{
	if((_Hex >= 'A') && (_Hex <= 'Z'))
	{
		return (char)(_Hex - 'A' + 10);
	}
	else if((_Hex >= 'a') && (_Hex <= 'z'))
	{
		return (char)(_Hex - 'a' + 10);
	}
	else if((_Hex >= '0') && (_Hex <= '9'))
	{
		return (char)(_Hex - '0');
	}
	return 0;
}

// 转换HEX至字符数组
IOS_AUTH_RUNTIME_API std::string auth_convert_hex_to_bytes(const void* _Hex, size_t _Size) noexcept
{
	if(_Hex == nullptr || _Size <= 1)
	{
		return {};
	}

	auto		vBuffer = (const char*)_Hex;
	auto		vConvert = std::string();
	for(auto vIndex = 0U; vIndex < _Size - 1; vIndex += 2)
	{
		auto 		vHexL = auth_convert_hex_to_char(vBuffer[vIndex + 0]);
		auto 		vHexR = auth_convert_hex_to_char(vBuffer[vIndex + 1]);
		auto		vChar = static_cast<char>(vHexL * 16 + vHexR);
		vConvert.push_back(vChar);
	}
	return vConvert;
}
