/**
 * \file udev_listener.h
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

#ifndef UDEV_LISTENER_H
#define UDEV_LISTENER_H

#include <stdint.h>

/**
 * The callback type definition for the function that is called when a device is added.
 */
typedef int (* MTPD_device_added_t) (uint32_t const busid, uint32_t const devid);

/**
 * The callback type definition for the function that is called when a device was removed.
 */
typedef int (* MTPD_device_removed_t) (uint32_t const busid, uint32_t const devid);

void
MTPD_start_udev_listener( MTPD_device_added_t const device_added_callback, MTPD_device_removed_t const device_removed_callback);

void
MTPD_stop_udev_listener();

#endif //UDEV_LISTENER_H
