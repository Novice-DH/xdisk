#include "xtools.h"

#ifdef _WIN32
#include <io.h>
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
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
    const char *dir = path.c_str();
    DIR *dp = 0;
    struct dirent *entry = 0; // 遍历文件的一个内部节点
    struct stat statbuf;
    dp = opendir(dir); // 打开路径
    if (dp == nullptr)
    {
        return data;
    }
    chdir(dir); // 切换到目标路径
    // 开始遍历路径
    char buf[1024] = {0};
    while ((entry = readdir(dp)) != nullptr)
    {
        lstat(entry->d_name, &statbuf);
        if (S_ISDIR(statbuf.st_mode)) // 略过目录
        {
            continue;
        }
        sprintf(buf, "%s,%ld;", entry->d_name, statbuf.st_size);
        data += buf;
    }

#endif
    // 去掉结尾 ;
    if (!data.empty())
    {
        data = data.substr(0, data.size() - 1);
    }
    return data;
}
