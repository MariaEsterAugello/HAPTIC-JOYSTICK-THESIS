/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.14.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDial>
#include <QtWidgets/QFrame>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QLabel *mot_enc_1_label;
    QLabel *mot_enc_2_label;
    QLabel *mot_enc_3_label;
    QSlider *horizontalSlider;
    QSlider *verticalSlider;
    QDial *dial;
    QLabel *SensoStick_label;
    QRadioButton *Front_Button_radioButton;
    QRadioButton *Mini_Stick_left_radioButton;
    QRadioButton *Mini_Stick_up_radioButton;
    QRadioButton *Flip_Switch_left_radioButton;
    QLabel *Front_label;
    QLabel *Rear_label;
    QRadioButton *Mini_Stick_right_radioButton;
    QRadioButton *Flip_Switch_right_radioButton;
    QRadioButton *Mini_Stick_down_radioButton;
    QRadioButton *Enable_Button_radioButton;
    QRadioButton *Mini_Stick_middle_radioButton;
    QRadioButton *Flip_Switch_middle_radioButton;
    QLabel *USB_Joystick_label;
    QSlider *USB_Centerstick_verticalSlider;
    QSlider *USB_Centerstick_horizontalSlider;
    QSlider *USB_Pedals_horizontalSlider;
    QFrame *line;
    QFrame *line_2;
    QFrame *line_3;
    QRadioButton *USB_Centerstick_left_middle_radioButton;
    QRadioButton *USB_Centerstick_left_down_radioButton;
    QRadioButton *USB_Centerstick_left_top_radioButton;
    QRadioButton *USB_Centerstick_Front_radioButton;
    QLabel *axes_and_button_label;
    QRadioButton *USB_Collective_left_radioButton;
    QRadioButton *USB_Collective_right_radioButton;
    QLabel *centerstick_horizontal_value_label;
    QLabel *USB_pedals_value_label;
    QLabel *centerstick_vertical_value_label;
    QLabel *collective_value_label;
    QSlider *USB_Collective_verticalSlider;
    QDial *USB_Collective_power_dial;
    QLabel *collective_power_value_label;
    QRadioButton *Hat2_up_radioButton;
    QRadioButton *Hat2_right_radioButton;
    QRadioButton *Hat2_down_radioButton;
    QRadioButton *Hat2_left_radioButton;
    QRadioButton *Hat2_middle_radioButton;
    QFrame *line_4;
    QFrame *line_5;
    QFrame *line_6;
    QFrame *line_7;
    QFrame *line_8;
    QFrame *line_9;
    QFrame *line_10;
    QFrame *line_11;
    QFrame *line_12;
    QFrame *line_13;
    QFrame *line_14;
    QFrame *line_15;
    QRadioButton *collective_left_switch_up_radioButton;
    QRadioButton *collective_left_switch_middle_radioButton;
    QRadioButton *collective_left_switch_down_radioButton;
    QFrame *line_16;
    QRadioButton *activate_Senso_Stick_radioButton;
    QRadioButton *activate_Puma_Center_Stick_radioButton;
    QFrame *line_17;
    QFrame *line_18;
    QFrame *line_19;
    QFrame *line_20;
    QFrame *line_21;
    QFrame *line_22;
    QFrame *line_23;
    QLabel *Device_label;
    QLabel *Sensostick_yaw_label;
    QCheckBox *Senso_Stick_yaw_checkBox;
    QLabel *trim_yaw_label;
    QLabel *trim_yaw_value_label;
    QLabel *trim_roll_label;
    QLabel *trim_roll_value_label;
    QLabel *trim_pitch_value_label;
    QLabel *trim_pitch_label;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1170, 725);
        QFont font;
        font.setPointSize(12);
        font.setBold(false);
        font.setWeight(50);
        MainWindow->setFont(font);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        mot_enc_1_label = new QLabel(centralWidget);
        mot_enc_1_label->setObjectName(QString::fromUtf8("mot_enc_1_label"));
        mot_enc_1_label->setGeometry(QRect(110, 170, 61, 16));
        mot_enc_2_label = new QLabel(centralWidget);
        mot_enc_2_label->setObjectName(QString::fromUtf8("mot_enc_2_label"));
        mot_enc_2_label->setGeometry(QRect(280, 170, 61, 16));
        mot_enc_3_label = new QLabel(centralWidget);
        mot_enc_3_label->setObjectName(QString::fromUtf8("mot_enc_3_label"));
        mot_enc_3_label->setGeometry(QRect(470, 170, 61, 16));
        horizontalSlider = new QSlider(centralWidget);
        horizontalSlider->setObjectName(QString::fromUtf8("horizontalSlider"));
        horizontalSlider->setGeometry(QRect(50, 270, 160, 19));
        horizontalSlider->setOrientation(Qt::Horizontal);
        verticalSlider = new QSlider(centralWidget);
        verticalSlider->setObjectName(QString::fromUtf8("verticalSlider"));
        verticalSlider->setGeometry(QRect(300, 220, 19, 160));
        verticalSlider->setOrientation(Qt::Vertical);
        dial = new QDial(centralWidget);
        dial->setObjectName(QString::fromUtf8("dial"));
        dial->setGeometry(QRect(470, 240, 50, 64));
        SensoStick_label = new QLabel(centralWidget);
        SensoStick_label->setObjectName(QString::fromUtf8("SensoStick_label"));
        SensoStick_label->setGeometry(QRect(230, 30, 281, 31));
        Front_Button_radioButton = new QRadioButton(centralWidget);
        Front_Button_radioButton->setObjectName(QString::fromUtf8("Front_Button_radioButton"));
        Front_Button_radioButton->setGeometry(QRect(190, 590, 16, 17));
        Mini_Stick_left_radioButton = new QRadioButton(centralWidget);
        Mini_Stick_left_radioButton->setObjectName(QString::fromUtf8("Mini_Stick_left_radioButton"));
        Mini_Stick_left_radioButton->setGeometry(QRect(220, 520, 16, 17));
        Mini_Stick_up_radioButton = new QRadioButton(centralWidget);
        Mini_Stick_up_radioButton->setObjectName(QString::fromUtf8("Mini_Stick_up_radioButton"));
        Mini_Stick_up_radioButton->setGeometry(QRect(240, 500, 16, 17));
        Flip_Switch_left_radioButton = new QRadioButton(centralWidget);
        Flip_Switch_left_radioButton->setObjectName(QString::fromUtf8("Flip_Switch_left_radioButton"));
        Flip_Switch_left_radioButton->setGeometry(QRect(120, 520, 16, 17));
        Front_label = new QLabel(centralWidget);
        Front_label->setObjectName(QString::fromUtf8("Front_label"));
        Front_label->setGeometry(QRect(180, 450, 47, 13));
        Rear_label = new QLabel(centralWidget);
        Rear_label->setObjectName(QString::fromUtf8("Rear_label"));
        Rear_label->setGeometry(QRect(390, 450, 47, 13));
        Mini_Stick_right_radioButton = new QRadioButton(centralWidget);
        Mini_Stick_right_radioButton->setObjectName(QString::fromUtf8("Mini_Stick_right_radioButton"));
        Mini_Stick_right_radioButton->setGeometry(QRect(260, 520, 16, 17));
        Flip_Switch_right_radioButton = new QRadioButton(centralWidget);
        Flip_Switch_right_radioButton->setObjectName(QString::fromUtf8("Flip_Switch_right_radioButton"));
        Flip_Switch_right_radioButton->setGeometry(QRect(160, 520, 16, 17));
        Mini_Stick_down_radioButton = new QRadioButton(centralWidget);
        Mini_Stick_down_radioButton->setObjectName(QString::fromUtf8("Mini_Stick_down_radioButton"));
        Mini_Stick_down_radioButton->setGeometry(QRect(240, 540, 16, 17));
        Enable_Button_radioButton = new QRadioButton(centralWidget);
        Enable_Button_radioButton->setObjectName(QString::fromUtf8("Enable_Button_radioButton"));
        Enable_Button_radioButton->setGeometry(QRect(400, 530, 16, 17));
        Mini_Stick_middle_radioButton = new QRadioButton(centralWidget);
        Mini_Stick_middle_radioButton->setObjectName(QString::fromUtf8("Mini_Stick_middle_radioButton"));
        Mini_Stick_middle_radioButton->setGeometry(QRect(240, 520, 16, 17));
        Flip_Switch_middle_radioButton = new QRadioButton(centralWidget);
        Flip_Switch_middle_radioButton->setObjectName(QString::fromUtf8("Flip_Switch_middle_radioButton"));
        Flip_Switch_middle_radioButton->setGeometry(QRect(140, 520, 16, 17));
        USB_Joystick_label = new QLabel(centralWidget);
        USB_Joystick_label->setObjectName(QString::fromUtf8("USB_Joystick_label"));
        USB_Joystick_label->setGeometry(QRect(760, 25, 211, 31));
        USB_Centerstick_verticalSlider = new QSlider(centralWidget);
        USB_Centerstick_verticalSlider->setObjectName(QString::fromUtf8("USB_Centerstick_verticalSlider"));
        USB_Centerstick_verticalSlider->setGeometry(QRect(890, 350, 19, 160));
        USB_Centerstick_verticalSlider->setOrientation(Qt::Vertical);
        USB_Centerstick_horizontalSlider = new QSlider(centralWidget);
        USB_Centerstick_horizontalSlider->setObjectName(QString::fromUtf8("USB_Centerstick_horizontalSlider"));
        USB_Centerstick_horizontalSlider->setGeometry(QRect(820, 540, 160, 19));
        USB_Centerstick_horizontalSlider->setOrientation(Qt::Horizontal);
        USB_Pedals_horizontalSlider = new QSlider(centralWidget);
        USB_Pedals_horizontalSlider->setObjectName(QString::fromUtf8("USB_Pedals_horizontalSlider"));
        USB_Pedals_horizontalSlider->setGeometry(QRect(800, 170, 160, 19));
        USB_Pedals_horizontalSlider->setOrientation(Qt::Horizontal);
        line = new QFrame(centralWidget);
        line->setObjectName(QString::fromUtf8("line"));
        line->setGeometry(QRect(560, 110, 21, 541));
        line->setFrameShape(QFrame::VLine);
        line->setFrameShadow(QFrame::Sunken);
        line_2 = new QFrame(centralWidget);
        line_2->setObjectName(QString::fromUtf8("line_2"));
        line_2->setGeometry(QRect(20, 90, 541, 20));
        line_2->setFrameShape(QFrame::HLine);
        line_2->setFrameShadow(QFrame::Sunken);
        line_3 = new QFrame(centralWidget);
        line_3->setObjectName(QString::fromUtf8("line_3"));
        line_3->setGeometry(QRect(580, 90, 541, 20));
        line_3->setFrameShape(QFrame::HLine);
        line_3->setFrameShadow(QFrame::Sunken);
        USB_Centerstick_left_middle_radioButton = new QRadioButton(centralWidget);
        USB_Centerstick_left_middle_radioButton->setObjectName(QString::fromUtf8("USB_Centerstick_left_middle_radioButton"));
        USB_Centerstick_left_middle_radioButton->setGeometry(QRect(803, 350, 82, 17));
        USB_Centerstick_left_down_radioButton = new QRadioButton(centralWidget);
        USB_Centerstick_left_down_radioButton->setObjectName(QString::fromUtf8("USB_Centerstick_left_down_radioButton"));
        USB_Centerstick_left_down_radioButton->setGeometry(QRect(803, 380, 82, 17));
        USB_Centerstick_left_top_radioButton = new QRadioButton(centralWidget);
        USB_Centerstick_left_top_radioButton->setObjectName(QString::fromUtf8("USB_Centerstick_left_top_radioButton"));
        USB_Centerstick_left_top_radioButton->setGeometry(QRect(803, 320, 82, 17));
        USB_Centerstick_Front_radioButton = new QRadioButton(centralWidget);
        USB_Centerstick_Front_radioButton->setObjectName(QString::fromUtf8("USB_Centerstick_Front_radioButton"));
        USB_Centerstick_Front_radioButton->setGeometry(QRect(833, 300, 82, 17));
        axes_and_button_label = new QLabel(centralWidget);
        axes_and_button_label->setObjectName(QString::fromUtf8("axes_and_button_label"));
        axes_and_button_label->setGeometry(QRect(690, 70, 381, 21));
        USB_Collective_left_radioButton = new QRadioButton(centralWidget);
        USB_Collective_left_radioButton->setObjectName(QString::fromUtf8("USB_Collective_left_radioButton"));
        USB_Collective_left_radioButton->setGeometry(QRect(610, 310, 82, 17));
        USB_Collective_right_radioButton = new QRadioButton(centralWidget);
        USB_Collective_right_radioButton->setObjectName(QString::fromUtf8("USB_Collective_right_radioButton"));
        USB_Collective_right_radioButton->setGeometry(QRect(690, 310, 82, 17));
        centerstick_horizontal_value_label = new QLabel(centralWidget);
        centerstick_horizontal_value_label->setObjectName(QString::fromUtf8("centerstick_horizontal_value_label"));
        centerstick_horizontal_value_label->setGeometry(QRect(870, 560, 61, 16));
        USB_pedals_value_label = new QLabel(centralWidget);
        USB_pedals_value_label->setObjectName(QString::fromUtf8("USB_pedals_value_label"));
        USB_pedals_value_label->setGeometry(QRect(850, 190, 61, 16));
        centerstick_vertical_value_label = new QLabel(centralWidget);
        centerstick_vertical_value_label->setObjectName(QString::fromUtf8("centerstick_vertical_value_label"));
        centerstick_vertical_value_label->setGeometry(QRect(910, 440, 61, 16));
        collective_value_label = new QLabel(centralWidget);
        collective_value_label->setObjectName(QString::fromUtf8("collective_value_label"));
        collective_value_label->setGeometry(QRect(670, 510, 61, 16));
        USB_Collective_verticalSlider = new QSlider(centralWidget);
        USB_Collective_verticalSlider->setObjectName(QString::fromUtf8("USB_Collective_verticalSlider"));
        USB_Collective_verticalSlider->setGeometry(QRect(650, 440, 19, 160));
        USB_Collective_verticalSlider->setOrientation(Qt::Vertical);
        USB_Collective_power_dial = new QDial(centralWidget);
        USB_Collective_power_dial->setObjectName(QString::fromUtf8("USB_Collective_power_dial"));
        USB_Collective_power_dial->setGeometry(QRect(630, 370, 50, 64));
        collective_power_value_label = new QLabel(centralWidget);
        collective_power_value_label->setObjectName(QString::fromUtf8("collective_power_value_label"));
        collective_power_value_label->setGeometry(QRect(690, 400, 47, 14));
        Hat2_up_radioButton = new QRadioButton(centralWidget);
        Hat2_up_radioButton->setObjectName(QString::fromUtf8("Hat2_up_radioButton"));
        Hat2_up_radioButton->setGeometry(QRect(1010, 290, 83, 18));
        Hat2_right_radioButton = new QRadioButton(centralWidget);
        Hat2_right_radioButton->setObjectName(QString::fromUtf8("Hat2_right_radioButton"));
        Hat2_right_radioButton->setGeometry(QRect(1060, 340, 83, 18));
        Hat2_down_radioButton = new QRadioButton(centralWidget);
        Hat2_down_radioButton->setObjectName(QString::fromUtf8("Hat2_down_radioButton"));
        Hat2_down_radioButton->setGeometry(QRect(1010, 390, 83, 18));
        Hat2_left_radioButton = new QRadioButton(centralWidget);
        Hat2_left_radioButton->setObjectName(QString::fromUtf8("Hat2_left_radioButton"));
        Hat2_left_radioButton->setGeometry(QRect(960, 340, 83, 18));
        Hat2_middle_radioButton = new QRadioButton(centralWidget);
        Hat2_middle_radioButton->setObjectName(QString::fromUtf8("Hat2_middle_radioButton"));
        Hat2_middle_radioButton->setGeometry(QRect(1010, 340, 83, 18));
        line_4 = new QFrame(centralWidget);
        line_4->setObjectName(QString::fromUtf8("line_4"));
        line_4->setGeometry(QRect(790, 280, 61, 20));
        line_4->setFrameShape(QFrame::HLine);
        line_4->setFrameShadow(QFrame::Sunken);
        line_5 = new QFrame(centralWidget);
        line_5->setObjectName(QString::fromUtf8("line_5"));
        line_5->setGeometry(QRect(790, 400, 61, 20));
        line_5->setFrameShape(QFrame::HLine);
        line_5->setFrameShadow(QFrame::Sunken);
        line_6 = new QFrame(centralWidget);
        line_6->setObjectName(QString::fromUtf8("line_6"));
        line_6->setGeometry(QRect(940, 260, 141, 20));
        line_6->setFrameShape(QFrame::HLine);
        line_6->setFrameShadow(QFrame::Sunken);
        line_7 = new QFrame(centralWidget);
        line_7->setObjectName(QString::fromUtf8("line_7"));
        line_7->setGeometry(QRect(940, 410, 141, 20));
        line_7->setFrameShape(QFrame::HLine);
        line_7->setFrameShadow(QFrame::Sunken);
        line_8 = new QFrame(centralWidget);
        line_8->setObjectName(QString::fromUtf8("line_8"));
        line_8->setGeometry(QRect(770, 300, 20, 101));
        line_8->setFrameShape(QFrame::VLine);
        line_8->setFrameShadow(QFrame::Sunken);
        line_9 = new QFrame(centralWidget);
        line_9->setObjectName(QString::fromUtf8("line_9"));
        line_9->setGeometry(QRect(850, 300, 20, 101));
        line_9->setFrameShape(QFrame::VLine);
        line_9->setFrameShadow(QFrame::Sunken);
        line_10 = new QFrame(centralWidget);
        line_10->setObjectName(QString::fromUtf8("line_10"));
        line_10->setGeometry(QRect(930, 280, 20, 131));
        line_10->setFrameShape(QFrame::VLine);
        line_10->setFrameShadow(QFrame::Sunken);
        line_11 = new QFrame(centralWidget);
        line_11->setObjectName(QString::fromUtf8("line_11"));
        line_11->setGeometry(QRect(1080, 280, 20, 131));
        line_11->setFrameShape(QFrame::VLine);
        line_11->setFrameShadow(QFrame::Sunken);
        line_12 = new QFrame(centralWidget);
        line_12->setObjectName(QString::fromUtf8("line_12"));
        line_12->setGeometry(QRect(600, 330, 111, 20));
        line_12->setFrameShape(QFrame::HLine);
        line_12->setFrameShadow(QFrame::Sunken);
        line_13 = new QFrame(centralWidget);
        line_13->setObjectName(QString::fromUtf8("line_13"));
        line_13->setGeometry(QRect(600, 170, 111, 20));
        line_13->setFrameShape(QFrame::HLine);
        line_13->setFrameShadow(QFrame::Sunken);
        line_14 = new QFrame(centralWidget);
        line_14->setObjectName(QString::fromUtf8("line_14"));
        line_14->setGeometry(QRect(580, 190, 20, 141));
        line_14->setFrameShape(QFrame::VLine);
        line_14->setFrameShadow(QFrame::Sunken);
        line_15 = new QFrame(centralWidget);
        line_15->setObjectName(QString::fromUtf8("line_15"));
        line_15->setGeometry(QRect(710, 190, 20, 141));
        line_15->setFrameShape(QFrame::VLine);
        line_15->setFrameShadow(QFrame::Sunken);
        collective_left_switch_up_radioButton = new QRadioButton(centralWidget);
        collective_left_switch_up_radioButton->setObjectName(QString::fromUtf8("collective_left_switch_up_radioButton"));
        collective_left_switch_up_radioButton->setGeometry(QRect(610, 200, 83, 18));
        collective_left_switch_middle_radioButton = new QRadioButton(centralWidget);
        collective_left_switch_middle_radioButton->setObjectName(QString::fromUtf8("collective_left_switch_middle_radioButton"));
        collective_left_switch_middle_radioButton->setGeometry(QRect(610, 230, 83, 18));
        collective_left_switch_down_radioButton = new QRadioButton(centralWidget);
        collective_left_switch_down_radioButton->setObjectName(QString::fromUtf8("collective_left_switch_down_radioButton"));
        collective_left_switch_down_radioButton->setGeometry(QRect(610, 260, 83, 18));
        line_16 = new QFrame(centralWidget);
        line_16->setObjectName(QString::fromUtf8("line_16"));
        line_16->setGeometry(QRect(600, 290, 111, 20));
        line_16->setFrameShape(QFrame::HLine);
        line_16->setFrameShadow(QFrame::Sunken);
        activate_Senso_Stick_radioButton = new QRadioButton(centralWidget);
        activate_Senso_Stick_radioButton->setObjectName(QString::fromUtf8("activate_Senso_Stick_radioButton"));
        activate_Senso_Stick_radioButton->setGeometry(QRect(540, 30, 83, 18));
        activate_Puma_Center_Stick_radioButton = new QRadioButton(centralWidget);
        activate_Puma_Center_Stick_radioButton->setObjectName(QString::fromUtf8("activate_Puma_Center_Stick_radioButton"));
        activate_Puma_Center_Stick_radioButton->setGeometry(QRect(590, 30, 83, 18));
        line_17 = new QFrame(centralWidget);
        line_17->setObjectName(QString::fromUtf8("line_17"));
        line_17->setGeometry(QRect(530, 60, 31, 20));
        line_17->setFrameShape(QFrame::HLine);
        line_17->setFrameShadow(QFrame::Sunken);
        line_18 = new QFrame(centralWidget);
        line_18->setObjectName(QString::fromUtf8("line_18"));
        line_18->setGeometry(QRect(580, 60, 31, 16));
        line_18->setFrameShape(QFrame::HLine);
        line_18->setFrameShadow(QFrame::Sunken);
        line_19 = new QFrame(centralWidget);
        line_19->setObjectName(QString::fromUtf8("line_19"));
        line_19->setGeometry(QRect(580, 0, 31, 16));
        line_19->setFrameShape(QFrame::HLine);
        line_19->setFrameShadow(QFrame::Sunken);
        line_20 = new QFrame(centralWidget);
        line_20->setObjectName(QString::fromUtf8("line_20"));
        line_20->setGeometry(QRect(530, 0, 31, 20));
        line_20->setFrameShape(QFrame::HLine);
        line_20->setFrameShadow(QFrame::Sunken);
        line_21 = new QFrame(centralWidget);
        line_21->setObjectName(QString::fromUtf8("line_21"));
        line_21->setGeometry(QRect(560, 20, 20, 41));
        line_21->setFrameShape(QFrame::VLine);
        line_21->setFrameShadow(QFrame::Sunken);
        line_22 = new QFrame(centralWidget);
        line_22->setObjectName(QString::fromUtf8("line_22"));
        line_22->setGeometry(QRect(510, 20, 20, 41));
        line_22->setFrameShape(QFrame::VLine);
        line_22->setFrameShadow(QFrame::Sunken);
        line_23 = new QFrame(centralWidget);
        line_23->setObjectName(QString::fromUtf8("line_23"));
        line_23->setGeometry(QRect(610, 20, 20, 41));
        line_23->setFrameShape(QFrame::VLine);
        line_23->setFrameShadow(QFrame::Sunken);
        Device_label = new QLabel(centralWidget);
        Device_label->setObjectName(QString::fromUtf8("Device_label"));
        Device_label->setGeometry(QRect(520, 70, 141, 20));
        Sensostick_yaw_label = new QLabel(centralWidget);
        Sensostick_yaw_label->setObjectName(QString::fromUtf8("Sensostick_yaw_label"));
        Sensostick_yaw_label->setGeometry(QRect(400, 370, 161, 21));
        Senso_Stick_yaw_checkBox = new QCheckBox(centralWidget);
        Senso_Stick_yaw_checkBox->setObjectName(QString::fromUtf8("Senso_Stick_yaw_checkBox"));
        Senso_Stick_yaw_checkBox->setGeometry(QRect(400, 340, 102, 24));
        trim_yaw_label = new QLabel(centralWidget);
        trim_yaw_label->setObjectName(QString::fromUtf8("trim_yaw_label"));
        trim_yaw_label->setGeometry(QRect(410, 400, 69, 20));
        trim_yaw_value_label = new QLabel(centralWidget);
        trim_yaw_value_label->setObjectName(QString::fromUtf8("trim_yaw_value_label"));
        trim_yaw_value_label->setGeometry(QRect(490, 400, 69, 20));
        trim_roll_label = new QLabel(centralWidget);
        trim_roll_label->setObjectName(QString::fromUtf8("trim_roll_label"));
        trim_roll_label->setGeometry(QRect(20, 400, 69, 20));
        trim_roll_value_label = new QLabel(centralWidget);
        trim_roll_value_label->setObjectName(QString::fromUtf8("trim_roll_value_label"));
        trim_roll_value_label->setGeometry(QRect(100, 400, 69, 20));
        trim_pitch_value_label = new QLabel(centralWidget);
        trim_pitch_value_label->setObjectName(QString::fromUtf8("trim_pitch_value_label"));
        trim_pitch_value_label->setGeometry(QRect(300, 400, 69, 20));
        trim_pitch_label = new QLabel(centralWidget);
        trim_pitch_label->setObjectName(QString::fromUtf8("trim_pitch_label"));
        trim_pitch_label->setGeometry(QRect(220, 400, 69, 20));
        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1170, 22));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MainWindow->setStatusBar(statusBar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        mot_enc_1_label->setText(QCoreApplication::translate("MainWindow", "TextLabel", nullptr));
        mot_enc_2_label->setText(QCoreApplication::translate("MainWindow", "TextLabel", nullptr));
        mot_enc_3_label->setText(QCoreApplication::translate("MainWindow", "TextLabel", nullptr));
        SensoStick_label->setText(QCoreApplication::translate("MainWindow", "TextLabel", nullptr));
        Front_Button_radioButton->setText(QCoreApplication::translate("MainWindow", "RadioButton", nullptr));
        Mini_Stick_left_radioButton->setText(QCoreApplication::translate("MainWindow", "RadioButton", nullptr));
        Mini_Stick_up_radioButton->setText(QCoreApplication::translate("MainWindow", "RadioButton", nullptr));
        Flip_Switch_left_radioButton->setText(QCoreApplication::translate("MainWindow", "RadioButton", nullptr));
        Front_label->setText(QCoreApplication::translate("MainWindow", "TextLabel", nullptr));
        Rear_label->setText(QCoreApplication::translate("MainWindow", "TextLabel", nullptr));
        Mini_Stick_right_radioButton->setText(QCoreApplication::translate("MainWindow", "RadioButton", nullptr));
        Flip_Switch_right_radioButton->setText(QCoreApplication::translate("MainWindow", "RadioButton", nullptr));
        Mini_Stick_down_radioButton->setText(QCoreApplication::translate("MainWindow", "RadioButton", nullptr));
        Enable_Button_radioButton->setText(QCoreApplication::translate("MainWindow", "RadioButton", nullptr));
        Mini_Stick_middle_radioButton->setText(QCoreApplication::translate("MainWindow", "RadioButton", nullptr));
        Flip_Switch_middle_radioButton->setText(QCoreApplication::translate("MainWindow", "RadioButton", nullptr));
        USB_Joystick_label->setText(QCoreApplication::translate("MainWindow", "TextLabel", nullptr));
        USB_Centerstick_left_middle_radioButton->setText(QCoreApplication::translate("MainWindow", "RadioButton", nullptr));
        USB_Centerstick_left_down_radioButton->setText(QCoreApplication::translate("MainWindow", "RadioButton", nullptr));
        USB_Centerstick_left_top_radioButton->setText(QCoreApplication::translate("MainWindow", "RadioButton", nullptr));
        USB_Centerstick_Front_radioButton->setText(QCoreApplication::translate("MainWindow", "RadioButton", nullptr));
        axes_and_button_label->setText(QCoreApplication::translate("MainWindow", "TextLabel", nullptr));
        USB_Collective_left_radioButton->setText(QCoreApplication::translate("MainWindow", "RadioButton", nullptr));
        USB_Collective_right_radioButton->setText(QCoreApplication::translate("MainWindow", "RadioButton", nullptr));
        centerstick_horizontal_value_label->setText(QCoreApplication::translate("MainWindow", "TextLabel", nullptr));
        USB_pedals_value_label->setText(QCoreApplication::translate("MainWindow", "TextLabel", nullptr));
        centerstick_vertical_value_label->setText(QCoreApplication::translate("MainWindow", "TextLabel", nullptr));
        collective_value_label->setText(QCoreApplication::translate("MainWindow", "TextLabel", nullptr));
        collective_power_value_label->setText(QCoreApplication::translate("MainWindow", "TextLabel", nullptr));
        Hat2_up_radioButton->setText(QCoreApplication::translate("MainWindow", "RadioButton", nullptr));
        Hat2_right_radioButton->setText(QCoreApplication::translate("MainWindow", "RadioButton", nullptr));
        Hat2_down_radioButton->setText(QCoreApplication::translate("MainWindow", "RadioButton", nullptr));
        Hat2_left_radioButton->setText(QCoreApplication::translate("MainWindow", "RadioButton", nullptr));
        Hat2_middle_radioButton->setText(QCoreApplication::translate("MainWindow", "RadioButton", nullptr));
        collective_left_switch_up_radioButton->setText(QCoreApplication::translate("MainWindow", "RadioButton", nullptr));
        collective_left_switch_middle_radioButton->setText(QCoreApplication::translate("MainWindow", "RadioButton", nullptr));
        collective_left_switch_down_radioButton->setText(QCoreApplication::translate("MainWindow", "RadioButton", nullptr));
        activate_Senso_Stick_radioButton->setText(QCoreApplication::translate("MainWindow", "RadioButton", nullptr));
        activate_Puma_Center_Stick_radioButton->setText(QCoreApplication::translate("MainWindow", "RadioButton", nullptr));
        Device_label->setText(QCoreApplication::translate("MainWindow", "TextLabel", nullptr));
        Sensostick_yaw_label->setText(QCoreApplication::translate("MainWindow", "TextLabel", nullptr));
        Senso_Stick_yaw_checkBox->setText(QCoreApplication::translate("MainWindow", "CheckBox", nullptr));
        trim_yaw_label->setText(QCoreApplication::translate("MainWindow", "TextLabel", nullptr));
        trim_yaw_value_label->setText(QCoreApplication::translate("MainWindow", "TextLabel", nullptr));
        trim_roll_label->setText(QCoreApplication::translate("MainWindow", "TextLabel", nullptr));
        trim_roll_value_label->setText(QCoreApplication::translate("MainWindow", "TextLabel", nullptr));
        trim_pitch_value_label->setText(QCoreApplication::translate("MainWindow", "TextLabel", nullptr));
        trim_pitch_label->setText(QCoreApplication::translate("MainWindow", "TextLabel", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
