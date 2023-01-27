#include "mainwindow.h"
#include "ui_mainwindow.h"

//das macht mal eben 167 Fehler weg
#include <minwindef.h>
#include "public.h"

//diese beiden typedefs waren das Problem
//normalerweise in <winuser.h>, aber dann gibt es 1800 Fehlermeldungen
typedef void *PVOID;
typedef PVOID HDEVNOTIFY;
#include "vjoyinterface.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    UDP_receive_position_from_Senso_Stick_socket = new QUdpSocket(this);
    UDP_receive_button_1_from_Senso_Stick_socket = new QUdpSocket(this);
    UDP_receive_button_2_from_Senso_Stick_socket = new QUdpSocket(this);

    UDP_receive_position_from_Senso_Stick_socket->bind(QHostAddress::Any, 5346);
    UDP_receive_button_1_from_Senso_Stick_socket->bind(QHostAddress::Any, 5348);
    UDP_receive_button_2_from_Senso_Stick_socket->bind(QHostAddress::Any, 5349);


    connect(UDP_receive_position_from_Senso_Stick_socket, SIGNAL(readyRead()), this, SLOT(process_UDP_Senso_Stick_position_message()));
    connect(UDP_receive_button_1_from_Senso_Stick_socket, SIGNAL(readyRead()), this, SLOT(process_UDP_Senso_Stick_button_1_message()));
    connect(UDP_receive_button_2_from_Senso_Stick_socket, SIGNAL(readyRead()), this, SLOT(process_UDP_Senso_Stick_button_2_message()));

    ui->mot_enc_1_label->setText("");
    ui->mot_enc_2_label->setText("");
    ui->mot_enc_3_label->setText("");

    ui->mot_enc_1_label->setText("");
    ui->mot_enc_2_label->setText("");
    ui->mot_enc_3_label->setText("");

    sliders_maxval=20;
    sliders_scaling_constant= 10.2;

    dial_maxval=17;
    dial_scaling_constant= 5.0;

    ui->horizontalSlider->setMaximum(my_round(sliders_maxval*sliders_scaling_constant));
    ui->horizontalSlider->setMinimum(-my_round(sliders_maxval*sliders_scaling_constant));
    ui->verticalSlider->setMaximum(my_round(sliders_maxval*sliders_scaling_constant));
    ui->verticalSlider->setMinimum(-my_round(sliders_maxval*sliders_scaling_constant));

    ui->dial->setMaximum(my_round(sliders_maxval*sliders_scaling_constant));
    ui->dial->setMinimum(-my_round(sliders_maxval*sliders_scaling_constant));
    //ui->dial->setValue(5);

    ui->SensoStick_label->setText("CAN SensoStick");



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

    ui->Hat2_up_radioButton->setAutoExclusive(false);
    ui->Hat2_right_radioButton->setAutoExclusive(false);
    ui->Hat2_down_radioButton->setAutoExclusive(false);
    ui->Hat2_left_radioButton->setAutoExclusive(false);
    ui->Hat2_middle_radioButton->setAutoExclusive(false);

    ui->Hat2_up_radioButton->setText("");
    ui->Hat2_right_radioButton->setText("");
    ui->Hat2_down_radioButton->setText("");
    ui->Hat2_left_radioButton->setText("");
    ui->Hat2_middle_radioButton->setText("");

    mini_stick_state=0;
    changeColorMyButton();

    //USB Joystick**********************************************************************
    joy = new QJoystick;
    //joy = new SDL_Joystick;;
    data_timer = new QTimer;

    data_timer->setInterval(get_USBdata_ms);


    // Initialize joysticks

    init_joystick();

    // If there's a joystick present, initialize axes and buttons displays
    // and start the update timer.
    if(joysavail>0)
    {
        connect(data_timer,SIGNAL(timeout()),this,SLOT(pollJoystick()));
        data_timer->start();
    }

    sixteen_bit_pos= 32767;
    sixteen_bit_neg= -32768;



    //ui->USB_Joystick_label->setText("USB Flight Setup");

    ui->USB_Collective_power_dial->setMinimum(sixteen_bit_neg);
    ui->USB_Collective_power_dial->setMaximum(sixteen_bit_pos);

    ui->USB_Collective_verticalSlider->setMinimum(sixteen_bit_neg);
    ui->USB_Collective_verticalSlider->setMaximum(sixteen_bit_pos);

    ui->USB_Centerstick_horizontalSlider->setMinimum(sixteen_bit_neg);
    ui->USB_Centerstick_horizontalSlider->setMaximum(sixteen_bit_pos);

    ui->USB_Centerstick_verticalSlider->setMinimum(sixteen_bit_neg);
    ui->USB_Centerstick_verticalSlider->setMaximum(sixteen_bit_pos);

    ui->USB_Pedals_horizontalSlider->setMinimum(sixteen_bit_neg);
    ui->USB_Pedals_horizontalSlider->setMaximum(sixteen_bit_pos);

    ui->USB_Centerstick_left_middle_radioButton->setAutoExclusive(false);
    ui->USB_Centerstick_left_top_radioButton->setAutoExclusive(false);
    ui->USB_Centerstick_left_down_radioButton->setAutoExclusive(false);
    ui->USB_Centerstick_Front_radioButton->setAutoExclusive(false);

    ui->collective_left_switch_up_radioButton->setAutoExclusive(false);
    ui->collective_left_switch_middle_radioButton->setAutoExclusive(false);
    ui->collective_left_switch_down_radioButton->setAutoExclusive(false);

    ui->collective_left_switch_up_radioButton->setText("");
    ui->collective_left_switch_middle_radioButton->setText("");
    ui->collective_left_switch_down_radioButton->setText("");

    ui->USB_Collective_left_radioButton->setAutoExclusive(false);
    ui->USB_Collective_right_radioButton->setAutoExclusive(false);


    ui->USB_Centerstick_left_middle_radioButton->setText("");
    ui->USB_Centerstick_left_top_radioButton->setText("");
    ui->USB_Centerstick_left_down_radioButton->setText("");
    ui->USB_Centerstick_Front_radioButton->setText("");

    ui->USB_Collective_left_radioButton->setText("");
    ui->USB_Collective_right_radioButton->setText("");


    //for VJoy
    vJoy_DevID=1;
    vJoy_device_acquired= false;

    update_vJoy_timer= new QTimer;
    update_vJoy_timer->setInterval(update_vJoy_ms_interval);
    connect(update_vJoy_timer, SIGNAL(timeout()), this, SLOT(update_vJoy_data()));


    check_if_vJoy_dll_matches_driver();
    get_VJoy_information();
    acquire_VJoy_device();

    iInterface=1;
    DevID= DEV_ID;
    //reset_VJoy_Interface();

    ui->activate_Senso_Stick_radioButton->setText("");
    ui->activate_Puma_Center_Stick_radioButton->setText("");

    Stick_Device= Senso_Stick;  //or Puma_Center_Stick
    if(Stick_Device==Senso_Stick)
    {
        ui->activate_Senso_Stick_radioButton->setChecked(true);
        ui->activate_Puma_Center_Stick_radioButton->setChecked(false);
        ui->Device_label->setText("Senso-Stick");
    }
    if(Stick_Device==Puma_Center_Stick)
    {
        ui->activate_Senso_Stick_radioButton->setChecked(false);
        ui->activate_Puma_Center_Stick_radioButton->setChecked(true);
        ui->Device_label->setText("Puma_Center_Stick");
    }
    connect(ui->activate_Senso_Stick_radioButton, SIGNAL(pressed()), this, SLOT(switch_to_Senso_Stick()));
    connect(ui->activate_Puma_Center_Stick_radioButton, SIGNAL(pressed()), this, SLOT(switch_to_Puma_Center_Stick()));


    ui->Sensostick_yaw_label->setText("(yaw over Sensostick)");
    ui->Senso_Stick_yaw_checkBox->setText("yaw");
    //ui->Senso_Stick_yaw_checkBox->setAutoExclusive(false);
    ui->Senso_Stick_yaw_checkBox->setChecked(false);
    yaw_control= Puma_Center_Stick;
    if(Stick_Device==Puma_Center_Stick)
    {
        ui->Senso_Stick_yaw_checkBox->setDisabled(true);
        yaw_control= Puma_Center_Stick;
    }
    connect(ui->Senso_Stick_yaw_checkBox, SIGNAL(pressed()), this, SLOT(handle_yaw_control()));
    //connect(ui->Senso_Stick_yaw_checkBox, SIGNAL(released()), this, SLOT(handle_yaw_by_Puma_Center_Stick()));

    trim_value_yaw= 1960;
    trim_value_yaw_stepsize= 5;
    trim_value_yaw_max=10000;

    ui->trim_yaw_label->setText("Trim: ");
    ui->trim_yaw_value_label->setText(QString::number(trim_value_yaw));

    trim_value_roll= -1120;
    trim_value_roll_stepsize=20;
    trim_value_roll_max=10000;

    ui->trim_roll_label->setText("Trim: ");
    ui->trim_roll_value_label->setText(QString::number(trim_value_roll));

    trim_value_pitch= 4120;
    trim_value_pitch_stepsize=20;
    trim_value_pitch_max=10000;

    ui->trim_pitch_label->setText("Trim: ");
    ui->trim_pitch_value_label->setText(QString::number(trim_value_pitch));

    intmax= 32767;
    intmin= -32768;

    //IHA correctiosn
    //socket
    //IHA
    UDP_receive_iha_corrections_socket = new QUdpSocket(this);
    UDP_receive_iha_corrections_socket->bind(QHostAddress::Any, 25346);
    connect(UDP_receive_iha_corrections_socket, SIGNAL(readyRead()), this, SLOT(process_UDP_iha_corrections_message()));

    //watchdog (resets correctiosn is no udp packet is received for 5 seconds
    iha_watchdog = 0;
    iha_watchdog_timer = new QTimer;
    iha_watchdog_timer->setInterval(500);
    connect(iha_watchdog_timer, SIGNAL(timeout()), this, SLOT(iha_watchdog_timer_inc()));

    //initialize corrections
    iha_correction_roll = 0;
    iha_correction_pitch = 0;
    iha_correction_yaw = 0;


}

MainWindow::~MainWindow()
{
    if(vJoy_device_acquired == true)
    {
        update_vJoy_timer->stop();
        RelinquishVJD(vJoy_DevID);
        vJoy_device_acquired= false;
    }

    delete ui;
}

void MainWindow::handle_yaw_control()
{
     if(yaw_control== Senso_Stick)
     {
         yaw_control= Puma_Center_Stick;
     }
     else if(yaw_control== Puma_Center_Stick)
     {
         yaw_control= Senso_Stick;
     }
     //qDebug()<<yaw_control;
}


void MainWindow::switch_to_Puma_Center_Stick()
{
    Stick_Device= Puma_Center_Stick;
    ui->Device_label->setText("Puma_Center_Stick");
    ui->Senso_Stick_yaw_checkBox->setDisabled(true);
    ui->Senso_Stick_yaw_checkBox->setChecked(false);
    yaw_control= Puma_Center_Stick;
}
void MainWindow::switch_to_Senso_Stick()
{
    Stick_Device= Senso_Stick;
    ui->Device_label->setText("Senso-Stick");
    ui->Senso_Stick_yaw_checkBox->setEnabled(true);
}

void MainWindow::reset_VJoy_Interface()
{
    ResetVJD(iInterface);
}

LONG MainWindow::convert_to_VJoy(int value, int sign)
{
    if(sign>=0)
    {
        return value/2+16384;   //32768/2
    }
    else
    {
        return 32767-(value/2+16384);
    }
}

void MainWindow::update_vJoy_data()
{
    //vjoy_data_struct vjoy_data;
    id= (BYTE) DevID;
    iReport.bDevice = id;


    if(Stick_Device==Senso_Stick)
    {
        //roll

        qDebug() << vjoy_data.SensoStick_axis_left_right;

        /*
        vjoy_data.SensoStick_axis_left_right=vjoy_data.SensoStick_axis_left_right+trim_value_roll;
        if(vjoy_data.SensoStick_axis_left_right> intmax)
        {
            vjoy_data.SensoStick_axis_left_right= intmax;
        }
        if(vjoy_data.SensoStick_axis_left_right< intmin)
        {
            vjoy_data.SensoStick_axis_left_right= intmin;
        }*/
        //UNIPI: moved to callback otherwise it may happen that a udp callabcak is called AFTER the trim value is added BUT BEFORE the data is sent to vjoy (call to UpdateVJD)

        iReport.wAxisXRot= convert_to_VJoy(vjoy_data.SensoStick_axis_left_right,1);
        //qDebug() << vjoy_data.SensoStick_axis_left_right;
        //pitch
        /*vjoy_data.SensoStick_axis_forward_backward=vjoy_data.SensoStick_axis_forward_backward+trim_value_pitch;
        if(vjoy_data.SensoStick_axis_forward_backward> intmax)
        {
            vjoy_data.SensoStick_axis_forward_backward= intmax;
        }
        if(vjoy_data.SensoStick_axis_forward_backward< intmin)
        {
            vjoy_data.SensoStick_axis_forward_backward= intmin;
        }*/
        iReport.wAxisYRot= convert_to_VJoy(vjoy_data.SensoStick_axis_forward_backward,1);


        iReport.bHats= vjoy_data.SensoStick_mini_stick_state;
        iReport.lButtons= (vjoy_data.SensoStick_enable_button_state<<1)|(vjoy_data.SensoStick_front_button_state<<2)|(vjoy_data.USB_Collective_button_left<<3)|(vjoy_data.USB_Collective_button_right<<4);
        /*
        if(yaw_control==Senso_Stick)
        {
            iReport.wAxisZRot= convert_to_VJoy(vjoy_data.SensoStick_axis_twist,1);
        }
        else if(yaw_control==Puma_Center_Stick)
        {
            iReport.wAxisZRot= convert_to_VJoy(vjoy_data.USB_Pedals,1);
        }
        */
    }
    else if(Stick_Device==Puma_Center_Stick)
    {
        iReport.wAxisXRot= convert_to_VJoy(vjoy_data.USB_Center_Stick_left_right,1);
        iReport.wAxisYRot= convert_to_VJoy(vjoy_data.USB_Center_Stick_forward_backward,-1);
        iReport.bHats= vjoy_data.USB_Center_Stick_hat;
        //gebe nur die Buttons an VJoy, die auch auf dem Sensostick vorhanden sind
        iReport.lButtons= (vjoy_data.USB_Center_Stick_button_front<<1)|(vjoy_data.USB_Center_Stick_button_top<<2)|(vjoy_data.USB_Collective_button_left<<3)|(vjoy_data.USB_Collective_button_right<<4);
        //iReport.wAxisZRot= convert_to_VJoy(vjoy_data.USB_Pedals,1);
    }

    iReport.wAxisZ= convert_to_VJoy(vjoy_data.USB_Collective_up_down,-1);
    //iReport.wAxisZRot= convert_to_VJoy(vjoy_data.USB_Pedals,1);
    if(yaw_control==Senso_Stick)
    {
        //yaw
        /*vjoy_data.SensoStick_axis_twist=vjoy_data.SensoStick_axis_twist+trim_value_yaw;
        if(vjoy_data.SensoStick_axis_twist> intmax)
        {
            vjoy_data.SensoStick_axis_twist= intmax;
        }
        if(vjoy_data.SensoStick_axis_twist< intmin)
        {
            vjoy_data.SensoStick_axis_twist= intmin;
        }*/

        iReport.wAxisZRot= convert_to_VJoy(vjoy_data.SensoStick_axis_twist,1);
    }
    else if(yaw_control==Puma_Center_Stick)
    {
        //iReport.wAxisZRot= convert_to_VJoy(vjoy_data.USB_Pedals,1);
        iReport.wAxisZRot= 17285;//16384
    }
    //iReport.wAxisXRot= convert_to_VJoy(vjoy_data.USB_Collective_power_twist,1);
    iReport.wSlider= convert_to_VJoy(vjoy_data.USB_Collective_power_twist,1);

    pPositionMessage= (PVOID)(&iReport);

    if(!UpdateVJD(DevID, pPositionMessage))
    {
        qDebug()<<"Feeding VJoy device failed ";
    }
    //qDebug()<<yaw_control<<"   "<<iReport.wAxisZRot;
}

void MainWindow::get_VJoy_information()
{
    if(!vJoyEnabled())
    {
        qDebug()<<"failed to get VJoy driver attributes";
    }
    else
    {
        qDebug()<<(char*)(GetvJoyManufacturerString());
        qDebug()<<(char*)(GetvJoyProductString());
        qDebug()<<(char*)(GetvJoySerialNumberString());
    }


    VjdStat status= GetVJDStatus(vJoy_DevID);
    switch (status)
    {
        case VJD_STAT_OWN:
            qDebug()<<"vJoy device: "<<vJoy_DevID<<" already owned by this feeder";
            break;
        case VJD_STAT_FREE:
            qDebug()<<"vJoy device: "<<vJoy_DevID<<" is free";
            break;
        case VJD_STAT_BUSY:
            qDebug()<<"vJoy device: "<<vJoy_DevID<<" is already owned by another feeder";
            break;
        case VJD_STAT_MISS:
            qDebug()<<"vJoy device: "<<vJoy_DevID<<" is not installed or disabled";
            break;
        default:
            qDebug()<<"vJoy device: "<<vJoy_DevID<<" has a general error";
            break;

    }
}

void MainWindow::check_if_vJoy_dll_matches_driver()
{
    WORD VerDLL, VerDrv;
    if(!DriverMatch(&VerDLL, &VerDrv))
    {
        qDebug()<<"vJoy Driver: "<<VerDrv<<" does not match vJoyInterface DLL: " << VerDLL;
    }
    else
    {
        qDebug()<<"vJoy Driver: "<<VerDrv<<" matches vJoyInterface DLL: " << VerDLL;
    }
}

void MainWindow::acquire_VJoy_device()
{
    if(!AcquireVJD(vJoy_DevID))
    {
        qDebug()<<"could not acquire vJoy device: "<<vJoy_DevID;
    }
    else
    {
        qDebug()<<"vJoy device: "<<vJoy_DevID<<" acquired";
        vJoy_device_acquired= true;
        update_vJoy_timer->start();
    }
}



void MainWindow::init_joystick()
{


    // Find number of joysticks present
    joysavail=joy->availableJoysticks();

    // Create joysticks list







        // Populate labels depending on the number of joysticks found
int selected_joystick = 0;
            if(joysavail==1)
            {
                qDebug()<<"1 USB joystick found, perfect!";
            }
            else if(joysavail==0)
            {
                qDebug()<<"sorry, no USB joysticks found";
            }
            else if(joysavail>1)
            {
                qDebug()<<"sorry, more than 1 USB joystick found";
                for(unsigned int j=0; j<joysavail;j++)
                {
                    qDebug()<< joy->joystickName(j);
                    if (joy->joystickName(j) == QString("Pro-Flight-Trainer")) {
                        qDebug()<<"Puma found !";
                        selected_joystick = j;
                    }
                }
             }

            //selected_joystick = 1;

            // Populate data structure for all joysticks

            joy->setJoystick(selected_joystick);


            // Axes
            joystick.number_axes = joy->joystickNumAxes(selected_joystick);

            for(unsigned int j=0; j<joystick.number_axes;j++)
            {
                joystick.axis.append(0);
            }

            // Buttons
            joystick.number_btn  = joy->joystickNumButtons(selected_joystick);

            for(unsigned int j=0; j<joystick.number_btn;j++)
            {
                joystick.button.append(false);
            }

            //Hats
            joystick.number_hats  = joy->joystickNumHats(selected_joystick);
            qDebug()<<joystick.number_hats ;
            for(unsigned int j=0; j<joystick.number_hats;j++)
            {
                joystick.hats.append(0);
            }

            //Balls
            joystick.number_balls  = joy->joystickNumBalls(selected_joystick);

            for(unsigned int j=0; j<joystick.number_balls;j++)
            {
                joystick.balls.append(0);
            }


    QString Device_name= joy->joystickName(selected_joystick);
    ui->USB_Joystick_label->setText(Device_name);

    QString USB_axes_and_button_String= QString::number(joystick.number_axes);
    USB_axes_and_button_String.append(" axes, ");
    USB_axes_and_button_String.append(QString::number(joystick.number_btn));
    USB_axes_and_button_String.append(" button, ");
    USB_axes_and_button_String.append(QString::number(joystick.number_hats));
    USB_axes_and_button_String.append(" POV-hats and ");
    USB_axes_and_button_String.append(QString::number(joystick.number_balls));
    USB_axes_and_button_String.append(" balls");

    ui->axes_and_button_label->setText(USB_axes_and_button_String);


}

// Extracts data from QJoystick class
void MainWindow::pollJoystick()
{
    joy->getdata();


    //Axes
    for(unsigned int i=0;i<joystick.number_axes;i++)
    {
        joystick.axis[i]=joy->axis[i];
    }



    ui->USB_Centerstick_horizontalSlider->setValue(joystick.axis[0]);
    ui->USB_Centerstick_verticalSlider->setValue(-joystick.axis[1]);
    ui->USB_Collective_verticalSlider->setValue(-joystick.axis[2]);
    ui->USB_Pedals_horizontalSlider->setValue(joystick.axis[3]);
    ui->USB_Collective_power_dial->setValue(-joystick.axis[4]);



    vjoy_data.USB_Center_Stick_left_right= joystick.axis[0];
    vjoy_data.USB_Center_Stick_forward_backward= joystick.axis[1];
    vjoy_data.USB_Collective_up_down= joystick.axis[2]; //Collective
    vjoy_data.USB_Pedals= joystick.axis[3]; //Pedals
    vjoy_data.USB_Collective_power_twist= joystick.axis[4]; //Collective_power


    ui->centerstick_horizontal_value_label->setText(QString::number(joystick.axis[0]));
    ui->centerstick_vertical_value_label->setText(QString::number(-joystick.axis[1]));
    ui->collective_value_label->setText(QString::number(-joystick.axis[2]));
    ui->USB_pedals_value_label->setText(QString::number(joystick.axis[3]));
    ui->collective_power_value_label->setText(QString::number(joystick.axis[4]));



    //Buttons
    for(unsigned int i=0;i<joystick.number_btn;i++)
    {
        joystick.button[i] = joy->buttons[i];
    }

    if(joystick.button[3]==true)
    {
        ui->USB_Centerstick_left_middle_radioButton->setChecked(true);
    }
    else
    {
        ui->USB_Centerstick_left_middle_radioButton->setChecked(false);
    }

    if(joystick.button[2]==true)
    {
        ui->USB_Centerstick_left_top_radioButton->setChecked(true);
    }
    else
    {
        ui->USB_Centerstick_left_top_radioButton->setChecked(false);
    }

    if(joystick.button[1]==true)
    {
        ui->USB_Centerstick_left_down_radioButton->setChecked(true);
    }
    else
    {
        ui->USB_Centerstick_left_down_radioButton->setChecked(false);
    }

    if(joystick.button[0]==true)
    {
        ui->USB_Centerstick_Front_radioButton->setChecked(true);
    }
    else
    {
        ui->USB_Centerstick_Front_radioButton->setChecked(false);
    }

    //left front
    if(joystick.button[4]==true)
    {
        ui->USB_Collective_left_radioButton->setChecked(true);
    }
    else
    {
        ui->USB_Collective_left_radioButton->setChecked(false);
    }

    //right front
    if(joystick.button[5]==true)
    {
        ui->USB_Collective_right_radioButton->setChecked(true);
    }
    else
    {
        ui->USB_Collective_right_radioButton->setChecked(false);
    }

    //left collectice switch
    if(joystick.button[6]==true)
    {
        ui->collective_left_switch_up_radioButton->setChecked(true);
        ui->collective_left_switch_middle_radioButton->setChecked(false);
        ui->collective_left_switch_down_radioButton->setChecked(false);
    }
    else if(joystick.button[7]==true)
    {
        ui->collective_left_switch_up_radioButton->setChecked(false);
        ui->collective_left_switch_middle_radioButton->setChecked(false);
        ui->collective_left_switch_down_radioButton->setChecked(true);
    }
    else if((joystick.button[6]==false)&&(joystick.button[7]==false))
    {
        ui->collective_left_switch_up_radioButton->setChecked(false);
        ui->collective_left_switch_middle_radioButton->setChecked(true);
        ui->collective_left_switch_down_radioButton->setChecked(false);
    }



    /*
    for(unsigned int i=6;i<joystick.number_btn;i++)
    {
        if(joystick.button[i]!=0)
        {
            qDebug()<<"Button: "<<i;
        }
    }
    */


    vjoy_data.USB_Center_Stick_button_middle= joystick.button[3];
    vjoy_data.USB_Center_Stick_button_top= joystick.button[2];
    vjoy_data.USB_Center_Stick_button_down= joystick.button[1];
    vjoy_data.USB_Center_Stick_button_front= joystick.button[0];

    vjoy_data.USB_Collective_button_left= joystick.button[4];
    vjoy_data.USB_Collective_button_right= joystick.button[5];


    //Hats
    for(unsigned int i=0; i<joystick.number_hats; i++)
    {
        joystick.hats[i] = joy->hats[i];
    }
    //qDebug()<<joystick.hats[2];
    if(joystick.hats[2] == 0)
    {
        ui->Hat2_up_radioButton->setChecked(false);
        ui->Hat2_right_radioButton->setChecked(false);
        ui->Hat2_down_radioButton->setChecked(false);
        ui->Hat2_left_radioButton->setChecked(false);
        ui->Hat2_middle_radioButton->setChecked(true);
    }
    else if(joystick.hats[2] == 1)
    {
        ui->Hat2_up_radioButton->setChecked(true);
        ui->Hat2_right_radioButton->setChecked(false);
        ui->Hat2_down_radioButton->setChecked(false);
        ui->Hat2_left_radioButton->setChecked(false);
        ui->Hat2_middle_radioButton->setChecked(false);
    }
    else if(joystick.hats[2] == 2)
    {
        ui->Hat2_up_radioButton->setChecked(false);
        ui->Hat2_right_radioButton->setChecked(true);
        ui->Hat2_down_radioButton->setChecked(false);
        ui->Hat2_left_radioButton->setChecked(false);
        ui->Hat2_middle_radioButton->setChecked(false);
    }
    else if(joystick.hats[2] == 4)
    {
        ui->Hat2_up_radioButton->setChecked(false);
        ui->Hat2_right_radioButton->setChecked(false);
        ui->Hat2_down_radioButton->setChecked(true);
        ui->Hat2_left_radioButton->setChecked(false);
        ui->Hat2_middle_radioButton->setChecked(false);
    }
    else if(joystick.hats[2] == 8)
    {
        ui->Hat2_up_radioButton->setChecked(false);
        ui->Hat2_right_radioButton->setChecked(false);
        ui->Hat2_down_radioButton->setChecked(false);
        ui->Hat2_left_radioButton->setChecked(true);
        ui->Hat2_middle_radioButton->setChecked(false);
    }

    int PUMA_converted_POV_hat= convert_PUMA_POV_hat_to_Vjoy(joystick.hats[2]);
    vjoy_data.USB_Center_Stick_hat= PUMA_converted_POV_hat;


}

int MainWindow::convert_PUMA_POV_hat_to_Vjoy(int input)
{
    int output=8;
    switch(input)
    {
        case 0:
            output= 8;
        break;

        case 1:
            output= 0;
        break;

        case 2:
            output= 1;
        break;

        case 4:
            output= 2;
        break;

        case 8:
            output= 3;
        break;

    default:
        output= 8;
    break;

    }

    return output;
}

int MainWindow::convert_Senso_Stick_POV_hat_to_Vjoy(int input)
{
    int output=8;
    switch(input)
    {
        case 0:
            output= 8;
        break;

        case 3:
            output= 0;
        break;

        case 2:
            output= 1;
        break;

        case 4:
            output= 2;
        break;

        case 1:
            output= 3;
        break;

    default:
        output= 8;
    break;

    }

    return output;
}

void MainWindow::decode_digital_2msg( unsigned int sixteen_bitval)
{


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

        trim_value_yaw= trim_value_yaw - trim_value_yaw_stepsize;
        if(trim_value_yaw<- trim_value_yaw_max)
        {
            trim_value_yaw= - trim_value_yaw_max;
        }
        ui->trim_yaw_value_label->setText(QString::number(trim_value_yaw));
    }
    if(flip_switch_state==2)    //right
    {
        ui->Flip_Switch_left_radioButton->setChecked(false);
        ui->Flip_Switch_middle_radioButton->setChecked(false);
        ui->Flip_Switch_right_radioButton->setChecked(true);

        trim_value_yaw= trim_value_yaw + trim_value_yaw_stepsize;
        if(trim_value_yaw>trim_value_yaw_max)
        {
            trim_value_yaw= trim_value_yaw_max;
        }
        ui->trim_yaw_value_label->setText(QString::number(trim_value_yaw));
    }

    if(front_button_state == false)    //not prsessed
    {
        ui->Front_Button_radioButton->setChecked(false);
    }
    else
    {
        ui->Front_Button_radioButton->setChecked(true);
    }

    vjoy_data.SensoStick_flip_switch_state= flip_switch_state;
    vjoy_data.SensoStick_front_button_state= front_button_state;
    vjoy_data.SensoStick_enable_button_state=enable_button_state;

}

void MainWindow::decode_digital_1msg(unsigned int sixteen_bitval)
{



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

        trim_value_roll= trim_value_roll - trim_value_roll_stepsize;
        if(trim_value_roll< -trim_value_roll_max)
        {
            trim_value_roll= -trim_value_roll_max;
        }
        ui->trim_roll_value_label->setText(QString::number(trim_value_roll));
    }
    if(mini_stick_state==2)    //right
    {
        ui->Mini_Stick_left_radioButton->setChecked(false);
        ui->Mini_Stick_right_radioButton->setChecked(true);
        ui->Mini_Stick_middle_radioButton->setChecked(false);
        changeColorMyButton();
        ui->Mini_Stick_up_radioButton->setChecked(false);
        ui->Mini_Stick_down_radioButton->setChecked(false);

        trim_value_roll= trim_value_roll + trim_value_roll_stepsize;
        if(trim_value_roll>trim_value_roll_max)
        {
            trim_value_roll= trim_value_roll_max;
        }
        ui->trim_roll_value_label->setText(QString::number(trim_value_roll));
    }

    if(mini_stick_state==3)    //up
    {
        ui->Mini_Stick_left_radioButton->setChecked(false);
        ui->Mini_Stick_right_radioButton->setChecked(false);
        ui->Mini_Stick_middle_radioButton->setChecked(false);
        changeColorMyButton();
        ui->Mini_Stick_up_radioButton->setChecked(true);
        ui->Mini_Stick_down_radioButton->setChecked(false);

        trim_value_pitch= trim_value_pitch + trim_value_pitch_stepsize;
        if(trim_value_pitch> trim_value_pitch_max)
        {
            trim_value_pitch= trim_value_roll_max;
        }
        ui->trim_pitch_value_label->setText(QString::number(trim_value_pitch));

    }
    if(mini_stick_state==4)    //down
    {
        ui->Mini_Stick_left_radioButton->setChecked(false);
        ui->Mini_Stick_right_radioButton->setChecked(false);
        ui->Mini_Stick_middle_radioButton->setChecked(false);
        changeColorMyButton();
        ui->Mini_Stick_up_radioButton->setChecked(false);
        ui->Mini_Stick_down_radioButton->setChecked(true);



        trim_value_pitch= trim_value_pitch - trim_value_pitch_stepsize;
        if(trim_value_pitch< -trim_value_pitch_max)
        {
            trim_value_pitch= -trim_value_roll_max;
        }
        ui->trim_pitch_value_label->setText(QString::number(trim_value_pitch));
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

    int Senso_Stick_converted_POV_hat= convert_Senso_Stick_POV_hat_to_Vjoy(mini_stick_state);
    vjoy_data.SensoStick_mini_stick_state= Senso_Stick_converted_POV_hat;
    vjoy_data.SensoStick_enable_button_state= enable_button_state;

}

//to change the color of the middle button
void MainWindow::changeColorMyButton(void)
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

int MainWindow::my_round(double value)
{
    int sign;
    if(value<0.0)
    {
        sign= -1;
        value= -value;
    }
    int posvalue= floor(value+0.5);
    if(sign==-1)
    {
        return -posvalue;
    }
    else
    {
        return posvalue;
    }
}

int MainWindow::scale_enc_1_to_int(double enc_pos)
{
    int sign;
    if(enc_pos<0.0)
    {
        sign= -1;
        enc_pos= -enc_pos;
    }

    //bei Skalierung 100 kommt max 1970 heraus
    //32767/1930*100= 1663.2995

    double scaleval_enc1= 1690;
    int posvalue= floor(scaleval_enc1*enc_pos+0.5);
    if(sign==-1)
    {
        if(posvalue>32768)
        {
            posvalue= 32768;
        }
        return -posvalue;
    }
    else
    {
        if(posvalue>32767)
        {
            posvalue= 32767;
        }
        return posvalue;
    }
}

int MainWindow::scale_enc_2_to_int(double enc_pos)
{
    int sign;
    if(enc_pos<0.0)
    {
        sign= -1;
        enc_pos= -enc_pos;
    }

    //bei Skalierung 100 kommt max 2014 heraus
    //32767/2014*100= 1626.9613

    double scaleval_enc1= 1680;
    int posvalue= floor(scaleval_enc1*enc_pos+0.5);
    if(sign==-1)
    {
        if(posvalue>32768)
        {
            posvalue= 32768;
        }
        return -posvalue;
    }
    else
    {
        if(posvalue>32767)
        {
            posvalue= 32767;
        }
        return posvalue;
    }

}

int MainWindow::scale_enc_3_to_int(double enc_pos)
{
    int sign;
    if(enc_pos<0.0)
    {
        sign= -1;
        enc_pos= -enc_pos;
    }

    //bei Skalierung 100 kommt max 1955 heraus
    //32767/1955*100= 1676.0614 (32767/30000 korrigiert)

    double scaleval_enc1= 1995;
    int posvalue= floor(scaleval_enc1*enc_pos+0.5);
    if(sign==-1)
    {
        if(posvalue>32768)
        {
            posvalue= 32768;
        }
        return -posvalue;
    }
    else
    {
        if(posvalue>32767)
        {
            posvalue= 32767;
        }
        return posvalue;
    }

}


void MainWindow::process_UDP_Senso_Stick_position_message()
{
    //qDebug()<<"Received something";

    while (UDP_receive_position_from_Senso_Stick_socket->hasPendingDatagrams())
    {
        QByteArray Received_ByteArray;
        Received_ByteArray.resize(UDP_receive_position_from_Senso_Stick_socket->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;
        UDP_receive_position_from_Senso_Stick_socket->readDatagram(Received_ByteArray.data(), Received_ByteArray.size(), &sender, &senderPort);


        QString Received_String= QString(Received_ByteArray.data());

        QStringList str_list= Received_String.split(",");
        //qDebug()<< str_list[0] << ", " << str_list[1];
        QString enc_nr_Str= str_list.first();
        QString enc_pos_Str= str_list.last();
        //qDebug()<< enc_pos_Str;

        unsigned int enc_nr= enc_nr_Str.toInt();
        double enc_pos= enc_pos_Str.toDouble();
        //qDebug() << enc_nr << ", " << enc_pos;

        if(enc_nr==1)
        {
            //ui->mot_enc_1_label->setText(enc_pos_Str);
            int scaled_pos= my_round(enc_pos*sliders_scaling_constant);
            ui->horizontalSlider->setValue(scaled_pos);

            mot_enc_1_int= scale_enc_1_to_int(enc_pos);
            vjoy_data.SensoStick_axis_left_right= mot_enc_1_int;
            ui->mot_enc_1_label->setText(QString::number(mot_enc_1_int));

            //and now add the trim value and saturate:
            vjoy_data.SensoStick_axis_left_right=vjoy_data.SensoStick_axis_left_right+trim_value_roll;

            //apply iha corrections
            vjoy_data.SensoStick_axis_left_right=vjoy_data.SensoStick_axis_left_right+iha_correction_roll;

            if(vjoy_data.SensoStick_axis_left_right> intmax)
            {
                vjoy_data.SensoStick_axis_left_right= intmax;
            }
            if(vjoy_data.SensoStick_axis_left_right< intmin)
            {
                vjoy_data.SensoStick_axis_left_right= intmin;
            }


        }
        if(enc_nr==2)
        {
            //ui->mot_enc_2_label->setText(enc_pos_Str);
            int scaled_pos= my_round(enc_pos*sliders_scaling_constant);
            ui->verticalSlider->setValue(scaled_pos);

            mot_enc_2_int= scale_enc_2_to_int(enc_pos);
            vjoy_data.SensoStick_axis_forward_backward= mot_enc_2_int;
            ui->mot_enc_2_label->setText(QString::number(mot_enc_2_int));

            vjoy_data.SensoStick_axis_forward_backward=vjoy_data.SensoStick_axis_forward_backward+trim_value_pitch;

            //apply iha corrections
            vjoy_data.SensoStick_axis_forward_backward=vjoy_data.SensoStick_axis_forward_backward+iha_correction_pitch;


                    if(vjoy_data.SensoStick_axis_forward_backward> intmax)
                    {
                        vjoy_data.SensoStick_axis_forward_backward= intmax;
                    }
                    if(vjoy_data.SensoStick_axis_forward_backward< intmin)
                    {
                        vjoy_data.SensoStick_axis_forward_backward= intmin;
                    }

        }
        if(enc_nr==3)
        {
            //ui->mot_enc_3_label->setText(enc_pos_Str);
            int scaled_pos= my_round(enc_pos*dial_scaling_constant);
            ui->dial->setValue(scaled_pos);

            mot_enc_3_int= scale_enc_3_to_int(enc_pos);
            vjoy_data.SensoStick_axis_twist= mot_enc_3_int;
            ui->mot_enc_3_label->setText(QString::number(mot_enc_3_int));

            vjoy_data.SensoStick_axis_twist=vjoy_data.SensoStick_axis_twist+trim_value_yaw;

            //apply iha corrections
            vjoy_data.SensoStick_axis_twist=vjoy_data.SensoStick_axis_twist+iha_correction_yaw;


            if(vjoy_data.SensoStick_axis_twist> intmax)
            {
                vjoy_data.SensoStick_axis_twist= intmax;
            }
            if(vjoy_data.SensoStick_axis_twist< intmin)
            {
                vjoy_data.SensoStick_axis_twist= intmin;
            }
        }
    }
}


struct UNIPI_container_iha_corrections
{
    double iha_roll;
    double iha_pitch;
    double iha_yaw;
} temp_rx_iha_corrections;

void MainWindow::iha_watchdog_timer_inc()
{
    if (iha_watchdog <10) { //timer period is 500 ms => 10 cycles == 5 seconds
        //ok
        iha_watchdog++;
    } else {
        //5 seconds passed => reset iha corrections
        iha_correction_roll = 0;
        iha_correction_pitch = 0;
        iha_correction_yaw = 0;
    }
}

void MainWindow::process_UDP_iha_corrections_message()
{
    //qDebug()<<"Received something";

    while (UDP_receive_iha_corrections_socket->hasPendingDatagrams())
    {
        //assume size is known !
        QHostAddress sender;
        quint16 senderPort;
        UDP_receive_iha_corrections_socket->readDatagram((char *) &temp_rx_iha_corrections, sizeof(temp_rx_iha_corrections), &sender, &senderPort);

        //store corrections
        iha_correction_roll = temp_rx_iha_corrections.iha_roll;
        iha_correction_pitch = temp_rx_iha_corrections.iha_pitch;
        iha_correction_yaw = temp_rx_iha_corrections.iha_yaw;

        //resest watchdog! if watchdog fires then iha correctiosn are reset to 0
        iha_watchdog = 0;
    }
}



void MainWindow::process_UDP_Senso_Stick_button_1_message()
{
    //qDebug()<<"Received something";

    while (UDP_receive_button_1_from_Senso_Stick_socket->hasPendingDatagrams())
    {
        QByteArray Received_ByteArray;
        Received_ByteArray.resize(UDP_receive_button_1_from_Senso_Stick_socket->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;
        UDP_receive_button_1_from_Senso_Stick_socket->readDatagram(Received_ByteArray.data(), Received_ByteArray.size(), &sender, &senderPort);


        QString Received_String= QString(Received_ByteArray.data());
        unsigned int button_1_code= (unsigned int)(Received_String.toInt());


        decode_digital_1msg( button_1_code);
        //qDebug() << Received_String;

    }


}

void MainWindow::process_UDP_Senso_Stick_button_2_message()
{
    //qDebug()<<"Received something";

    while (UDP_receive_button_2_from_Senso_Stick_socket->hasPendingDatagrams())
    {
        QByteArray Received_ByteArray;
        Received_ByteArray.resize(UDP_receive_button_2_from_Senso_Stick_socket->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;
        UDP_receive_button_2_from_Senso_Stick_socket->readDatagram(Received_ByteArray.data(), Received_ByteArray.size(), &sender, &senderPort);


        QString Received_String= QString(Received_ByteArray.data());
        unsigned int button_2_code= (unsigned int)(Received_String.toInt());


        decode_digital_2msg( button_2_code);
        //qDebug() << Received_String;
    }
}
