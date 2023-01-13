#include "xdisk_gui.h"
#include "xdisk_client.h"
#include <QMessageBox>
#include <QFileDialog>
#include <string>

using namespace std;

XDiskGUI::XDiskGUI(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    // 每个模块在使用时，都主动初始化
    // 不要将初始化放在构造函数中，较难控制
    XDiskClient::Get()->Init();
}

XDiskGUI::~XDiskGUI()
{
}

void XDiskGUI::Refresh()
{
    // 服务器路径 IP 端口
    int port = ui.portBox->value();
    string ip = ui.ipEdit->text().toStdString();
    string root = ui.pathEdit->text().toStdString();
    XDiskClient::Get()->set_port(port);
    XDiskClient::Get()->set_server_ip(ip);
    XDiskClient::Get()->set_server_root(root);
    XDiskClient::Get()->GetDir();

    //QMessageBox::information(this, "", "Refresh");

    // 1 连接服务器

    // 设置回调
}

void XDiskGUI::Upload()
{
    // 用户选择一个文件
    QString filename = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("请选择上传文件"));
    if (filename.isEmpty())
    {
        return;
    }

    // 插入到列表
    ui.filelistWidget->insertRow(0); // 插入在首行
    ui.filelistWidget->setItem(0, 0, new QTableWidgetItem(filename));
    ui.filelistWidget->setItem(0, 1, new QTableWidgetItem(tr("%1Byte").arg(100)));
}