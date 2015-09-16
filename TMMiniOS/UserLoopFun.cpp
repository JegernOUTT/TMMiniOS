#include "Model.h"

/*
    //======= Begin of Modbus Kernel =======

    if(mtModbusPort[iScanCOMPort].EnableMode==_ModbusRTU_Slave)
        CheckModbusRTURequest(iScanCOMPort);        // Is any request from Modbus/RTU Master ?
        
    if(mtModbusPort[iScanCOMPort].EnableMode==_ModbusASCII_Slave)
        CheckModbusASCIIRequest(iScanCOMPort);      // Is any request from Modbus/ASCII Master ?
        
    if(mtModbusPort[iScanCOMPort].EnableMode==_ModbusRTU_Gateway)
    {
        SendModbusRequest(iScanCOMPort);        // Passes request to modbus slave device.
        CheckResponseTimeout(iScanCOMPort);     // If response timeout, sets iModbusAction
                                                // to IDLE status.
        CheckModbusResponse(iScanCOMPort);      // Is any response from modbus slave device?
        SendModbusResponse(iScanCOMPort);       // Passes response to Modbus/RTU Master.
    }
    iScanCOMPort++;
    if(iScanCOMPort>iTotalCOMPort)
        iScanCOMPort=1;            

    //======= End of Modbus Kernel =======   
*/

void UserLoopFun(void)
{
	CycleController & cycleController = CycleController::getInstance();
	cycleController.read();
	#ifdef VC_1992_16BIT
		Time::timeUpdate();
	#endif
}