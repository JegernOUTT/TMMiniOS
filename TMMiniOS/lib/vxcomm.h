#ifdef __cplusplus 
extern "C"
      {
#endif
void UserInit(void);
/*
  User must support the function : UserInit();
*/
void UserLoopFun(void);
/*
  User must support the function : UserLoopFun();
*/
int UserCmd(unsigned char *Cmd,unsigned char *Response);
/*
  When user want use COMMAND "19",
  User must support the function : UserCmd();
  COMMAND "19" will pass the user command to function UserCmd();
  If the function will return something back, must put the data on
  the output buffer "Response", and return value must >0 .
*/
void AddUserTimerFunction(void (*fun)(void),unsigned timems);
/*
   The vxcomm.LIB support AddUserTimerFunction();
   when user want to install a function, and for a period time
   the system will call the function, just call this function to
   install it.

   for example:

    unsigned cnt;
    void UserCount(void)
    {
       cnt++;
    }

    void UserInit(void)
    {
      AddUserTimerFunction(UserCount,10); // every 10 ms will call UserCnt
    }
*/

int EnableCom(int port);
/*
    The function can force the eXserver to skip the com port
*/
int DisableCom(int port);
/*
    The function can force the eXserver to re-work with the com port
*/

extern int SocketConnected;
/*
  SocketConnected : total connect socket number
*/

extern int EchoMode;
/*
  EchoMode:
   0, default mode.
   1, will add the command number to the echo message.
      for example:
      input: 10
      output:107188E2
*/

typedef struct t_TcpReadData{
	int Comport;
	int Socket;
	int Length;
	char* ReadUartChar;
}TCPREADDATA;

int VcomSendSocket(int skt,char *data,int cnt);

extern unsigned Port9999,Port502,Port10000,PortUser;
/*
  By default Port10000=10000,Port9999=9999, Port502=502, PortUser=0
  user's function(UserInit()) can change them to others port value.
*/

void VcomSaveComData(int port);

#define MAX_COMPORT 4

typedef struct COM_DATA {
  unsigned long baud;
  char databit;
  char parity;
  char stopbit;
  unsigned char sum;
} *pCOM_DATA,COM_DATA;

extern COM_DATA ComData[MAX_COMPORT];
/*
  The COM port setting value will be save in EEPROM and VCOMnnnn will read it from EEPROM.
  And it will be display on the 5DigitLed.
  If user's function want change the setting value, just set the variable ComData[port],
and can call VcomSaveComData(int port) to save the new value to EEPROM.
*/

int TcpPrint(int skt,int mode,char *fmt,...);
/*
 [09/09/2002] Add. VCOM3009 or later
 Use TcpPrint like printf, add the message will be send out by TCP/IP.

 skt: socket
 mode: 0: send to internal buffer first,if the buffered size > 1360
          the buffered msg will be send out.
       1: send out immediately.
*/

#ifndef _TCPIP_H_

struct in_addr {                /* Internet address */
    unsigned long   s_addr;
};

struct sockaddr_in {            /* Internet socket address */
    short           sin_family; /* should be unsigned but this is BSD */
    unsigned short  sin_port;   /* network order !!! */
    struct in_addr  sin_addr;
    char            sin_zero[8];
};

typedef struct socket_state {
  int                active;  /* socket is active */
  struct sockaddr_in sin;     /* client address */
} socket_state;


/* Global Variable */
extern socket_state sst[];    /* sockets state table */

/* After establish connection to Port9999/Port520/PortUser, system will call these function. 
   user program must support these three functions.
   It can send welcome message to client side, or the message to ask user id/password, or ...
*/
void Port9999Start(int skt);
void Port502Start(int skt);
void PortUserStart(int skt);

#endif
#ifdef __cplusplus
      }
#endif
