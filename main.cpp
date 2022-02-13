/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: abratchik
 *
 * Created on January 8, 2022, 2:12 PM
 */

#include <dirent.h>
#include <iterator>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <sys/stat.h>
#include <syslog.h>
#include <vector>

#include "AuxDisplay.h"

using namespace std;

const string AUXDISPLAY = "auxdisplay-daemon";

bool keepalive = true;

void do_heartbeat(AuxDisplay* auxdisplay)
{
    
    auxdisplay->show();
    
}

void signal_handler(int sig) {
    
    switch(sig) {
        case SIGTERM:
            keepalive = false;
            break;
        default:
            break;
    }
}

/*
 * 
 */
int main(void) {
    
   // Define variables
   pid_t pid, sid;

#ifndef NOFORK
   // Fork the current process
   pid = fork();
   // The parent process continues with a process ID greater than 0
   if(pid > 0)
   {
      exit(EXIT_SUCCESS);
   }
   
   // A process ID lower than 0 indicates a failure in either process
   else if(pid < 0)
   {
      exit(EXIT_FAILURE);
   }
   
   // The parent process has now terminated, and the forked child process will continue
   // (the pid of the child process was 0)

   // Since the child process is a daemon, the umask needs to be set so files and logs can be written
   umask(0);
#endif
   
   // Open system logs for the child process
   openlog(AUXDISPLAY.c_str(), LOG_NOWAIT | LOG_PID, LOG_USER);
   
   syslog(LOG_NOTICE, "Successfully started %s", AUXDISPLAY.c_str());

#ifndef NOSESSION
   // Generate a session ID for the child process
   sid = setsid();
   // Ensure a valid SID for the child process
   if(sid < 0)
   {
      // Log failure and exit
      syslog(LOG_ERR, "Could not generate session ID for child process of %s", AUXDISPLAY.c_str());

      // If a new session ID could not be generated, we must terminate the child process
      // or it will be orphaned
      exit(EXIT_FAILURE);
   }
#endif
   
#ifndef NOWORKDIR
   // Change the current working directory to a directory guaranteed to exist
   if((chdir("/tmp")) < 0)
   {
      // Log failure and exit
      syslog(LOG_ERR, "Could not change working directory to / for %s", AUXDISPLAY.c_str());

      // If our guaranteed directory does not exist, terminate the child process to ensure
      // the daemon has not been hijacked
      exit(EXIT_FAILURE);
   }
#endif

   // A daemon cannot use the terminal, so close standard file descriptors for security reasons
   close(STDIN_FILENO);
   
#ifndef NOCLOSEIO   
   close(STDOUT_FILENO);
   close(STDERR_FILENO);
#endif

   // Enter daemon loop

   AuxDisplay auxdisplay;
   syslog(LOG_NOTICE, "Initializing driver for %s", AUXDISPLAY.c_str());
   
   if (auxdisplay.load_config() != CONFIG_LOAD_SUCCESS) {
        syslog(LOG_ERR, "Could not initialize %s", AUXDISPLAY.c_str());
   }

   // Register signal handlers
   signal(SIGSTOP, signal_handler);
   signal(SIGTERM, signal_handler);
   
    syslog(LOG_NOTICE, "Starting daemon loop for %s", AUXDISPLAY.c_str());
    while(keepalive)
    {
       if (auxdisplay.initialize()) {

            // Execute daemon heartbeat, where your recurring activity occurs
            do_heartbeat(&auxdisplay);
            // Sleep for a period of time
            // syslog(LOG_NOTICE, "Executed show for %s", AUXDISPLAY.c_str());
       }
       else {
            syslog(LOG_ERR, "Could not initialize %s", AUXDISPLAY.c_str());
       }

       usleep(auxdisplay.get_refresh() * 1000);
    }


   // Close system logs for the child process
   syslog(LOG_NOTICE, "Stopping %s", AUXDISPLAY.c_str());
   closelog();

   // Terminate the child process when the daemon completes
   exit(EXIT_SUCCESS);

   return 0;
}

