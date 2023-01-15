#pragma once

#ifdef _WIN32
#ifdef XCOM_EXPORTS
#define XCOM_API __declspec(dllexport)
#else
#define XCOM_API __declspec(dllimport)
#endif
#else
#define XCOM_API
#endif

#include <string>
XCOM_API std::string GetDirData(std::string path);

class XTools
{
};

