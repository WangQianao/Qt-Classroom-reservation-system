#include "managerwindow.h"
#include "ui_managerwindow.h"
#include <QMessageBox>
#include <QTextStream>
#include <QTextEdit>
ManagerWindow::ManagerWindow(Manager * man,QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ManagerWindow)
{
    ui->setupUi(this);
    this->setFixedSize(800,500);
    this->setWindowTitle("欢迎登陆机房预约系统");
    ui->stackedWidget->setCurrentIndex(0);
    //点击注销登录按钮后返回主界面
    connect(ui->exitButton,&QPushButton::clicked,[=](){
        this->hide();
        emit managerWindowBack();
    });
    //点击添加账号按钮后,切换到添加账号页面
    connect(ui->addpersonpushButton,&QPushButton::clicked,[=](){
               ui->stackedWidget->setCurrentIndex(1);

    });
    //点击查看账号按钮后,切换到查看账号页面
    connect(ui->checkPersonButton,&QPushButton::clicked,[=](){
               ui->stackedWidget->setCurrentIndex(2);

    });
    ui->stackedWidget_2->setCurrentIndex(0);
    ui->addSturadioButton->setChecked(true);
    //点击添加学生切换到添加学生页面
    connect(ui->addSturadioButton,&QRadioButton::clicked,[=](){
        ui->stackedWidget_2->setCurrentIndex(0);
    });
    //点击添加老师切换到添加老师页面
    connect(ui->addTearadioButton,&QRadioButton::clicked,[=](){
        ui->stackedWidget_2->setCurrentIndex(1);
    });
    //点击确认添加按钮添加账号
    connect(ui->confirmButton,&QPushButton::clicked,[=](){
        addPerson(man);
    });
    //点击添加账号界面的返回按钮返回到管理员界面
    connect(ui->exitpushButton_2,&QPushButton::clicked,[=](){
        ui->stackedWidget->setCurrentIndex(0);
    });
    //点击查看账号信息界面返回的按钮返回管理员界面
    connect(ui->exitcommandLinkButton,&QCommandLinkButton::clicked,[=](){
        ui->stackedWidget->setCurrentIndex(0);
    });
    //点击查看所有学生按钮查看所有学生
    connect(ui->checkallstupushButton,&QPushButton::clicked,[=](){
        showPerson(man,0);
    });
    //点击查看所有老师按钮查看所有老师
    connect(ui->checkallteapushButton,&QPushButton::clicked,[=](){
        showPerson(man,1);
    });
    //点击查看机房信息按钮查看机房信息
    connect(ui->checkComroomButton,&QPushButton::clicked,[=](){
        showComputer(man);
    });
    //点击清空预约文件按钮清空预约文件
    connect(ui->cleanOrderButton,&QPushButton::clicked,[=](){
        cleanFile();
    });
}
//清空预约记录
void ManagerWindow::cleanFile()
{
    QFile ofs( "order.txt");
    QMessageBox msgBox;
    if(!ofs.open(QIODevice::WriteOnly|QIODevice::Text))
    {
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setText("打开文件失败");
        msgBox.exec();
    }
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setText("预约记录已经清空!");
    msgBox.exec();
    ofs.close();

}
//添加账号
void ManagerWindow::addPerson(Manager * man)
{

    QString fileName; //操作文件名
    QString errorTip; //重复错误提示

   QFile ofs;//文件操作对象
    QMessageBox msgBox;
    if (ui->stackedWidget_2->currentIndex() == 0)
    {
        //添加的是学生
        fileName = "student.txt";
        errorTip = "学号重复，请重新输入";

    }
    else
    {
        fileName = "teacher.txt";
        errorTip = "职工号重复，请重新输入";

    }
    ofs.setFileName(fileName);
    //利用追加的方式 写文件
    if(!ofs.open(QIODevice::WriteOnly | QIODevice::Text|QIODevice::Append))
    {
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setText("打开文件失败");
        msgBox.exec();
        exit(0);
    }
    QString name;
    QString pwd; //密码
    if(ui->stackedWidget_2->currentIndex()==0)
    {
        name=ui->addstulineEdit->text();
        pwd=ui->addstupasswordlineEdit->text();
    }else
    {
        name=ui->addtealineEdit->text();
        pwd=ui->addteapasswordlineEdit->text();
    }
    if(name.isEmpty()||pwd.isEmpty())
    {
        msgBox.setText("输入的账号信息不完整,无法添加");
        msgBox.exec();
    }
    else
    {

        bool ret = man->checkRepeat(name,ui->stackedWidget_2->currentIndex() );
        if(!ret)
        {
            QTextStream out(&ofs);
            out << name <<"&"<< pwd<<"\n";
            msgBox.setText("添加成功");
            msgBox.exec();

        }else//有重复
        {
            msgBox.setIcon(QMessageBox::Critical);
            msgBox.setText(errorTip);
            msgBox.exec();
        }

        ofs.close();

        //调用初始化容器接口，从新获取文件中的数据
        man->initVector();
    }
    if(ui->stackedWidget_2->currentIndex()==0)
    {
        ui->addstulineEdit->clear();
        ui->addstupasswordlineEdit->clear();
    }else
    {
        ui->addtealineEdit->clear();
        ui->addteapasswordlineEdit->clear();
    }


}
void ManagerWindow::showComputer(Manager * man)
{
    QMainWindow * wid = new QMainWindow(this);
    wid->setWindowTitle("机房信息");
    wid->setFixedSize(320,500);
    QTextEdit * edit = new QTextEdit(wid);
    edit->setFixedSize(wid->width(),wid->height());
    QString s;
    QString t;
    foreach ( ComputerRoom com, man->vCom) {
        t = QString("机房编号：%1 机房最大容量：%2").arg(com.m_ComId).arg(com.m_MaxNum);
        s.append(t);
    }
     edit->append(s);
     wid->show();
     edit->show();



}
void ManagerWindow::showPerson(Manager * man,int type)
{

    QMainWindow * wid = new QMainWindow(this);
    wid->setWindowTitle("账号信息");
    wid->setFixedSize(320,500);
    QTextEdit * edit = new QTextEdit(wid);
    edit->setFixedSize(wid->width(),wid->height());
     QString s;
        if (type == 0)
        {
            //查看学生


              foreach ( Student* str, man->vStu) {
                  s.append(man->printStudent(str));
              }


        }
        else
        {
            //查看老师
            foreach ( Teacher* tea, man->vTea) {
                s.append(man->printTeacher(tea));
            }


        }

       edit->append(s);
        wid->show();
        edit->show();

}
ManagerWindow::~ManagerWindow()
{
    delete ui;
}
