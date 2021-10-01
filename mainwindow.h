#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort/QSerialPort>
#include <QTimer>
#include <QMessageBox>
#include <qpaintbox.h>

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
    void on_pushButton_clicked();
    void onQSerialPort1Rx();
    void onQTimer1();

    void on_radioButton_toggled(bool checked);

private:
    Ui::MainWindow *ui;
    QSerialPort *QSerialPort1;
   // QString strRx;
    uint8_t bufRx[48], index, nbytes, cks, header, timeoutRx;
    QTimer *QTimer1;
    QPaintBox *QPaintBox1;
};
#endif // MAINWINDOW_H
