#include "xtools.h"

#ifdef _WIN32
#include <io.h>
#endif

using namespace std;

// 目录遍历
XCOM_API std::string GetDirData(std::string path)
{
    string data = "";
#ifdef _WIN32
    // 存储文件信息
    _finddata_t file;
    string dirpath = path + "/*.*"; // 遍历目录下的所有文件

    // 目录上下文
    intptr_t dir = _findfirst(dirpath.c_str(), &file); // 找到第一个文件
    if (dir < 0)
    {
        return data;
    }
    do
    {
        if (file.attrib & _A_SUBDIR)
        {
            continue; // 暂时不处理子目录
        }
        char buf[1024] = {0};
        sprintf(buf, "%s,%u;", file.name, file.size);
        data += buf;
    } while (_findnext(dir, &file) == 0);
#else
#endif
    // 去掉结尾 ;
    if (!data.empty())
    {
        data = data.substr(0, data.size() - 1);
    }
    return data;
}
