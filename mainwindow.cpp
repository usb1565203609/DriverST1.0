#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->lineEdit->setFocusPolicy(Qt::NoFocus);
    list_service();
    this->setWindowFlags(Qt::WindowCloseButtonHint|Qt::WindowMinimizeButtonHint);
    this->setMaximumHeight(this->height());
    this->setMaximumWidth(this->width());
    this->setMinimumHeight(this->height());
    this->setMinimumWidth(this->width());
    ui->textBrowser_2->setContextMenuPolicy(Qt::CustomContextMenu);
    QObject::connect(ui->textBrowser_2,SIGNAL(customContextMenuRequested(const QPoint&)),this,SLOT(leftMenu(const QPoint&)));
    QObject::connect(ui->action,SIGNAL(triggered(bool)),this,SLOT(About()));
    QObject::connect(ui->actionbang,SIGNAL(triggered(bool)),this,SLOT(Help()));
    QObject::connect(ui->actiontui,SIGNAL(triggered(bool)),this,SLOT(END()));
    QObject::connect(ui->action_2,SIGNAL(triggered(bool)),this,SLOT(clearTextB()));
}

//添加右击菜单
void MainWindow::leftMenu(const QPoint&)
{
    action_Menu.addAction(ui->action_2);
    action_Menu.exec(QCursor::pos());
}

//清空选项事件
void MainWindow::clearTextB()
{
    ui->textBrowser_2->clear();
}
//刷新服务列表
void MainWindow::list_service()
{
    ui->textBrowser->clear();
    DWORD ArrayNumber=0;
    DWORD NEED=0;
    LPENUM_SERVICE_STATUS_PROCESS DATAs=NULL;
    SC_HANDLE OPM=OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);
    if(OPM==NULL)
    {
        ui->textBrowser->setText(tr("Output Error!"));
        CloseServiceHandle(OPM);
        return;
    }
    EnumServicesStatusEx(OPM,SC_ENUM_PROCESS_INFO,SERVICE_WIN32,SERVICE_STATE_ALL,(LPBYTE)DATAs,0,&NEED,&ArrayNumber,NULL,NULL);
    DATAs=(LPENUM_SERVICE_STATUS_PROCESS)malloc(NEED);
    ZeroMemory(DATAs,NEED);
    QString he="*";
    if(NEED!=0)
    {
        BOOL S=EnumServicesStatusEx(OPM,SC_ENUM_PROCESS_INFO,SERVICE_WIN32,SERVICE_STATE_ALL,(LPBYTE)DATAs,NEED,&NEED,&ArrayNumber,NULL,NULL);
        if(S)
            for(DWORD i=0;i<ArrayNumber;i++)
            {
                ui->textBrowser->append(tr("----------------------------------------------------"));
                ui->textBrowser->append(QString::fromStdWString(DATAs[i].lpServiceName));
            }
    }
    free(DATAs);
}

//打开选择驱动服务文件
void MainWindow::OpenSysfile()
{
    FilePath=QFileDialog::getOpenFileName(this,"sys","C:/","Driver file(*.sys);;service exe(*.exe)");
    ui->lineEdit->setText(FilePath);
}

//安装驱动程序
void MainWindow::Setupservice()
{
    if(ui->lineEdit->text().isEmpty()||ui->lineEdit_2->text().isEmpty())
    {
        QMessageBox::information(this,"提示","请输入驱动文件路径或服务名称!",QMessageBox::Ok,QMessageBox::Warning);
        return;
    }
    SC_HANDLE OPM=OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);
    if(OPM==NULL)
    {
        QMessageBox::information(this,tr("提示"),tr("SCManager打开失败!"),QMessageBox::Ok,QMessageBox::Warning);
        CloseServiceHandle(OPM);
        return;
    }
    SC_HANDLE CSE=CreateService
            (
                OPM,
                LPCWSTR(ui->lineEdit_2->text().toStdWString().c_str()),
                LPCWSTR(ui->lineEdit_2->text().toStdWString().c_str()),
                SERVICE_ALL_ACCESS,
                SERVICE_KERNEL_DRIVER,
                SERVICE_DEMAND_START,
                SERVICE_ERROR_IGNORE,
                LPCWSTR(ui->lineEdit->text().toStdWString().c_str()),
                NULL,NULL,NULL,NULL,NULL
            );
    DWORD NS=GetLastError();
    if(CSE==NULL)
    {
        if(NS==ERROR_ACCESS_DENIED)
            QMessageBox::information(this,tr("提示"),tr("权限不足，请以管理员权限打开!"),QMessageBox::Ok,QMessageBox::Warning);
        if(NS==ERROR_DUPLICATE_SERVICE_NAME)
            QMessageBox::information(this,tr("提示"),tr("当前名称服务已存在!"),QMessageBox::Ok,QMessageBox::Warning);
        if(NS==ERROR_INVALID_NAME)
            QMessageBox::information(this,tr("提示"),tr("服务名称无效!"),QMessageBox::Ok,QMessageBox::Warning);
        if(NS==ERROR_SERVICE_EXISTS)
            QMessageBox::information(this,tr("提示"),tr("已存在完全相同的服务!"),QMessageBox::Ok,QMessageBox::Warning);
        else
            QMessageBox::information(this,tr("提示"),tr("服务创建失败!"),QMessageBox::Ok,QMessageBox::Warning);
    }
    else
         ui->textBrowser_2->append(tr("服务安装成功!"));
    CloseServiceHandle(OPM);
    CloseServiceHandle(CSE);
    list_service();
    return;
}

 //加载驱动
void MainWindow::Openservice()
{
    if(ui->lineEdit_2->text().isEmpty())
    {
        QMessageBox::information(this,tr("提示"),tr("请输入服务名称!"),QMessageBox::Ok,QMessageBox::Warning);
        return;
    }
    SC_HANDLE OPM=OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);
    if(OPM==NULL)
    {
        QMessageBox::information(this,tr("提示"),tr("SCManager打开失败!"),QMessageBox::Ok,QMessageBox::Warning);
        CloseServiceHandle(OPM);
        return;
    }
    SC_HANDLE OSE=OpenService(OPM,LPCWSTR(ui->lineEdit_2->text().toStdWString().c_str()),SERVICE_START);
    if(OSE==NULL)
    {
        QMessageBox::information(this,tr("提示"),tr("服务打开失败!"),QMessageBox::Ok,QMessageBox::Warning);
         ui->textBrowser_2->append(tr("error"));
    }
    BOOL STO=StartService(OSE,NULL,NULL);
    DWORD retN=GetLastError();
    if(STO==FALSE)
    {
        error_N(retN);
    }
    else
        ui->textBrowser_2->append(tr("驱动加载成功!"));
    list_service();
    CloseServiceHandle(OPM);
    CloseServiceHandle(OSE);
}

//分析并输出错误码
void MainWindow::error_N(DWORD e)
{
    ui->textBrowser_2->append("加载失败!\n错误原因:");
    switch(e)
    {
        case ERROR_ACCESS_DENIED:
            ui->textBrowser_2->append("没有启动服务的访问权限");
            break;
        case ERROR_INVALID_HANDLE:
            ui->textBrowser_2->append("文件无效");
            break;
        case ERROR_PATH_NOT_FOUND:
            ui->textBrowser_2->append("当前路径下未找到文件");
            break;
        case ERROR_SERVICE_ALREADY_RUNNING:
            ui->textBrowser_2->append("当前服务已在运行");
            break;
        case ERROR_SERVICE_DATABASE_LOCKED:
            ui->textBrowser_2->append("服务数据库已被锁定");
            break;
        case ERROR_SERVICE_DEPENDENCY_DELETED:
            ui->textBrowser_2->append("当前服务依赖于不存在或者已删除的服务");
            break;
        case ERROR_SERVICE_DEPENDENCY_FAIL:
            ui->textBrowser_2->append("当前服务依赖于另一个未启动的服务");
            break;
        case ERROR_SERVICE_DISABLED:
            ui->textBrowser_2->append("此服务已被禁用");
            break;
        case ERROR_SERVICE_LOGON_FAILED:
            ui->textBrowser_2->append("当前账户没有权限启动服务");
            break;
        case ERROR_SERVICE_MARKED_FOR_DELETE:
            ui->textBrowser_2->append("当前服务已被标记为删除");
            break;
        case ERROR_SERVICE_NO_THREAD:
            ui->textBrowser_2->append("无法创建服务线程");
            break;
        case ERROR_SERVICE_REQUEST_TIMEOUT:
            ui->textBrowser_2->append("服务启动超时");
            break;
        default:
            ui->textBrowser_2->append("未知错误，可能为代码或程序错误");
            break;
    }
    return;
}

//关闭服务
//关闭停止服务
void MainWindow::Closeservice()
{
    if(ui->lineEdit_2->text().isEmpty())
    {
        QMessageBox::information(this,"提示","请输入服务名称!",QMessageBox::Ok,QMessageBox::Warning);
        return;
    }
    SC_HANDLE OPM=OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);
    if(OPM==NULL)
    {
        QMessageBox::information(this,tr("提示"),tr("SCManager打开失败！"),QMessageBox::Ok,QMessageBox::Warning);
        CloseServiceHandle(OPM);
        return;
    }
    SC_HANDLE OSE=OpenService(OPM,LPCWSTR(ui->lineEdit_2->text().toStdWString().c_str()),SERVICE_STOP);
    if(OSE==NULL)
    {
        QMessageBox::information(this,tr("提示"),tr("服务打开失败！"),QMessageBox::Ok,QMessageBox::Warning);
        ui->textBrowser_2->append(tr("error"));
    }
    SERVICE_STATUS svst={0};
    BOOL CLS=ControlService(OSE,SERVICE_CONTROL_STOP,&svst);
    if(CLS==FALSE)
    {
        QMessageBox::information(this,tr("提示"),tr("驱动停止失败！"),QMessageBox::Ok,QMessageBox::Warning);
        ui->textBrowser_2->append(tr("驱动停止失败！"));
    }
    else
        ui->textBrowser_2->append(tr("驱动停止成功！"));
    ui->textBrowser->clear();
    list_service();
    CloseServiceHandle(OPM);
    CloseServiceHandle(OSE);
}

//卸载驱动程序
void MainWindow::Uninstallservice()
{
    if(ui->lineEdit_2->text().isEmpty())
    {
        QMessageBox::information(this,"提示","请输入驱动文件路径或服务名称!",QMessageBox::Ok,QMessageBox::Warning);
        return;
    }
    SC_HANDLE OPM=OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);
    if(OPM==NULL)
    {
        QMessageBox::information(this,tr("提示"),tr("SCManager打开失败！"),QMessageBox::Ok,QMessageBox::Warning);
        CloseServiceHandle(OPM);
        return;
    }
    SC_HANDLE OSE=OpenService(OPM,LPCWSTR(ui->lineEdit_2->text().toStdWString().c_str()),SERVICE_STOP|DELETE);
    if(OSE==NULL)
    {
        QMessageBox::information(this,tr("提示"),tr("服务打开失败!"),QMessageBox::Ok,QMessageBox::Warning);
        ui->textBrowser_2->append(tr("error"));
    }
    BOOL DELS=DeleteService(OSE);
    if(DELS==FALSE)
    {
        QMessageBox::information(this,tr("提示"),tr("驱动卸载失败!"),QMessageBox::Ok,QMessageBox::Warning);
        ui->textBrowser_2->append(tr("驱动卸载失败!"));
    }
    else
        ui->textBrowser_2->append(tr("驱动卸载成功!"));
    ui->textBrowser->clear();
    list_service();
    CloseServiceHandle(OPM);
    CloseServiceHandle(OSE);
}

void MainWindow::About()
{
    QMessageBox::information
            (
                this,
                tr("关于"),
                tr("本程序由QQ1565203609编写，有疑问请加Q，本程序是以服务的形式安装加载驱动，所以理论上也可以安装.exe服务程序"),
                QMessageBox::Ok
             );
}

void MainWindow::Help()
{
    QMessageBox::information(this,tr("help"),tr("如有疑问请直接联系QQ：1565203609，\n提交程序bug,发送到邮箱：1565203609@qq.com"),QMessageBox::Ok);
    return;
}

void MainWindow::END()
{
    int yx=QMessageBox::question(this,tr("提示"),tr("确认退出?"),QMessageBox::Yes,QMessageBox::No);
    if(QMessageBox::Yes==yx)
    {
        this->close();
        exit(0);
    }
}
MainWindow::~MainWindow()
{
    delete ui;
}
