#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtNetwork/QUdpSocket>
#include <QtNetwork/QHostAddress>
#include <QDebug>
#include <QString>
#include <QByteArray>
#include <QSlider>
#include <QList>
#include <QTimer>
#include "qJoyStick.h"
#include <minwindef.h>  //sonst gibt es in public.h 167 Fehlermeldungen
#include "public.h"
#include "settings.h"


#include <math.h>
#include <windows.h>


#define Puma_Center_Stick 1
#define Senso_Stick 2

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QUdpSocket* UDP_receive_position_from_Senso_Stick_socket;
    QUdpSocket* UDP_receive_button_1_from_Senso_Stick_socket;
    QUdpSocket* UDP_receive_button_2_from_Senso_Stick_socket;

    //IHA
    QUdpSocket* UDP_receive_iha_corrections_socket;

    QByteArray QBA_send_to_Senso_Stick;

    double mot_enc_1_value;
    double mot_enc_2_value;
    double mot_enc_3_value;

    int mot_enc_1_int;
    int mot_enc_2_int;
    int mot_enc_3_int;

    unsigned int sliders_maxval;
    double sliders_scaling_constant;

    unsigned int dial_maxval;
    double dial_scaling_constant;


    int my_round(double);


    bool enable_button_state;
    unsigned int mini_stick_state;
    unsigned int flip_switch_state;
    bool front_button_state;


    void changeColorMyButton();
    void decode_digital_1msg(unsigned int);
    void decode_digital_2msg(unsigned int);

    int scale_enc_1_to_int(double);
    int scale_enc_2_to_int(double);
    int scale_enc_3_to_int(double);


    //USB Joystick************************************************
    int sixteen_bit_pos;
    int sixteen_bit_neg;

    struct joydata
    {
        unsigned int number_axes;
        unsigned int number_btn;
        unsigned int number_hats;
        unsigned int number_balls;

        QList<int> axis;
        QList<bool> button;
        QList<int> hats;
        QList<int> balls;
    };

    // joystick data
    joydata joystick;

    // Available joystick count. Only set at initialization
    int joysavail;

    int yaw_control;

    struct vjoy_data_struct
    {
        int SensoStick_axis_left_right;
        int SensoStick_axis_forward_backward;
        int SensoStick_axis_twist;
        bool SensoStick_enable_button_state;        //0: unpressed 1: pressed
        unsigned int SensoStick_mini_stick_state;  //0: middle unpressed, 1: left, 2: right, 3: up, 4: down, 5: pressed
        unsigned int SensoStick_flip_switch_state;  //0: middle, 1: , 2: left, 2: right
        bool SensoStick_front_button_state;         //0: unpressed 1: pressed

        int USB_Center_Stick_left_right;
        int USB_Center_Stick_forward_backward;
        int USB_Collective_up_down;
        int USB_Collective_power_twist;
        int USB_Pedals; //..

        bool USB_Center_Stick_button_front;
        bool USB_Center_Stick_button_top;
        bool USB_Center_Stick_button_middle;
        bool USB_Center_Stick_button_down;
        int USB_Center_Stick_hat;
        int USB_Collectice_switch;
        bool USB_Collective_button_left;
        bool USB_Collective_button_right;

    };

    vjoy_data_struct vjoy_data;

    unsigned int DevID;
    unsigned int iInterface;
    BYTE id;
    JOYSTICK_POSITION_V2 iReport;
    PVOID pPositionMessage;
    int vJoy_DevID;
    bool vJoy_device_acquired;
    void get_VJoy_information();
    void check_if_vJoy_dll_matches_driver();
    void acquire_VJoy_device();
    void reset_VJoy_Interface();
    LONG convert_to_VJoy(int, int);

    QTimer *update_vJoy_timer;

    int Stick_Device;
    int convert_PUMA_POV_hat_to_Vjoy(int);
    int convert_Senso_Stick_POV_hat_to_Vjoy(int);

    int trim_value_yaw;
    int trim_value_yaw_stepsize;
    int trim_value_yaw_max;

    int trim_value_roll;
    int trim_value_roll_stepsize;
    int trim_value_roll_max;

    int trim_value_pitch;
    int trim_value_pitch_stepsize;
    int trim_value_pitch_max;

    int intmax;
    int intmin;

    //**************************************************************
    //IHA corrections
    int iha_correction_roll;
    int iha_correction_pitch;
    int iha_correction_yaw;
    QTimer *iha_watchdog_timer;
    void iha_watchdog_timer_inc();
    int iha_watchdog;


    //**************************************************************

public slots:


    void process_UDP_Senso_Stick_position_message();
    void process_UDP_Senso_Stick_button_1_message();
    void process_UDP_Senso_Stick_button_2_message();
    //IHA
    void process_UDP_iha_corrections_message();
    //vJoy
    void update_vJoy_data();
    void switch_to_Senso_Stick();
    void switch_to_Puma_Center_Stick();
    void handle_yaw_control();
    //void handle_yaw_by_Puma_Center_Stick();


private:
    Ui::MainWindow *ui;


//USB Joystick********************************************************
    QJoystick *joy;

    void init_joystick();
    QTimer *data_timer;


private slots:

    void pollJoystick();



};
//********************************************************************



#endif // MAINWINDOW_H
