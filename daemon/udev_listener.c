/**
 * \file udev_listener.c
 * \short Implementation of the udev listener
 * 
 * Implementation of the thread listening to udev for new or removed 
 * devices. Uses callbacks to inform the daemon about them if they are 
 * indeed MTP devices.
 *
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

#include "udev_listener.h"

#include <pthread.h>
#include <fcntl.h>
#include <libmtp.h>
#include <libudev.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <glib-2.0/glib.h>
#include <syslog.h>

typedef struct 
{
    int busnum;
    int devnum;
} device_ident_t;

typedef struct
{
    MTPD_device_added_t device_added_callback;
    MTPD_device_removed_t device_removed_callback;
} callbacks_t;

pthread_t thread;

void MTPD_run_udev_listener(void *arg)
{
    callbacks_t* callbacks = (callbacks_t*)arg;

    struct udev *udev;
    struct udev_device *dev;
    struct udev_monitor *mon;

    GArray *ident_list;

    udev = udev_new();


    if (!udev)
    {
        syslog(LOG_ERR, "Can't create udev");
        return;
    }

    // Get monitor and set subsystem to usb and device type to usb devices
    mon = udev_monitor_new_from_netlink(udev, "udev");
    udev_monitor_filter_add_match_subsystem_devtype(mon, "usb", "usb_device");
    udev_monitor_enable_receiving(mon);

    // Get fildescriptor and set it to blocking mode (default is non blocking)
    int fd = udev_monitor_get_fd(mon);
    int flags = fcntl(fd, F_GETFL);
    fcntl(fd, F_SETFL, flags & ~O_NONBLOCK);

    ident_list = g_array_new (FALSE, FALSE, sizeof (device_ident_t));

    //While the thread runs, wait for incoming udev-events
    while (1)
    {
        fd_set fds;
        int ret;

        FD_ZERO(&fds);
        FD_SET(fd, &fds);

        // Use select so we have a canncellation point
        ret = select(fd+1, &fds, NULL, NULL, NULL);

        if (ret > 0 && FD_ISSET(fd, &fds))
        {
            dev = udev_monitor_receive_device(mon);

            const char* action = udev_device_get_action(dev);
            
            device_ident_t ident;
            ident.busnum = atoi(udev_device_get_property_value(dev, "BUSNUM"));
            pthread_t thread;
            ident.devnum = atoi(udev_device_get_property_value(dev, "DEVNUM"));

            if (strcmp("add", action) == 0)
            {
                int isMtpDevice = LIBMTP_Check_Specific_Device(ident.busnum, ident.devnum);
                if (isMtpDevice == 1)
                {
                    // Add to internal device list
                    g_array_append_vals(ident_list, &ident, sizeof(device_ident_t*));

                    callbacks->device_added_callback( ident.busnum, ident.devnum );
                }
            }
            if (strcmp("remove", action) == 0)
            {
                int device_index = -1, i;
                for ( i = 0; i < ident_list->len; i++ )
                {
                    device_ident_t current = g_array_index( ident_list, device_ident_t, i );

                    if (current.busnum == ident.busnum && current.devnum == ident.devnum)
                    {
                        device_index = i;
                        break;
                    }
                }

                if (device_index >= 0)
                {
                    g_array_remove_index(ident_list, device_index);
                    callbacks->device_removed_callback(ident.busnum, ident.devnum);
                }
            }

            udev_device_unref(dev);
        }
    }

    g_array_free( ident_list, false );
}

void MTPD_start_udev_listener(const MTPD_device_added_t device_added_callback, const MTPD_device_removed_t device_removed_callback)
{
    callbacks_t *callbacks = malloc(sizeof(callbacks_t));
    callbacks->device_added_callback = device_added_callback;
    callbacks->device_removed_callback = device_removed_callback;

    pthread_create(&thread, NULL, MTPD_run_udev_listener, callbacks);

}

void MTPD_stop_udev_listener()
{
    pthread_cancel(&thread);
}

