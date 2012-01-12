/*
 * Copyright (C) 2011 The CyanogenMod Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/mount.h>

/* Set serialnumber properties on ancora device, since it's
 * not exported by the bootloader to cmdline as expected */


/* Ugly hack is ugly...
 *
 * There's a very narrow window in which the hardware properties
 * can be written, since init sets them immediately after launching
 * the property service, and they can only be set once.
 * So our only chance to overwrite them is to set them during that
 * launch, by inserting them into default.prop */

int main() {
	FILE *fp;
	char buf[256];
	char serial[32];

	if ((fp = fopen("/proc/cpuinfo", "r")) == NULL)
		return 0;

	while(fgets(buf, 256, fp) != NULL) {
		if (strstr(buf, "Serial") != NULL) {
			strtok(buf, ":");
			strncpy(serial, strtok(NULL, " "), 32);
		}
	}
	fclose(fp);

	if (serial == NULL)
		return 0;

	mount("rootfs", "/", "rootfs", MS_REMOUNT|0, NULL);

	if ((fp = fopen("/default.prop", "a")) == NULL)
		return 0;
	sprintf(buf, "ro.serialno=%s", serial);
	fputs(buf, fp);
	fclose(fp);

	//mount("rootfs","/","rootfs",MS_REMOUNT|MS_RDONLY,NULL);

	return 0;
}
