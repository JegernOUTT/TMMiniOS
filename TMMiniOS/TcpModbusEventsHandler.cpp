#include "Model.h"
//Временные затычки
//extern struct TCPREADDATA {int Socket;};
//extern void VcomSendSocket(int, const char*, int);

//int VcomUserBinaryCmd(TCPREADDATA *p)
//{
 /* VXCOMM.EXE 2.6.12(09/04/2001) or later will support this function.

        TCP PORT 10000, command 23 will call this function.
        user can get the following message:
        p->ReadUartChar : the buffer store the command data(include "23")
        p->Length : the command data length(include the two byte "23")
        p->Socket : the socket number that receive the command, that is when the user function want
                                return message to the client, just use the socket to send data.
                                use:  VcomSendSocket(p->Socket,pdata,datalength);
 */
//    VcomSendSocket(p->Socket,"User-defined command(23)",24);    // return 24 bytes.
//    return 1;   /* any value will be accept */
//}

void Modbus_Request_Event(char* CommandData,int* CommandLength)
{   
    /*
    Modbus_Request_Event is supported since version 1.6.8 [2007,03,13]. 
    
    char* CommandData: For Modbus/TCP, it includes 6 leading bytes. (needful) 
                       For Modbus/RTU, it includes 6 leading bytes. (needless) 
                       
    int* CommandLength: For Modbus/TCP, it includes 6 leading bytes. 
                        For Modbus/RTU, it includes 6 leading bytes.                      
    */
    /* Example code */
    //int i;    
    //printCom1("FC:%2d  StartAddress:%3d  IOCount:%4d\n\r",iModbusRequest_Fun, iModbusRequest_Addr,iModbusRequest_IOCount);
    //printCom1("Modbus Request\n\r    In==>");
    //for(i=0;i<*CommandLength;i++)
    //    printCom1("[%02X] ",CommandData[i]&0xFF);
}    

void Modbus_Response_Event(char* ResponseData,int* ResponseLength)
{
    /*
    char* ResponseData: For Modbus/TCP, it includes 6 leading bytes. 
                        For Modbus/RTU, it doesn't include 6 leading bytes
                       
    int* CommandLength: For Modbus/TCP, it includes 6 leading bytes. 
                        For Modbus/RTU, it doesn't include 6 leading bytes 
    */
    //If you want to change the content of the ResponseData, 
    //you have to do 2 steps for Modbus/TCP or Modbus/RTU.
    
    //Step1: Change content (Note:you must know the modbus protocol well)
    //ResponseData[6]=0x19;
    //ResponseData[7]=0x75;
    //ResponseData[8]=0x04;
    //ResponseData[9]=0x01;
    
    //Step2: Update data length
    //*ResponseLength=10;
    //int i;
    //printCom1("\n\r    Out==>");
    //for(i=0;i<*ResponseLength;i++)
    //    printCom1("[%02X] ",ResponseData[i]&0xFF);
    //printCom1("\n\r");  
}                               

