#ifdef VC_1992_16BIT

#include "lib\7188e.h"
#include "lib\mbtcp.h"   
#include "lib\vxcomm.h"     
                             
                                                     
void _ftol(void);
void Port9999Start(int skt)
{
    /* XS8_3200.Lib (20,Apr,2004) or later version supports this function.
    
    When a TCP/IP client connects to the 7188E/8000E TCP port 9999, 
    the Xserver calls the function once.
    
    You can use function VcomSendSocket to send a message to the client 
    when a connection is established.
    For example:
        VcomSendSocket(skt,"Connection is established.",26);	// return 26 bytes.

    skt: socket number assigned to the TCP/IP client.
    */
    
    skt=skt; //do nothing
}    

int VcomUserBinaryCmd(TCPREADDATA *p)
{
 /* VXCOMM.EXE 2.6.12(09/04/2001) or later will support this function.

        TCP PORT 10000, command 23 will call this function.
        user can get the following message:
        p->ReadUartChar : the buffer store the command data(include "23")
        p->Length : the command data length(include the two byte "23")
        p->Socket : the socket number that receive the command, that is when the user function want
                                return message to the client, just use the socket to send data.
                                use:  VcomSendSocket(p->Socket,pdata,datalength);
 */
    VcomSendSocket(p->Socket,"User-defined command(23)",24);    // return 24 bytes.
    return 1;   /* any value will be accept */
}
int VcomCmd7000(TCPREADDATA *p)
{ /*
    VCOM3001 or later will call this function for port 9999.
    when socket from TCP PORT 9999 will call this function.
    user can get the following message:
    p->ReadUartChar : the buffer store the command data.
    p->Length : the command data length.
    p->Socket : the socket number that receive the command, that is when the user function want 
                return message to the client, just use the socket to send data.
                use:  VcomSendSocket(p->Socket,pdata,datalength);
  */
  
  /*       here just send back the command come from port 9999. */
	VcomSendSocket(p->Socket,p->ReadUartChar,p->Length);
	return 1;	/* any value will be accept */
}
void  _fastcall far _FTOLF()
{   
_ftol();                              
}

void _fastcall far _SPUSHF()
{                                             
_asm{
	leave
	pop bx
	pop es
	sub sp,cx
	push es
	push bx
	push di
	push ds
	mov di,sp
	add di,8
	mov bx,ss
	mov es,bx
	mov ds,dx
	xchg ax,si
	cld
	shr cx,1
	rep movsw
	adc cx,cx
	rep movsb
	xchg ax,si
	pop ds
	pop di
	retf
    }
}   
     
void _setargv__(void)
{
}
int VcomCmdUser(TCPREADDATA *p)  
{ 
    /*
    VCOM3005 (Feb,22,2002) or later will call this function for PortUser.
    
    When packets received by TCP PORT PortUser(user defined) 
    of 7188E/8000E, Xserver will call this function.
    user can get the following message:
    p->ReadUartChar : the buffer store the command data.
                      Maximum length of p->ReadUartChar is 32767 bytes.
    p->Length : the command data length.
    p->Socket : the socket number that receive the command, that is when the user function wants 
                return message to the client, just use the socket to send data.
                usage:  VcomSendSocket(p->Socket,pdata,datalength);
    */
  
    /* here just send back the command to the client. */
    VcomSendSocket(p->Socket,p->ReadUartChar,p->Length);
    return 1;   /* any value will be accept */
}
      

int UserCmd(unsigned char *Cmd,unsigned char *Response)
{
	//sprintf((char *)Response,"%s",Cmd);
	return 1;		// return ok
}


void PortUserStart(int skt)
{
    /* XS8_3200.Lib Version 3.2.00 (20,Apr,2004) 
       or later version supports this function.
    
    When a TCP/IP client connects to the 7188E/8000E TCP port 10000, 
    the Xserver calls the function once.
    
    You can use function VcomSendSocket to send a message to the client 
    when a connection is established.
    For example:
        VcomSendSocket(skt,"Connection is established.",26); //return 26 bytes.

    skt: socket number assigned to the TCP/IP client.
    */
                      
    
    skt=skt; //do nothing
}

#endif
