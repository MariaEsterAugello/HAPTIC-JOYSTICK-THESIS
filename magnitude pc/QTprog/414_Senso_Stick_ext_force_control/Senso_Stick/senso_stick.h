#ifndef SENSO_STICK_H
#define SENSO_STICK_H

#include <QMainWindow>

#include <QByteArray>
#include <QtNetwork/QUdpSocket>
#include <QtNetwork/QHostAddress>
#include <QDebug>


#include <QString>
#include <QTimer>

#include "can.h"

//#include <windows.h>    //DWORD in Pcan_usb.h
#include <stdint.h>

namespace Ui {
class Senso_Stick;
}

class Senso_Stick : public QMainWindow
{
    Q_OBJECT

public:
    explicit Senso_Stick(QWidget *parent = 0);
    ~Senso_Stick();

    DWORD can_init_reply;
    DWORD can_status_reply;
    DWORD can_communication_test_reply;
    //CAN stuff

    QTimer *CAN_send_messages_timer;
    QTimer *CAN_check_for_messages_timer;
    CAN *PCANUSB;

    void evaluate_SensoStick_CAN_message();

    void show_status( BYTE, BYTE);
    void show_error( BYTE, BYTE);

    int CAN_send_period_init;
    int CAN_check_messages_period_init;
    int CAN_send_period_reference;
    int CAN_check_messages_period_reference;
    int CAN_send_period_normal;
    int CAN_check_messages_period_normal;
    int CAN_send_period_base;
    int CAN_check_messages_period_base;

    unsigned int SensoStick_state_machine;
    void show_statemachine();
    double compute_motorencoder_position( BYTE, BYTE, BYTE);

    double mot_enc_pos_1;
    double mot_enc_pos_2;
    double mot_enc_pos_3;
    QString Motor_Enc_1_Pos_String;
    QString Motor_Enc_2_Pos_String;
    QString Motor_Enc_3_Pos_String;

    unsigned int last_CAN_error;    //need to store, because reference drive needs last communication test error
    double Encoder_Index;           //first needed in reference drive
    double compute_encoder_index( BYTE, BYTE);
    BYTE alive_counter_current_return_value;
    BYTE alive_counter_old_return_value;
    BYTE alive_counter_set_value;
    bool alive_counter_resetted;

    void init_Peak_CAN();

    unsigned int processed_axis;
    void start_Senso_Stick_CAN_communication();
    bool first_CAN_comm_message_1;
    bool first_CAN_comm_message_2;
    bool first_CAN_comm_message_3;

    void reference_drive_1();
    void reference_drive_2();
    void reference_drive_3();

    unsigned int ref_drive_axis;

    //left right: y
    quint16 y_Spring_Stiffness;
    byte y_spring_stiffness_byte7;
    byte y_spring_stiffness_byte8;

    quint16 y_Friction;
    byte y_friction_byte3;
    byte y_friction_byte4;

    quint8 y_damping_byte5;

    qint16 y_Torque;
    byte y_torque_byte1;
    byte y_torque_byte2;

    //forward-backward: x
    quint16 x_Spring_Stiffness;
    byte x_spring_stiffness_byte7;
    byte x_spring_stiffness_byte8;

    quint16 x_Friction;
    byte x_friction_byte3;
    byte x_friction_byte4;

    quint8 x_damping_byte5;

    qint16 x_Torque;
    byte x_torque_byte1;
    byte x_torque_byte2;

    //rotation: z
    quint16 z_Spring_Stiffness;
    byte z_spring_stiffness_byte7;
    byte z_spring_stiffness_byte8;

    quint16 z_Friction;
    byte z_friction_byte3;
    byte z_friction_byte4;

    quint8 z_damping_byte5;

    qint16 z_Torque;
    byte z_torque_byte1;
    byte z_torque_byte2;

    void convert_spring_stiffness_to_bytes(quint16, byte*, byte*);
    void convert_friction_to_bytes(quint16, byte*, byte*);
    void convert_torque_to_bytes(qint16, byte*, byte*);

    byte torque_limit_byte7;
    byte peak_torque_limit_byte8;

    TPCANMsg* received_CAN_message_Struct;

    void decode_digital_1msg(byte ,byte );
    void decode_digital_2msg(byte, byte);

    bool enable_button_state;
    unsigned int mini_stick_state;
    unsigned int flip_switch_state;
    bool front_button_state;

    void changeColorMyButton();

    void send_button_1_data_to_vehicle_model_PC(unsigned int);
    void send_button_2_data_to_vehicle_model_PC(unsigned int);
    //QByteArray conv_button_value_to_QByteArray(byte value);

    QUdpSocket *UDP_send_socket_to_vehicle_model_PC;
    QUdpSocket* UDP_receive_socket_from_vehicle_model_PC;

    void send_position_data_to_vehicle_model_PC(unsigned int, double);
    QByteArray conv_position_value_to_QByteArray(double value);
    bool paste_over_network;

    unsigned int alive_counter_off;
    unsigned int acknowledge_button_off;

    //left-right: y configuration
    quint16 last_y_Spring_Stiffness;       //0x14  //max 2500
    quint16 last_y_Friction;               //0x0A  //max 500
    quint8 last_y_damping_byte5;          //0x0A  //max 50
    qint16 last_y_Torque;         //0x00  //-32768 32767

    //forward-backward: x configuration
    quint16 last_x_Spring_Stiffness;
    quint16 last_x_Friction;
    quint8 last_x_damping_byte5;
    qint16 last_x_Torque;

    //rotation: z configuration
    quint16 last_z_Spring_Stiffness;
    quint16 last_z_Friction;
    quint8 last_z_damping_byte5;
    qint16 last_z_Torque;


private:
    Ui::Senso_Stick *ui;

public slots:

    void process_received_spring_friction_damping_torque_from_UDP();
    void switch_to_external_control();

    void send_PCANUSB_messages();
    void check_for_PCANUSB_messages();
    void start_PeakCAN_communication();
    void stop_CAN_communication();
    void check_Motorencoder();
    void prepare_reference_drive();
    void normal_mode_off();
    void normal_mode_ready();
    void normal_mode_on();
    void normal_mode_spring_damper();
    void stop_SensoStick();
    void check_SensoStick_CAN_communication();
    void clear_Status_textBrowser();
    void clear_Error_textBrowser();

    void set_x_Spring_Stiffness(int);
    void set_x_Friction(int);
    void set_x_Damping(int);
    void set_x_Torque(int);

    void set_y_Spring_Stiffness(int);
    void set_y_Friction(int);
    void set_y_Damping(int);
    void set_y_Torque(int);

    void set_z_Spring_Stiffness(int);
    void set_z_Friction(int);
    void set_z_Damping(int);
    void set_z_Torque(int);


    void set_Torque_Limit(int);
    void set_Peak_Torque_Limit(int);


};

#endif // SENSO_STICK_H
