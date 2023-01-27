
#include "can.h"



CAN::CAN()
{
    CAN_send_Message_Struct= new(TPCANMsg);
    CAN_receive_Message_Struct= new(TPCANMsg);
}

void CAN::check_CAN_communication(DWORD* CAN_init_reply, DWORD* CAN_status_reply)
{

    CAN_send_Message_Struct->ID= 0x20;
    CAN_send_Message_Struct->MSGTYPE= MSGTYPE_STANDARD;

    CAN_Baudrate=CAN_BAUD_1M;

    *CAN_init_reply= CAN_Init(CAN_Baudrate, MSGTYPE_STANDARD);
    *CAN_status_reply= CAN_Status();

}


DWORD CAN::send_CAN_message(TPCANMsg* message_struct)
{
    return CAN_Write(message_struct);
}


DWORD CAN::close_CAN_connection(void)
{
    return CAN_Close();
}

DWORD CAN::reset_CAN_connection(void)
{
    return CAN_ResetClient();
}

unsigned int CAN::check_for_CAN_messages(TPCANMsg* received_CAN_message_Struct)
{
    //qDebug()<<"check...";

    //Die message queu kann ziemlich lang sein, also zunächst queu leeren
    //#define CAN_ERR_QRCVEMPTY     0x00020  // Receive queue is empty
    int Stop_CAN_reading=0;
    unsigned int read_counter=0;
    BYTE CAN_read_response;
    //BYTE store_last_valid_CAN_Pneumatic_Cylinder_read_data;

    while(Stop_CAN_reading==0)
    {
        CAN_read_response= CAN_Read( CAN_receive_Message_Struct);

        if(CAN_read_response==CAN_ERR_OK)
        {
            //store message;
            read_counter+=1;
            //qDebug()<<CAN_receive_Message_Struct->ID<<"  "<<CAN_receive_Message_Struct->DATA[0];
            *received_CAN_message_Struct= *CAN_receive_Message_Struct;
            //qDebug()<<received_CAN_message_Struct->ID<<"  "<<received_CAN_message_Struct->DATA[0];
        }
        if(CAN_read_response==CAN_ERR_QRCVEMPTY)
        {
            Stop_CAN_reading=1;
        }

    }
    if(read_counter>0)  //need at least one valid message, otherwise we loose 200ms
    {
        return 1;
    }
    else
    {
        return 0;
    }
}





