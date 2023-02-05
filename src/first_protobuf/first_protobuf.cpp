#include "xmsg_head.pb.h"
#include <iostream>
#include <fstream>

using namespace std;
using namespace xmsg;

int main()
{
    cout << "test protobuf!\n";
    XMsgHead msg1;
    msg1.set_msg_size(1024);
    msg1.set_msg_str("test0001");

    cout << "msg1 size = " << msg1.msg_size() << endl;
    cout << "msg1 msg_str = " << msg1.msg_str() << endl;

    // 序列化到 string
    cout << "序列化到 string：-----------------" << endl;
    string str1;
    msg1.SerializeToString(&str1);
    cout << "str1 size = " << str1.size() << endl;
    cout << "str1 = " << str1 << endl;

    // 序列化到 文件
    cout << "序列化到 文件：-----------------" << endl;
    ofstream ofs;
    ofs.open("test.txt", ios::binary); // 应以 二进制 方式写入，而不能 以 默认 ASCII 方式写入
    msg1.SerializePartialToOstream(&ofs);
    ofs.close();

    // 从文件 反序列化
    cout << "从文件 反序列化：-----------------" << endl;
    ifstream ifs;
    ifs.open("test.txt", ios::binary);
    XMsgHead msg2;
    cout << msg2.ParseFromIstream(&ifs) << endl;
    cout << "msg2 str = " << msg2.msg_str() << endl;

    // 从 string 中 反序列化
    cout << "从 string 中 反序列化：-----------------" << endl;
    msg2.set_msg_str("change msg2 str");
    string str2;
    msg2.SerializeToString(&str2);
    cout << "msg2 size = " << str2.size() << endl;
    cout << "msg2 str = " << str2 << endl;

    XMsgHead msg3;
    msg3.ParseFromArray(str2.data(), str2.size());
    cout << "msg3 size = " << msg3.msg_size() << endl;
    cout << "msg3 str = " << msg3.msg_str() << endl;

    //system("pause");
}
