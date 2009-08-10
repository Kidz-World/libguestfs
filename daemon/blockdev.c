/* libguestfs - the guestfsd daemon
 * Copyright (C) 2009 Red Hat Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <unistd.h>

#include "daemon.h"
#include "actions.h"

/* These functions are all about using the blockdev command, so
 * we centralize it in one call.
 */
static int64_t
call_blockdev (char *device, char *switc, int extraarg, int prints)
{
  int r;
  int64_t rv;
  char *out, *err;
  const char *argv[] = {
    "/sbin/blockdev",
    switc,
    NULL,
    NULL,
    NULL
  };
  char buf[64];

  RESOLVE_DEVICE (device, return -1);

  if (extraarg > 0) {
    snprintf (buf, sizeof buf, "%d", extraarg);
    argv[2] = buf;
    argv[3] = device;
  } else
    argv[2] = device;

  r = commandv (&out, &err, argv);

  if (r == -1) {
    reply_with_error ("%s: %s", argv[0], err);
    free (err);
    free (out);
    return -1;
  }

  rv = 0;

  if (prints) {
    if (sscanf (out, "%" SCNi64, &rv) != 1) {
      reply_with_error ("%s: expected output, but got nothing", argv[0]);
      free (out);
      return -1;
    }
  }

  free (out);

  return rv;
}

int
do_blockdev_setro (char *device)
{
  return (int) call_blockdev (device, "--setro", 0, 0);
}

int
do_blockdev_setrw (char *device)
{
  return (int) call_blockdev (device, "--setrw", 0, 0);
}

int
do_blockdev_getro (char *device)
{
  return (int) call_blockdev (device, "--getro", 0, 1);
}

int
do_blockdev_getss (char *device)
{
  return (int) call_blockdev (device, "--getss", 0, 1);
}

int
do_blockdev_getbsz (char *device)
{
  return (int) call_blockdev (device, "--getbsz", 0, 1);
}

int
do_blockdev_setbsz (char *device, int blocksize)
{
  if (blocksize <= 0 /* || blocksize >= what? */) {
    reply_with_error ("blockdev_setbsz: blocksize must be > 0");
    return -1;
  }
  return (int) call_blockdev (device, "--setbsz", blocksize, 0);
}

int64_t
do_blockdev_getsz (char *device)
{
  return call_blockdev (device, "--getsz", 0, 1);
}

int64_t
do_blockdev_getsize64 (char *device)
{
  return call_blockdev (device, "--getsize64", 0, 1);
}

int
do_blockdev_flushbufs (char *device)
{
  return call_blockdev (device, "--flushbufs", 0, 0);
}

int
do_blockdev_rereadpt (char *device)
{
  return call_blockdev (device, "--rereadpt", 0, 0);
}
