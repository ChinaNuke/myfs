#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class FileSystem;
}
QT_END_NAMESPACE

class FileSystem : public QWidget {
    Q_OBJECT

   public:
    FileSystem(QWidget *parent = nullptr);
    ~FileSystem();
/*
   private slots:
    void on_btnCall_clicked();
*/
   private:
    Ui::FileSystem *ui;
};
#endif  // FILESYSTEM_H
