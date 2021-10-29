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
    QSerialPort1 -> setParity(QSerialPort :: NoParity);
    QSerialPort1 -> setFlowControl(QSerialPort :: NoFlowControl);

    connect(QSerialPort1, &QSerialPort::readyRead, this, &MainWindow::onQSerialPort1Rx);

    connect(QTimer1, &QTimer::timeout, this, &MainWindow::onQTimer1);

    header = 0; //Esperando la 'U'

    QTimer1 -> start(50);
    ui->lcdNumber->setStyleSheet("color: green;");
    ui->lcdNumber_2->setStyleSheet("color: red;");
    ui->lcdNumber_3->setStyleSheet("color: red;");
    ui->statusbar->showMessage("PUERTO: " + (QSerialPort1 -> portName()) + "//////BAUDRATE: " + QString::number(QSerialPort1 -> baudRate()) + "//////PARITY: NoParity");
    strTx = "";
    gameState = STARTING;
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

    if(QSerialPort1 -> isOpen()){
        switch(gameState){
            case STARTING:
                commandTx = GETLEDSTATE;
                onQSerialPort1Tx();
                if(commandRx == GETLEDSTATE){
                    for(int i = 0; i < 4; i++){
                        if(leds[i] == '0'){
                            ledDrawer(false, i);
                        }else if(leds[i] == '1'){
                            ledDrawer(true, i);
                        }
                    }
                }
                commandTx = GETBUTTONSTATE;
                onQSerialPort1Tx();
                if(commandRx == GETBUTTONSTATE){
                    for(int i = 0; i < 4; i++){
                        if(buttons[i] == '0'){
                            buttonDrawer(false, i);
                        }else if(buttons[i] == '1'){
                            buttonDrawer(true, i);
                        }
                    }
                }
                gameState = IDLE;
            break;
            case IDLE:
                if(commandRx == GETBUTTONSTATE){
                    for(int i = 0; i < 4; i++){
                        if(buttons[i] == '0'){
                            buttonDrawer(false, i);
                        }else if(buttons[i] == '1'){
                            buttonDrawer(true, i);
                        }
                    }
                }
            break;

            case GAMEON:
            break;

            case LOST:
            break;
        }
    }

    if(header){
        timeoutRx--;
        if(!timeoutRx){
            header = 0;
        }
    }
}



void MainWindow::ledDrawer(bool onoff, int led){
    QBrush brush;
    QPainter paint(QPaintBox1 -> getCanvas());
    int h = QPaintBox1 -> height();
    int w = QPaintBox1 -> width();
    int r = h/2;
    int xDrawPos;

    switch(led){
        case LED1:
            xDrawPos = -150;
        break;

        case LED2:
            xDrawPos = -21;
        break;

        case LED3:
            xDrawPos = 108;
        break;

        case LED4:
            xDrawPos = 237;
        break;
    }

    if(!onoff){
        brush.setColor(Qt::white);
        brush.setStyle(Qt::SolidPattern);
        paint.setBrush(brush);
        paint.drawEllipse((w/2) - r + xDrawPos, r - (r / 1.7) - 30, (2 * r) - 90, (2 * r) - 90);
    }
    if(onoff){
        brush.setColor(Qt::red);
        brush.setStyle(Qt::SolidPattern);
        paint.setBrush(brush);
        paint.drawEllipse((w/2) - r + xDrawPos, r - (r / 1.7) - 30, (2 * r) - 90, (2 * r) - 90);
    }
    QPaintBox1 -> update();
}


void MainWindow::buttonDrawer(bool onoff, int button){
    QBrush brush;
    QPainter paint(QPaintBox1 -> getCanvas());
    int h = QPaintBox1 -> height();
    int w = QPaintBox1 -> width();
    int r = h/2;
    int xDrawPos;

    switch(button){
        case BUTTON1:
            xDrawPos = -105;
        break;

        case BUTTON2:
            xDrawPos = 24;
        break;

        case BUTTON3:
            xDrawPos = 153;
        break;

        case BUTTON4:
            xDrawPos = 282;
        break;
    }
    if(!onoff){
        brush.setColor(Qt::gray);
        brush.setStyle(Qt::SolidPattern);
        paint.setBrush(brush);
        paint.drawEllipse(((w/2) - r + xDrawPos), r - (r / 1.7) + 90, (2 * r) - 180, (2 * r) - 180);
    }
    if(onoff){
        brush.setColor(Qt::blue);
        brush.setStyle(Qt::SolidPattern);
        paint.setBrush(brush);
        paint.drawEllipse(((w/2) - r + xDrawPos), r - (r / 1.7) + 90, (2 * r) - 180, (2 * r) - 180);
    }
    QPaintBox1 -> update();
}


void MainWindow::onQSerialPort1Tx(){
    uint8_t tx[12];
    if(QSerialPort1 -> isOpen()){
        tx[0] = 'U';
        tx[1] = 'N';
        tx[2] = 'E';
        tx[3] = 'R';
        tx[5] = ':';
        tx[6] = commandTx;
        if(commandTx == ALIVE || commandTx == GETLEDSTATE || commandTx == GETBUTTONSTATE){
            tx[4] = 2;
            tx[7] = 0;
            for(int i = 0; i < 7; i++){
                tx[7] ^= tx[i];
            }

/////////////////////////////////////////////////////////////////////////////////////////////////
           /* for(int i = 0; i < 8; i++){
                strTx = strTx + QString("%1").arg(tx[i], 2, 16, QChar('0')).toUpper();
            }
            ui->lineEdit->setText(strTx);
            strTx = "";*/
//////////////////////////////////////////////////////////////////////////////////////////////////

            //potato.jpg
            //QMessageBox::information(this, "PORT", "Conexion exitosa!", "OK");
            QSerialPort1 -> write((char *)tx, 8);
        }
        if(commandTx == SETLEDSTATE){
            tx[4] = 3;
            tx[8] = 0;
            tx[7] = payloadTx[0];
            for(int i = 0; i < 8; i++){
                tx[8] ^= tx[i];
            }

            //QMessageBox::information(this, "PORT", "Conexion exitosa!", "OK");
            QSerialPort1 -> write((char *)tx, 9);
        }
    }else{
        QMessageBox::information(this, "PORT", "El dispositivo no esta conectado");
    }
}


void MainWindow::onQSerialPort1Rx(){
    int count;
    uint8_t *buf;
    bool ok;

    count = QSerialPort1 -> bytesAvailable();
    if(count <= 0){
        return;
    }
    buf = new uint8_t [count];
    QSerialPort1 -> read((char *)buf, count);
    for (int i = 0; i < count; i++) {
        strRx = strRx + QString("%1").arg(buf[i], 2, 16, QChar('0')).toUpper();
    }

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

                //Identifico el comando recibido
                IDRx = strRx.mid(12);
                IDRx = IDRx.mid(0, 2);


                //Separo la cadena del payload de la cadena entera
                payloadRx = QString("%1").arg(strRx.midRef(14, strRx.length() - 18).toULongLong(&ok, 16), 4, 2, QChar('0'));

                if(nbytes != 1){
                    cks ^= buf[i];
                }
                nbytes--;
                if(!nbytes){
                    header = 6;
                    if(buf[i] == cks){
                        recieved = true;
                        ui->lineEdit->clear();
                        ui->lineEdit->setText(strRx);
                        strRx = "";
                    }else{
                        recieved = false;
                    }
                }
            break;
            default:
                header = 0;
        }
    }

    if(recieved){
        //Decodifico el comando recibido
        commandRx = IDRx.toUInt(&ok, 16);

        switch(commandRx){
            case ALIVE:

            break;
            case GETLEDSTATE:
                for(int i = 0; i < 4; i++){
                    leds[i] = payloadRx.mid(i, 1);
                }
            break;
            case SETLEDSTATE:

            break;
            case BUTTONEVENT:

            break;
            case GETBUTTONSTATE:
                for(int i = 0; i < 4; i++){
                    buttons[i] = payloadRx.mid(i, 1);
                }
            break;
        }
    }
    delete[] buf;
}

void MainWindow::on_radioButton_toggled(bool checked){

    if(checked){
        if(QSerialPort1 -> open(QSerialPort::ReadWrite)){
            commandTx = ALIVE;
            onQSerialPort1Tx();
        }else{
            ui -> radioButton_2 -> setChecked(true);
            QMessageBox::information(this, "PORT", "No se pudo abrir el puerto");
        }
    }else{
        if(QSerialPort1 -> isOpen()){
            QSerialPort1 -> close();
            ui -> lineEdit -> clear();
        }
    }
}
