#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include<QMainWindow>
#include<QDebug>
#include<QMenu>
#include<QAction>
#include<windows.h>
#include<QString>
#include<QPaintEvent>
#include<QFileDialog>
#include<QFile>
#include<QIODevice>
#include<QMessageBox>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QString FilePath;
    QMenu action_Menu;
    void list_service();
    void error_N(DWORD);
private slots:
    void leftMenu(const QPoint&);
    void clearTextB();
    void OpenSysfile();
    void Setupservice();
    void Openservice();
    void Closeservice();
    void Uninstallservice();
    void About();
    void Help();
    void END();
};

#endif // MAINWINDOW_H
