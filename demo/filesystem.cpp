#include "filesystem.h"

#include "ui_filesystem.h"
/*
extern "C" {
#include "peng.h"
}
*/

FileSystem::FileSystem(QWidget *parent)
    : QWidget(parent), ui(new Ui::FileSystem) {
    ui->setupUi(this);
}

FileSystem::~FileSystem() { delete ui; }

/*
void FileSystem::on_btnCall_clicked() {
    QByteArray inputArray = ui->inputLine->text().toLatin1();
    char *input = inputArray.data();
    ui->outputLine->setText(QString(QLatin1String(amazing(input))));
}
*/
