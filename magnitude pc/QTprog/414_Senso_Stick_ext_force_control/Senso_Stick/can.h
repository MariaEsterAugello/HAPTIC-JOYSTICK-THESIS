#ifndef CAN_H
#define CAN_H

#include <QMainWindow>
#include <QString>
#include <QTimer>
#include <QDebug>

#include <windows.h>    //DWORD in Pcan_usb.h
//#include "pcan_usb.h"
#include "pcan_pci.h"
class CAN
{
    public:

    CAN();

    //CAN stuff
    int CANMsgType;
    TPCANMsg* CAN_send_Message_Struct;
    TPCANMsg* CAN_receive_Message_Struct;
/*
    typedef struct {
        DWORD ID;        // 11/29 bit identifier
        BYTE  MSGTYPE;   // Bits from MSGTYPE_*
        BYTE  LEN;       // Data Length Code of the Msg (0..8)
        BYTE  DATA[8];   // Data 0 .. 7
    } TPCANMsg;
*/
    WORD CAN_Baudrate;

    void check_CAN_communication(DWORD*, DWORD*);
    unsigned int check_for_CAN_messages(TPCANMsg* );
    DWORD send_CAN_message(TPCANMsg*);
    DWORD close_CAN_connection(void);
    DWORD reset_CAN_connection(void);


};


#endif // CAN_H
