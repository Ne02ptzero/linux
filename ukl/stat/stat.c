#include <sys/stat.h>
#include <linux/kernel.h>
#include <linux/stat.h>
#include <linux/fs.h>
#include <linux/cred.h>

#ifndef INIT_STRUCT_STAT_PADDING
#define INIT_STRUCT_STAT_PADDING(st) memset(&st, 0, sizeof(st))
#endif /* INIT_STRUCT_STAT_PADDING */

#if BITS_PER_LONG == 32
#define choose_32_64(a, b) a
#else
#define choose_32_64(a, b) b
#endif /* BITS_PER_LONG */

#define valid_dev(x) choose_32_64(old_valid_dev(x), true)
#define encode_dev(x) choose_32_64(old_encode_dev, new_encode_dev)(x)

static int ukl_cp_new_stat(struct kstat *stat, struct stat *statbuf)
{
	struct stat tmp;

	if (!valid_dev(stat->dev) || !valid_dev(stat->rdev))
		return -EOVERFLOW;

#if BITS_PER_LONG == 32
	if (stat->size > MAX_NON_LFS)
		return -EOVERFLOW;
#endif

	INIT_STRUCT_STAT_PADDING(tmp);
	tmp.st_dev = encode_dev(stat->dev);
	tmp.st_ino = stat->ino;

	if (sizeof(tmp.st_ino) < sizeof(stat->ino) && tmp.st_ino != stat->ino)
		return -EOVERFLOW;

	tmp.st_mode = stat->mode;
	tmp.st_nlink = stat->nlink;

	if (tmp.st_nlink != stat->nlink)
		return -EOVERFLOW;

	SET_UID(tmp.st_uid, from_kuid_munged(current_user_ns(), stat->uid));
	SET_GID(tmp.st_gid, from_kgid_munged(current_user_ns(), stat->gid));
	tmp.st_rdev = encode_dev(stat->rdev);
	tmp.st_size = stat->size;
	tmp.st_atime = stat->atime.tv_sec;
	tmp.st_mtime = stat->mtime.tv_sec;
	tmp.st_ctime = stat->ctime.tv_sec;

#ifdef STAT_HAVE_NSEC
	tmp.st_atime_nsec = stat->atime.tv_nsec;
	tmp.st_mtime_nsec = stat->mtime.tv_nsec;
	tmp.st_ctime_nsec = stat->ctime.tv_nsec;
#endif

	tmp.st_blocks = stat->blocks;
	tmp.st_blksize = stat->blksize;
	memcpy(statbuf, &tmp, sizeof(tmp));
	return 0;
}

int stat(const char *pathname, struct stat *statbuf)
{
	struct kstat stat;
	int error = vfs_stat(pathname, &stat);

	if (error)
		return error;

	return ukl_cp_new_stat(&stat, statbuf);
}

int fstat(int fd, struct stat *statbuf)
{
	struct kstat stat;
	int error = vfs_fstat(fd, &stat);

	if (error)
		return error;

	return ukl_cp_new_stat(&stat, statbuf);
}

int lstat(const char *pathname, struct stat *statbuf)
{
	struct kstat stat;
	int error = vfs_lstat(pathname, &stat);

	if (error)
		return error;

	return ukl_cp_new_stat(&stat, statbuf);
}

int newfstatat(int dfd, const char *pathname, struct stat *statbuf, int flag)
{
	struct kstat stat;
	int error = vfs_fstatat(dfd, pathname, &stat, flag);

	if (error)
		return error;

	return ukl_cp_new_stat(&stat, statbuf);
}
