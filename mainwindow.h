#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort/QSerialPort>
#include <QTimer>
#include <QMessageBox>
#include <qpaintbox.h>
#include <stdlib.h>

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
    void onQSerialPort1Rx();
    void onQSerialPort1Tx();
    void onQTimer1();
    void on_radioButton_toggled(bool checked);

private:
    typedef enum{
        STARTING,
        IDLE,
        GAMEON,
        LOST,
    }_eGameState;
    Ui::MainWindow *ui;
    QSerialPort *QSerialPort1;
    QString strRx, IDRx, payloadRx, strTx;
    uint8_t bufRx[48], index, nbytes, cks, header, timeoutRx, command, gameState = IDLE, payloadTx[48];
    uint32_t puntaje;
    QTimer *QTimer1;
    QPaintBox *QPaintBox1;

    typedef enum{
           ACK=0x0D,
           ALIVE=0xF0,
           GETLEDSTATE=0xFB,
           SETLEDSTATE=0xFC,
           BUTTONEVENT=0xFA,
           GETBUTTONSTATE=0xFD
    }_eID;

    typedef enum{
        LED1,
        LED2,
        LED3,
        LED4,
    }_eLEDS;
};
#endif // MAINWINDOW_H
