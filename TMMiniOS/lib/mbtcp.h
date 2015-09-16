/******************************************************/
/*  Modbus/TCP to Modbus/RTU protocol head file       */
/*  For 7188E (with Xserver structure)                */
/*                                                    */
/*  Note: head file version is different to           */
/*        lib file version.                           */
/*                                                    */
/******************************************************/
/*
Version 1.6.8 [8,Sep,2006] by Kevin
    1. Add one constant define
        _Console 5
---------------------------------------------------------------------
Version 1.6.7 [16,Aug,2006] by Kevin
    1. Add global variables
            lModbusRequest_TimeTicks
---------------------------------------------------------------------
Version 1.6.6 [04,Jul,2006] by Kevin
    1. All controller use same head file
        For Modbus/RTU ===> MBRTU.h
        For Modbus/TCP ===> MBTCP.h
        
    2. Remove variable MAX_REGISTER_COUNT.
       From version 1.6.4, the registers are declared by user's self.
       So, MAX_REGISTER_COUNT is useless.
       
    3. Improve ModbusRTU_Master2Slave and ModbusASCII_Master2Slave
        (a) add non-blocked communication mode.
            there is one iWait parameter can decide to use 
            blocked (original mode) or non-blocked (new mode).
        (b) add functions
                ModbusRTU_Master_Send
                ModbusRTU_Master_Receive
                ModbusASCII_Master_Send
                ModbusASCII_Master_Receive
                
        Please refer to function comments for details.        
---------------------------------------------------------------------        
Version 1.6.4 [17,Feb,2006] by Kevin
    1. Add command buffer for Modbus/RTU Gateway 
        Each COM port can store 4 commands in the buffer. While several 
        Modbus masters use same COM port gateway, the buffer can store
        the commands and then bypass them to Modbus/RTU device that
        connected to the COM port.
        The function can avoid data collision.
        
    For programming
        1. support XS structor.
        2. add RefreshWDT to ModbusRTU_Master2Slave.
            this can avoid reset by WDT when the timeout parameter is
            more than 0.8 second.
        3. add Debug information for Modbus/AsCII Master to Slave.
        4. change InitModbus
            the function now needs four extra parameters.
        5. add Modbus/TCP master to slave function     
---------------------------------------------------------------------
Version 1.6.3 [29,Sep,2005] by Kevin
    Change ModbusMaster2Slave:
        Rename to ModbusRTU_Master2Slave,
        Add a lTimeout parameter.
        
    Add ModbusASCII_Master2Slave 
---------------------------------------------------------------------
Version 1.6.2 [23,Aug,2005] by Kevin
    Add functions for Modbus/TCP master 
        (1) ModbusTCP_Init
        (2) ModbusTCP_Master2Slave
        (3) ModbusTCP_Close
---------------------------------------------------------------------
Version 1.6.0 [08,Apr,2005] by Kevin
    Remove 
        #define MODBUS_ACTION_IDLE                          0
        #define MODBUS_ACTION_CHECK_REQUEST_FROM_MASTER     1
        #define MODBUS_ACTION_SEND_REQUEST_2DEVICE          2
        #define MODBUS_ACTION_CHECK_RESPONSE_FROM_DEVICE    3
        #define MODBUS_ACTION_SCAN_MASTER                   10
        #define MODBUS_ACTION_2MEMORY                       41  //For 7188/XA/XB/XC only
        
        They are used in the Modbus lirbray. Demo programs don't need it.
---------------------------------------------------------------------
Version 1.5.9 [22,Sep,2004] by Kevin
    Provides a Modbus/RTU library (MBR7Ennn.Lib and MBRTU_7E.h) for 7188EX.
    It can be integrated to user's program that not developed on 
    the Xserver architecture.
---------------------------------------------------------------------
Version 1.5.8 [26,Jun,2004] by Kevin
    Modify function ModbusMaster2Slave:
        supports modbus function code 5 and 6.
        cleafy return value
            before: always return 0
            now: return different value to identify different 
                 situation, such as timeout, CRC check error,
                 exception code, etc.
    
    Add global variables
        iModbusRequest_Fun
        iModbusRequest_Addr
        iModbusRequest_IOCount
---------------------------------------------------------------------
Version 1.5.7 [22,Jun,2004] by Kevin
    add global variables
      iModbusRequest_Fun
      iModbusRequest_Addr;
      iModbusRequest_IOCount;
      
    ModbusMaster2Slave supports modbus function code 5 and 6.
---------------------------------------------------------------------
Version 1.5.5 [24,Nov,2003] by Kevin
    Remove functions
        Set_DebugPort
        Set_ModbusUpLinkPort
    
    Remove variables
        iModbusUpLinkPort
        iDebugPort   
        
    Modify Set_EnableMode
        adds two new modes (UpLink, Debug) 
        to replace function Set_DebugPort and Set_ModbusUpLinkPort.
    
    Modify comment of MODBUS_TYPE  
    
    Modify comment of function CheckModbusRequest
    
    Change function name
        Set_StationCountPerCOMPort === Changed to ===> Set_StationPerCOMPort    
        Set_EnableMode             === Changed to ===> Set_COMEnableMode
        Set_Timeout                === Changed to ===> Set_ModbusTimeout
---------------------------------------------------------------------
Version 1.5.3 [13,Jun,2003] by Kevin
    Add function CheckModbusRequest.
---------------------------------------------------------------------
Version 1.5.2 [5,Jun,2003] by Kevin
    Add comment to ModbusMaster2Slave
---------------------------------------------------------------------
Version: 1.5.1 [30,Apr,2003] by Kevin                          
    Change declaration                          
        1. MAX_REGISTER_COUNT: constant ==> int 
        2. iMemory_DI, iMemory_DO,              
           iMemory_AI, iMemory_AO ==> to pointer
---------------------------------------------------------------------
Version: 1.5.0 [20,Feb,2003] by Kevin                          
    Change:                                     
        iModbusInternalID ==> iModbusNetID      
---------------------------------------------------------------------
Version: 1.3.0 [16,Oct,2002] by Kevin  
*/
#ifdef __cplusplus
extern "C" {
#endif
#define MODBUS_DATA_LENGTH 256  // modbus data max length (byte)
#define _Modbus_Command_Buffer 4
typedef struct t_ModbusPacket
{
    int             EnableMode;     // =1 to eanble COM port as a modbus port
    int             Socket[_Modbus_Command_Buffer];
    int             COMPort[_Modbus_Command_Buffer];
    int             Status[_Modbus_Command_Buffer];     // to identify which process the kernal is running
    int             Length[_Modbus_Command_Buffer];     // Data length, doesn't include Modbus/TCP leading 6 bytes and 2 CRC bytes
    int             Timeout;
    STOPWATCH       Timer;
    int             Current_Index;
    unsigned char   Data[_Modbus_Command_Buffer][MODBUS_DATA_LENGTH];
} MODBUS_TYPE;
extern MODBUS_TYPE mtModbusPort[9]; // 7188 series has a maximum of 8 COM ports

// Constant for EnableMode
#define _VxComm         0  //==> VxComm                                  
#define _ModbusRTU      1  //==> Modbus/RTU                              
#define _Programming    2  //==> Programming                             
#define _UpLink         3  //==> UpLink (to be a Modbus/RTU slave port)  
#define _Debug          4  //==> Debug (print out some message for debug)
#define _Console        5  //==> Consol (ET-6000 uses it to do simple config)


//Following 3 variables record the information 
//of last Modbus/TCP or Modbus/RTU request.
extern unsigned int iModbusRequest_Fun;
    // 1 ==> Reads  multi  DO from internal register (iMemory_DO)
    // 2 ==> Reads  multi  DI from internal register (iMemory_DI)
    // 3 ==> Reads  multi  AO from internal register (iMemory_AO)
    // 4 ==> Reads  multi  AI from internal register (iMemory_AI)
    // 5 ==> Write    one  DO to   internal register (iMemory_DO)
    // 6 ==> Write    one  AO to   internal register (iMemory_AO)
    //15 ==> Writes multi  DO to   internal register (iMemory_DO)
    //16 ==> Writes multi  AO to   internal register (iMemory_AO)
    
extern unsigned int iModbusRequest_Addr;
extern unsigned int iModbusRequest_IOCount;
extern unsigned long lModbusRequest_TimeTicks;

extern int iModbusNetID;
extern int iTotalCOMPort;
extern int iEnableUpLinkPort;
extern unsigned long lModbusRequest_TimeTicks;
extern unsigned long lCRCError,lTimeoutError;

//===============================================//
//          Prototype of functions               //
//===============================================//
int InitModbus(unsigned char far *iPointer_DI, 
               unsigned char far *iPointer_DO,
                         int far *iPointer_AI,
                         int far *iPointer_AO);
    /*                                
    step 1. declare 4 global arrays
    step 2. give the 4 array pointers to InitModbus.
        For example:
            unsinged char far Array_DI[100];
            unsinged char far Array_DO[100];
                      int far Array_AI[100];
                      int far Array_AO[100];
            
            main()
            {
                InitModbus(Array_DI,Array_DO,Array_AI,Array_AO);
                
                //Giving initial values.
                Array_DI[0]=1;
                Array_DO[10]=0;
                Array_AI[5]=100;
                Array_AO[20]=-500;
                
                ....
                ....
            }
*/
    
void CheckModbusRequest(int iPort);
    /*
    iPort: Is any Modbus/RTU request from COM port ?
    
    The enable mode of the COM port must be set in UpLink.
    Two methods to set the enable mode
    Method 1: Using the Modbus Utility (after version 1.2.6)
    Method 2: Calling function Set_COMEnableMode in the C program
    
    The destination that the Modbus request 
       will be passed is decided by the NetID (or called Station number)
       in the Modbus request and the global variable ,iStationCountPerCOMPort.
       You can set the two key values in the Modbus Utility.
    
    Enter this function when iModbusAction=MODBUS_ACTION_IDLE
    Sets iModbusAction=MODBUS_ACTION_CHECK_REQUEST_FROM_PC 
        when gets correct modbus request.
    */
        
void SendModbusRequest(int iPort);
    /*
    Enter this function when mtModbusPort[iPort].Status=MODBUS_ACTION_CHECK_REQUEST_FROM_MASTER
    Sets mtModbusPort[iPort].Status=MODBUS_ACTION_SEND_REQUEST_2DEVICE 
        when finish sending out modbus request.
    */
    
void CheckResponseTimeout(int iPort);
    /*
    Enter this function when mtModbusPort[iPort].Status=MODBUS_ACTION_SEND_REQUEST_2DEVICE
    Sets mtModbusPort[iPort].Status=MODBUS_ACTION_IDLE when timeout is up. 
    */

void CheckModbusResponse(int iPort);
    /*
    Enter this function when mtModbusPort[iPort].Status=MODBUS_ACTION_SEND_REQUEST_2DEVICE
    Sets mtModbusPort[iPort].Status=MODBUS_ACTION_CHECK_RESPONSE_FROM_DEVICE 
        when receiving correct modbus response.
    Sets mtModbusPort[iPort].Status=MODBUS_ACTION_IDLE when error.
    */
    
void SendModbusResponse(int iPort);
    /*
    Enter this function when iModbusAction=MODBUS_ACTION_CHECK_RESPONSE_FROM_DEVICE
    Sets mtModbusPort[iPort].Status=MODBUS_ACTION_IDLE when finish sending out modbus response.
    */

int Set_NetID(int iNetID);
    /*
    Sets Modbus NetID.
    If the iNetID doesn't match the current setting,
        this function changes the current setting 
        and sotre the setting to the EEPROM.
    If the iNetID matchs the current setting, 
        holds the currnet setting.
    
    iNetID: 0~255
    
    Return: 0  ==> success
            -1 ==> writes the setting to EEPROM failure
    */  
    
#define Set_StationCountPerCOMPort Set_StationPerCOMPort    
int Set_StationPerCOMPort(int iCount);
    /*
    Sets station counter per COM port.
    If the iCount doesn't match the current setting,
        this function changes the current setting 
        and sotre the setting to the EEPROM.
    If the iCount matchs the current setting, 
        holds the currnet setting.
    
    iCount: 1~255
    
    Return: 0  ==> success
            -1 ==> writes the setting to EEPROM failure
    */

#define Set_EnableMode Set_COMEnableMode
int Set_COMEnableMode(int iPort, int iMode);
    /*
    Sets COM port enable mode.
    If the iMode doesn't match the current setting,
        this function changes the current setting 
        and sotre the setting to the EEPROM.
    If the iMode matchs the current setting, 
        holds the currnet setting.
    
    iPort: 1 ~ total COM Port
    iMode: 0 (_VxComm)      ==> VxComm (This is valid for 7188E/8000E)
           1 (_ModbusRTU)   ==> Modbus/RTU converter
           2 (_Programming) ==> Programming 
                                The Modbus kernel doesn't control the COM port.
                                Users can use the COM port to control other 
                                RS-232/485 devices.
           3 (_UpLink)      ==> UpLink 
                                Be a Modbus/RTU slave port. Modbus clients 
                                or masters (HMI or SCADA software) can send
                                Modbus requests to the COM port.
           4 (_Debug)       ==> Debug  
                                The Modbus kernel prints out some messages 
                                while communicating with Modbus clients or 
                                masters.
                                The messages includes
                                (0) receives Modbus request
                                    response to Modbus clients or masters
                                (1) by passes Modbus request to COM port
                                (2) Send Modbus request to COM port
                                (3) Check Modbus response from COM port
                                (4) Send Modbus response to Modbus client or 
                                    masters.
           
    Return: 0  ==> success
            -1 ==> writes the setting to EEPROM failure
    */

#define Set_Timeout Set_ModbusTimeout
int Set_ModbusTimeout(int iPort, int iTimeout);
    /*
    Sets COM port timeout.
    If the iTimeout doesn't match the current setting,
        this function changes the current setting 
        and sotre the setting to the EEPROM.
    If the iTimeout matchs the current setting, 
        holds the currnet setting.
    
    iPort: 1 ~ iTotalCOMPort
    iTimeout: 5~32767 (ms)
    
    Return: 0  ==> success
            -1 ==> writes the setting to EEPROM failure
    */
    
//==================================================//
//                                                  //
//      Modbus master functions                     //
//        for Modbus/RTU                            //
//        for Modbus/ASCII                          //
//        for Modbus/TCP                            // 
//==================================================//
int ModbusRTU_Master(int iPort,unsigned char cNetID, unsigned char cFunction,
    int iLocalMemoryBaseAddress, int iRemoteMemoryBaseAddress,int iIOCount,unsigned long lTimeout,int iWait);
    /*
    This functions can send Modbus/RTU request to Modbus/Slave devices.
    1. You just only change internal registers (iMemory_DI, iMemory_DO,
       iMemory_AI, iMemory_AO) then call this function to send out request to turn on coils
       or write analog registers.
    2. Sends out  
    
    iPort: 1 ==> COM1 
           2 ==> COM2 
           ...
           Sends request to which COM port?
    cNetID: 0~0xFF, the NetID (Station Number) of destination Modbus/Slave device.
    cFunction:  1 ==> Reads  several DO from device, then puts to iMemory_DO.
                2 ==> Reads  several DI from device, then puts to iMemory_DI.
                3 ==> Reads  several AO from device, then puts to iMemory_AO.
                4 ==> Reads  several AI from device, then puts to iMemory_AI.
                5 ==> Writes     one DO from iMemory_DO to device. 
                6 ==> Writes     one AO from iMemory_AO to device.
               15 ==> Writes several DO from iMemory_DO to device.
               16 ==> Writes several AO from iMemory_AO to device.

              102 ==> Reads  several DI from device, then puts to iMemory_DO (Use Fun 2).
              104 ==> Reads  several AI from device, then puts to iMemory_AO (Use Fun 4).
              105 ==> Writes     one DO from iMemory_DI to device's DO (Use Fun 5). 
              106 ==> Writes     one AO from iMemory_AI to device's AO (Use Fun 6).
              115 ==> Writes several DO from iMemory_DI to device's DO (Use Fun 15).
              116 ==> Writes several AO from iMemory_AI to device's AO (Use Fun 16).
                             
    iLocalMemoryBaseAddress: internal register base address that you want to deal.
    iRemoteMemoryBaseAddress: register base address of device that you want to deal.
    iIOCount: count of coils or registers that you want to deal.
    lTimeout: unit=ms
    iWait: 0=non-blocked communication mode
             if there is no data in COM port input buffer, 
             return the function immediatly.
           1=blocked communication mode
             wait to receive data till timeout, 
             even there is no data in COM port input buffer.
    
    Return value:
         -1: CRC error
         -2: Receiving response timeout (no data)
      0~255: Exception code from the Modbus slave device.
        500: COM port is busy, cann't send command.
        501: Command not sent yet.
        502: Command already sent, but no data received.
        503: parameters of previous command not match parameters of this commnad.
        (return code 500~503 are for wait=0 (non-blocked) mode)
    */

int ModbusRTU_Master_Send(int iPort,unsigned char cNetID, unsigned char cFunction,
    int iLocalMemoryBaseAddress, int iRemoteMemoryBaseAddress,int iIOCount);

int ModbusRTU_Master_Receive(int iPort,unsigned char cNetID, unsigned char cFunction,
    int iLocalMemoryBaseAddress, int iRemoteMemoryBaseAddress,int iIOCount,
    unsigned long lTimeout,int iWait);    
    
int ModbusASCII_Master(int iPort,unsigned char cNetID, unsigned char cFunction,
    int iLocalMemoryBaseAddress, int iRemoteMemoryBaseAddress,int iIOCount,unsigned long lTimeout,int iWait);
    /*
    This functions can send Modbus/RTU request to Modbus/Slave devices.
    1. You just only change internal registers (iMemory_DI, iMemory_DO,
       iMemory_AI, iMemory_AO) then call this function to send out request to turn on coils
       or write analog registers.
    2. Sends out  
    
    iPort: 1 ==> COM1 
           2 ==> COM2 
           ...
           Sends request to which COM port?
    cNetID: 0~0xFF, the NetID (Station Number) of destination Modbus/Slave device.
    cFunction:  1 ==> Reads  several DO from device, then puts to iMemory_DO.
                2 ==> Reads  several DI from device, then puts to iMemory_DI.
                3 ==> Reads  several AO from device, then puts to iMemory_AO.
                4 ==> Reads  several AI from device, then puts to iMemory_AI.
                5 ==> Writes     one DO from iMemory_DO to device. 
                6 ==> Writes     one AO from iMemory_AO to device.
               15 ==> Writes several DO from iMemory_DO to device.
               16 ==> Writes several AO from iMemory_AO to device.

              102 ==> Reads  several DI from device, then puts to iMemory_DO (Use Fun 2).
              104 ==> Reads  several AI from device, then puts to iMemory_AO (Use Fun 4).
              105 ==> Writes     one DO from iMemory_DI to device's DO (Use Fun 5). 
              106 ==> Writes     one AO from iMemory_AI to device's AO (Use Fun 6).
              115 ==> Writes several DO from iMemory_DI to device's DO (Use Fun 15).
              116 ==> Writes several AO from iMemory_AI to device's AO (Use Fun 16).
                             
    iLocalMemoryBaseAddress: internal register base address that you want to deal.
    iRemoteMemoryBaseAddress: register base address of device that you want to deal.
    iIOCount: count of coils or registers that you want to deal.
    lTimeout: unit=ms
    iWait: 0=non-blocked communication mode
             if there is no data in COM port input buffer, 
             return the function immediatly.
           1=blocked communication mode
             wait to receive data till timeout, 
             even there is no data in COM port input buffer.
    
    Return value:
         -1: LRC error
         -2: Receiving response timeout (no data)
      0~255: Exception code from the Modbus slave device.
        500: COM port is busy, cann't send command.
        501: Command not sent yet.
        502: Command already sent, but no data received.
        503: parameters of previous command not match parameters of this commnad.
        (return code 500~503 are for wait=0 (non-blocked) mode)
    */  

int ModbusASCII_Master_Send(int iPort,unsigned char cNetID, unsigned char cFunction,
    int iLocalMemoryBaseAddress, int iRemoteMemoryBaseAddress,int iIOCount);
    
int ModbusASCII_Master_Receive(int iPort,unsigned char cNetID, unsigned char cFunction,
    int iLocalMemoryBaseAddress, int iRemoteMemoryBaseAddress,int iIOCount,unsigned long lTimeout,int iWait);      
    
int ModbusTCP_Init(int iIndex,unsigned char *cRemote_IP,int iRemote_TCPPort,
                   unsigned long lConnect_Timeout,unsigned long lReConnect_Time_Interval);
    /*
    Give Modbus/TCP intial values.
    there are 8 connections available. You can assign several 
    connection to one IP address or one connection to each IP address. 
    
    iIndex: 
          0~7. The library can assign totaly 8 connections.
    cRemote_IP: 
          IP address for remote device.
          For example: "192.168.255.100"
    iRemote_TCPPort: 
          TCP port for remote device.
          For example: 502 
    lConnect_Timeout: 
          Timeout to connect to remote device. 
          unit: ms       
    lReConnect_Time_Interval:
          Time interval to reconnect to remote device
          when the
          
    return: 0= ok.
            other = Error.      
    */
    
int ModbusTCP_Master2Slave(int iIndex,unsigned char cNetID, unsigned char cFunction,
                           int iLocalMemoryBaseAddress, int iRemoteMemoryBaseAddress,
                           int iIOCount, unsigned long lTimeout);
    /*
    This functions can send Modbus/TCP request to Modbus/Slave devices.
    1. You just only change internal registers (iMemory_DI, iMemory_DO,
       iMemory_AI, iMemory_AO) then call this function to send out request to turn on coils
       or write analog registers.
    2. Sends out  
    
    iIndex: 
          0~7. The library can assign totaly 8 connections.
               Each connection's configuration is defined by 
               function ModbusTCP_Init.
               
    cNetID: 0~0xFF, the NetID (Station Number) of destination Modbus/Slave device.
    cFunction:  1 ==> Reads  several DO from device, then puts to iMemory_DO.
                2 ==> Reads  several DI from device, then puts to iMemory_DI.
                3 ==> Reads  several AO from device, then puts to iMemory_AO.
                4 ==> Reads  several AI from device, then puts to iMemory_AI.
                5 ==> Writes     one DO from iMemory_DO to device. 
                6 ==> Writes     one AO from iMemory_AO to device.
               15 ==> Writes several DO from iMemory_DO to device.
               16 ==> Writes several AO from iMemory_AO to device.

              102 ==> Reads  several DI from device, then puts to iMemory_DO (Use Fun 2).
              104 ==> Reads  several AI from device, then puts to iMemory_AO (Use Fun 4).
              105 ==> Writes     one DO from iMemory_DI to device's DO (Use Fun 5). 
              106 ==> Writes     one AO from iMemory_AI to device's AO (Use Fun 6).
              115 ==> Writes several DO from iMemory_DI to device's DO (Use Fun 15).
              116 ==> Writes several AO from iMemory_AI to device's AO (Use Fun 16).
                             
    iLocalMemoryBaseAddress: internal register base address that you want to deal.
    iRemoteMemoryBaseAddress: register base address of device that you want to deal.
    iIOCount: count of coils or registers that you want to deal.
    lTimeout: 1~65535. unit: ms
              timeout to receive response from Modbus/Slave device.
    return:
         0: No Error.
            Send the command ok, and receive the response ok.
         1: send command ok,
            begin to wait response next time.
         2: send command ok,
            not receive the response yet,
            keep to receive the response next time.
        -1: connect to slave device error,
            try to re-connect next timer interval.
        -2: connection is not builded. Cannot send command. 
        -3: connect to the device ok,
            send command error,
            try to re-connect the device next time interval.
        -4: send command ok,
            wait response timeout,
            try to re-connect the device next time interval.  
        -5: send command ok,
            receive response error.
        -6: the connection is closed or not initialized.  
            You must call ModbusTCP_Init first then this function. 
    */
        
int ModbusTCP_Close(int iIndex);      
    /*
    Close the connection.
    
    iIndex: 
          0~7. The library can assign totaly 8 connections.
    */
void Modbus_Request_Event(char* CommandData,int* CommandLength);
    /*
    char* CommandData: For Modbus/TCP, it includes 6 leading bytes. (needful) 
                       For Modbus/RTU, it includes 6 leading bytes. (needless) 
                       
    int* CommandLength: For Modbus/TCP, it includes 6 leading bytes. 
                        For Modbus/RTU, it includes 6 leading bytes.  
    Following related global variables are also usefull:
            extern unsigned int iModbusRequest_Fun;
            extern unsigned int iModbusRequest_Addr;
            extern unsigned int iModbusRequest_IOCount;
            extern unsigned long lModbusRequest_TimeTicks;
    */
void Modbus_Response_Event(char* ResponseData,int* ResponseLength);
    /*
    char* ResponseData: For Modbus/TCP, it includes 6 leading bytes. 
                        For Modbus/RTU, it doesn't include 6 leading bytes
                       
    int* CommandLength: For Modbus/TCP, it includes 6 leading bytes. 
                        For Modbus/RTU, it doesn't include 6 leading bytes
    */
#ifdef __cplusplus
     }
#endif
