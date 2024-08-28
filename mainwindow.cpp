#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QMessageBox>
#include <QGridLayout>
#include <QSpinBox>
#include <QLineEdit>
#include <QTextCodec>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QSerialPort serial;

    //serial = new QSerialPort(this);
    auto infos = QSerialPortInfo::availablePorts();
    for (auto &info : infos) {
        ui->comboBox->addItem(info.portName());
    }
    // Инициализация переменных для бегущей строки
    position = 0;

    ChangeNameButton();
    QObject::connect(this, &MainWindow::changeIsConnected, this, &MainWindow::ChangeNameButton);
    // Устанавливаем скорость передачи
        ui->comboBox_baudRate->addItem(QStringLiteral("9600"), QSerialPort::Baud9600);
        ui->comboBox_baudRate->addItem(QStringLiteral("19200"), QSerialPort::Baud19200);
        ui->comboBox_baudRate->addItem(QStringLiteral("38400"), QSerialPort::Baud38400);
        ui->comboBox_baudRate->addItem(QStringLiteral("115200"), QSerialPort::Baud115200);
        ui->comboBox_baudRate->addItem(tr("Custom"));

             // Устанавливаем биты данных
        ui->comboBox_dataBits->addItem(QStringLiteral("5"), QSerialPort::Data5);
        ui->comboBox_dataBits->addItem(QStringLiteral("6"), QSerialPort::Data6);
        ui->comboBox_dataBits->addItem(QStringLiteral("7"), QSerialPort::Data7);
        ui->comboBox_dataBits->addItem(QStringLiteral("8"), QSerialPort::Data8);
        ui->comboBox_dataBits->setCurrentIndex(3);

             // Установить бит четности
        ui->comboBox_parity->addItem(tr("None"), QSerialPort::NoParity);
        ui->comboBox_parity->addItem(tr("Even"), QSerialPort::EvenParity);
        ui->comboBox_parity->addItem(tr("Odd"), QSerialPort::OddParity);
        ui->comboBox_parity->addItem(tr("Mark"), QSerialPort::MarkParity);
        ui->comboBox_parity->addItem(tr("Space"), QSerialPort::SpaceParity);

             // Установить стоп-бит
        ui->comboBox_stopBit->addItem(QStringLiteral("1"), QSerialPort::OneStop);
        ui->comboBox_stopBit->addItem(QStringLiteral("2"), QSerialPort::TwoStop);

             // Добавить управление потоком
        ui->comboBox_flowBit->addItem(tr("None"), QSerialPort::NoFlowControl);
        ui->comboBox_flowBit->addItem(tr("RTS/CTS"), QSerialPort::HardwareControl);
        ui->comboBox_flowBit->addItem(tr("XON/XOFF"), QSerialPort::SoftwareControl);

}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::ChangeNameButton()
{
    if (isConnected) {
        ui->Connect->setText("Отключить");
        QPixmap pix(":/img/img/1.png");
        ui->image->setPixmap(pix);
        ui->statusbar -> showMessage("Подключено");
    }
    else
    {
        ui->Connect->setText("Подключить");
        QPixmap pix(":/img/img/2.png");
        ui->image->setPixmap(pix);
        ui->statusbar -> showMessage("Не подключено");
    }

}

void MainWindow::setFirstLineTicker(bool status)
{
    this->firstLineTicker = status;

   // emit this->firstLineStatusTickerChanged(status);
};

void MainWindow::startFristLineTicker()
{
    this->setFirstLineTicker(true);
    thread->start();
}


void MainWindow::on_Connect_clicked()
{
    if (isConnected) {
        connect(false);
        serialPort.close();
        auto infos = QSerialPortInfo::availablePorts();
        ui->comboBox->clear();
        for (auto &info : infos) {
            ui->comboBox->addItem(info.portName());
        }
    }
    else {
    QString s = ui->comboBox->currentText();
    serialPort.setPortName(s);
    serialPort.setBaudRate(ui->comboBox_baudRate->currentText().toInt());
    serialPort.setDataBits(QSerialPort::Data8);
    serialPort.setParity(QSerialPort::NoParity);
    serialPort.setStopBits(QSerialPort::OneStop);
    connect(serialPort.open(QIODevice::ReadWrite));
    auto infos = QSerialPortInfo::availablePorts();
    ui->comboBox->clear();
    for (auto &info : infos) {
        ui->comboBox->addItem(info.portName());
    }
    }

}


void MainWindow::on_pushButton_clicked()
{


    if (!isConnected) return;
    if (!isCheck_1 && !isCheck_2) {

        QByteArray str;
        QString unicodeText = ui->lineEdit->text();
        QTextCodec* codec = QTextCodec::codecForName("IBM866");//изменение кодировки на cp866
        QByteArray cp866Text = codec->fromUnicode(unicodeText);
        str.append("\x1b" "\x40");//сброс дисплея
        str.append("\x1B" "\x61" "\x00");//выравнивание слева
        str.append("\x1B" "\x74" "\x06");//изменение кодировки
        str.append(cp866Text);
        serialPort.clear(QSerialPort::AllDirections);
        serialPort.write(str);

    }

}

void MainWindow::on_pushButton_2_clicked()
{
    QByteArray str ;
    str.append("\x1b" "\x40");//сброс дисплея

    serialPort.clear(QSerialPort::AllDirections);
    serialPort.write(str);
    if (timer.isActive()) {
           timer.stop();
       }
}

void MainWindow::sendData()
{
    QString unicodeText =  ui->lineEdit->text();
    QTextCodec* codec = QTextCodec::codecForName("IBM866");//изменение кодировки на cp866
    QByteArray cp866Text = codec->fromUnicode(unicodeText);
    QByteArray str (unicodeText.toLocal8Bit());

    str.append(QByteArray(20 - unicodeText.size(),' '));//расширение сообщение до 20 символов
    //пока поток открыт, сообщение движиться
    while (isCheck_1) {
    if (serialPort.isOpen()) {
      serialPort.write("\x0c"+str);//сдвиг только в первой строке
      serialPort.waitForBytesWritten();
      copyBack = str.back();//копирование символа в начало
      str.remove(20 - 1, 1);//удаление с конца
      str.push_front(copyBack);//вставка в начало
      QThread::sleep(1);
    }
  };

}

void MainWindow::changePosition()
{
    QByteArray str;
    QString unicodeText = ui->lineEdit->text();
    QTextCodec* codec = QTextCodec::codecForName("IBM866");//изменение кодировки на cp866
    QByteArray cp866Text = codec->fromUnicode(unicodeText);
    str.append("\x1b" "\x40");//сброс дисплея
    str.append("\x1B" "\x61" "\x00");//выравнивание слева
    str.append("\x0a");//перевод каретки на другую строку
    str.append("\x1B" "\x74" "\x06");//изменение кодировки
    str.append(cp866Text);
    serialPort.clear(QSerialPort::AllDirections);
    serialPort.write(str);


}

void MainWindow::on_checkBox_stateChanged(int arg1)
{
    if (arg1) {
        isCheck_1 = true;
        thread->start();

        QObject::connect(thread, &QThread::started, this, &MainWindow::sendData);
        QObject::connect(this, &MainWindow::finished, thread, &QThread::quit);

    }else if(!arg1) {
        emit finished();

        thread->quit();
        thread->wait();
    isCheck_1 = false;
}
}


void MainWindow::on_checkBox_2_stateChanged(int arg1)
{
    if (arg1) {
        isCheck_2 =true;
        QObject::connect(ui->pushButton,&QPushButton::clicked, this, &MainWindow::changePosition);
    }
    else {
        QObject::connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::on_pushButton_clicked);
        isCheck_2 = false;
    }
}

