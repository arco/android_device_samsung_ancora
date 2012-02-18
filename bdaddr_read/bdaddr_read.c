#include <fcntl.h>
#include <string.h>
#include <cutils/properties.h>
#include <cutils/log.h>

#define LOG_TAG "bdaddr"
#define SAMSUNG_BDADDR_PATH "/data/.bt.info"
#define BDADDR_PATH "/data/bdaddr"

/* Read bluetooth MAC from SAMSUNG_BDADDR_PATH (different format),
 * write it to BDADDR_PATH, and set ro.bt.bdaddr_path to BDADDR_PATH
 *
 * Adapted from bdaddr_read.c of thunderg
 */

int main() {
    char tmpbdaddr[23]; // bt_macaddr:xxxxxxxxxxxx
    char bdaddr[18];
    int count;
    int fd;

    fd = open(SAMSUNG_BDADDR_PATH, O_RDONLY);
    if(fd < 0) {
        fprintf(stderr, "open(%s) failed\n", SAMSUNG_BDADDR_PATH);
        LOGE("Can't open %s\n", SAMSUNG_BDADDR_PATH);
        return -1;
    }

    count = read(fd, tmpbdaddr, sizeof(tmpbdaddr));
    if (count < 0) {
        fprintf(stderr, "read(%s) failed\n", SAMSUNG_BDADDR_PATH);
        LOGE("Can't read %s\n", SAMSUNG_BDADDR_PATH);
        return -1;
    }
    else if (count != sizeof(tmpbdaddr)) {
        fprintf(stderr, "read(%s) unexpected size %d\n", SAMSUNG_BDADDR_PATH, count);
        LOGE("Error reading %s (unexpected size %d)\n", SAMSUNG_BDADDR_PATH, count);
        return -1;
    }

    count = sprintf(bdaddr, "%2.2s:%2.2s:%2.2s:%2.2s:%2.2s:%2.2s\0",
            tmpbdaddr+11,tmpbdaddr+13,tmpbdaddr+15,tmpbdaddr+17,tmpbdaddr+19,tmpbdaddr+21);

    fd = open(BDADDR_PATH, O_WRONLY|O_CREAT|O_TRUNC, 00600|00060|00006);
    if (fd < 0) {
        fprintf(stderr, "open(%s) failed\n", BDADDR_PATH);
        LOGE("Can't open %s\n", BDADDR_PATH);
        return -2;
    }
    write(fd, bdaddr, 18);
    close(fd);
    property_set("ro.bt.bdaddr_path", BDADDR_PATH);
    return 0;
}
