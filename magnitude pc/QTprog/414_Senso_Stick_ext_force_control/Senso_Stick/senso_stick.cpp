#include "senso_stick.h"
#include "ui_senso_stick.h"

Senso_Stick::Senso_Stick(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Senso_Stick)
{
    ui->setupUi(this);


    received_CAN_message_Struct= new(TPCANMsg);



    //left-right: y configuration
    y_Spring_Stiffness= 100;       //0x14  //max 2500
    y_Friction= 10;               //0x0A  //max 500
    y_damping_byte5= 10;          //0x0A  //max 50
    y_Torque= 00;         //0x00  //-32768 32767

    //forward-backward: x configuration
    x_Spring_Stiffness= 100;
    x_Friction= 10;
    x_damping_byte5= 10;
    x_Torque= 00;

    //rotation: z configuration
    z_Spring_Stiffness= 100;
    z_Friction= 10;
    z_damping_byte5= 10;
    z_Torque= 00;

    //general Torque settings
    torque_limit_byte7= 20;       //0x14  //max. 255

    //peak Torque settings
    peak_torque_limit_byte8= 20;  //0x14  //max. 100



    CAN_send_period_init = 20;
    CAN_check_messages_period_init = 10;
    CAN_send_period_reference = 10;
    CAN_check_messages_period_reference = 5;
    CAN_send_period_normal = 4;            //20    //10
    CAN_check_messages_period_normal = 4;  //10    //10
    CAN_send_period_base = 4;               //4 2
    CAN_check_messages_period_base = 2;     //2 1


    SensoStick_state_machine=0;
    show_statemachine();



    CAN_send_messages_timer = new QTimer();
    connect(CAN_send_messages_timer, SIGNAL(timeout()), this, SLOT(send_PCANUSB_messages()));


    CAN_check_for_messages_timer = new QTimer();
    connect(CAN_check_for_messages_timer, SIGNAL(timeout()), this, SLOT(check_for_PCANUSB_messages()));


    ui->Peak_CAN_start_label->setText("Peak CAN");
    ui->Senso_Stick_label->setText("SensoStick");

    ui->CAN_init_label->setText("CAN init:");
    ui->CAN_status_label->setText("CAN status:");
    ui->CAN_communication_test_label->setText("CAN communication:");


    ui->CAN_init_checkBox->setDisabled(true);
    ui->CAN_init_checkBox->setChecked(false);

    ui->CAN_status_checkBox->setDisabled(true);
    ui->CAN_status_checkBox->setChecked(false);

    ui->CAN_communication_test_1_checkBox->setDisabled(true);
    ui->CAN_communication_test_1_checkBox->setChecked(false);
    ui->CAN_communication_test_2_checkBox->setDisabled(true);
    ui->CAN_communication_test_2_checkBox->setChecked(false);
    ui->CAN_communication_test_3_checkBox->setDisabled(true);
    ui->CAN_communication_test_3_checkBox->setChecked(false);

    ui->CAN_init_Error_label->setText("");
    ui->CAN_status_Error_label->setText("");
    ui->CAN_communication_Error_1_label->setText("");
    ui->CAN_communication_Error_2_label->setText("");
    ui->CAN_communication_Error_3_label->setText("");

    ui->Start_pushButton->setText("1. Start");
    ui->Stop_Peak_CANpushButton->setText("7. Stop PeakCAN");
    ui->Start_pushButton->setVisible(true);
    ui->Stop_Peak_CANpushButton->setVisible(false);

    connect(ui->Start_pushButton,SIGNAL(clicked(bool)), this, SLOT(start_PeakCAN_communication()));
    connect(ui->Stop_Peak_CANpushButton,SIGNAL(clicked(bool)), this, SLOT(stop_CAN_communication()));

    ui->Status_label->setText("Status: ");
    ui->Error_label->setText("Error: ");

    //ui->Status_message_label->setText("");
    //ui->Error_message_label->setText("");

    ui->State_label->setText("Statemachine: ");
    ui->Statemachine_label->setText("");

    ui->Check_Motorencoder_pushButton->setText("3. Check MotEnc");
    connect(ui->Check_Motorencoder_pushButton,SIGNAL(clicked(bool)), this, SLOT(check_Motorencoder()));

    mot_enc_pos_1= 0.0;
    ui->Motorencoderpos_1_label->setText("MotorEncoderpos 1: ");
    ui->Motorencoderpos_value_1_label->setText("");
    ui->Motorencoderpos_2_label->setText("MotorEncoderpos 2: ");
    ui->Motorencoderpos_value_2_label->setText("");
    ui->Motorencoderpos_3_label->setText("MotorEncoderpos 3: ");
    ui->Motorencoderpos_value_3_label->setText("");

    ui->ReferenceDrive_pushButton->setText("4. Ref. Drive");
    connect(ui->ReferenceDrive_pushButton,SIGNAL(clicked(bool)), this, SLOT(prepare_reference_drive()));

    last_CAN_error=0xFFFF;
    Encoder_Index=0x00;
    ui->Encoder_Index_1_label->setText("Encoder Index 1: ");
    ui->Encoder_Index_2_label->setText("Encoder Index 2: ");
    ui->Encoder_Index_3_label->setText("Encoder Index 3: ");
    ui->Encoder_Indexvalue_1_label->setText("");
    ui->Encoder_Indexvalue_2_label->setText("");
    ui->Encoder_Indexvalue_3_label->setText("");

    ui->Normal_Mode_Off_pushButton->setText("5.0. Normal Mode Off");
    ui->Normal_Mode_Ready_pushButton->setText("5.1. Normal Mode Ready");
    ui->Normal_Mode_On_pushButton->setText("5.2. Normal Mode On");
    ui->Normal_Mode_Spring_Damper_pushButton->setText("5.3. Normal Mode spring/damper");
    connect(ui->Normal_Mode_Off_pushButton,SIGNAL(clicked(bool)), this, SLOT(normal_mode_off()));
    connect(ui->Normal_Mode_Ready_pushButton,SIGNAL(clicked(bool)), this, SLOT(normal_mode_ready()));
    connect(ui->Normal_Mode_On_pushButton,SIGNAL(clicked(bool)), this, SLOT(normal_mode_on()));
    connect(ui->Normal_Mode_Spring_Damper_pushButton,SIGNAL(clicked(bool)), this, SLOT(normal_mode_spring_damper()));


    alive_counter_current_return_value=0;
    alive_counter_old_return_value=0;
    alive_counter_resetted=false;
    alive_counter_set_value=0;

    connect(ui->Stop_Senso_Stick_pushButton,SIGNAL(clicked(bool)), this, SLOT(stop_SensoStick()));

    processed_axis=1;

    ui->SensoStick_CAN_Comm_pushButton->setText("2. SensoSt. CAN Comm.");


    connect(ui->SensoStick_CAN_Comm_pushButton,SIGNAL(clicked(bool)), this, SLOT(check_SensoStick_CAN_communication()));
    first_CAN_comm_message_1=false;
    first_CAN_comm_message_2=false;
    first_CAN_comm_message_3=false;

    ui->Stop_Senso_Stick_pushButton->setText("6. Stop SensoStick");


    ui->Status_textBrowser->setText("status messages:");
    ui->Error_textBrowser->setText("error messages:");

    SensoStick_state_machine=0;
    show_statemachine();

    ui->Clear_Status_pushButton->setText("Clear Status Browser");
    ui->Clear_Error_pushButton->setText("Clear Error Browser");
    connect(ui->Clear_Status_pushButton,SIGNAL(clicked(bool)), this, SLOT(clear_Status_textBrowser()));
    connect(ui->Clear_Error_pushButton,SIGNAL(clicked(bool)), this, SLOT(clear_Error_textBrowser()));

    ref_drive_axis=0;

    ui->Enable_Button_label->setText("");

    //ui->Start_pushButton->setDisabled(true);
    ui->SensoStick_CAN_Comm_pushButton->setDisabled(true);
    ui->Check_Motorencoder_pushButton->setDisabled(true);
    ui->ReferenceDrive_pushButton->setDisabled(true);
    ui->Normal_Mode_Off_pushButton->setDisabled(true);
    ui->Normal_Mode_Ready_pushButton->setDisabled(true);
    ui->Normal_Mode_On_pushButton->setDisabled(true);
    ui->Normal_Mode_Spring_Damper_pushButton->setDisabled(true);
    ui->Stop_Senso_Stick_pushButton->setDisabled(true);



    //left-right: y configuration
    ui->y_Spring_Stiffness_label->setText("Spring stiffness:");

    convert_spring_stiffness_to_bytes(y_Spring_Stiffness, &y_spring_stiffness_byte7, &y_spring_stiffness_byte8);

    ui->y_Spring_Stiffness_horizontalSlider->setMaximum(2500);
    ui->y_Spring_Stiffness_horizontalSlider->setMinimum(0);
    ui->y_Spring_Stiffness_horizontalSlider->setValue((quint16)(y_Spring_Stiffness));
    connect(ui->y_Spring_Stiffness_horizontalSlider,SIGNAL(valueChanged(int)), this, SLOT(set_y_Spring_Stiffness(int)));

    ui->y_Friction_label->setText("Friction:");

    convert_friction_to_bytes(y_Friction, &y_friction_byte3, &y_friction_byte4);

    ui->y_Friction_horizontalSlider->setMaximum(500);
    ui->y_Friction_horizontalSlider->setMinimum(0);
    ui->y_Friction_horizontalSlider->setValue((int)(y_Friction));
    connect(ui->y_Friction_horizontalSlider,SIGNAL(valueChanged(int)), this, SLOT(set_y_Friction(int)));

    ui->y_Damping_label->setText("Damping:");


    ui->y_Damping_horizontalSlider->setMaximum(50);
    ui->y_Damping_horizontalSlider->setMinimum(0);
    ui->y_Damping_horizontalSlider->setValue(y_damping_byte5);
    connect(ui->y_Damping_horizontalSlider,SIGNAL(valueChanged(int)), this, SLOT(set_y_Damping(int)));

    ui->y_Torque_label->setText("Torque:");

    convert_torque_to_bytes(y_Torque, &y_torque_byte1, &y_torque_byte2);

    ui->y_Torque_horizontalSlider->setMaximum(32767);
    ui->y_Torque_horizontalSlider->setMinimum(-32768);
    ui->y_Torque_horizontalSlider->setValue(y_Torque);
    connect(ui->y_Torque_horizontalSlider,SIGNAL(valueChanged(int)), this, SLOT(set_y_Torque(int)));



    //forward-backward: x configuration
    ui->x_Spring_Stiffness_label->setText("Spring stiffness:");

    convert_spring_stiffness_to_bytes(x_Spring_Stiffness, &x_spring_stiffness_byte7, &x_spring_stiffness_byte8);

    ui->x_Spring_Stiffness_horizontalSlider->setMaximum(2500);
    ui->x_Spring_Stiffness_horizontalSlider->setMinimum(0);
    ui->x_Spring_Stiffness_horizontalSlider->setValue(x_Spring_Stiffness);
    connect(ui->x_Spring_Stiffness_horizontalSlider,SIGNAL(valueChanged(int)), this, SLOT(set_x_Spring_Stiffness(int)));

    ui->x_Friction_label->setText("Friction:");

    convert_friction_to_bytes(x_Friction, &x_friction_byte3, &x_friction_byte4);

    ui->x_Friction_horizontalSlider->setMaximum(500);
    ui->x_Friction_horizontalSlider->setMinimum(0);
    ui->x_Friction_horizontalSlider->setValue(x_Friction);
    connect(ui->x_Friction_horizontalSlider,SIGNAL(valueChanged(int)), this, SLOT(set_x_Friction(int)));

    ui->x_Damping_label->setText("Damping:");


    ui->x_Damping_horizontalSlider->setMaximum(50);
    ui->x_Damping_horizontalSlider->setMinimum(0);
    ui->x_Damping_horizontalSlider->setValue(x_damping_byte5);
    connect(ui->x_Damping_horizontalSlider,SIGNAL(valueChanged(int)), this, SLOT(set_x_Damping(int)));

    ui->x_Torque_label->setText("Torque:");

    convert_torque_to_bytes(x_Torque, &x_torque_byte1, &x_torque_byte2);

    ui->x_Torque_horizontalSlider->setMaximum(32767);
    ui->x_Torque_horizontalSlider->setMinimum(-32768);
    ui->x_Torque_horizontalSlider->setValue(x_Torque);
    connect(ui->x_Torque_horizontalSlider,SIGNAL(valueChanged(int)), this, SLOT(set_x_Torque(int)));



    //rotation: z configuration
    ui->z_Spring_Stiffness_label->setText("Spring stiffness:");

    convert_spring_stiffness_to_bytes(z_Spring_Stiffness, &z_spring_stiffness_byte7, &z_spring_stiffness_byte8);

    ui->z_Spring_Stiffness_horizontalSlider->setMaximum(2500);
    ui->z_Spring_Stiffness_horizontalSlider->setMinimum(0);
    ui->z_Spring_Stiffness_horizontalSlider->setValue(z_Spring_Stiffness);
    connect(ui->z_Spring_Stiffness_horizontalSlider,SIGNAL(valueChanged(int)), this, SLOT(set_z_Spring_Stiffness(int)));

    ui->z_Friction_label->setText("Friction:");

    convert_friction_to_bytes(z_Friction, &z_friction_byte3, &z_friction_byte4);

    ui->z_Friction_horizontalSlider->setMaximum(500);
    ui->z_Friction_horizontalSlider->setMinimum(0);
    ui->z_Friction_horizontalSlider->setValue(z_Friction);
    connect(ui->z_Friction_horizontalSlider,SIGNAL(valueChanged(int)), this, SLOT(set_z_Friction(int)));

    ui->z_Damping_label->setText("Damping:");


    ui->z_Damping_horizontalSlider->setMaximum(50);
    ui->x_Damping_horizontalSlider->setMinimum(0);
    ui->z_Damping_horizontalSlider->setValue(z_damping_byte5);
    connect(ui->z_Damping_horizontalSlider,SIGNAL(valueChanged(int)), this, SLOT(set_z_Damping(int)));

    ui->z_Torque_label->setText("Torque:");

    convert_torque_to_bytes(z_Torque, &z_torque_byte1, &z_torque_byte2);

    ui->z_Torque_horizontalSlider->setMaximum(32767);
    ui->z_Torque_horizontalSlider->setMinimum(-32768);
    ui->z_Torque_horizontalSlider->setValue(z_Torque);
    connect(ui->z_Torque_horizontalSlider,SIGNAL(valueChanged(int)), this, SLOT(set_z_Torque(int)));


    //general Torque settings
    ui->Torque_Limit_label->setText("Torque Limit (betw. endstops):");
    ui->Torque_Limit_value_label->setText("");

    ui->Torque_Limit_value_label->setText(QString::number(torque_limit_byte7));
    ui->Torque_Limit_horizontalSlider->setMaximum(255);
    ui->Torque_Limit_horizontalSlider->setMinimum(0);
    ui->Torque_Limit_horizontalSlider->setValue(torque_limit_byte7);
    connect(ui->Torque_Limit_horizontalSlider,SIGNAL(valueChanged(int)), this, SLOT(set_Torque_Limit(int)));

    //peak Torque settings
    ui->Peak_Torque_Limit_label->setText("Peak Torque Limit (endstops):");
    ui->Peak_Torque_Limit_value_label->setText("");

    ui->Peak_Torque_Limit_value_label->setText(QString::number(peak_torque_limit_byte8));
    ui->Peak_Torque_Limit_horizontalSlider->setMaximum(100);
    ui->Peak_Torque_Limit_horizontalSlider->setMinimum(0);
    ui->Peak_Torque_Limit_horizontalSlider->setValue(torque_limit_byte7);
    connect(ui->Peak_Torque_Limit_horizontalSlider,SIGNAL(valueChanged(int)), this, SLOT(set_Peak_Torque_Limit(int)));


    ui->General_Torque_label->setText("for all axes");

    enable_button_state= false;
    mini_stick_state=0;
    flip_switch_state=0;
    front_button_state=false;

    ui->Front_label->setText("Front");
    ui->Rear_label->setText("Rear");

    ui->Mini_Stick_down_radioButton->setAutoExclusive(false);
    ui->Mini_Stick_up_radioButton->setAutoExclusive(false);
    ui->Mini_Stick_left_radioButton->setAutoExclusive(false);
    ui->Mini_Stick_right_radioButton->setAutoExclusive(false);
    ui->Mini_Stick_middle_radioButton->setAutoExclusive(false);
    ui->Flip_Switch_middle_radioButton->setAutoExclusive(false);
    ui->Flip_Switch_right_radioButton->setAutoExclusive(false);
    ui->Flip_Switch_left_radioButton->setAutoExclusive(false);
    ui->Enable_Button_radioButton->setAutoExclusive(false);
    ui->Front_Button_radioButton->setAutoExclusive(false);

    changeColorMyButton();


    ui->y_settings_label->setText("left-right");
    ui->x_settings_label->setText("forw.-backw.");
    ui->z_settings_label->setText("rotation");


    UDP_send_socket_to_vehicle_model_PC = new QUdpSocket(this);


    UDP_receive_socket_from_vehicle_model_PC = new QUdpSocket(this);
    UDP_receive_socket_from_vehicle_model_PC->bind(QHostAddress::Any, 5347);

    connect(UDP_receive_socket_from_vehicle_model_PC, SIGNAL(readyRead()), this, SLOT(process_received_spring_friction_damping_torque_from_UDP()));

    ui->external_control_radioButton->setText("external control");
    ui->external_control_radioButton->setCheckable(true);
    ui->external_control_radioButton->setChecked(false);
    ui->external_control_radioButton->setAutoExclusive(false);

    connect(ui->external_control_radioButton, SIGNAL(toggled(bool)),this, SLOT(switch_to_external_control()));



    paste_over_network= true;

    mot_enc_pos_1= 0.0;
    mot_enc_pos_2= 0.0;
    mot_enc_pos_3= 0.0;

    alive_counter_off= 0x10;
    acknowledge_button_off= 0x08;

    ui->switch_off_acknowledge_button_radioButton->setText("acknowledge button off");
    ui->switch_off_acknowledge_button_radioButton->setAutoExclusive(false);

    //otherwise it oscillates at start
    //ui->x_Damping_horizontalSlider->setValue(0);
    ui->x_Friction_horizontalSlider->setValue(0);
    //ui->x_Spring_Stiffness_horizontalSlider->setValue(0);
    ui->x_Torque_horizontalSlider->setValue(0);

    //ui->y_Damping_horizontalSlider->setValue(0);
    ui->y_Friction_horizontalSlider->setValue(0);
    //ui->y_Spring_Stiffness_horizontalSlider->setValue(0);
    ui->y_Torque_horizontalSlider->setValue(0);

    //ui->z_Damping_horizontalSlider->setValue(0);
    ui->z_Friction_horizontalSlider->setValue(0);
    //ui->z_Spring_Stiffness_horizontalSlider->setValue(0);
    ui->z_Torque_horizontalSlider->setValue(0);



    ui->x_Spring_Stiffness_horizontalSlider->setValue(1500);
    ui->y_Spring_Stiffness_horizontalSlider->setValue(1500);
    ui->z_Spring_Stiffness_horizontalSlider->setValue(1500);

    ui->x_Damping_horizontalSlider->setValue(20);
    ui->y_Damping_horizontalSlider->setValue(20);
    ui->z_Damping_horizontalSlider->setValue(20);

    //ui->x_Friction_horizontalSlider->setValue(100);
    //ui->y_Friction_horizontalSlider->setValue(100);
    //ui->z_Friction_horizontalSlider->setValue(100);
}

void Senso_Stick::switch_to_external_control()
{
    if(ui->external_control_radioButton->isChecked()==true)
    {
/*
        //Store the last settings
        //wird in den Funktionen set_xyz_... gesetzt
        //left-right: y configuration
        last_y_Spring_Stiffness= y_Spring_Stiffness;       //0x14  //max 2500
        last_y_Friction= y_Friction;               //0x0A  //max 500
        last_y_damping_byte5= y_damping_byte5;          //0x0A  //max 50
        last_y_Torque= y_Torque;         //0x00  //-32768 32767

        //forward-backward: x configuration
        last_x_Spring_Stiffness= x_Spring_Stiffness;
        last_x_Friction= x_Friction;
        last_x_damping_byte5= x_damping_byte5;
        last_x_Torque= x_Torque;

        //rotation: z configuration
        last_z_Spring_Stiffness= z_Spring_Stiffness;
        last_z_Friction= z_Friction;
        last_z_damping_byte5= z_damping_byte5;
        last_z_Torque= z_Torque;
*/
        ui->x_Damping_horizontalSlider->setValue(0);
        ui->x_Friction_horizontalSlider->setValue(0);
        ui->x_Spring_Stiffness_horizontalSlider->setValue(0);
        ui->x_Torque_horizontalSlider->setValue(0);

        ui->y_Damping_horizontalSlider->setValue(0);
        ui->y_Friction_horizontalSlider->setValue(0);
        ui->y_Spring_Stiffness_horizontalSlider->setValue(0);
        ui->y_Torque_horizontalSlider->setValue(0);

        ui->z_Damping_horizontalSlider->setValue(0);
        ui->z_Friction_horizontalSlider->setValue(0);
        ui->z_Spring_Stiffness_horizontalSlider->setValue(0);
        ui->z_Torque_horizontalSlider->setValue(0);

        ui->x_Damping_horizontalSlider->setDisabled(true);
        ui->x_Friction_horizontalSlider->setDisabled(true);
        ui->x_Spring_Stiffness_horizontalSlider->setDisabled(true);
        ui->x_Torque_horizontalSlider->setDisabled(true);

        ui->y_Damping_horizontalSlider->setDisabled(true);
        ui->y_Friction_horizontalSlider->setDisabled(true);
        ui->y_Spring_Stiffness_horizontalSlider->setDisabled(true);
        ui->y_Torque_horizontalSlider->setDisabled(true);

        ui->z_Damping_horizontalSlider->setDisabled(true);
        ui->z_Friction_horizontalSlider->setDisabled(true);
        ui->z_Spring_Stiffness_horizontalSlider->setDisabled(true);
        ui->z_Torque_horizontalSlider->setDisabled(true);

        set_y_Spring_Stiffness(0);
        set_y_Friction(0);
        set_y_Damping(0);
        set_y_Torque(0);

        set_x_Spring_Stiffness(0);
        set_x_Friction(0);
        set_x_Damping(0);
        set_x_Torque(0);

        set_z_Spring_Stiffness(0);
        set_z_Friction(0);
        set_z_Damping(0);
        set_z_Torque(0);
    }
    else if(ui->external_control_radioButton->isChecked()==false)
    {
        set_y_Spring_Stiffness(last_y_Spring_Stiffness);
        set_y_Friction(last_y_Friction);
        set_y_Damping(last_y_damping_byte5);
        set_y_Torque(last_y_Torque);

        set_x_Spring_Stiffness(last_x_Spring_Stiffness);
        set_x_Friction(last_x_Friction);
        set_x_Damping(last_x_damping_byte5);
        set_x_Torque(last_x_Torque);

        set_z_Spring_Stiffness(last_z_Spring_Stiffness);
        set_z_Friction(last_z_Friction);
        set_z_Damping(last_z_damping_byte5);
        set_z_Torque(last_z_Torque);


        ui->x_Damping_horizontalSlider->setDisabled(false);
        ui->x_Friction_horizontalSlider->setDisabled(false);
        ui->x_Spring_Stiffness_horizontalSlider->setDisabled(false);
        ui->x_Torque_horizontalSlider->setDisabled(false);

        ui->y_Damping_horizontalSlider->setDisabled(false);
        ui->y_Friction_horizontalSlider->setDisabled(false);
        ui->y_Spring_Stiffness_horizontalSlider->setDisabled(false);
        ui->y_Torque_horizontalSlider->setDisabled(false);

        ui->z_Damping_horizontalSlider->setDisabled(false);
        ui->z_Friction_horizontalSlider->setDisabled(false);
        ui->z_Spring_Stiffness_horizontalSlider->setDisabled(false);
        ui->z_Torque_horizontalSlider->setDisabled(false);


        ui->x_Damping_horizontalSlider->setValue((int)(last_x_damping_byte5));
        ui->x_Friction_horizontalSlider->setValue((int)(last_x_Friction));
        ui->x_Spring_Stiffness_horizontalSlider->setValue((int)(last_x_Spring_Stiffness));
        ui->x_Torque_horizontalSlider->setValue((int)(last_x_Torque));

        ui->y_Damping_horizontalSlider->setValue((int)(last_y_damping_byte5));
        ui->y_Friction_horizontalSlider->setValue((int)(last_y_Friction));
        ui->y_Spring_Stiffness_horizontalSlider->setValue((int)(last_y_Spring_Stiffness));
        ui->y_Torque_horizontalSlider->setValue((int)(last_y_Torque));

        ui->z_Damping_horizontalSlider->setValue((int)(last_z_damping_byte5));
        ui->z_Friction_horizontalSlider->setValue((int)(last_z_Friction));
        ui->z_Spring_Stiffness_horizontalSlider->setValue((int)(last_z_Spring_Stiffness));
        ui->z_Torque_horizontalSlider->setValue((int)(last_z_Torque));

    }
}
void Senso_Stick::process_received_spring_friction_damping_torque_from_UDP()
{
    while (UDP_receive_socket_from_vehicle_model_PC->hasPendingDatagrams())
    {
        //qDebug()<< "received something";
        QByteArray myByteArray;
        myByteArray.resize(UDP_receive_socket_from_vehicle_model_PC->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;

        QVector<qint16> int16_buffer(12);

        qint64 readBytes=UDP_receive_socket_from_vehicle_model_PC->readDatagram((char*)int16_buffer.data(), int16_buffer.size()*sizeof(qint16), &sender, &senderPort);
        int16_buffer.resize(readBytes/sizeof(qint16));


        //qDebug() << int32_buffer.value(0) << ", " << int32_buffer.value(1) << ", " << int32_buffer.value(2)<< ", " << int32_buffer.value(3) << ", " << int32_buffer.value(4);

        int axis_1_spring_value= int16_buffer.value(0);
        int axis_1_friction_value= int16_buffer.value(1);
        int axis_1_damping_value= int16_buffer.value(2);
        int axis_1_torque_value= int16_buffer.value(3);

        int axis_2_spring_value= int16_buffer.value(4);
        int axis_2_friction_value= int16_buffer.value(5);
        int axis_2_damping_value= int16_buffer.value(6);
        int axis_2_torque_value= int16_buffer.value(7);

        int axis_3_spring_value= int16_buffer.value(8);
        int axis_3_friction_value= int16_buffer.value(9);
        int axis_3_damping_value= int16_buffer.value(10);
        int axis_3_torque_value= int16_buffer.value(11);

        //qDebug() << "S: "<< axis_1_spring_value << "   F: "<< axis_1_friction_value << "   D: "<< axis_1_damping_value << "   T: "<< axis_1_torque_value;



        if(ui->external_control_radioButton->isChecked()==true)
        {
            //left right: y
            /*
            ui->y_Spring_Stiffness_horizontalSlider->setValue(axis_1_spring_value);
            ui->y_Friction_horizontalSlider->setValue(axis_1_friction_value);
            ui->y_Damping_horizontalSlider->setValue(axis_1_damping_value);
            ui->y_Torque_horizontalSlider->setValue(axis_1_torque_value);
            */
            set_y_Spring_Stiffness(axis_1_spring_value);
            set_y_Friction(axis_1_friction_value);
            set_y_Damping(axis_1_damping_value);
            set_y_Torque(axis_1_torque_value);

            //forward-backward: x
            /*
            ui->x_Spring_Stiffness_horizontalSlider->setValue(axis_2_spring_value);
            ui->x_Friction_horizontalSlider->setValue(axis_2_friction_value);
            ui->x_Damping_horizontalSlider->setValue(axis_2_damping_value);
            ui->x_Torque_horizontalSlider->setValue(axis_2_torque_value);
            */
            set_x_Spring_Stiffness(axis_2_spring_value);
            set_x_Friction(axis_2_friction_value);
            set_x_Damping(axis_2_damping_value);
            set_x_Torque(axis_2_torque_value);

            //rotation: z
            /*
            ui->z_Spring_Stiffness_horizontalSlider->setValue(axis_3_spring_value);
            ui->z_Friction_horizontalSlider->setValue(axis_3_friction_value);
            ui->z_Damping_horizontalSlider->setValue(axis_3_damping_value);
            ui->z_Torque_horizontalSlider->setValue(axis_3_torque_value);
            */
            set_z_Spring_Stiffness(axis_3_spring_value);
            set_z_Friction(axis_3_friction_value);
            set_z_Damping(axis_3_damping_value);
            set_z_Torque(axis_3_torque_value);

        }
    }
}
/*
void Senso_Stick::process_received_spring_friction_damping_torque_from_UDP()
{
    while (UDP_receive_socket_from_vehicle_model_PC->hasPendingDatagrams())
    {
        //qDebug()<< "received something";
        QByteArray myByteArray;
        myByteArray.resize(UDP_receive_socket_from_vehicle_model_PC->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;

        QVector<qint32> int32_buffer(12);

        qint64 readBytes=UDP_receive_socket_from_vehicle_model_PC->readDatagram((char*)int32_buffer.data(), int32_buffer.size()*sizeof(qint32), &sender, &senderPort);
        int32_buffer.resize(readBytes/sizeof(qint32));


        //qDebug() << int32_buffer.value(0) << ", " << int32_buffer.value(1) << ", " << int32_buffer.value(2)<< ", " << int32_buffer.value(3) << ", " << int32_buffer.value(4);

        int axis_1_spring_value= int32_buffer.value(0);
        int axis_1_friction_value= int32_buffer.value(1);
        int axis_1_damping_value= int32_buffer.value(2);
        int axis_1_torque_value= int32_buffer.value(3);

        int axis_2_spring_value= int32_buffer.value(4);
        int axis_2_friction_value= int32_buffer.value(5);
        int axis_2_damping_value= int32_buffer.value(6);
        int axis_2_torque_value= int32_buffer.value(7);

        int axis_3_spring_value= int32_buffer.value(8);
        int axis_3_friction_value= int32_buffer.value(9);
        int axis_3_damping_value= int32_buffer.value(10);
        int axis_3_torque_value= int32_buffer.value(11);

        //qDebug() << "S: "<< axis_1_spring_value << "   F: "<< axis_1_friction_value << "   D: "<< axis_1_damping_value << "   T: "<< axis_1_torque_value;



        if(ui->external_control_radioButton->isChecked()==true)
        {
            //left right: y
            ui->y_Spring_Stiffness_horizontalSlider->setValue(axis_1_spring_value);
            ui->y_Friction_horizontalSlider->setValue(axis_1_friction_value);
            ui->y_Damping_horizontalSlider->setValue(axis_1_damping_value);
            ui->y_Torque_horizontalSlider->setValue(axis_1_torque_value);

            set_y_Spring_Stiffness(axis_1_torque_value);
            set_y_Friction(axis_1_friction_value);
            set_y_Damping(axis_1_damping_value);
            set_y_Torque(axis_1_torque_value);

            //forward-backward: x
            ui->x_Spring_Stiffness_horizontalSlider->setValue(axis_2_spring_value);
            ui->x_Friction_horizontalSlider->setValue(axis_2_friction_value);
            ui->x_Damping_horizontalSlider->setValue(axis_2_damping_value);
            ui->x_Torque_horizontalSlider->setValue(axis_2_torque_value);

            set_x_Spring_Stiffness(axis_2_torque_value);
            set_x_Friction(axis_2_friction_value);
            set_x_Damping(axis_2_damping_value);
            set_x_Torque(axis_2_torque_value);

            //rotation: z
            ui->z_Spring_Stiffness_horizontalSlider->setValue(axis_3_spring_value);
            ui->z_Friction_horizontalSlider->setValue(axis_3_friction_value);
            ui->z_Damping_horizontalSlider->setValue(axis_3_damping_value);
            ui->z_Torque_horizontalSlider->setValue(axis_3_torque_value);

            set_z_Spring_Stiffness(axis_3_torque_value);
            set_z_Friction(axis_3_friction_value);
            set_z_Damping(axis_3_damping_value);
            set_z_Torque(axis_3_torque_value);

        }
    }
}
*/


void Senso_Stick::send_position_data_to_vehicle_model_PC(unsigned int mot_enc_nr, double mot_enc_pos)
{
    QByteArray QBA_send;

    QByteArray QBA_temp;

    QBA_temp= conv_position_value_to_QByteArray(mot_enc_nr);
    QBA_send= QBA_temp;

    QBA_send.push_back(",");

    QBA_temp= conv_position_value_to_QByteArray(mot_enc_pos);
    QBA_send.push_back(QBA_temp);

    UDP_send_socket_to_vehicle_model_PC->writeDatagram(QBA_send.data(), QHostAddress("192.168.51.167"), 5346);
}

void Senso_Stick::send_button_1_data_to_vehicle_model_PC(unsigned int sixteenbitval)
{
    QByteArray QBA_send= QByteArray::number(sixteenbitval);
    UDP_send_socket_to_vehicle_model_PC->writeDatagram(QBA_send.data(), QHostAddress("192.168.51.167"), 5348);
}

void Senso_Stick::send_button_2_data_to_vehicle_model_PC(unsigned int sixteenbitval)
{
    QByteArray QBA_send= QByteArray::number(sixteenbitval);
    UDP_send_socket_to_vehicle_model_PC->writeDatagram(QBA_send.data(), QHostAddress("192.168.51.167"), 5349);
}


QByteArray Senso_Stick::conv_position_value_to_QByteArray(double value)
{
    QByteArray ByteArray= QByteArray::number(value);
    //QByteArray ByteArray;
    //QString String= QString::number(value);
    //ByteArray.append(String);
    return ByteArray;
}

/*
QByteArray Senso_Stick::conv_button_value_to_QByteArray(byte value)
{
    QByteArray ByteArray;
    //ByteArray.fromRawData(value,1);

    return ByteArray;

}
*/

void Senso_Stick::set_y_Spring_Stiffness(int springvalue)
{
    convert_spring_stiffness_to_bytes((quint16)(springvalue), &y_spring_stiffness_byte7, &y_spring_stiffness_byte8);
    if(ui->external_control_radioButton->isChecked()==false)
    {
        last_y_Spring_Stiffness= (quint16)(springvalue);
    }
}

void Senso_Stick::set_y_Friction(int friction)
{
    convert_friction_to_bytes((quint16)(friction), &y_friction_byte3, &y_friction_byte4);
    if(ui->external_control_radioButton->isChecked()==false)
    {
        last_y_Friction= (quint16)(friction);
    }
}

void Senso_Stick::set_y_Damping(int damping)
{
    quint8 damping_uint8= (quint8)(damping);
    quint8 max_damping_value= 50;
    if(damping_uint8>max_damping_value)
    {
        damping_uint8= max_damping_value;
    }
    y_damping_byte5= damping_uint8;
    if(ui->external_control_radioButton->isChecked()==false)
    {
        last_y_damping_byte5= damping_uint8;
    }
}

void Senso_Stick::set_y_Torque(int torque)
{
    convert_torque_to_bytes((qint16)(torque), &y_torque_byte1, &y_torque_byte2);
    if(ui->external_control_radioButton->isChecked()==false)
    {
        last_y_Torque= (qint16)(torque);
    }
}


void Senso_Stick::set_x_Spring_Stiffness(int springvalue)
{
    convert_spring_stiffness_to_bytes((quint16)(springvalue), &x_spring_stiffness_byte7, &x_spring_stiffness_byte8);
    if(ui->external_control_radioButton->isChecked()==false)
    {
        last_x_Spring_Stiffness= (quint16)(springvalue);
    }
}


void Senso_Stick::set_x_Friction(int friction)
{
    convert_friction_to_bytes((quint16)(friction), &x_friction_byte3, &x_friction_byte4);
    if(ui->external_control_radioButton->isChecked()==false)
    {
        last_x_Friction= (quint16)(friction);
    }
}

void Senso_Stick::set_x_Damping(int damping)
{
    quint8 damping_uint8= (quint8)(damping);
    quint8 max_damping_value= 50;
    if(damping_uint8>max_damping_value)
    {
        damping_uint8= max_damping_value;
    }
    x_damping_byte5= damping_uint8;
    if(ui->external_control_radioButton->isChecked()==false)
    {
        last_x_damping_byte5= damping_uint8;
    }
}

void Senso_Stick::set_x_Torque(int torque)
{
    convert_torque_to_bytes((qint16)(torque), &x_torque_byte1, &x_torque_byte2);
    if(ui->external_control_radioButton->isChecked()==false)
    {
        last_x_Torque= (qint16)(torque);
    }
}

void Senso_Stick::set_z_Spring_Stiffness(int springvalue)
{
    convert_spring_stiffness_to_bytes((quint16)(springvalue), &z_spring_stiffness_byte7, &z_spring_stiffness_byte8);
    if(ui->external_control_radioButton->isChecked()==false)
    {
        last_z_Spring_Stiffness= (quint16)(springvalue);
    }
}

void Senso_Stick::set_z_Friction(int friction)
{
    convert_friction_to_bytes((quint16)(friction), &z_friction_byte3, &z_friction_byte4);
    if(ui->external_control_radioButton->isChecked()==false)
    {
        last_z_Friction= (quint16)(friction);
    }
}

void Senso_Stick::set_z_Damping(int damping)
{
    quint8 damping_uint8= (quint8)(damping);
    quint8 max_damping_value= 50;
    if(damping_uint8>max_damping_value)
    {
        damping_uint8= max_damping_value;
    }
    z_damping_byte5= damping_uint8;
    if(ui->external_control_radioButton->isChecked()==false)
    {
        last_z_damping_byte5= damping_uint8;
    }
}

void Senso_Stick::set_z_Torque(int torque)
{
    convert_torque_to_bytes((qint16)(torque), &z_torque_byte1, &z_torque_byte2);
    if(ui->external_control_radioButton->isChecked()==false)
    {
        last_z_Torque= (qint16)(torque);
    }
}

void Senso_Stick::set_Torque_Limit(int torquelimit)
{
    quint8 torque_limit_uint8= (quint8)(torquelimit);
    quint8 max_torque_limit_value= 255;
    if(torque_limit_uint8 > max_torque_limit_value)
    {
        torque_limit_uint8= max_torque_limit_value;
    }
    torque_limit_byte7= torque_limit_uint8;
    ui->Torque_Limit_value_label->setText(QString::number(torque_limit_byte7));
}

void Senso_Stick::set_Peak_Torque_Limit(int peak_torque_limit)
{
    quint8 peak_torque_limit_uint8= (quint8)(peak_torque_limit);
    quint8 max_peak_torque_limit_value= 100;
    if(peak_torque_limit_uint8>max_peak_torque_limit_value)
    {
        peak_torque_limit_uint8= max_peak_torque_limit_value;
    }
    peak_torque_limit_byte8= peak_torque_limit_uint8;
    ui->Peak_Torque_Limit_value_label->setText(QString::number(peak_torque_limit_byte8));
}


void Senso_Stick::convert_spring_stiffness_to_bytes(quint16 springvalue, byte* byte7, byte* byte8)
{
    quint16 max_spring_value=2500;
    if(springvalue>max_spring_value)
    {
        springvalue=max_spring_value;
    }
    *byte7= (springvalue & 0xFF);
    *byte8= (springvalue>>8);
    //qDebug()<<"Spring: "<<springvalue<<"    Byte1: "<<*byte7<<"   Byte2: "<<*byte8;
}

void Senso_Stick::convert_friction_to_bytes(quint16 frictionvalue, byte* byte3, byte* byte4)
{
    quint16 max_friction_value=500;
    if(frictionvalue>max_friction_value)
    {
        frictionvalue=max_friction_value;
    }
    *byte3= (frictionvalue & 0xFF);
    *byte4= (frictionvalue>>8);
}

void Senso_Stick::convert_torque_to_bytes(qint16 torquevalue, byte* byte1, byte* byte2)
{
    qint16 min_torque_value=-32768;
    qint16 max_torque_value=32767;
    if(torquevalue>max_torque_value)
    {
        torquevalue=max_torque_value;
    }
    if(torquevalue<min_torque_value)
    {
        torquevalue=min_torque_value;
    }
    bool positive= true;
    if(torquevalue<0)
    {
        positive=false;
    }
    quint16 posval;
    if(positive==false)
    {
        posval= -torquevalue;
    }
    else
    {
        posval= torquevalue;
    }
    if(positive==true)
    {
        *byte1= (posval & 0xFF);
        *byte2= (posval >> 8);
    }
    //little endian convention...  For 16-bit numbers, -x is equal to 65536-x
    if(positive==false)
    {
        quint16 negval;
        negval= 65536-posval;
        *byte1= (negval & 0xFF);
        *byte2= (negval >> 8);        
    }
    //qDebug()<<"Byte 1: "<<*byte1<<"     Byte 2: "<< *byte2;

}


void Senso_Stick::clear_Status_textBrowser()
{
    ui->Status_textBrowser->clear();
}
void Senso_Stick::clear_Error_textBrowser()
{
    ui->Error_textBrowser->clear();
}

void Senso_Stick::init_Peak_CAN()
{
    PCANUSB = new CAN();
    CAN_send_messages_timer->start(CAN_send_period_init);
    CAN_check_for_messages_timer->start(CAN_check_messages_period_init);
}

void Senso_Stick::start_PeakCAN_communication()
{
    init_Peak_CAN();

    ui->Start_pushButton->setVisible(false);
    //ui->Stop_pushButton->setVisible(true);
    //ui->Stop_pushButton->setCheckable(false);

    can_init_reply= 0xFF;
    can_status_reply=0xFF;
    can_communication_test_reply=0xFF;

    PCANUSB->check_CAN_communication(&can_init_reply, &can_status_reply);

    if(can_init_reply==0)
    {
        ui->CAN_init_checkBox->setChecked(true);
    }
    else
    {
        ui->CAN_init_Error_label->setText(QString::number(can_init_reply));
    }
    if(can_status_reply==0)
    {
        ui->CAN_status_checkBox->setChecked(true);
    }
    else
    {
        ui->CAN_status_Error_label->setText(QString::number(can_status_reply));
    }

    SensoStick_state_machine=5;
    show_statemachine();

    ui->Start_pushButton->setDisabled(true);
    ui->SensoStick_CAN_Comm_pushButton->setDisabled(false);
    ui->Check_Motorencoder_pushButton->setDisabled(true);
    ui->ReferenceDrive_pushButton->setDisabled(true);
    ui->Normal_Mode_Off_pushButton->setDisabled(true);
    ui->Normal_Mode_Ready_pushButton->setDisabled(true);
    ui->Normal_Mode_On_pushButton->setDisabled(true);
    ui->Normal_Mode_Spring_Damper_pushButton->setDisabled(true);
    ui->Stop_Senso_Stick_pushButton->setDisabled(false);
}

void Senso_Stick::check_SensoStick_CAN_communication()
{
    processed_axis=1;
    SensoStick_state_machine=10;
    show_statemachine();
    start_Senso_Stick_CAN_communication();

    ui->Start_pushButton->setDisabled(true);
    ui->SensoStick_CAN_Comm_pushButton->setDisabled(true);
    ui->Check_Motorencoder_pushButton->setDisabled(false);
    ui->ReferenceDrive_pushButton->setDisabled(true);
    ui->Normal_Mode_Off_pushButton->setDisabled(true);
    ui->Normal_Mode_Ready_pushButton->setDisabled(true);
    ui->Normal_Mode_On_pushButton->setDisabled(true);
    ui->Normal_Mode_Spring_Damper_pushButton->setDisabled(true);
    ui->Stop_Senso_Stick_pushButton->setDisabled(false);
}

void Senso_Stick::start_Senso_Stick_CAN_communication()
{
    //check communication with Sensostick

    if(processed_axis==1)
    {     
        PCANUSB->CAN_send_Message_Struct->LEN= 8;
        PCANUSB->CAN_send_Message_Struct->ID= 0x201;
        PCANUSB->CAN_send_Message_Struct->DATA[0]=0x10;
        //PCANUSB->CAN_send_Message_Struct->DATA[1]=0x00;
        if(ui->switch_off_acknowledge_button_radioButton->isChecked()==false)
        {
          PCANUSB->CAN_send_Message_Struct->DATA[1]=0x10; //switch off alive counter
        }
        else
        {
            PCANUSB->CAN_send_Message_Struct->DATA[1]=0x18; //switch off alive counter (10h) and Acknowledge Switch (08h)
        }
        PCANUSB->CAN_send_Message_Struct->DATA[2]=0x00;
        PCANUSB->CAN_send_Message_Struct->DATA[3]=0x00;
        PCANUSB->CAN_send_Message_Struct->DATA[4]=0x00;
        PCANUSB->CAN_send_Message_Struct->DATA[5]=0x00;
        PCANUSB->CAN_send_Message_Struct->DATA[6]=0x00;
        PCANUSB->CAN_send_Message_Struct->DATA[7]=0x00;
        can_communication_test_reply= PCANUSB->send_CAN_message(PCANUSB->CAN_send_Message_Struct);
        if(can_communication_test_reply!=0x00)
        {
            ui->CAN_communication_Error_1_label->setText(QString::number(can_communication_test_reply));
        }
        //SensoStick_state_machine=10;
        //show_statemachine();
    }

    if(processed_axis==2)
    {
        PCANUSB->CAN_send_Message_Struct->LEN= 8;
        PCANUSB->CAN_send_Message_Struct->ID= 0x202;
        PCANUSB->CAN_send_Message_Struct->DATA[0]=0x10;
        //PCANUSB->CAN_send_Message_Struct->DATA[1]=0x00;
        if(ui->switch_off_acknowledge_button_radioButton->isChecked()==false)
        {
          PCANUSB->CAN_send_Message_Struct->DATA[1]=0x10; //switch off alive counter
        }
        else
        {
            PCANUSB->CAN_send_Message_Struct->DATA[1]=0x18; //switch off alive counter (10h) and Acknowledge Switch (08h)
        }
        PCANUSB->CAN_send_Message_Struct->DATA[2]=0x00;
        PCANUSB->CAN_send_Message_Struct->DATA[3]=0x00;
        PCANUSB->CAN_send_Message_Struct->DATA[4]=0x00;
        PCANUSB->CAN_send_Message_Struct->DATA[5]=0x00;
        PCANUSB->CAN_send_Message_Struct->DATA[6]=0x00;
        PCANUSB->CAN_send_Message_Struct->DATA[7]=0x00;
        can_communication_test_reply= PCANUSB->send_CAN_message(PCANUSB->CAN_send_Message_Struct);
        if(can_communication_test_reply!=0x00)
        {
            ui->CAN_communication_Error_2_label->setText(QString::number(can_communication_test_reply));
        }
        //SensoStick_state_machine=10;
        //show_statemachine();
    }
    if(processed_axis==3)
    {
        PCANUSB->CAN_send_Message_Struct->LEN= 8;
        PCANUSB->CAN_send_Message_Struct->ID= 0x203;
        PCANUSB->CAN_send_Message_Struct->DATA[0]=0x10;
        //PCANUSB->CAN_send_Message_Struct->DATA[1]=0x00;
        if(ui->switch_off_acknowledge_button_radioButton->isChecked()==false)
        {
          PCANUSB->CAN_send_Message_Struct->DATA[1]=0x10; //switch off alive counter
        }
        else
        {
            PCANUSB->CAN_send_Message_Struct->DATA[1]=0x18; //switch off alive counter (10h) and Acknowledge Switch (08h)
        }
        PCANUSB->CAN_send_Message_Struct->DATA[2]=0x00;
        PCANUSB->CAN_send_Message_Struct->DATA[3]=0x00;
        PCANUSB->CAN_send_Message_Struct->DATA[4]=0x00;
        PCANUSB->CAN_send_Message_Struct->DATA[5]=0x00;
        PCANUSB->CAN_send_Message_Struct->DATA[6]=0x00;
        PCANUSB->CAN_send_Message_Struct->DATA[7]=0x00;
        can_communication_test_reply= PCANUSB->send_CAN_message(PCANUSB->CAN_send_Message_Struct);
        if(can_communication_test_reply!=0x00)
        {
            ui->CAN_communication_Error_3_label->setText(QString::number(can_communication_test_reply));
        }
        //SensoStick_state_machine=10;
        //show_statemachine();
    }
    //qDebug()<<processed_axis;
}


//if communicaton check returend an error !=0 we have to reset the error here (5.4 1))
//aber ein Fehlerrreset resettet auch den alive_counter, also machen wir es mal
//anpassen der Zeiten für den Watchdog im Normal Mode
void Senso_Stick::prepare_reference_drive()
{
    //50ms, 5ms for base mode, we operate in reference and normal mode, in these modes the CAN watchdog needs < 50ms
    CAN_send_messages_timer->stop();
    CAN_check_for_messages_timer->stop();
    CAN_send_messages_timer->start(CAN_send_period_reference);
    CAN_check_for_messages_timer->start(CAN_check_messages_period_reference);
    SensoStick_state_machine=31;
    show_statemachine();

    //dann müsste auch der alive_Counter im Stick bei bei 0 sein, also hier auch zu 0 setzen
    //alive_counter_resetted=true;//mod1
}

void Senso_Stick::reference_drive_1()
{
    //here we go directly to 5.4 2)
    SensoStick_state_machine=32;
    //in state 32 a CAN message is sent (the one with 0x42)
    //
    //in state 40 the stick moves forth and back and returns to the center position (0x44 message)
    show_statemachine();
    processed_axis=1;
    ref_drive_axis=1;

}

void Senso_Stick::reference_drive_2()
{
    //here we go directly to 5.4 2)
    SensoStick_state_machine=35;
    //in state 32 a CAN message is sent (the one with 0x42)
    //
    //in state 40 the stick moves forth and back and returns to the center position (0x44 message)
    show_statemachine();
    processed_axis=1;
    ref_drive_axis=1;

}

void Senso_Stick::reference_drive_3()
{
    //here we go directly to 5.4 2)
    SensoStick_state_machine=38;
    //in state 32 a CAN message is sent (the one with 0x42)
    //
    //in state 40 the stick moves forth and back and returns to the center position (0x44 message)
    show_statemachine();
    processed_axis=1;
    ref_drive_axis=1;

}

void Senso_Stick::normal_mode_off() //Quit Error Steuerwort F, Betriebsart 1 ->1F
{
    //50ms, 5ms for base mode, we operate in reference and normal mode, in these modes the CAN watchdog needs < 50ms
    CAN_send_messages_timer->stop();
    CAN_check_for_messages_timer->stop();
    CAN_send_messages_timer->start(CAN_send_period_normal);
    CAN_check_for_messages_timer->start(CAN_check_messages_period_normal);
    SensoStick_state_machine=50;
    show_statemachine();

    ui->Start_pushButton->setDisabled(true);
    ui->SensoStick_CAN_Comm_pushButton->setDisabled(true);
    ui->Check_Motorencoder_pushButton->setDisabled(true);
    ui->ReferenceDrive_pushButton->setDisabled(true);
    ui->Normal_Mode_Off_pushButton->setDisabled(true);
    ui->Normal_Mode_Ready_pushButton->setDisabled(false);
    ui->Normal_Mode_On_pushButton->setDisabled(true);
    ui->Normal_Mode_Spring_Damper_pushButton->setDisabled(true);
    ui->Stop_Senso_Stick_pushButton->setDisabled(false);
}

void Senso_Stick::normal_mode_ready() // Steuerwort 2, Betriebsart 1 ->12
{
    SensoStick_state_machine=60;
    show_statemachine();

    ui->Start_pushButton->setDisabled(true);
    ui->SensoStick_CAN_Comm_pushButton->setDisabled(true);
    ui->Check_Motorencoder_pushButton->setDisabled(true);
    ui->ReferenceDrive_pushButton->setDisabled(true);
    ui->Normal_Mode_Off_pushButton->setDisabled(true);
    ui->Normal_Mode_Ready_pushButton->setDisabled(true);
    ui->Normal_Mode_On_pushButton->setDisabled(false);
    ui->Normal_Mode_Spring_Damper_pushButton->setDisabled(true);
    ui->Stop_Senso_Stick_pushButton->setDisabled(false);
}

void Senso_Stick::normal_mode_on() // Steuerwort 2, Betriebsart 1 ->12
{
    SensoStick_state_machine=70;
    show_statemachine();


    ui->Start_pushButton->setDisabled(true);
    ui->SensoStick_CAN_Comm_pushButton->setDisabled(true);
    ui->Check_Motorencoder_pushButton->setDisabled(true);
    ui->ReferenceDrive_pushButton->setDisabled(true);
    ui->Normal_Mode_Off_pushButton->setDisabled(true);
    ui->Normal_Mode_Ready_pushButton->setDisabled(true);
    ui->Normal_Mode_On_pushButton->setDisabled(true);
    ui->Normal_Mode_Spring_Damper_pushButton->setDisabled(false);
    ui->Stop_Senso_Stick_pushButton->setDisabled(false);
}

void Senso_Stick::normal_mode_spring_damper() // Steuerwort 2, Betriebsart 1 ->12
{
    SensoStick_state_machine=80;
    show_statemachine();

    ui->Enable_Button_label->setText("");

    ui->Start_pushButton->setDisabled(true);
    ui->SensoStick_CAN_Comm_pushButton->setDisabled(true);
    ui->Check_Motorencoder_pushButton->setDisabled(true);
    ui->ReferenceDrive_pushButton->setDisabled(true);
    ui->Normal_Mode_Off_pushButton->setDisabled(true);
    ui->Normal_Mode_Ready_pushButton->setDisabled(true);
    ui->Normal_Mode_On_pushButton->setDisabled(true);
    ui->Normal_Mode_Spring_Damper_pushButton->setDisabled(true);
    ui->Stop_Senso_Stick_pushButton->setDisabled(false);
}


void Senso_Stick::check_Motorencoder()
{
    SensoStick_state_machine=20;
    show_statemachine();

    ui->Start_pushButton->setDisabled(true);
    ui->SensoStick_CAN_Comm_pushButton->setDisabled(true);
    ui->Check_Motorencoder_pushButton->setDisabled(true);
    ui->ReferenceDrive_pushButton->setDisabled(false);
    ui->Normal_Mode_Off_pushButton->setDisabled(true);
    ui->Normal_Mode_Ready_pushButton->setDisabled(true);
    ui->Normal_Mode_On_pushButton->setDisabled(true);
    ui->Normal_Mode_Spring_Damper_pushButton->setDisabled(true);
    ui->Stop_Senso_Stick_pushButton->setDisabled(false);
}

void Senso_Stick::show_statemachine()
{
    QString statemachine_String= QString::number(SensoStick_state_machine);
    ui->Statemachine_label->setText(statemachine_String);
}

void Senso_Stick::stop_SensoStick()
{
    CAN_send_messages_timer->stop();
    CAN_check_for_messages_timer->stop();

    processed_axis=1;
    //send a CAN message that switches to normal mode


    ui->Stop_Peak_CANpushButton->setVisible(true);
    SensoStick_state_machine=100;
    show_statemachine();

    ui->Start_pushButton->setDisabled(true);
    ui->SensoStick_CAN_Comm_pushButton->setDisabled(true);
    ui->Check_Motorencoder_pushButton->setDisabled(true);
    ui->ReferenceDrive_pushButton->setDisabled(true);
    ui->Normal_Mode_Off_pushButton->setDisabled(true);
    ui->Normal_Mode_Ready_pushButton->setDisabled(true);
    ui->Normal_Mode_On_pushButton->setDisabled(true);
    ui->Normal_Mode_Spring_Damper_pushButton->setDisabled(true);
    ui->Stop_Senso_Stick_pushButton->setDisabled(true);
}

void Senso_Stick::stop_CAN_communication()
{


    ui->Start_pushButton->setVisible(true);
    ui->Stop_Peak_CANpushButton->setVisible(false);

    ui->CAN_init_checkBox->setChecked(false);
    ui->CAN_status_checkBox->setChecked(false);
    ui->CAN_communication_test_1_checkBox->setChecked(false);
    ui->CAN_communication_test_2_checkBox->setChecked(false);
    ui->CAN_communication_test_3_checkBox->setChecked(false);

    ui->CAN_init_Error_label->setText("");
    ui->CAN_status_Error_label->setText("");
    ui->CAN_communication_Error_1_label->setText("");
    ui->CAN_communication_Error_2_label->setText("");
    ui->CAN_communication_Error_3_label->setText("");

    PCANUSB->reset_CAN_connection();
    PCANUSB->close_CAN_connection();

    delete(PCANUSB);

    SensoStick_state_machine=0;
    show_statemachine();

    ui->Start_pushButton->setDisabled(false);
    ui->SensoStick_CAN_Comm_pushButton->setDisabled(true);
    ui->Check_Motorencoder_pushButton->setDisabled(true);
    ui->ReferenceDrive_pushButton->setDisabled(true);
    ui->Normal_Mode_Off_pushButton->setDisabled(true);
    ui->Normal_Mode_Ready_pushButton->setDisabled(true);
    ui->Normal_Mode_On_pushButton->setDisabled(true);
    ui->Normal_Mode_Spring_Damper_pushButton->setDisabled(true);
    ui->Stop_Senso_Stick_pushButton->setDisabled(true);


}

Senso_Stick::~Senso_Stick()
{
    PCANUSB->reset_CAN_connection();
    PCANUSB->close_CAN_connection();
    delete ui;
}

void Senso_Stick::send_PCANUSB_messages()
{

    if(/*(SensoStick_state_machine==20)||(SensoStick_state_machine==45)||(SensoStick_state_machine==53)||(SensoStick_state_machine==63)||*/
       (SensoStick_state_machine==73)||(SensoStick_state_machine==80)||(SensoStick_state_machine==81)||(SensoStick_state_machine==82))
        //Alive Counter in allen States in denen an die Adresse 21x geschickt wird
    {
        if(alive_counter_current_return_value==alive_counter_set_value)
        {
            alive_counter_set_value= alive_counter_current_return_value+1;
            alive_counter_old_return_value= alive_counter_current_return_value;
        }
        if(alive_counter_set_value>15)
        {
            alive_counter_set_value=0;
        }
    }
/*
    if(alive_counter_resetted==true)
    {
        alive_counter_set_value=1;
        alive_counter_old_return_value=0;
        alive_counter_resetted=false;
    }
*/



    switch(SensoStick_state_machine)
    {
        case 20: //check Motorencoder
            if(processed_axis==1)
            {
                PCANUSB->CAN_send_Message_Struct->LEN= 8;
                PCANUSB->CAN_send_Message_Struct->ID= 0x211;
                PCANUSB->CAN_send_Message_Struct->DATA[0]=0x00;
                PCANUSB->CAN_send_Message_Struct->DATA[1]=0x00;
                PCANUSB->CAN_send_Message_Struct->DATA[2]=0x00;
                PCANUSB->CAN_send_Message_Struct->DATA[3]=0x00;
                PCANUSB->CAN_send_Message_Struct->DATA[4]=0x00;
                PCANUSB->CAN_send_Message_Struct->DATA[5]=alive_counter_set_value;
                PCANUSB->CAN_send_Message_Struct->DATA[6]=0x00;
                PCANUSB->CAN_send_Message_Struct->DATA[7]=0x00;
            }
            if(processed_axis==2)
            {
                PCANUSB->CAN_send_Message_Struct->LEN= 8;
                PCANUSB->CAN_send_Message_Struct->ID= 0x212;
                PCANUSB->CAN_send_Message_Struct->DATA[0]=0x00;
                PCANUSB->CAN_send_Message_Struct->DATA[1]=0x00;
                PCANUSB->CAN_send_Message_Struct->DATA[2]=0x00;
                PCANUSB->CAN_send_Message_Struct->DATA[3]=0x00;
                PCANUSB->CAN_send_Message_Struct->DATA[4]=0x00;            
                PCANUSB->CAN_send_Message_Struct->DATA[5]=alive_counter_set_value;
                PCANUSB->CAN_send_Message_Struct->DATA[6]=0x00;
                PCANUSB->CAN_send_Message_Struct->DATA[7]=0x00;
            }
            if(processed_axis==3)
            {
                PCANUSB->CAN_send_Message_Struct->LEN= 8;
                PCANUSB->CAN_send_Message_Struct->ID= 0x213;
                PCANUSB->CAN_send_Message_Struct->DATA[0]=0x00;
                PCANUSB->CAN_send_Message_Struct->DATA[1]=0x00;
                PCANUSB->CAN_send_Message_Struct->DATA[2]=0x00;
                PCANUSB->CAN_send_Message_Struct->DATA[3]=0x00;
                PCANUSB->CAN_send_Message_Struct->DATA[4]=0x00;
                PCANUSB->CAN_send_Message_Struct->DATA[5]=alive_counter_set_value;
                PCANUSB->CAN_send_Message_Struct->DATA[6]=0x00;
                PCANUSB->CAN_send_Message_Struct->DATA[7]=0x00;
            }
            can_communication_test_reply= PCANUSB->send_CAN_message(PCANUSB->CAN_send_Message_Struct);
            processed_axis+=1;
            if(processed_axis>3)
            {
                processed_axis=1;
            }
        break;

        case 31:    //Fehlerreset, eigentlich nicht nötig, falls es bis dahin keine Probleme gab
                PCANUSB->CAN_send_Message_Struct->LEN= 8;
                PCANUSB->CAN_send_Message_Struct->ID= 0x201;
                PCANUSB->CAN_send_Message_Struct->DATA[0]=0x1F;
                //PCANUSB->CAN_send_Message_Struct->DATA[1]=0x00;
                if(ui->switch_off_acknowledge_button_radioButton->isChecked()==false)
                {
                  PCANUSB->CAN_send_Message_Struct->DATA[1]=0x10; //switch off alive counter
                }
                else
                {
                    PCANUSB->CAN_send_Message_Struct->DATA[1]=0x18; //switch off alive counter (10h) and Acknowledge Switch (08h)
                }
                PCANUSB->CAN_send_Message_Struct->DATA[2]=0x00;
                PCANUSB->CAN_send_Message_Struct->DATA[3]=0x00;
                PCANUSB->CAN_send_Message_Struct->DATA[4]=0x00;
                PCANUSB->CAN_send_Message_Struct->DATA[5]=0x00;
                PCANUSB->CAN_send_Message_Struct->DATA[6]=0x64;
                PCANUSB->CAN_send_Message_Struct->DATA[7]=0x64;
                can_communication_test_reply= PCANUSB->send_CAN_message(PCANUSB->CAN_send_Message_Struct);
                alive_counter_resetted= true;
                //qDebug()<<"sent 1F command";
        break;

        case 32: //first reference drive command
                   //ref_drive_axis ist mit Buttonpress auf 1 gesetzt worden
                 //processed_axis ist mit Buttonpress auf 1 gesetzt worden

                PCANUSB->CAN_send_Message_Struct->LEN= 8;
                PCANUSB->CAN_send_Message_Struct->ID= 0x201;
                PCANUSB->CAN_send_Message_Struct->DATA[0]=0x42;
                PCANUSB->CAN_send_Message_Struct->DATA[1]=0x00;
                PCANUSB->CAN_send_Message_Struct->DATA[2]=0x00;
                PCANUSB->CAN_send_Message_Struct->DATA[3]=0x00;
                PCANUSB->CAN_send_Message_Struct->DATA[4]=0x00;
                PCANUSB->CAN_send_Message_Struct->DATA[5]=0x00;
                PCANUSB->CAN_send_Message_Struct->DATA[6]=0x64;
                PCANUSB->CAN_send_Message_Struct->DATA[7]=0x64;
                can_communication_test_reply= PCANUSB->send_CAN_message(PCANUSB->CAN_send_Message_Struct);
            break;


        case 34:    //Fehlerreset zum Testen, eigentlich nicht vorgesehen, hat aber immer nach Refdrive Achse 1 abgebrochen
                PCANUSB->CAN_send_Message_Struct->LEN= 8;
                PCANUSB->CAN_send_Message_Struct->ID= 0x202;
                PCANUSB->CAN_send_Message_Struct->DATA[0]=0x1F;
                //PCANUSB->CAN_send_Message_Struct->DATA[1]=0x00;
                if(ui->switch_off_acknowledge_button_radioButton->isChecked()==false)
                {
                  PCANUSB->CAN_send_Message_Struct->DATA[1]=0x10; //switch off alive counter
                }
                else
                {
                    PCANUSB->CAN_send_Message_Struct->DATA[1]=0x18; //switch off alive counter (10h) and Acknowledge Switch (08h)
                }
                PCANUSB->CAN_send_Message_Struct->DATA[2]=0x00;
                PCANUSB->CAN_send_Message_Struct->DATA[3]=0x00;
                PCANUSB->CAN_send_Message_Struct->DATA[4]=0x00;
                PCANUSB->CAN_send_Message_Struct->DATA[5]=0x00;
                PCANUSB->CAN_send_Message_Struct->DATA[6]=0x64;
                PCANUSB->CAN_send_Message_Struct->DATA[7]=0x64;
                can_communication_test_reply= PCANUSB->send_CAN_message(PCANUSB->CAN_send_Message_Struct);
                alive_counter_resetted= true;
                //qDebug()<<"sent 1F command";
        break;

         case 35:
                PCANUSB->CAN_send_Message_Struct->LEN= 8;
                PCANUSB->CAN_send_Message_Struct->ID= 0x202;
                PCANUSB->CAN_send_Message_Struct->DATA[0]=0x42;
                PCANUSB->CAN_send_Message_Struct->DATA[1]=0x00;
                PCANUSB->CAN_send_Message_Struct->DATA[2]=0x00;
                PCANUSB->CAN_send_Message_Struct->DATA[3]=0x00;
                PCANUSB->CAN_send_Message_Struct->DATA[4]=0x00;
                PCANUSB->CAN_send_Message_Struct->DATA[5]=0x00;
                PCANUSB->CAN_send_Message_Struct->DATA[6]=0x64;
                PCANUSB->CAN_send_Message_Struct->DATA[7]=0x64;
                can_communication_test_reply= PCANUSB->send_CAN_message(PCANUSB->CAN_send_Message_Struct);
         break;

        case 37:    //Fehlerreset zum Testen, eigentlich nicht vorgesehen, hat aber immer nach Refdrive Achse 1 abgebrochen
                PCANUSB->CAN_send_Message_Struct->LEN= 8;
                PCANUSB->CAN_send_Message_Struct->ID= 0x203;
                PCANUSB->CAN_send_Message_Struct->DATA[0]=0x1F;
                //PCANUSB->CAN_send_Message_Struct->DATA[1]=0x00;
                if(ui->switch_off_acknowledge_button_radioButton->isChecked()==false)
                {
                  PCANUSB->CAN_send_Message_Struct->DATA[1]=0x10; //switch off alive counter
                }
                else
                {
                    PCANUSB->CAN_send_Message_Struct->DATA[1]=0x18; //switch off alive counter (10h) and Acknowledge Switch (08h)
                }
                PCANUSB->CAN_send_Message_Struct->DATA[2]=0x00;
                PCANUSB->CAN_send_Message_Struct->DATA[3]=0x00;
                PCANUSB->CAN_send_Message_Struct->DATA[4]=0x00;
                PCANUSB->CAN_send_Message_Struct->DATA[5]=0x00;
                PCANUSB->CAN_send_Message_Struct->DATA[6]=0x64;
                PCANUSB->CAN_send_Message_Struct->DATA[7]=0x64;
                can_communication_test_reply= PCANUSB->send_CAN_message(PCANUSB->CAN_send_Message_Struct);
                alive_counter_resetted= true;
                //qDebug()<<"sent 1F command";
        break;

         case 38:
                PCANUSB->CAN_send_Message_Struct->LEN= 8;
                PCANUSB->CAN_send_Message_Struct->ID= 0x203;
                PCANUSB->CAN_send_Message_Struct->DATA[0]=0x42;
                PCANUSB->CAN_send_Message_Struct->DATA[1]=0x00;
                PCANUSB->CAN_send_Message_Struct->DATA[2]=0x00;
                PCANUSB->CAN_send_Message_Struct->DATA[3]=0x00;
                PCANUSB->CAN_send_Message_Struct->DATA[4]=0x00;
                PCANUSB->CAN_send_Message_Struct->DATA[5]=0x00;
                PCANUSB->CAN_send_Message_Struct->DATA[6]=0x64;
                PCANUSB->CAN_send_Message_Struct->DATA[7]=0x64;
                can_communication_test_reply= PCANUSB->send_CAN_message(PCANUSB->CAN_send_Message_Struct);
          break;


        case 40: //second reference drive command, Stick moves left right for a while
                PCANUSB->CAN_send_Message_Struct->LEN= 8;
                PCANUSB->CAN_send_Message_Struct->ID= 0x201;
                PCANUSB->CAN_send_Message_Struct->DATA[0]=0x44;
                PCANUSB->CAN_send_Message_Struct->DATA[1]=0x00;
                PCANUSB->CAN_send_Message_Struct->DATA[2]=0x00;
                PCANUSB->CAN_send_Message_Struct->DATA[3]=0x00;
                PCANUSB->CAN_send_Message_Struct->DATA[4]=0x00;
                PCANUSB->CAN_send_Message_Struct->DATA[5]=0x00;
                PCANUSB->CAN_send_Message_Struct->DATA[6]=0x64;
                PCANUSB->CAN_send_Message_Struct->DATA[7]=0x64;
                can_communication_test_reply= PCANUSB->send_CAN_message(PCANUSB->CAN_send_Message_Struct);
        break;
        case 41: //second reference drive command, Stick moves forward backward for a while
                PCANUSB->CAN_send_Message_Struct->LEN= 8;
                PCANUSB->CAN_send_Message_Struct->ID= 0x202;
                PCANUSB->CAN_send_Message_Struct->DATA[0]=0x44;
                PCANUSB->CAN_send_Message_Struct->DATA[1]=0x00;
                PCANUSB->CAN_send_Message_Struct->DATA[2]=0x00;
                PCANUSB->CAN_send_Message_Struct->DATA[3]=0x00;
                PCANUSB->CAN_send_Message_Struct->DATA[4]=0x00;
                PCANUSB->CAN_send_Message_Struct->DATA[5]=0x00;
                PCANUSB->CAN_send_Message_Struct->DATA[6]=0x64;
                PCANUSB->CAN_send_Message_Struct->DATA[7]=0x64;
                can_communication_test_reply= PCANUSB->send_CAN_message(PCANUSB->CAN_send_Message_Struct);
        break;
        case 42: //third reference drive command, Stick turns left right for a while
                PCANUSB->CAN_send_Message_Struct->LEN= 8;
                PCANUSB->CAN_send_Message_Struct->ID= 0x203;
                PCANUSB->CAN_send_Message_Struct->DATA[0]=0x44;
                PCANUSB->CAN_send_Message_Struct->DATA[1]=0x00;
                PCANUSB->CAN_send_Message_Struct->DATA[2]=0x00;
                PCANUSB->CAN_send_Message_Struct->DATA[3]=0x00;
                PCANUSB->CAN_send_Message_Struct->DATA[4]=0x00;
                PCANUSB->CAN_send_Message_Struct->DATA[5]=0x00;
                PCANUSB->CAN_send_Message_Struct->DATA[6]=0x64;
                PCANUSB->CAN_send_Message_Struct->DATA[7]=0x64;
                can_communication_test_reply= PCANUSB->send_CAN_message(PCANUSB->CAN_send_Message_Struct);
        break;

        case 45: //wait for QT button press (normal) state after reference drive
                if(processed_axis==1)
                {
                    PCANUSB->CAN_send_Message_Struct->LEN= 8;
                    PCANUSB->CAN_send_Message_Struct->ID= 0x211;
                    PCANUSB->CAN_send_Message_Struct->DATA[0]=0x00;
                    PCANUSB->CAN_send_Message_Struct->DATA[1]=0x00;
                    PCANUSB->CAN_send_Message_Struct->DATA[2]=0x00;
                    PCANUSB->CAN_send_Message_Struct->DATA[3]=0x00;
                    PCANUSB->CAN_send_Message_Struct->DATA[4]=0x00;
                    PCANUSB->CAN_send_Message_Struct->DATA[5]=alive_counter_set_value;
                    PCANUSB->CAN_send_Message_Struct->DATA[6]=0x00;
                    PCANUSB->CAN_send_Message_Struct->DATA[7]=0x00;
                }
                if(processed_axis==2)
                {
                    PCANUSB->CAN_send_Message_Struct->LEN= 8;
                    PCANUSB->CAN_send_Message_Struct->ID= 0x212;
                    PCANUSB->CAN_send_Message_Struct->DATA[0]=0x00;
                    PCANUSB->CAN_send_Message_Struct->DATA[1]=0x00;
                    PCANUSB->CAN_send_Message_Struct->DATA[2]=0x00;
                    PCANUSB->CAN_send_Message_Struct->DATA[3]=0x00;
                    PCANUSB->CAN_send_Message_Struct->DATA[4]=0x00;
                    PCANUSB->CAN_send_Message_Struct->DATA[5]=alive_counter_set_value;
                    PCANUSB->CAN_send_Message_Struct->DATA[6]=0x00;
                    PCANUSB->CAN_send_Message_Struct->DATA[7]=0x00;
                }
                if(processed_axis==3)
                {
                    PCANUSB->CAN_send_Message_Struct->LEN= 8;
                    PCANUSB->CAN_send_Message_Struct->ID= 0x213;
                    PCANUSB->CAN_send_Message_Struct->DATA[0]=0x00;
                    PCANUSB->CAN_send_Message_Struct->DATA[1]=0x00;
                    PCANUSB->CAN_send_Message_Struct->DATA[2]=0x00;
                    PCANUSB->CAN_send_Message_Struct->DATA[3]=0x00;
                    PCANUSB->CAN_send_Message_Struct->DATA[4]=0x00;
                    PCANUSB->CAN_send_Message_Struct->DATA[5]=alive_counter_set_value;
                    PCANUSB->CAN_send_Message_Struct->DATA[6]=0x00;
                    PCANUSB->CAN_send_Message_Struct->DATA[7]=0x00;
                }
                can_communication_test_reply= PCANUSB->send_CAN_message(PCANUSB->CAN_send_Message_Struct);
                processed_axis+=1;
                if(processed_axis>3)
                {
                    processed_axis=1;
                }
        break;

        case 50: //start of Senso Stick with moment limited to 20% -> switch to normal mode Part 1 P17
                PCANUSB->CAN_send_Message_Struct->LEN= 8;
                PCANUSB->CAN_send_Message_Struct->ID= 0x201;
                PCANUSB->CAN_send_Message_Struct->DATA[0]=0x1F;
                //PCANUSB->CAN_send_Message_Struct->DATA[1]=0x00;
                if(ui->switch_off_acknowledge_button_radioButton->isChecked()==false)
                {
                  PCANUSB->CAN_send_Message_Struct->DATA[1]=0x10; //switch off alive counter
                }
                else
                {
                    PCANUSB->CAN_send_Message_Struct->DATA[1]=0x18; //switch off alive counter (10h) and Acknowledge Switch (08h)
                }
                PCANUSB->CAN_send_Message_Struct->DATA[2]=0x00;
                PCANUSB->CAN_send_Message_Struct->DATA[3]=0x00;
                PCANUSB->CAN_send_Message_Struct->DATA[4]=0x00;
                PCANUSB->CAN_send_Message_Struct->DATA[5]=0x00;
                PCANUSB->CAN_send_Message_Struct->DATA[6]=torque_limit_byte7;
                PCANUSB->CAN_send_Message_Struct->DATA[7]=peak_torque_limit_byte8;
                can_communication_test_reply= PCANUSB->send_CAN_message(PCANUSB->CAN_send_Message_Struct);
        break;
        case 51: //start of Senso Stick with moment limited to 20% -> switch to normal mode
                PCANUSB->CAN_send_Message_Struct->LEN= 8;
                PCANUSB->CAN_send_Message_Struct->ID= 0x202;
                PCANUSB->CAN_send_Message_Struct->DATA[0]=0x1F;
                //PCANUSB->CAN_send_Message_Struct->DATA[1]=0x00;
                if(ui->switch_off_acknowledge_button_radioButton->isChecked()==false)
                {
                  PCANUSB->CAN_send_Message_Struct->DATA[1]=0x10; //switch off alive counter
                }
                else
                {
                    PCANUSB->CAN_send_Message_Struct->DATA[1]=0x18; //switch off alive counter (10h) and Acknowledge Switch (08h)
                }
                PCANUSB->CAN_send_Message_Struct->DATA[2]=0x00;
                PCANUSB->CAN_send_Message_Struct->DATA[3]=0x00;
                PCANUSB->CAN_send_Message_Struct->DATA[4]=0x00;
                PCANUSB->CAN_send_Message_Struct->DATA[5]=0x00;
                PCANUSB->CAN_send_Message_Struct->DATA[6]=torque_limit_byte7;
                PCANUSB->CAN_send_Message_Struct->DATA[7]=peak_torque_limit_byte8;
                can_communication_test_reply= PCANUSB->send_CAN_message(PCANUSB->CAN_send_Message_Struct);
        break;
        case 52: //start of Senso Stick with moment limited to 20% -> switch to normal mode
                PCANUSB->CAN_send_Message_Struct->LEN= 8;
                PCANUSB->CAN_send_Message_Struct->ID= 0x203;
                PCANUSB->CAN_send_Message_Struct->DATA[0]=0x1F;
                //PCANUSB->CAN_send_Message_Struct->DATA[1]=0x00;
                if(ui->switch_off_acknowledge_button_radioButton->isChecked()==false)
                {
                  PCANUSB->CAN_send_Message_Struct->DATA[1]=0x10; //switch off alive counter
                }
                else
                {
                    PCANUSB->CAN_send_Message_Struct->DATA[1]=0x18; //switch off alive counter (10h) and Acknowledge Switch (08h)
                }
                PCANUSB->CAN_send_Message_Struct->DATA[2]=0x00;
                PCANUSB->CAN_send_Message_Struct->DATA[3]=0x00;
                PCANUSB->CAN_send_Message_Struct->DATA[4]=0x00;
                PCANUSB->CAN_send_Message_Struct->DATA[5]=0x00;
                PCANUSB->CAN_send_Message_Struct->DATA[6]=torque_limit_byte7;
                PCANUSB->CAN_send_Message_Struct->DATA[7]=peak_torque_limit_byte8;
                can_communication_test_reply= PCANUSB->send_CAN_message(PCANUSB->CAN_send_Message_Struct);
        break;

        case 53: //check Motorencoder
                if(processed_axis==1)
                {
                    PCANUSB->CAN_send_Message_Struct->LEN= 8;
                    PCANUSB->CAN_send_Message_Struct->ID= 0x211;
                    PCANUSB->CAN_send_Message_Struct->DATA[0]=0x00;
                    PCANUSB->CAN_send_Message_Struct->DATA[1]=0x00;
                    PCANUSB->CAN_send_Message_Struct->DATA[2]=0x00;
                    PCANUSB->CAN_send_Message_Struct->DATA[3]=0x00;
                    PCANUSB->CAN_send_Message_Struct->DATA[4]=0x00;
                    PCANUSB->CAN_send_Message_Struct->DATA[5]=alive_counter_set_value;
                    PCANUSB->CAN_send_Message_Struct->DATA[6]=0x00;
                    PCANUSB->CAN_send_Message_Struct->DATA[7]=0x00;
                }
                if(processed_axis==2)
                {
                    PCANUSB->CAN_send_Message_Struct->LEN= 8;
                    PCANUSB->CAN_send_Message_Struct->ID= 0x212;
                    PCANUSB->CAN_send_Message_Struct->DATA[0]=0x00;
                    PCANUSB->CAN_send_Message_Struct->DATA[1]=0x00;
                    PCANUSB->CAN_send_Message_Struct->DATA[2]=0x00;
                    PCANUSB->CAN_send_Message_Struct->DATA[3]=0x00;
                    PCANUSB->CAN_send_Message_Struct->DATA[4]=0x00;
                    PCANUSB->CAN_send_Message_Struct->DATA[5]=alive_counter_set_value;
                    PCANUSB->CAN_send_Message_Struct->DATA[6]=0x00;
                    PCANUSB->CAN_send_Message_Struct->DATA[7]=0x00;
                }
                if(processed_axis==3)
                {
                    PCANUSB->CAN_send_Message_Struct->LEN= 8;
                    PCANUSB->CAN_send_Message_Struct->ID= 0x213;
                    PCANUSB->CAN_send_Message_Struct->DATA[0]=0x00;
                    PCANUSB->CAN_send_Message_Struct->DATA[1]=0x00;
                    PCANUSB->CAN_send_Message_Struct->DATA[2]=0x00;
                    PCANUSB->CAN_send_Message_Struct->DATA[3]=0x00;
                    PCANUSB->CAN_send_Message_Struct->DATA[4]=0x00;
                    PCANUSB->CAN_send_Message_Struct->DATA[5]=alive_counter_set_value;
                    PCANUSB->CAN_send_Message_Struct->DATA[6]=0x00;
                    PCANUSB->CAN_send_Message_Struct->DATA[7]=0x00;
                }
                can_communication_test_reply= PCANUSB->send_CAN_message(PCANUSB->CAN_send_Message_Struct);
                processed_axis+=1;
                if(processed_axis>3)
                {
                    processed_axis=1;
                }
        break;


        case 60: //start of Senso Stick with moment limited to 20% -> switch from off to ready state Part 2 P17
                PCANUSB->CAN_send_Message_Struct->LEN= 8;
                PCANUSB->CAN_send_Message_Struct->ID= 0x201;
                PCANUSB->CAN_send_Message_Struct->DATA[0]=0x12;
                //PCANUSB->CAN_send_Message_Struct->DATA[1]=0x00;
                if(ui->switch_off_acknowledge_button_radioButton->isChecked()==false)
                {
                  PCANUSB->CAN_send_Message_Struct->DATA[1]=0x10; //switch off alive counter
                }
                else
                {
                    PCANUSB->CAN_send_Message_Struct->DATA[1]=0x18; //switch off alive counter (10h) and Acknowledge Switch (08h)
                }
                PCANUSB->CAN_send_Message_Struct->DATA[2]=0x00;
                PCANUSB->CAN_send_Message_Struct->DATA[3]=0x00;
                PCANUSB->CAN_send_Message_Struct->DATA[4]=0x00;
                PCANUSB->CAN_send_Message_Struct->DATA[5]=0x00;
                PCANUSB->CAN_send_Message_Struct->DATA[6]=torque_limit_byte7;
                PCANUSB->CAN_send_Message_Struct->DATA[7]=peak_torque_limit_byte8;
                can_communication_test_reply= PCANUSB->send_CAN_message(PCANUSB->CAN_send_Message_Struct);
        break;
        case 61: //start of Senso Stick with moment limited to 20% -> switch from off to ready state
                PCANUSB->CAN_send_Message_Struct->LEN= 8;
                PCANUSB->CAN_send_Message_Struct->ID= 0x202;
                PCANUSB->CAN_send_Message_Struct->DATA[0]=0x12;
                //PCANUSB->CAN_send_Message_Struct->DATA[1]=0x00;
                if(ui->switch_off_acknowledge_button_radioButton->isChecked()==false)
                {
                  PCANUSB->CAN_send_Message_Struct->DATA[1]=0x10; //switch off alive counter
                }
                else
                {
                    PCANUSB->CAN_send_Message_Struct->DATA[1]=0x18; //switch off alive counter (10h) and Acknowledge Switch (08h)
                }
                PCANUSB->CAN_send_Message_Struct->DATA[2]=0x00;
                PCANUSB->CAN_send_Message_Struct->DATA[3]=0x00;
                PCANUSB->CAN_send_Message_Struct->DATA[4]=0x00;
                PCANUSB->CAN_send_Message_Struct->DATA[5]=0x00;
                PCANUSB->CAN_send_Message_Struct->DATA[6]=torque_limit_byte7;
                PCANUSB->CAN_send_Message_Struct->DATA[7]=peak_torque_limit_byte8;
                can_communication_test_reply= PCANUSB->send_CAN_message(PCANUSB->CAN_send_Message_Struct);
        break;
        case 62: //start of Senso Stick with moment limited to 20% -> switch from off to ready state
                PCANUSB->CAN_send_Message_Struct->LEN= 8;
                PCANUSB->CAN_send_Message_Struct->ID= 0x203;
                PCANUSB->CAN_send_Message_Struct->DATA[0]=0x12;
                //PCANUSB->CAN_send_Message_Struct->DATA[1]=0x00;
                if(ui->switch_off_acknowledge_button_radioButton->isChecked()==false)
                {
                  PCANUSB->CAN_send_Message_Struct->DATA[1]=0x10; //switch off alive counter
                }
                else
                {
                    PCANUSB->CAN_send_Message_Struct->DATA[1]=0x18; //switch off alive counter (10h) and Acknowledge Switch (08h)
                }
                PCANUSB->CAN_send_Message_Struct->DATA[2]=0x00;
                PCANUSB->CAN_send_Message_Struct->DATA[3]=0x00;
                PCANUSB->CAN_send_Message_Struct->DATA[4]=0x00;
                PCANUSB->CAN_send_Message_Struct->DATA[5]=0x00;
                PCANUSB->CAN_send_Message_Struct->DATA[6]=torque_limit_byte7;
                PCANUSB->CAN_send_Message_Struct->DATA[7]=peak_torque_limit_byte8;
                can_communication_test_reply= PCANUSB->send_CAN_message(PCANUSB->CAN_send_Message_Struct);
        break;

        case 63: //check Motorencoder
                if(processed_axis==1)
                {
                    PCANUSB->CAN_send_Message_Struct->LEN= 8;
                    PCANUSB->CAN_send_Message_Struct->ID= 0x211;
                    PCANUSB->CAN_send_Message_Struct->DATA[0]=0x00;
                    PCANUSB->CAN_send_Message_Struct->DATA[1]=0x00;
                    PCANUSB->CAN_send_Message_Struct->DATA[2]=0x00;
                    PCANUSB->CAN_send_Message_Struct->DATA[3]=0x00;
                    PCANUSB->CAN_send_Message_Struct->DATA[4]=0x00;
                    PCANUSB->CAN_send_Message_Struct->DATA[5]=alive_counter_set_value;
                    PCANUSB->CAN_send_Message_Struct->DATA[6]=0x00;
                    PCANUSB->CAN_send_Message_Struct->DATA[7]=0x00;
                }
                if(processed_axis==2)
                {
                    PCANUSB->CAN_send_Message_Struct->LEN= 8;
                    PCANUSB->CAN_send_Message_Struct->ID= 0x212;
                    PCANUSB->CAN_send_Message_Struct->DATA[0]=0x00;
                    PCANUSB->CAN_send_Message_Struct->DATA[1]=0x00;
                    PCANUSB->CAN_send_Message_Struct->DATA[2]=0x00;
                    PCANUSB->CAN_send_Message_Struct->DATA[3]=0x00;
                    PCANUSB->CAN_send_Message_Struct->DATA[4]=0x00;
                    PCANUSB->CAN_send_Message_Struct->DATA[5]=alive_counter_set_value;
                    PCANUSB->CAN_send_Message_Struct->DATA[6]=0x00;
                    PCANUSB->CAN_send_Message_Struct->DATA[7]=0x00;
                }
                if(processed_axis==3)
                {
                    PCANUSB->CAN_send_Message_Struct->LEN= 8;
                    PCANUSB->CAN_send_Message_Struct->ID= 0x213;
                    PCANUSB->CAN_send_Message_Struct->DATA[0]=0x00;
                    PCANUSB->CAN_send_Message_Struct->DATA[1]=0x00;
                    PCANUSB->CAN_send_Message_Struct->DATA[2]=0x00;
                    PCANUSB->CAN_send_Message_Struct->DATA[3]=0x00;
                    PCANUSB->CAN_send_Message_Struct->DATA[4]=0x00;
                    PCANUSB->CAN_send_Message_Struct->DATA[5]=alive_counter_set_value;
                    PCANUSB->CAN_send_Message_Struct->DATA[6]=0x00;
                    PCANUSB->CAN_send_Message_Struct->DATA[7]=0x00;
                }
                can_communication_test_reply= PCANUSB->send_CAN_message(PCANUSB->CAN_send_Message_Struct);
                processed_axis+=1;
                if(processed_axis>3)
                {
                    processed_axis=1;
                }
        break;



        case 70: //start of Senso Stick with moment limited to 20% -> switch from ready to on state Part 3 P18
                PCANUSB->CAN_send_Message_Struct->LEN= 8;
                PCANUSB->CAN_send_Message_Struct->ID= 0x201;
                PCANUSB->CAN_send_Message_Struct->DATA[0]=0x14;
                //PCANUSB->CAN_send_Message_Struct->DATA[1]=0x00;
                if(ui->switch_off_acknowledge_button_radioButton->isChecked()==false)
                {
                  PCANUSB->CAN_send_Message_Struct->DATA[1]=0x10; //switch off alive counter
                }
                else
                {
                    PCANUSB->CAN_send_Message_Struct->DATA[1]=0x18; //switch off alive counter (10h) and Acknowledge Switch (08h)
                }
                PCANUSB->CAN_send_Message_Struct->DATA[2]=0x00;
                PCANUSB->CAN_send_Message_Struct->DATA[3]=0x00;
                PCANUSB->CAN_send_Message_Struct->DATA[4]=0x00;
                PCANUSB->CAN_send_Message_Struct->DATA[5]=0x00;
                PCANUSB->CAN_send_Message_Struct->DATA[6]=torque_limit_byte7;
                PCANUSB->CAN_send_Message_Struct->DATA[7]=peak_torque_limit_byte8;
                can_communication_test_reply= PCANUSB->send_CAN_message(PCANUSB->CAN_send_Message_Struct);
        break;
        case 71: //start of Senso Stick with moment limited to 20% -> switch from ready to on state
                PCANUSB->CAN_send_Message_Struct->LEN= 8;
                PCANUSB->CAN_send_Message_Struct->ID= 0x202;
                PCANUSB->CAN_send_Message_Struct->DATA[0]=0x14;
                //PCANUSB->CAN_send_Message_Struct->DATA[1]=0x00;
                if(ui->switch_off_acknowledge_button_radioButton->isChecked()==false)
                {
                  PCANUSB->CAN_send_Message_Struct->DATA[1]=0x10; //switch off alive counter
                }
                else
                {
                    PCANUSB->CAN_send_Message_Struct->DATA[1]=0x18; //switch off alive counter (10h) and Acknowledge Switch (08h)
                }
                PCANUSB->CAN_send_Message_Struct->DATA[2]=0x00;
                PCANUSB->CAN_send_Message_Struct->DATA[3]=0x00;
                PCANUSB->CAN_send_Message_Struct->DATA[4]=0x00;
                PCANUSB->CAN_send_Message_Struct->DATA[5]=0x00;
                PCANUSB->CAN_send_Message_Struct->DATA[6]=torque_limit_byte7;
                PCANUSB->CAN_send_Message_Struct->DATA[7]=peak_torque_limit_byte8;
                can_communication_test_reply= PCANUSB->send_CAN_message(PCANUSB->CAN_send_Message_Struct);
        break;
        case 72: //start of Senso Stick with moment limited to 20% -> switch from ready to on state
                PCANUSB->CAN_send_Message_Struct->LEN= 8;
                PCANUSB->CAN_send_Message_Struct->ID= 0x203;
                PCANUSB->CAN_send_Message_Struct->DATA[0]=0x14;
                //PCANUSB->CAN_send_Message_Struct->DATA[1]=0x00;
                if(ui->switch_off_acknowledge_button_radioButton->isChecked()==false)
                {
                  PCANUSB->CAN_send_Message_Struct->DATA[1]=0x10; //switch off alive counter
                }
                else
                {
                    PCANUSB->CAN_send_Message_Struct->DATA[1]=0x18; //switch off alive counter (10h) and Acknowledge Switch (08h)
                }
                PCANUSB->CAN_send_Message_Struct->DATA[2]=0x00;
                PCANUSB->CAN_send_Message_Struct->DATA[3]=0x00;
                PCANUSB->CAN_send_Message_Struct->DATA[4]=0x00;
                PCANUSB->CAN_send_Message_Struct->DATA[5]=0x00;
                PCANUSB->CAN_send_Message_Struct->DATA[6]=torque_limit_byte7;
                PCANUSB->CAN_send_Message_Struct->DATA[7]=peak_torque_limit_byte8;
                can_communication_test_reply= PCANUSB->send_CAN_message(PCANUSB->CAN_send_Message_Struct);
        break;


        case 73: //check Motorencoder
                if(processed_axis==1)
                {
                    PCANUSB->CAN_send_Message_Struct->LEN= 8;
                    PCANUSB->CAN_send_Message_Struct->ID= 0x211;
                    PCANUSB->CAN_send_Message_Struct->DATA[0]=0x00;
                    PCANUSB->CAN_send_Message_Struct->DATA[1]=0x00;
                    PCANUSB->CAN_send_Message_Struct->DATA[2]=0x00;
                    PCANUSB->CAN_send_Message_Struct->DATA[3]=0x00;
                    PCANUSB->CAN_send_Message_Struct->DATA[4]=0x00;
                    PCANUSB->CAN_send_Message_Struct->DATA[5]=alive_counter_set_value;
                    PCANUSB->CAN_send_Message_Struct->DATA[6]=0x00;
                    PCANUSB->CAN_send_Message_Struct->DATA[7]=0x00;
                }
                if(processed_axis==2)
                {
                    PCANUSB->CAN_send_Message_Struct->LEN= 8;
                    PCANUSB->CAN_send_Message_Struct->ID= 0x212;
                    PCANUSB->CAN_send_Message_Struct->DATA[0]=0x00;
                    PCANUSB->CAN_send_Message_Struct->DATA[1]=0x00;
                    PCANUSB->CAN_send_Message_Struct->DATA[2]=0x00;
                    PCANUSB->CAN_send_Message_Struct->DATA[3]=0x00;
                    PCANUSB->CAN_send_Message_Struct->DATA[4]=0x00;
                    PCANUSB->CAN_send_Message_Struct->DATA[5]=alive_counter_set_value;
                    PCANUSB->CAN_send_Message_Struct->DATA[6]=0x00;
                    PCANUSB->CAN_send_Message_Struct->DATA[7]=0x00;
                }
                if(processed_axis==3)
                {
                    PCANUSB->CAN_send_Message_Struct->LEN= 8;
                    PCANUSB->CAN_send_Message_Struct->ID= 0x213;
                    PCANUSB->CAN_send_Message_Struct->DATA[0]=0x00;
                    PCANUSB->CAN_send_Message_Struct->DATA[1]=0x00;
                    PCANUSB->CAN_send_Message_Struct->DATA[2]=0x00;
                    PCANUSB->CAN_send_Message_Struct->DATA[3]=0x00;
                    PCANUSB->CAN_send_Message_Struct->DATA[4]=0x00;
                    PCANUSB->CAN_send_Message_Struct->DATA[5]=alive_counter_set_value;
                    PCANUSB->CAN_send_Message_Struct->DATA[6]=0x00;
                    PCANUSB->CAN_send_Message_Struct->DATA[7]=0x00;
                }
                can_communication_test_reply= PCANUSB->send_CAN_message(PCANUSB->CAN_send_Message_Struct);
                processed_axis+=1;
                if(processed_axis>3)
                {
                    processed_axis=1;
                }
        break;



        case 80: //Parametrierung Part 1
                PCANUSB->CAN_send_Message_Struct->LEN= 8;
                PCANUSB->CAN_send_Message_Struct->ID= 0x211;
                PCANUSB->CAN_send_Message_Struct->DATA[0]= y_torque_byte1;
                PCANUSB->CAN_send_Message_Struct->DATA[1]= y_torque_byte2;
                PCANUSB->CAN_send_Message_Struct->DATA[2]= y_friction_byte3;
                PCANUSB->CAN_send_Message_Struct->DATA[3]= y_friction_byte4;
                PCANUSB->CAN_send_Message_Struct->DATA[4]= y_damping_byte5;
                PCANUSB->CAN_send_Message_Struct->DATA[5]= alive_counter_set_value;
                PCANUSB->CAN_send_Message_Struct->DATA[6]= y_spring_stiffness_byte7;
                PCANUSB->CAN_send_Message_Struct->DATA[7]= y_spring_stiffness_byte8;
                can_communication_test_reply= PCANUSB->send_CAN_message(PCANUSB->CAN_send_Message_Struct);

        break;
        case 81:
                PCANUSB->CAN_send_Message_Struct->LEN= 8;
                PCANUSB->CAN_send_Message_Struct->ID= 0x212;
                PCANUSB->CAN_send_Message_Struct->DATA[0]= x_torque_byte1;
                PCANUSB->CAN_send_Message_Struct->DATA[1]= x_torque_byte2;
                PCANUSB->CAN_send_Message_Struct->DATA[2]= x_friction_byte3;
                PCANUSB->CAN_send_Message_Struct->DATA[3]= x_friction_byte4;
                PCANUSB->CAN_send_Message_Struct->DATA[4]= x_damping_byte5;
                PCANUSB->CAN_send_Message_Struct->DATA[5]= alive_counter_set_value;
                PCANUSB->CAN_send_Message_Struct->DATA[6]= x_spring_stiffness_byte7;
                PCANUSB->CAN_send_Message_Struct->DATA[7]= x_spring_stiffness_byte8;
                can_communication_test_reply= PCANUSB->send_CAN_message(PCANUSB->CAN_send_Message_Struct);
        break;
        case 82:
                PCANUSB->CAN_send_Message_Struct->LEN= 8;
                PCANUSB->CAN_send_Message_Struct->ID= 0x213;
                PCANUSB->CAN_send_Message_Struct->DATA[0]= z_torque_byte1;
                PCANUSB->CAN_send_Message_Struct->DATA[1]= z_torque_byte2;
                PCANUSB->CAN_send_Message_Struct->DATA[2]= z_friction_byte3;
                PCANUSB->CAN_send_Message_Struct->DATA[3]= z_friction_byte4;
                PCANUSB->CAN_send_Message_Struct->DATA[4]= z_damping_byte5;
                PCANUSB->CAN_send_Message_Struct->DATA[5]= alive_counter_set_value;
                PCANUSB->CAN_send_Message_Struct->DATA[6]= z_spring_stiffness_byte7;
                PCANUSB->CAN_send_Message_Struct->DATA[7]= z_spring_stiffness_byte8;
                can_communication_test_reply= PCANUSB->send_CAN_message(PCANUSB->CAN_send_Message_Struct);
        break;

        //Schalter
        case 85:
                PCANUSB->CAN_send_Message_Struct->LEN= 0;
                PCANUSB->CAN_send_Message_Struct->ID= 0x251;
                can_communication_test_reply= PCANUSB->send_CAN_message(PCANUSB->CAN_send_Message_Struct);
        break;


        case 86:
                PCANUSB->CAN_send_Message_Struct->LEN= 0;
                PCANUSB->CAN_send_Message_Struct->ID= 0x252;
                can_communication_test_reply= PCANUSB->send_CAN_message(PCANUSB->CAN_send_Message_Struct);
        break;

        default:

        break;
    }
    if(SensoStick_state_machine==32)
    {
        SensoStick_state_machine=40;
        show_statemachine();
    }
    if(SensoStick_state_machine==35)
    {
        SensoStick_state_machine=41;
        show_statemachine();
    }
    if(SensoStick_state_machine==38)
    {
        SensoStick_state_machine=42;
        show_statemachine();
    }


}

void Senso_Stick::check_for_PCANUSB_messages()
{
    unsigned int CAN_return_value= PCANUSB->check_for_CAN_messages(received_CAN_message_Struct);
    if(CAN_return_value==0x01)
    {
        //qDebug()<<received_CAN_message_Struct->ID<<"  "<<received_CAN_message_Struct->DATA[0];
        evaluate_SensoStick_CAN_message();
    }
}

void Senso_Stick::evaluate_SensoStick_CAN_message()
{

    DWORD id;
    id = received_CAN_message_Struct->ID;

    BYTE BYTE1, BYTE2, BYTE3, BYTE4, BYTE5, BYTE6, BYTE7, BYTE8;
    BYTE1= received_CAN_message_Struct->DATA[0];
    BYTE2= received_CAN_message_Struct->DATA[1];
    BYTE3= received_CAN_message_Struct->DATA[2];
    BYTE4= received_CAN_message_Struct->DATA[3];
    BYTE5= received_CAN_message_Struct->DATA[4];
    BYTE6= received_CAN_message_Struct->DATA[5];
    //die Antworten der Abfragen der Buttons haben nur 6 Byte
    if((id!=0x151)&&(id!=152))
    {
        BYTE7= received_CAN_message_Struct->DATA[6];
        BYTE8= received_CAN_message_Struct->DATA[7];
    }

    //qDebug()<<"check... "<<BYTE1;
    BYTE len;
    len= received_CAN_message_Struct->LEN;

    switch(id)
    {
        //reply of communication test
        case 0x101:
            ui->CAN_communication_test_1_checkBox->setChecked(true);
            show_status(BYTE1,BYTE2);
            show_error(BYTE3,BYTE4);
            if(first_CAN_comm_message_1==false)
            {
                processed_axis=2;
                SensoStick_state_machine=11;
                show_statemachine();
                first_CAN_comm_message_1= true;
                start_Senso_Stick_CAN_communication();

            }

            last_CAN_error= BYTE3 + BYTE4*256;
            if(BYTE1 == 0x42)    //reply to first reference drive command; statemachine =32
            {  
                Encoder_Index= compute_encoder_index( BYTE7, BYTE8);
                ui->Encoder_Indexvalue_1_label->setText(QString::number(Encoder_Index));
            }
            if(BYTE1 == 0x44)      //page 31
            {
                Encoder_Index=compute_encoder_index( BYTE7, BYTE8);
                ui->Encoder_Indexvalue_1_label->setText(QString::number(Encoder_Index));
                //we are in state 40 and have to wait until the reference drive for this axis is done
                if((BYTE2==0x68)&&(SensoStick_state_machine==40))
                {
                    //qDebug()<<"End Ref 40 (M1) det.";
                    SensoStick_state_machine=34;//Fehlerreset Achse 2
                    show_statemachine();
                }
            }

            if(BYTE1 == 0x10)    //reply to state 50 normal mode, still off
            {

                if(SensoStick_state_machine==50)
                {
                    SensoStick_state_machine=51;    //Motor 1
                    show_statemachine();
                    Encoder_Index=compute_encoder_index( BYTE7, BYTE8);
                    ui->Encoder_Indexvalue_1_label->setText(QString::number(Encoder_Index));
                }
/*
                if(SensoStick_state_machine==90)
                {
                    SensoStick_state_machine=91;    //Motor 1
                    show_statemachine();
                    Encoder_Index=compute_encoder_index( BYTE7, BYTE8);
                    ui->Encoder_Indexvalue_1_label->setText(QString::number(Encoder_Index));
                }

*/
                if(SensoStick_state_machine==31)
                {
                    reference_drive_1();  //das wechselt in state 32
                }
            }

            if((BYTE1 == 0x12)&&(SensoStick_state_machine==60))    //reply to state 60 normal mode, ready state
            {
                SensoStick_state_machine=61;    //Motor 1
                show_statemachine();
                Encoder_Index=compute_encoder_index( BYTE7, BYTE8);
                ui->Encoder_Indexvalue_1_label->setText(QString::number(Encoder_Index));
            }


            if((BYTE1 == 0x14)&&(SensoStick_state_machine==70))    //reply to state 70 normal mode, ready state
            {
                SensoStick_state_machine=71;    //Motor 1
                show_statemachine();
                Encoder_Index=compute_encoder_index( BYTE7, BYTE8);
                ui->Encoder_Indexvalue_1_label->setText(QString::number(Encoder_Index));
            }


        break;
            //reply of communication test
            case 0x102:
                ui->CAN_communication_test_2_checkBox->setChecked(true);
                show_status(BYTE1,BYTE2);
                show_error(BYTE3,BYTE4);
                if(first_CAN_comm_message_2==false)
                {
                    processed_axis=3;
                    SensoStick_state_machine=12;
                    show_statemachine();
                    first_CAN_comm_message_2= true;
                    start_Senso_Stick_CAN_communication();

                }

                last_CAN_error= BYTE3 + BYTE4*256;
                if(BYTE1 == 0x42)    //reply to first reference drive command; statemachine =32
                {
                    Encoder_Index= compute_encoder_index( BYTE7, BYTE8);
                    ui->Encoder_Indexvalue_2_label->setText(QString::number(Encoder_Index));
                }
                if(BYTE1 == 0x44)      //page 31
                {
                    Encoder_Index=compute_encoder_index( BYTE7, BYTE8);
                    ui->Encoder_Indexvalue_2_label->setText(QString::number(Encoder_Index));
                     //we are in state 41 and have to wait until the reference drive for the second axis is done
                    if((BYTE2==0x68)&&(SensoStick_state_machine==41))
                    {

                        //qDebug()<<"End Ref 41 (M2) det.";
                        SensoStick_state_machine=37;//Fehlerreset Achse 2
                        show_statemachine();
                    }
                }

                if(BYTE1 == 0x10)    //reply to state 50 normal mode, still off
                {

                    if(SensoStick_state_machine==51)
                    {
                        SensoStick_state_machine=52;        //Motor 2
                        show_statemachine();
                        Encoder_Index=compute_encoder_index( BYTE7, BYTE8);
                        ui->Encoder_Indexvalue_2_label->setText(QString::number(Encoder_Index));
                    }
/*
                    if(SensoStick_state_machine==91)
                    {
                        SensoStick_state_machine=92;        //Motor 2
                        show_statemachine();
                        Encoder_Index=compute_encoder_index( BYTE7, BYTE8);
                        ui->Encoder_Indexvalue_2_label->setText(QString::number(Encoder_Index));
                    }
*/

                    if(SensoStick_state_machine==34)
                    {
                        reference_drive_2();  //das wechselt in state 32
                    }
                }


                if((BYTE1 == 0x12)&&(SensoStick_state_machine==61))    //reply to state 61 normal mode, ready state
                {
                    SensoStick_state_machine=62;        //Motor 2
                    show_statemachine();
                    Encoder_Index=compute_encoder_index( BYTE7, BYTE8);
                    ui->Encoder_Indexvalue_2_label->setText(QString::number(Encoder_Index));
                }


                if((BYTE1 == 0x14)&&(SensoStick_state_machine==71))    //reply to state 71 normal mode, ready state
                {
                    SensoStick_state_machine=72;        //Motor 2
                    show_statemachine();
                    Encoder_Index=compute_encoder_index( BYTE7, BYTE8);
                    ui->Encoder_Indexvalue_2_label->setText(QString::number(Encoder_Index));
                }

            break;

                //reply of communication test
                case 0x103:
                    ui->CAN_communication_test_3_checkBox->setChecked(true);
                    show_status(BYTE1,BYTE2);
                    show_error(BYTE3,BYTE4);
                    if(first_CAN_comm_message_3==false)
                    {
                        processed_axis=1;
                        SensoStick_state_machine=13;
                        show_statemachine();
                        first_CAN_comm_message_3= true;
                    }

                    last_CAN_error= BYTE3 + BYTE4*256;
                    if(BYTE1 == 0x42)    //reply to first reference drive command; statemachine =32
                    {
                        Encoder_Index= compute_encoder_index( BYTE7, BYTE8);
                        ui->Encoder_Indexvalue_3_label->setText(QString::number(Encoder_Index));
                        //now the third axis has the first reference command
                        //SensoStick_state_machine=40;
                        //show_statemachine();
                    }
                    if(BYTE1 == 0x44)    //reply to second reference drive command; statemachine =42, third motor
                    {
                        Encoder_Index=compute_encoder_index( BYTE7, BYTE8);
                        ui->Encoder_Indexvalue_3_label->setText(QString::number(Encoder_Index));
                        //we are in state 42 and have to wait until the reference drive for the third axis is done

                        if((BYTE2==0x68)&&(SensoStick_state_machine==42))
                        {

                            //qDebug()<<"End Ref 42 (M3) det.";
                            SensoStick_state_machine=45;
                            //SensoStick_state_machine=80;
                            //SensoStick_state_machine=50;  //direkt nach Referenzfahrt zum Normal Mode
                            show_statemachine();
                            ref_drive_axis=0;
                            processed_axis=1;

                            ui->Start_pushButton->setDisabled(true);
                            ui->SensoStick_CAN_Comm_pushButton->setDisabled(true);
                            ui->Check_Motorencoder_pushButton->setDisabled(true);
                            ui->ReferenceDrive_pushButton->setDisabled(true);
                            ui->Normal_Mode_Off_pushButton->setDisabled(false);
                            ui->Normal_Mode_Ready_pushButton->setDisabled(true);
                            ui->Normal_Mode_On_pushButton->setDisabled(true);
                            ui->Normal_Mode_Spring_Damper_pushButton->setDisabled(true);
                            ui->Stop_Senso_Stick_pushButton->setDisabled(false);
                        }
                    }


                    if(BYTE1 == 0x10)    //reply to state 50 normal mode, still off
                    {

                        if(SensoStick_state_machine==52)
                        {
                            SensoStick_state_machine=53;        //Motor 3// 60 hier Wartestate 53 eingefügt wie state 20
                            show_statemachine();
                            Encoder_Index=compute_encoder_index( BYTE7, BYTE8);
                            ui->Encoder_Indexvalue_3_label->setText(QString::number(Encoder_Index));
                        }
/*
                        if(SensoStick_state_machine==92)
                        {
                            SensoStick_state_machine=90;        //Motor 3
                            show_statemachine();
                            Encoder_Index=compute_encoder_index( BYTE7, BYTE8);
                            ui->Encoder_Indexvalue_3_label->setText(QString::number(Encoder_Index));
                        }
*/
                        if(SensoStick_state_machine==37)
                        {
                            reference_drive_3();  //das wechselt in state 32
                        }
                    }

                    if((BYTE1 == 0x12)&&(SensoStick_state_machine==62))    //reply to state 62 normal mode, ready state
                    {
                        SensoStick_state_machine=63;            //nach Zustand Ready state 63, wie state 20 und 53 Encoderpositionen
                        show_statemachine();
                        Encoder_Index=compute_encoder_index( BYTE7, BYTE8);
                        ui->Encoder_Indexvalue_3_label->setText(QString::number(Encoder_Index));
                        if(ui->switch_off_acknowledge_button_radioButton->isChecked()==false)
                        {
                            ui->Enable_Button_label->setText("Press Enable Button!");
                        }

                    }


                    if((BYTE1 == 0x14)&&(SensoStick_state_machine==72))    //reply to state 72 normal mode, ready state
                    {
                        SensoStick_state_machine=73;    //Motor 3 //nach Zustand Ready state 73, wie state 20 und 53 und 63 Encoderpositionen
                        show_statemachine();
                        //alive_counter_return_value=0;
                        Encoder_Index=compute_encoder_index( BYTE7, BYTE8);
                        ui->Encoder_Indexvalue_3_label->setText(QString::number(Encoder_Index));
                    }


                break;




        //reply of check motorencoder
        case 0x111:
            mot_enc_pos_1= compute_motorencoder_position( BYTE1, BYTE2, BYTE3);
            //qDebug()<<BYTE3<<"\t"<<BYTE2<<"\t"<<BYTE1<<"\t";
            Motor_Enc_1_Pos_String= QString::number(mot_enc_pos_1);
            ui->Motorencoderpos_value_1_label->setText(Motor_Enc_1_Pos_String);
            alive_counter_current_return_value= BYTE4;
            if(paste_over_network==true)
            {
                send_position_data_to_vehicle_model_PC(1, mot_enc_pos_1);
                //qDebug()<<"sent pos";
            }

            if(SensoStick_state_machine==80)
            {
                SensoStick_state_machine=81;
                show_statemachine();
            }
        break;

        case 0x112:
            mot_enc_pos_2= compute_motorencoder_position( BYTE1, BYTE2, BYTE3);
            Motor_Enc_2_Pos_String= QString::number(mot_enc_pos_2);
            ui->Motorencoderpos_value_2_label->setText(Motor_Enc_2_Pos_String);
            alive_counter_current_return_value= BYTE4;
            if(paste_over_network==true)
            {
                send_position_data_to_vehicle_model_PC(2, mot_enc_pos_2);
            }
            if(SensoStick_state_machine==81)
            {
                SensoStick_state_machine=82;
                show_statemachine();
            }
        break;

        case 0x113:
            mot_enc_pos_3= compute_motorencoder_position( BYTE1, BYTE2, BYTE3);
            Motor_Enc_3_Pos_String= QString::number(mot_enc_pos_3);
            ui->Motorencoderpos_value_3_label->setText(Motor_Enc_3_Pos_String);
            alive_counter_current_return_value= BYTE4;
            if(paste_over_network==true)
            {
                send_position_data_to_vehicle_model_PC(3, mot_enc_pos_3);
            }
            if(SensoStick_state_machine==82)
            {
                SensoStick_state_machine=85;    //this will build a loop betwenn state 80 and 82, if this doesn't work make a wait state 90 as in state 45
                show_statemachine();
            }
        break;


        case 0x151:

            decode_digital_1msg( BYTE1, BYTE2);

            if(SensoStick_state_machine==85)
            {
                SensoStick_state_machine=86;    //this will build a loop betwenn state 80 and 82, if this doesn't work make a wait state 90 as in state 45
                show_statemachine();
            }
        break;

        case 0x152:
            decode_digital_2msg(BYTE1,BYTE2);

            if(SensoStick_state_machine==86)
            {
                SensoStick_state_machine=80;    //this will build a loop betwenn state 80 and 82, if this doesn't work make a wait state 90 as in state 45
                show_statemachine();
            }
        break;

        default:

        break;
    }


    if(id==0x10)
    {

    }
   //qDebug()<<alive_counter_return_value;
   //qDebug()<<SensoStick_state_machine;
    //qDebug()<<"ret: "<<alive_counter_current_return_value<<"   set:" <<alive_counter_set_value;

}




void Senso_Stick::decode_digital_1msg(byte BYTE1, byte BYTE2)
{
    unsigned int sixteen_bitval= BYTE2*256+ BYTE1;
    send_button_1_data_to_vehicle_model_PC(sixteen_bitval);

    enable_button_state= false;
    mini_stick_state= 0;


    if(sixteen_bitval & 0x01)
    {
        enable_button_state=true;   //pressed
    }

    if(sixteen_bitval & 0x02)
    {
        mini_stick_state= 1; //left
    }
    if(sixteen_bitval & 0x04)
    {
        mini_stick_state= 2; //right
    }
    if(sixteen_bitval & 0x08)
    {
        mini_stick_state= 3; //up
    }
    if(sixteen_bitval & 0x10)
    {
        mini_stick_state= 4; //down
    }
    if(sixteen_bitval & 0x20)
    {
        mini_stick_state= 5; //pressed
    }

    if(enable_button_state == false)    //not prsessed
    {
        ui->Enable_Button_radioButton->setChecked(false);
    }
    else
    {
        ui->Enable_Button_radioButton->setChecked(true);
    }

    if(mini_stick_state==0)    //middle
    {
        ui->Mini_Stick_left_radioButton->setChecked(false);
        ui->Mini_Stick_right_radioButton->setChecked(false);
        ui->Mini_Stick_middle_radioButton->setChecked(true);
        changeColorMyButton();
        ui->Mini_Stick_up_radioButton->setChecked(false);
        ui->Mini_Stick_down_radioButton->setChecked(false);
    }
    if(mini_stick_state==1)    //left
    {
        ui->Mini_Stick_left_radioButton->setChecked(true);
        ui->Mini_Stick_right_radioButton->setChecked(false);
        ui->Mini_Stick_middle_radioButton->setChecked(false);
        changeColorMyButton();
        ui->Mini_Stick_up_radioButton->setChecked(false);
        ui->Mini_Stick_down_radioButton->setChecked(false);
    }
    if(mini_stick_state==2)    //right
    {
        ui->Mini_Stick_left_radioButton->setChecked(false);
        ui->Mini_Stick_right_radioButton->setChecked(true);
        ui->Mini_Stick_middle_radioButton->setChecked(false);
        changeColorMyButton();
        ui->Mini_Stick_up_radioButton->setChecked(false);
        ui->Mini_Stick_down_radioButton->setChecked(false);
    }

    if(mini_stick_state==3)    //up
    {
        ui->Mini_Stick_left_radioButton->setChecked(false);
        ui->Mini_Stick_right_radioButton->setChecked(false);
        ui->Mini_Stick_middle_radioButton->setChecked(false);
        changeColorMyButton();
        ui->Mini_Stick_up_radioButton->setChecked(true);
        ui->Mini_Stick_down_radioButton->setChecked(false);
    }
    if(mini_stick_state==4)    //down
    {
        ui->Mini_Stick_left_radioButton->setChecked(false);
        ui->Mini_Stick_right_radioButton->setChecked(false);
        ui->Mini_Stick_middle_radioButton->setChecked(false);
        changeColorMyButton();
        ui->Mini_Stick_up_radioButton->setChecked(false);
        ui->Mini_Stick_down_radioButton->setChecked(true);
    }
    if(mini_stick_state==5)    //pressed
    {
        ui->Mini_Stick_left_radioButton->setChecked(false);
        ui->Mini_Stick_right_radioButton->setChecked(false);
        ui->Mini_Stick_middle_radioButton->setChecked(true);
        changeColorMyButton();
        ui->Mini_Stick_up_radioButton->setChecked(false);
        ui->Mini_Stick_down_radioButton->setChecked(false);
    }
}

//to change the color of the middle button
void Senso_Stick::changeColorMyButton(void)
{
    if(mini_stick_state==5)
    {
        ui->Mini_Stick_middle_radioButton->setStyleSheet("background-color: green");
    }
    else
    {
        ui->Mini_Stick_middle_radioButton->setStyleSheet("background-color: none");
    }
}

void Senso_Stick::decode_digital_2msg(byte BYTE1,byte BYTE2)
{
    unsigned int sixteen_bitval= BYTE2*256+BYTE1;
    send_button_2_data_to_vehicle_model_PC(sixteen_bitval);

    flip_switch_state= 0; //middle
    enable_button_state= false; //not pressed
    front_button_state= false; //not pressed


    if(sixteen_bitval & 0x01)
    {
        enable_button_state= true;   //pressed
    }

    if(sixteen_bitval & 0x02)
    {
        flip_switch_state= 1; //left
    }
    if(sixteen_bitval & 0x04)
    {
        flip_switch_state= 2; //right
    }
    if(sixteen_bitval & 0x08)
    {
        front_button_state= true; //pressed
    }


    if(enable_button_state == false)    //not prsessed
    {
        ui->Enable_Button_radioButton->setChecked(false);
    }
    else
    {
        ui->Enable_Button_radioButton->setChecked(true);
    }

    if(flip_switch_state==0)    //middle
    {
        ui->Flip_Switch_left_radioButton->setChecked(false);
        ui->Flip_Switch_middle_radioButton->setChecked(true);
        ui->Flip_Switch_right_radioButton->setChecked(false);
    }
    if(flip_switch_state==1)    //left
    {
        ui->Flip_Switch_left_radioButton->setChecked(true);
        ui->Flip_Switch_middle_radioButton->setChecked(false);
        ui->Flip_Switch_right_radioButton->setChecked(false);
    }
    if(flip_switch_state==2)    //right
    {
        ui->Flip_Switch_left_radioButton->setChecked(false);
        ui->Flip_Switch_middle_radioButton->setChecked(false);
        ui->Flip_Switch_right_radioButton->setChecked(true);
    }

    if(front_button_state == false)    //not prsessed
    {
        ui->Front_Button_radioButton->setChecked(false);
    }
    else
    {
        ui->Front_Button_radioButton->setChecked(true);
    }

}

double Senso_Stick::compute_encoder_index( BYTE byte_7, BYTE byte_8)
{
    return byte_7 + byte_8*256;
}

void Senso_Stick::show_status(BYTE byte_1, BYTE byte_2)
{
    unsigned int status_value= byte_1 + byte_2*256;


    if(status_value & 0x00)
    {
        //Status_String.append("state off");
        ui->Status_textBrowser->append("state off");
    }
    if(status_value & 0x02)
    {
        //Status_String.append("state ready");
        ui->Status_textBrowser->append("state ready");
    }
    if(status_value & 0x04)
    {
        //Status_String.append("state on, motor active");
        ui->Status_textBrowser->append("state on, motor active");
    }
    if(status_value & 0x08)
    {
        //Status_String.append("state error");
        ui->Status_textBrowser->append("state error");
    }
    if(status_value & 0x10)
    {
        //Status_String.append("operation mode: normal");
        ui->Status_textBrowser->append("operation mode: normal");
    }
    if(status_value & 0x20)
    {
        //Status_String.append("operation mode: base");
        ui->Status_textBrowser->append("operation mode: base");
    }
    if(status_value & 0x40)
    {
        //Status_String.append("operation mode: reference drive");
        ui->Status_textBrowser->append("operation mode: reference drive");
    }
    if(status_value & 0x80)
    {
        //Status_String.append("alive counter activated");
        ui->Status_textBrowser->append("alive counter activated");
    }
    if(status_value & 0x100)
    {
        //Status_String.append("CAN watchdog off");
        ui->Status_textBrowser->append("CAN watchdog off");
    }
    if(status_value & 0x200)
    {
        //Status_String.append("over curr. prot. active");
        ui->Status_textBrowser->append("over curr. prot. active");
    }
    if(status_value & 0x400)
    {
        //Status_String.append("end stops active");
        ui->Status_textBrowser->append("end stops active");
    }
    if(status_value & 0x800)
    {
        //Status_String.append("encoderindex");
        ui->Status_textBrowser->append("encoderindex");
    }
    if(status_value & 0x1000)
    {
        //Status_String.append("acknowledge button off");
        ui->Status_textBrowser->append("acknowledge button off");
    }
    if(status_value & 0x2000)
    {
        //Status_String.append("reference drive finished");
        ui->Status_textBrowser->append("reference drive finished");
    }
    if(status_value & 0x4000)
    {
        //Status_String.append("test CPU watchdog successful");
        ui->Status_textBrowser->append("test CPU watchdog successful");
    }
    ui->Status_textBrowser->append("\n");
}

void Senso_Stick::show_error(BYTE byte_3, BYTE byte_4)
{
    unsigned int error_value= byte_3 + byte_4*256;


    if(error_value & 0x01)
    {
        //Error_String.append("overcurrent power stage");
        ui->Error_textBrowser->append("overcurrent power stage");
    }
    if(error_value & 0x02)
    {
        //Error_String.append("supply voltage too high");
        ui->Error_textBrowser->append("supply voltage too high");
    }
    if(error_value & 0x04)
    {
        //Error_String.append("supply voltage too low");
        ui->Error_textBrowser->append("supply voltage too low");
    }
    if(error_value & 0x08)
    {
        //Error_String.append("overtemperature power stage");
        ui->Error_textBrowser->append("overtemperature power stage");
    }
    if(error_value & 0x10)
    {
        //Error_String.append("error in hall sensor");
        ui->Error_textBrowser->append("error in hall sensor");
    }
    if(error_value & 0x20)
    {
        //Error_String.append("CAN error");
        ui->Error_textBrowser->append("CAN error");
    }
    if(error_value & 0x40)
    {
        //Error_String.append("hall sensors mixed up");
        ui->Error_textBrowser->append("hall sensors mixed up");
    }
    if(error_value & 0x80)
    {
        //Error_String.append("watchdog exceeded time limit");
        ui->Error_textBrowser->append("watchdog exceeded time limit");
    }
    if(error_value & 0x100)
    {
        //Error_String.append("position not allowed >90°");
        ui->Error_textBrowser->append("position not allowed >90°");
    }
    if(error_value & 0x200)
    {
        //Error_String.append("position grater than limit");
        ui->Error_textBrowser->append("position grater than limit");
    }
    if(error_value & 0x400)
    {
        //Error_String.append("reference drive exceeded time limit");
        ui->Error_textBrowser->append("reference drive exceeded time limit");
    }
    if(error_value & 0x800)
    {
        //Error_String.append("overtemperature brake chopper");
        ui->Error_textBrowser->append("overtemperature brake chopper");
    }
    if(error_value & 0x1000)
    {
        //Error_String.append("alive counter error (diff>1)");
        ui->Error_textBrowser->append("alive counter error (diff>1)");
        //qDebug()<<"Error: ret: "<<alive_counter_current_return_value<<"   set:" <<alive_counter_set_value;
    }
    if(error_value & 0x2000)
    {
        //Error_String.append("collective error");
        ui->Error_textBrowser->append("collective error");
    }
    if(error_value & 0x4000)
    {
        //Error_String.append("wrong update-telegram count");
        ui->Error_textBrowser->append("wrong update-telegram count");
    }
    if(error_value & 0x8000)
    {
        //Error_String.append("update error checksum");
        ui->Error_textBrowser->append("update error checksum");
    }
    if((error_value & 0xFFFF)!=0)
    {
        ui->Error_textBrowser->append("\n");
    }
}

double Senso_Stick::compute_motorencoder_position(BYTE byte_1, BYTE byte_2, BYTE byte_3)
{
    unsigned int value=  byte_3*65536+ byte_2*256 + byte_1;
    unsigned int pos_value;
    if(value>=8388608)
    {
        pos_value= 16777216- value;
        return (-0.001)*pos_value;
    }
    else
    {
        pos_value= value;
        return (0.001)*pos_value;
    }
/*
    if(byte_3==0x00)
    {
        return ( byte_2*256 + byte_1)*0.001;
    }
    if(byte_3==0xFF)
    {
        return -( (255-byte_2)*256 + 255-byte_1)*0.001;
    }
*/
}






