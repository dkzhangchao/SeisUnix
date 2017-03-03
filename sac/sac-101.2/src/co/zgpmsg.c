/*
CDOC
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 
 *** Function:	zgpmsg_(prmt,msg,prmtlen,msglen)		D. Trimmer
 
 *** Purpose:	Prints a prompt message on the standard output, and
		receives a reply on the standard input.

 *** Inputs:	prmt	A pointer to a string containing the prompt.
			The end of prompt delimeter is a '$'.
		msg	A pointer to a string to receive the prompt.
		prmtlen A long containing the size of prmt (automatically
			generated by UNIX VAX F77)
		msglen  A long containing the size of msg (automatically
			generated by UNIX VAX F77)
 
 *** Returns:	message from standard input is returned in msg
 
 *** Notes:	This function is to be called by a FORTRAN routine.  The
		'_' is appended to the name for compatibility with FORTRAN.
 
		Arguments prmtlen and msglen are for generated by F77.
 
 *** Calls:	UNIX function getfline() or local function getxline()
 
 *** History:	07/23/84   Under development--D. Trimmer
		07/24/84   Tested--D. Trimmer
		08/01/84   Prefill return message buffer with NULLs--D. Trimmer
		08/17/84   Call getfline() instead of getline()--no EOS and
			   array is padded with spaces.--D. Trimmer
                01/09/87   Changed to handle SUN graphics events.--B. Hickman 
		07/30/87   Fixed bug concerning setting flags.--B.Hickman
                11/03/87   Doubled size of "command" to 160 characters.--J.Tull
                03/29/88   Deleted special SUN graphics event handling.--J.Tull
                06/28/91   Added hack contributed by Doug Neuhauser of UCB
                           Seismographic Station, fixes problem with the
                           continuous prompt when a control-d is type at
                           the command prompt (wct).
		
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
CEND
*/
 
#include "stdu.h"
#include <sys/select.h>
#include <X11/Xlib.h>
#include <string.h>
#include "config.h"
#include "../../inc/mach.h"
#include "../../inc/gd3.x11.h"
#include "../../inc/gdm.h"
#include <stdio.h>
#include <errno.h>
static int global_msglen;
static int global_before_newline;
static char global_string[MCMSG+1];

#if !defined(READLINE) && !defined(USE_X11_DOUBLE_BUFFER)

zgpmsg(prmt,prmtlen,msg,msglen)
char *prmt;		/* pointer to prompt message */
long prmtlen;		/* length of prmt array */
char *msg;		/* pointer to character array to receive input */
long msglen;		/* length of msg array */
 
{
	int i;			/* index for prefilling string w/ NULLs */
	char *psave;		/* save msg */
 
	psave = msg;
	for (i=0;i<(int)msglen;++i)	/* prefill with NULLs */
		*(psave++) = '\0';
 
	while (*prmt != '$')
		putchar (*(prmt++));	/* print prompt */


        fflush(stdout);

/* A control-d sets the message response to quit */
	if ( getfline (stdin,msg,(short)msglen) == -1)
	   if ( msglen >= 5 ) strncpy (msg, "quit", 4);
	
}

#else 

#ifdef SOLARIS
#define __sun__
#endif /* SOLARIS */

#include <readline/readline.h>
#include "../../inc/select.h"



static void
process_line(char *p) {
  int i;

  select_loop_continue(SELECT_OFF); /* Turn off select loop */
  select_loop_message(p, SELECT_MSG_SET); /* Set the outgoing message */

  if(p && *p)
    add_history(p);
  rl_callback_handler_remove();
}

void
zgpmsg(prmt,prmtlen,msg,msglen)
char *prmt;		/* pointer to prompt message */
long prmtlen;		/* length of prmt array */
char *msg;		/* pointer to character array to receive input */
long msglen;		/* length of msg array */
{
  select_loop(prmt, prmtlen, msg, msglen, NULL, process_line);
}

#endif /* !READLINE */