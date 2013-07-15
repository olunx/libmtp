/**
 * \file mtpd.c
 *
 * Copyright (C) 2013 Philip Langdale <philipl@overt.org>
 * Copyright (C) 2013 Philipp Schmidt <philschmidt@gmx.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */


#include <unistd.h>
#include <signal.h>
#include <syslog.h>

#include "udev_listener.h"

void
signal_handler(int sig) {
    
    switch(sig) {
        case SIGHUP:
            // We can safely ignore this as we are a daemon
            break;
            
        case SIGTERM:
        case SIGQUIT:
            // Graceful shutdown
            
            MTPD_stop_udev_listener();
            
            
            break;
            
        default:
            // Don't react on any other signals
            break;
    }
}

int
main(int argc, char **argv) {
    
    // Set up signal handlers
    signal(SIGHUP, signal_handler);
    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler);
    signal(SIGQUIT, signal_handler);
    
    openlog( "LIBMTP_daemon",
             LOG_PID | LOG_CONS | LOG_NDELAY, LOG_DAEMON );
    
    syslog(LOG_INFO, "Daemoninsing");
    
    int daemon_err = daemon(0, 0);
    
    if (daemon_err < 0)
        syslog(LOG_ERR, "Error daemonising");
    
    /*
     * Set up the actual stuff. E.g. dbus service, udev service, init connected devices (if any)
     */
    
    pause();
    
    closelog();
    
    return 0;
}

