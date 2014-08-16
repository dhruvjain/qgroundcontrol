#include "QGCViewModeSelection.h"
#include "ui_QGCViewModeSelection.h"
#include "QGC.h"
#include "MainWindow.h"
#include<QString>
#include <string.h>

QGCViewModeSelection::QGCViewModeSelection(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QGCViewModeSelection),
    selected(false)
{
    ui->setupUi(this);
    socket =new QUdpSocket(this);


    connect(ui->viewModeGeneric, SIGNAL(clicked()), this, SLOT(selectGeneric()));
    connect(ui->viewModeAR, SIGNAL(clicked()), this, SLOT(selectWifi()));
    connect(ui->viewModePX4, SIGNAL(clicked()), this, SLOT(selectPX4()));
    connect(ui->viewModeAPM, SIGNAL(clicked()), this, SLOT(selectAPM()));
    connect(ui->notAgainCheckBox, SIGNAL(clicked(bool)), this, SIGNAL(settingsStorageRequested(bool)));
    connect(ui->pushButton,SIGNAL(clicked(bool)),this,SLOT(selectlogin()));
    connect(socket, SIGNAL(readyRead()),this, SLOT(readPendingDatagrams()));
    connect(ui->pushButton_2,SIGNAL(clicked(bool)),this,SLOT(selectuav()));
    connect(ui->pushButton_3,SIGNAL(clicked(bool)),this,SLOT(goback()));


}

QGCViewModeSelection::~QGCViewModeSelection()
{
    delete ui;
}

enum MainWindow::CUSTOM_MODE QGCViewModeSelection::waitForInput() {
    while (!selected)
        QGC::SLEEP::msleep(200);

    return mode;
}

void QGCViewModeSelection::selectGeneric() {
    emit customViewModeSelected(MainWindow::CUSTOM_MODE_NONE);
    mode = MainWindow::CUSTOM_MODE_NONE;
    selected = true;
}

void QGCViewModeSelection::selectWifi() {
    emit customViewModeSelected(MainWindow::CUSTOM_MODE_WIFI);
    mode = MainWindow::CUSTOM_MODE_WIFI;
    selected = true;
}

void QGCViewModeSelection::selectPX4() {
    emit customViewModeSelected(MainWindow::CUSTOM_MODE_PX4);
    mode = MainWindow::CUSTOM_MODE_PX4;
    selected = true;
}

void QGCViewModeSelection::selectAPM() {
    emit customViewModeSelected(MainWindow::CUSTOM_MODE_APM);
    mode = MainWindow::CUSTOM_MODE_APM;
    selected = true;
}
void QGCViewModeSelection::selectlogin(){
    serverip=ui->lineEdit0->text();
     qDebug()<<serverip;


    socket->bind(8080,QUdpSocket::ShareAddress);


    QByteArray Data;
    Data.append(ui->lineEdit->text());

    socket->writeDatagram(Data,QHostAddress(serverip),8080);



    ui->frame->setHidden(true);


}

void QGCViewModeSelection::readPendingDatagrams(){

    while (socket->hasPendingDatagrams()) {

       qDebug()<<"reading datagram \n";

       QByteArray datagram;
       datagram.resize(socket->pendingDatagramSize());
       QHostAddress sender;
       quint16 senderPort;

       socket->readDatagram(datagram.data(), datagram.size(),
                                   &sender, &senderPort);

       QString noofuavs(datagram);
       nuavs=noofuavs.split(" ")[0].toInt();
       qDebug()<<noofuavs;
       ui->lineEdit_3->setText(noofuavs);
   }



}

void QGCViewModeSelection::selectuav(){

    QByteArray Data;
    QString qstr=ui->lineEdit0_2->text();
    int n;

    if (qstr.trimmed().isEmpty()){
       QMessageBox::information(
           this,
           tr("UAV INFO"),
           tr("Please Enter some UAV ID") );
       return;
   }
   else{
    n=qstr.split(" ")[0].toInt();
    if(n>nuavs){
        QMessageBox::information(
            this,
            tr("UAV INFO"),
            tr("Please Enter valid ID") );
        return;
    }

   else{
    Data.append(qstr);
    socket->writeDatagram(Data,QHostAddress(serverip),8080);
    }
   }
}

void QGCViewModeSelection::goback(){

     ui->frame->setHidden(false);


}













