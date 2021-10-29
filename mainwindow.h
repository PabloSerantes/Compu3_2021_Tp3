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
    void ledDrawer(bool onoff, int led);
    void buttonDrawer(bool onoff, int button);

private:
    typedef enum{
        STARTING,
        IDLE,
        GAMEON,
        LOST,
    }_eGameState;

    Ui::MainWindow *ui;
    QSerialPort *QSerialPort1;
    QString strRx, payloadRx, strTx, IDRx, leds[4], buttons[4];
    uint8_t bufRx[48], index, nbytes, cks, header, timeoutRx, commandTx, commandRx, gameState = IDLE, payloadTx[48];
    uint32_t puntaje;
    QTimer *QTimer1;
    QPaintBox *QPaintBox1;
    bool recieved;

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

    typedef enum{
        BUTTON1,
        BUTTON2,
        BUTTON3,
        BUTTON4
    }_eBUTTON;

};
#endif // MAINWINDOW_H
