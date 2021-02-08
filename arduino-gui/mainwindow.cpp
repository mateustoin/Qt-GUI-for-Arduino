#include "mainwindow.h"
#include "ui_mainwindow.h"

//Includes das bibliotecas que serão utilizadas para a comunicação serial
#include <QSerialPortInfo>
#include <QDebug>

#include <QPixmap>
#include <QIcon>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow){

    ui->setupUi(this);

    ui->lcdNumber->display("-------");

    // Garante que a barra do servo será de no mínimo 0 e máximo 180, limites do servo em graus
    this->servoValue = 0;
    ui->sliderServo->setMinimum(0);
    ui->sliderServo->setMaximum(180);
    ui->sliderServo->setValue(0);

    ui->servoPosition->setText("0º");
    ui->connectionStatus->setText(QString("<span style=\" font-size:18pt; color:red;\">Desconectado</span>"));

    serialBuffer = "";

    this->arduino_is_available = false; //Iniciando variável
    this->arduino_port_name = ""; //Iniciando variável com nada
    this->arduino = new QSerialPort; //Adicionado de acordo com o vídeo

    //ui->luminosidadeLed->setFixedWidth(40);
    QPixmap pixmapLed("/home/mateus/Documentos/QtProjects/led_control/led.jpg");
    QPixmap pixmapServo("/home/mateus/Documentos/QtProjects/led_control/servo.jpg");
    QPixmap pixmapRefreshIcon("/home/mateus/Documentos/QtProjects/led_control/refresh.png");

    QIcon refreshIcon(pixmapRefreshIcon);
    ui->refresh->setIcon(refreshIcon);

    QIcon buttonIcon(pixmapLed);
    ui->ledButton->setIcon(buttonIcon);
    ui->ledButton->setIconSize(pixmapLed.rect().size());

    ui->servoImage->setPixmap(pixmapServo);

    //int w = pixmap.width ();
    //int h = pixmap.height ();
    //qDebug() << w << " " << h;

    on_refresh_clicked();
}

MainWindow::~MainWindow(){
    sendSerialCommand("a0");

    // Configurando destrutor para fechar comunicação serial quando finalizar
    if (arduino->isOpen()){
        arduino->close();
    }

    delete ui;
}

void MainWindow::on_sliderLedAzul_valueChanged(int value)
{
    this->sendSerialCommand(QString("l%1").arg(value));
    int labelValue = value/255.0 * 100;
    ui->luminosidadeLed->setText(QString("%1%").arg(labelValue));
}

void MainWindow::sendSerialCommand(QString command)
{
    updateConnectionStatus();

    if (arduino->isWritable()){
        arduino->write(command.toStdString().c_str());
    }
}

void MainWindow::readSerial()
{
    //qDebug() << arduino->readLine();
    QStringList bufferSplit = serialBuffer.split(",");
    if (bufferSplit.length() < 3){
        serialData = arduino->readAll();
        serialBuffer += QString::fromStdString(serialData.toStdString());
    }else{
        //qDebug() << bufferSplit[1];
        this->updatePotenciometerData(bufferSplit[1]);
        serialBuffer = "";
    }
}

void MainWindow::updatePotenciometerData(QString data){
    updateConnectionStatus();

    ui->lcdNumber->display(data);
    int potProgressBar = data.toInt()/10;
    ui->progressBar->setValue(potProgressBar);
}

void MainWindow::updateConnectionStatus(){
    // Realiza veirficação se o Arduino está conectado
    on_refresh_clicked();
    if (this->devices.contains(arduino_port_name)){
        return;
    }else{
        arduino->close();
        ui->connectionStatus->setText(QString("<span style=\" font-size:18pt; color:red;\">Desconectado</span>"));
        QMessageBox::warning(this, "Arduino desconectado",
                             "A conexão com o Arduino foi rompida.");
    }
}

void MainWindow::on_ledButton_pressed(){
    sendSerialCommand("l255");
}

void MainWindow::on_ledButton_released(){
    sendSerialCommand("l0");
}

void MainWindow::keyPressEvent(QKeyEvent *event){
    // Verifica se a letra L foi clicada
    if(event->key() == Qt::Key_L){
        // Verifica se a letra está pressionada. Se estiver, retorna True
        if (event->isAutoRepeat()){
            return;
        }

        sendSerialCommand("l255");
        qDebug() << "Tecla L pressionada";
    }

    // Verifica se a letra L foi clicada
    if(event->key() == Qt::Key_B){
        // Verifica se a letra está pressionada. Se estiver, retorna True
        if (event->isAutoRepeat()){
            return;
        }

        sendSerialCommand("b255");
        qDebug() << "Tecla B pressionada";
    }

    // Verifica se a tecla arrowRight foi clicada
    if(event->key() == Qt::Key_Right){
        // Verifica se a letra está pressionada. Se estiver, retorna True
        if (event->isAutoRepeat()){
            if (this->servoValue >= 180){
                return;
            }
            this->servoValue = ui->sliderServo->value() + 1;
            ui->sliderServo->setValue(this->servoValue);
            ui->servoPosition->setText(QString("%1º").arg(this->servoValue));
            this->sendSerialCommand(QString("s%1").arg(this->servoValue));
            return;
        }
        qDebug() << "Tecla Arrow Right pressionada";
    }

    // Verifica se a tecla arrowLeft foi clicada
    if(event->key() == Qt::Key_Left){
        // Verifica se a letra está pressionada. Se estiver, retorna True
        if (event->isAutoRepeat()){
            if (this->servoValue <= 0){
                return;
            }
            this->servoValue = ui->sliderServo->value() - 1;
            ui->sliderServo->setValue(this->servoValue);
            ui->servoPosition->setText(QString("%1º").arg(this->servoValue));
            this->sendSerialCommand(QString("s%1").arg(this->servoValue));
            return;
        }
        qDebug() << "Tecla Arrow Left pressionada";
    }
}

void MainWindow::keyReleaseEvent(QKeyEvent *event){
    // Verifica se a tecla L foi clicada
    if(event->key() == Qt::Key_L){
        // Verifica se a letra está pressionada. Se estiver, retorna True
        if (event->isAutoRepeat()){
            return;
        }

        sendSerialCommand("l0");
        qDebug() << "Tecla L solta";
    }

    // Verifica se a tecla L foi clicada
    if(event->key() == Qt::Key_B){
        // Verifica se a letra está pressionada. Se estiver, retorna True
        if (event->isAutoRepeat()){
            return;
        }

        sendSerialCommand("b0");
        qDebug() << "Tecla B solta";
    }
}

void MainWindow::on_sliderServo_valueChanged(int value){
    updateConnectionStatus();
    this->sendSerialCommand(QString("s%1").arg(value));
    ui->servoPosition->setText(QString("%1º").arg(value));

    this->servoValue = value;
}

void MainWindow::on_refresh_clicked(){
    // Limpa stringlist e caixinha para atualizar com os dispositivos que serão lidos no USB
    this->devices.clear();
    ui->deviceList->clear();

    foreach(const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts()){ //Vendor e Product ID está relacionado a venda do hardware ou algo relacionado á identificação dos produtos
        //qDebug() << "Info: " << serialPortInfo.manufacturer();
        //PELO PRODUCT ID POR EXEMPLO CONSEGUIMOS DIFERENCIAR E IDENTIFICAR QUAL PLACA ESTÁ CONECTADA (UNO = 67, MEGA = 66)
        //qDebug() << "Tem vendor ID: " << serialPortInfo.hasVendorIdentifier();
        //if (serialPortInfo.hasVendorIdentifier()){
        //    qDebug() << "Vendor ID: " << serialPortInfo.vendorIdentifier();
        //}
        //qDebug() << "Tem product ID: " << serialPortInfo.hasProductIdentifier();
        //if (serialPortInfo.hasProductIdentifier()){
        //    qDebug() << "Product ID: " << serialPortInfo.productIdentifier();
        //}
        if (serialPortInfo.hasProductIdentifier() && serialPortInfo.hasVendorIdentifier()){
            this->devices.append(serialPortInfo.portName());
        }
    }

    ui->deviceList->addItems(this->devices);
}

void MainWindow::on_connectBoard_clicked(){
    if (ui->deviceList->count() <= 0){
        QMessageBox::warning(this, "Erro na porta",
                             "Não foi possívei identificar nenhum dispositivo conectado no computador. "
                             "Verifique as entradas e tente novamente!");
    }else{
        arduino_is_available = true; // Se passar por todas as verificações recebe true na verificação
        arduino_port_name = ui->deviceList->currentText(); // Se passar por todas as verificações recebe o nome da porta
        ui->connectionStatus->setText(QString("<span style=\" font-size:18pt; color:green;\">Conectado</span>"));

        arduino->setPortName(arduino_port_name); // Mudando o nome da porta para o que encontramos do arduino
        arduino->open(QSerialPort::ReadWrite); //Acessando arduino em modo de escrita para a placa, pois só iremos controlar os leds
        arduino->setBaudRate(QSerialPort::Baud9600);
        arduino->setDataBits(QSerialPort::Data8); // Arduino, 8 bits, dãã
        arduino->setParity(QSerialPort::NoParity); // Descobrir depois o que está relacionado a isso!!!! <<<<<<<<<<<
        arduino->setStopBits(QSerialPort::OneStop); // A opção half and stop é só para o windows
        arduino->setFlowControl(QSerialPort::NoFlowControl); // Descobrir pra que diachos isso serve também!!! <<<<<<<

        // Evento que será chamado toda vez que tiver dados disponíveis
        QObject::connect(arduino, SIGNAL(readyRead()), this, SLOT(readSerial()));
    }
}
