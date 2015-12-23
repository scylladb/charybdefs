/*
 * ** 27-12-2015
 * **
 * ** The author disclaims copyright to this source code.  In place of
 * ** a legal notice, here is a blessing:
 * **
 * **    May you do good and not evil.
 * **    May you find forgiveness for yourself and forgive others.
 * **    May you share freely, never taking more than you give.
 * **
 */

#ifndef CHARYBDE_OPS_HH
#define CHARYBDE_OPS_HH

#include <stdint.h>
#include <sys/types.h>

#include <fuse.h>

int charybde_getattr(const char *, struct stat *);
int charybde_readlink(const char *, char *, size_t);
int charybde_mknod(const char *, mode_t, dev_t);
int charybde_mkdir(const char *, mode_t);
int charybde_unlink(const char *);
int charybde_rmdir(const char *);
int charybde_symlink(const char *, const char *);
int charybde_rename(const char *, const char *);
int charybde_link(const char *, const char *);
int charybde_chmod(const char *, mode_t);
int charybde_chown(const char *, uid_t, gid_t);
int charybde_truncate(const char *, off_t);
int charybde_open(const char *, struct fuse_file_info *);
int charybde_read(const char *, char *, size_t, off_t,
                 struct fuse_file_info *);
int charybde_write(const char *, const char *, size_t, off_t,
                  struct fuse_file_info *);
int charybde_statfs(const char *, struct statvfs *);
int charybde_flush(const char *, struct fuse_file_info *);
int charybde_release(const char *, struct fuse_file_info *);
int charybde_fsync(const char *, int, struct fuse_file_info *);
int charybde_setxattr(const char *, const char *, const char *, size_t, int);
int charybde_getxattr(const char *, const char *, char *, size_t);
int charybde_listxattr(const char *, char *, size_t);
int charybde_removexattr(const char *, const char *);
int charybde_opendir(const char *, struct fuse_file_info *);
int charybde_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                    off_t offset, struct fuse_file_info *fi);
int charybde_releasedir(const char *, struct fuse_file_info *);
int charybde_fsyncdir(const char *, int, struct fuse_file_info *);

void *charybde_init(struct fuse_conn_info *conn);
void charybde_destroy(void *);

int charybde_access(const char *, int);
int charybde_create(const char *, mode_t, struct fuse_file_info *);
int charybde_ftruncate(const char *, off_t, struct fuse_file_info *);
int charybde_fgetattr(const char *, struct stat *, struct fuse_file_info *);
int charybde_lock(const char *, struct fuse_file_info *, int cmd,
                 struct flock *);
int charybde_utimens(const char *, const struct timespec tv[2]);
int charybde_bmap(const char *, size_t blocksize, uint64_t *idx);
int charybde_ioctl(const char *, int cmd, void *arg,
                  struct fuse_file_info *, unsigned int flags, void *data);
int charybde_poll(const char *path, struct fuse_file_info *fi,
                 struct fuse_pollhandle *ph, unsigned *reventsp);
int charybde_write_buf(const char *, struct fuse_bufvec *buf, off_t off,
                      struct fuse_file_info *);
int charybde_read_buf(const char *, struct fuse_bufvec **bufp,
                     size_t size, off_t off, struct fuse_file_info *);
int charybde_flock(const char *, struct fuse_file_info *, int op);
int charybde_fallocate(const char *, int, off_t, off_t,
                      struct fuse_file_info *);

#endif
