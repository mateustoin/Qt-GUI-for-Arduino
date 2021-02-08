#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QDialog>
#include <QSerialPort>
#include <QMessageBox>

#include <QtGui>
#include <QtWidgets>
#include <QKeyEvent>

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
    void on_sliderLedAzul_valueChanged(int value);
    void readSerial();

    void on_ledButton_pressed();

    void on_ledButton_released();

    void on_sliderServo_valueChanged(int value);

    void on_refresh_clicked();

    void on_connectBoard_clicked();

protected:
    void keyPressEvent(QKeyEvent *);
    void keyReleaseEvent(QKeyEvent *);

private:
    Ui::MainWindow *ui;

    QSerialPort *arduino;

    QString arduino_port_name; // variável que vai receber o nome da porta que o arduino está conectado
    bool arduino_is_available; // booleano para comparar se o arduino ainda está conectado

    void sendSerialCommand(QString);

    //Inicializando atributos para leitura da porta serial
    QByteArray serialData;
    QString serialBuffer;

    QStringList devices;

    int servoValue;

    void updatePotenciometerData(QString);
    void updateConnectionStatus();
};
#endif // MAINWINDOW_H
