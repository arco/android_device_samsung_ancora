#ifndef _GENLOCK_H_
#define _GENLOCK_H_

#define GENLOCK_UNLOCK 0
#define GENLOCK_WRLOCK 1
#define GENLOCK_RDLOCK 2

#define GENLOCK_NOBLOCK       (1 << 0)
#define GENLOCK_WRITE_TO_READ (1 << 1)

struct genlock_lock {
	int fd;
	int op;
	int flags;
	int timeout;
};

#define GENLOCK_IOC_MAGIC     'G'

#define GENLOCK_IOC_NEW _IO(GENLOCK_IOC_MAGIC, 0)
#define GENLOCK_IOC_EXPORT _IOR(GENLOCK_IOC_MAGIC, 1, \
	struct genlock_lock)
#define GENLOCK_IOC_ATTACH _IOW(GENLOCK_IOC_MAGIC, 2, \
	struct genlock_lock)

/* Deprecated */
#define GENLOCK_IOC_LOCK _IOW(GENLOCK_IOC_MAGIC, 3, \
	struct genlock_lock)

/* Deprecated */
#define GENLOCK_IOC_RELEASE _IO(GENLOCK_IOC_MAGIC, 4)
#define GENLOCK_IOC_WAIT _IOW(GENLOCK_IOC_MAGIC, 5, \
	struct genlock_lock)
#define GENLOCK_IOC_DREADLOCK _IOW(GENLOCK_IOC_MAGIC, 6, \
	struct genlock_lock)
#endif
