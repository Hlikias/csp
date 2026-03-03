#include "serverwidget.h"
#include "ui_serverwidget.h"

serverwidget::serverwidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::serverwidget)
{
    ui->setupUi(this);
    this->resize(1920,1080);
}

serverwidget::~serverwidget()
{
    delete ui;
}
