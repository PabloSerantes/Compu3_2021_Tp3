#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QPaintBox1 = new QPaintBox(0, 0, ui->widget);
    QTimer1 = new QTimer(this);
    QSerialPort1 = new QSerialPort(this);

    QSerialPort1 -> setPortName("COM3");
    QSerialPort1 -> setBaudRate(115200);
    QSerialPort1 -> setDataBits(QSerialPort :: Data8);
    QSerialPort1 -> setParity(QSerialPort :: EvenParity); // Para st no parity
    QSerialPort1 -> setFlowControl(QSerialPort :: NoFlowControl);

    connect(QSerialPort1, &QSerialPort::readyRead, this, &MainWindow::onQSerialPort1Rx);
    connect(QTimer1, &QTimer::timeout, this, &MainWindow::onQTimer1);

    header = 0; //Esperando la 'U'
    QSerialPort1 -> open(QSerialPort::ReadWrite);
    QTimer1 -> start(50);
    ui->lcdNumber->setStyleSheet("color: green;");
    ui->lcdNumber_2->setStyleSheet("color: red;");
    ui->lcdNumber_3->setStyleSheet("color: red;");
}

MainWindow::~MainWindow()
{
    delete QTimer1;
    delete QPaintBox1;
    delete ui;
    if(QSerialPort1 -> isOpen()){
        QSerialPort1 -> close();
    }
}

void MainWindow::onQTimer1(){
    if(header){
        timeoutRx--;
        if(!timeoutRx){
            header = 0;
        }
    }
}

void MainWindow::onQSerialPort1Rx(){
    int count;
    uint8_t *buf;

    count = QSerialPort1 -> bytesAvailable();
    if(count <= 0){
        return;
    }
    buf = new uint8_t [count];
    QSerialPort1 -> read((char *)buf, count);

   /* for (int i = 0; i < count; i++) {
        strRx = strRx + QString("%1").arg(buf[i], 2, 16, QChar('0')).toUpper();
    }*/

    for(int i=0; i < count; i++){
        switch (header) {
            case 0: //Esperando la 'U'
                if(buf[i] == 'U'){
                    header = 1;
                    timeoutRx = 3;
                }
            break;
            case 1: //N
                if(buf[i] == 'N'){
                    header = 2;
                }else{
                    header = 0;
                    i--;
                }
            break;
            case 2: //E
                if(buf[i] == 'E'){
                    header = 3;
                }else{
                    header = 0;
                    i--;
                }
            break;
            case 3: //R
                if(buf[i] == 'R'){
                    header = 4;
                }else{
                    header = 0;
                    i--;
                }
            break;
            case 4: //Cantidad de bytes
                nbytes = buf[i];
                header = 5;
            break;
            case 5: //Esperar el token ':'
                if(buf[i] == ':'){
                    header = 6;
                    cks = 'U' ^ 'N' ^ 'E' ^ 'R' ^ nbytes ^ ':';
                    index = 0;
                }else{
                    header = 6;
                    i--;
                }
            break;
            case 6: //ID + PAYLOAD + CKS
                bufRx[index++] = buf[i];
                if(nbytes != 1){
                    cks ^= buf[i];
                }
                nbytes--;
                if(!nbytes){
                    header = 6;
                    if(buf[i] == cks){
                        QMessageBox::information(this, "RX", "I'M ALIVE");
                    }
                }
            break;
            default:
                header = 0;
        }
    }
    delete[] buf;
}

void MainWindow::on_pushButton_clicked()
{
    uint8_t tx[12];
    if(QSerialPort1 -> isOpen()){
        tx[0] = 'U';
        tx[1] = 'N';
        tx[2] = 'E';
        tx[3] = 'R';
        tx[4] = 2;
        tx[5] = ':';
        tx[6] = 0xF0;
        for(int i = 0; i < 7; i++){
            tx[7] ^= tx[i];
        }
        QSerialPort1 -> write((char *)tx, 8);
        //strRx = "0x";
    }else{
        QMessageBox::information(this, "PORT", "El puerto está cerrado");
    }
}

void MainWindow::on_radioButton_toggled(bool checked)
{
    if(checked){
        ui -> comboBox_2 -> setDisabled(true);
    }else{
        ui -> comboBox_2 -> setDisabled(false);
    }
}

