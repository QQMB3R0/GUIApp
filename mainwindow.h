#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort/QSerialPort>
#include <QSerialPortInfo>
#include <QMessageBox>
#include <QPixmap>
#include <QTextCodec>
#include <QTimer>
#include <QDebug>
#include <QLineEdit>
#include <QThread>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void ChangeNameButton();

    void on_Connect_clicked();//подключение по порту

    void on_pushButton_clicked();//отправка сообщения

    void on_pushButton_2_clicked();//отчистка сообщения

    void on_checkBox_stateChanged(int arg1);//режим бегущей строки
    void setFirstLineTicker(bool status);
    void startFristLineTicker();
    void sendData();//отправка/смена позиции бег. строки
    void changePosition();//перевод на 2ю строку
    void on_checkBox_2_stateChanged(int arg1);//ожидание нажатия для перевода строки
private:
    Ui::MainWindow *ui;
    QSerialPort serialPort;
    QTimer timer;
    QString message;
    QString firstLineTh();
    QThread *thread = new QThread();
    char copyBack;
    int N = 0;
    bool firstLineTicker = false;
    int position;
    bool isCheck_1 = false;
    bool isCheck_2 = false;
    int shiftArray; //сдвиг строки от начала

    bool isConnected = false;

    void connect(bool x) {
        if (isConnected!=x)
        {
            isConnected = x;
            emit changeIsConnected();
        }
    }
signals:
    void changeIsConnected();
    void finished();
};
#endif // MAINWINDOW_H
