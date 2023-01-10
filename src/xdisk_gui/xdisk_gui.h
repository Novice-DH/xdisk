#pragma once

#include <QtWidgets/QWidget>
#include "ui_xdisk_gui.h"

class XDiskGUI : public QWidget
{
    Q_OBJECT

public:
    XDiskGUI(QWidget *parent = nullptr);
    ~XDiskGUI();
public slots:
    void Refresh();
    void Upload();
private:
    Ui::XDiskGUIClass ui;
};
