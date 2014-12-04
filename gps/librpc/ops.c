/* Copyright (c) 2010, Code Aurora Forum. */

#include <rpc/rpc.h>
#include <rpc/rpc_router_ioctl.h>
#include <debug.h>

#include <sys/types.h>   
#include <sys/stat.h>     
#include <fcntl.h>        
#include <unistd.h>      
#include <stdio.h>
#include <errno.h>

#define DUMP_DATA 0
/* Wait for server in seconds, 0 - no wait, -ve - infinite wait */
#define SERVER_WAIT_DURATION 15
#define POLL_INTERVAL_MS 500

int r_open(const char *router)
{
  char name[32];
  struct stat statbuf;
  int handle;
  struct timespec polling_timer;
  int poll_count;

  if (stat("/dev/oncrpc", &statbuf) == 0)
      snprintf(name, sizeof(name), "/dev/oncrpc/%s", router);
  else
      snprintf(name, sizeof(name), "/dev/%s", router);

  polling_timer.tv_sec = 0;
  polling_timer.tv_nsec = (POLL_INTERVAL_MS * 1000000);
  poll_count = SERVER_WAIT_DURATION * (1000 / POLL_INTERVAL_MS);

  while (stat(name, &statbuf) && poll_count) {
      nanosleep(&polling_timer, NULL);
      if (poll_count > 0)
          poll_count--;
  }

  handle = open(name, O_RDWR, 0);

  if(handle < 0)
      E("error opening %s: %s\n", name, strerror(errno));
  return handle;
}

void r_close(int handle)
{
    if(close(handle) < 0) E("error: %s\n", strerror(errno));
}

int r_read(int handle, char *buf, uint32 size)
{
	int rc = read((int) handle, (void *)buf, size);
	if (rc < 0)
		E("error reading RPC packet: %d (%s)\n", errno, strerror(errno));
#if DUMP_DATA
	else {
		int len = rc / 4;
		uint32_t *data = (uint32_t *)buf;
		fprintf(stdout, "RPC in  %02d:", rc);
		while (len--)
			fprintf(stdout, " %08x", *data++);
		fprintf(stdout, "\n");
	}
#endif
	return rc;
}

int r_write (int handle, const char *buf, uint32 size)
{
	int rc = write(handle, (void *)buf, size);
	if (rc < 0)
		E("error writing RPC packet: %d (%s)\n", errno, strerror(errno));
#if DUMP_DATA
	else {
		int len = rc / 4;
		uint32_t *data = (uint32_t *)buf;
		fprintf(stdout, "RPC out %02d:", rc);
		while (len--)
			fprintf(stdout, " %08x", *data++);
		fprintf(stdout, "\n");
	}
#endif
	return rc;
}

int r_control(int handle, const uint32 cmd, void *arg)
{
  return ioctl(handle, cmd, arg);
}



