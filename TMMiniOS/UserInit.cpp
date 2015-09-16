#include "Model.h"

/*

    In this function, user CAN:
    1. initialize user's program.
    2. set time interval for calling UserCount().
    3. set initial value of I/O or variables for UserLoopFun().
    4. set initial value of I/O or variables for another functions.
    5. change the default TCP PORT 10000/9999/502 to others value. [after vcom3004.lib]
       Syntax:
        Port10000=newport_10000;  for calling UserCmd       (user.c)
        Port9999=newport_9999;    for calling VcomCmd7000   (v7000.c)
        Port502=newport_502;      for calling VcomCmdModbus (vModbus.c) [after vcom3002.lib]
        PortUser=newport_User;    for calling VcomCmdUser   (user.c)    [after vcom3005.lib]
        
        Default port value:
        Port10000=10000;
        Port9999=9999;
        Port502=502;
        PortUser=0;
        If the port value is 0, Xserver will not 
        listen that port. That means the port will 
        be disable.

*/


void UserInit(void)
{
	CycleController & cycleController = CycleController::getInstance();
	cycleController.init();
}