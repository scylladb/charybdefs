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

#include "server.hh"

#include <iostream>

extern "C" {

#include <iostream>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/vfs.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/xattr.h>
#include <dirent.h>
#include <sys/file.h>
#include <sys/ioctl.h>

#include <string.h>

#include "charybde_ops.h"

int charybde_getattr(const char *path, struct stat *buf)
{
    static volatile int in_flight = 0;
    in_flight++;
    int ret = error_inject(in_flight, path, "getattr");
    if (ret) {
        in_flight--;
        return ret;
    }

    ret = stat(path, buf);
    if (ret < 0) {
        in_flight--;
        return -errno;
    }
    
    in_flight--;
    return 0;    
}

int charybde_readlink(const char *path, char *buf, size_t bufsiz)
{
    static volatile int in_flight = 0;
    in_flight++;
    int ret = error_inject(in_flight, path, "readlink");
    if (ret) {
        in_flight--;
        return ret;
    }

    ret = readlink(path, buf, bufsiz);
    if (ret < 0) {
        in_flight--;
        return -errno;
    }

    in_flight--;
    return 0;
}

int charybde_mknod(const char *path, mode_t mode, dev_t dev)
{
    static volatile int in_flight = 0;
    in_flight++;
    int ret = error_inject(in_flight, path, "mknod");
    if (ret) {
        in_flight--;
        return ret;
    }

    ret = mknod(path, mode, dev);    
    if (ret < 0) {
        in_flight--;
        return -errno;
    }

    in_flight--;
    return 0;
}

int charybde_mkdir(const char *path, mode_t mode)
{
    static volatile int in_flight = 0;
    in_flight++;
    int ret = error_inject(in_flight, path, "mkdir");
    if (ret) {
        in_flight--;
        return ret;
    }

    ret = mkdir(path, mode);
    if (ret < 0) {
        in_flight--;
        return -errno;
    }

    in_flight--;
    return 0;
}

int charybde_unlink(const char *path)
{
    static volatile int in_flight = 0;
    in_flight++;
    int ret = error_inject(in_flight, path, "unlink");
    if (ret) {
        in_flight--;
        return ret;
    }

    ret = unlink(path); 
    if (ret < 0) {
        in_flight--;
        return -errno;
    }

    in_flight--;
    return 0;
}

int charybde_rmdir(const char *path)
{
    static volatile int in_flight = 0;
    in_flight++;
    int ret = error_inject(in_flight, path, "rmdir");
    if (ret) {
        in_flight--;
        return ret;
    }

    ret = rmdir(path); 
    if (ret < 0) {
        in_flight--;
        return -errno;
    }

    in_flight--;
    return 0;
}

int charybde_symlink(const char *target, const char *linkpath)
{
    static volatile int in_flight = 0;
    in_flight++;
    int ret = error_inject(in_flight, target, "symlink");
    if (ret) {
        in_flight--;
        return ret;
    }

    ret = error_inject(in_flight, linkpath, "symlink");
    if (ret) {
        in_flight--;
        return ret;
    }

    ret = symlink(target, linkpath);
    if (ret < 0) {
        in_flight--;
        return -errno;
    }

    in_flight--;
    return 0;
}

int charybde_rename(const char *oldpath, const char *newpath)
{
    static volatile int in_flight = 0;
    in_flight++;
    int ret = error_inject(in_flight, oldpath, "rename");
    if (ret) {
        in_flight--;
        return ret;
    }

    ret = error_inject(in_flight, newpath, "rename");
    if (ret) {
        in_flight--;
        return ret;
    }

    ret = rename(oldpath, newpath);
    if (ret < 0) {
        in_flight--;
        return -errno;
    }

    in_flight--;
    return 0;
}

int charybde_link(const char *oldpath, const char *newpath)
{
    static volatile int in_flight = 0;
    in_flight++;
    int ret = error_inject(in_flight, oldpath, "link");
    if (ret) {
        in_flight--;
        return ret;
    }

    ret = error_inject(in_flight, newpath, "link");
    if (ret) {
        in_flight--;
        return ret;
    }
    
    ret = link(oldpath, newpath);
    if (ret < 0) {
        in_flight--;
        return -errno;
    }

    in_flight--;
    return 0;
}

int charybde_chmod(const char *path, mode_t mode)
{
    static volatile int in_flight = 0;
    in_flight++;
    int ret = error_inject(in_flight, path, "chmod");
    if (ret) {
        in_flight--;
        return ret;
    }
    
    ret = chmod(path, mode);
    if (ret < 0) {
        in_flight--;
        return -errno;
    }

    in_flight--;
    return 0;
}

int charybde_chown(const char *path, uid_t owner, gid_t group)
{
    static volatile int in_flight = 0;
    in_flight++;
    int ret = error_inject(in_flight, path, "chown");
    if (ret) {
        in_flight--;
        return ret;
    }

    ret = chown(path, owner, group);
    if (ret < 0) {
        in_flight--;
        return -errno;
    }

    in_flight--;
    return 0;
}

int charybde_truncate(const char *path, off_t length)
{
    static volatile int in_flight = 0;
    in_flight++;
    int ret = error_inject(in_flight, path, "truncate");
    if (ret) {
        in_flight--;
        return ret;
    }

    ret = truncate(path, length); 
    if (ret < 0) {
        in_flight--;
        return -errno;
    }

    in_flight--;
    return 0;
}

int charybde_open(const char *path, struct fuse_file_info *fi)
{
    static volatile int in_flight = 0;
    in_flight++;
    int ret = error_inject(in_flight, path, "open");
    if (ret) {
        in_flight--;
        return ret;
    }

    ret = open(path, fi->flags);
    if (ret < 0) {
        in_flight--;
        return -errno;
    }

    fi->fh = ret;
    in_flight--;
    return 0;
}

int charybde_read(const char *path, char *buf, size_t size, off_t offset,
                 struct fuse_file_info *fi)
{
    static volatile int in_flight = 0;
    in_flight++;
    int ret = error_inject(in_flight, path, "read");
    if (ret) {
        in_flight--;
        return ret;
    }

    ret = pread(fi->fh, buf, size, offset);
    if (ret == -1) {
        ret = -errno;
    }

    in_flight--;
    return ret;
}

int charybde_write(const char *path, const char *buf, size_t size, off_t offset,
                  struct fuse_file_info *fi)
{
    static volatile int in_flight = 0;
    in_flight++;
    int ret = error_inject(in_flight, path, "write", const_cast<char*>(buf), size);
    if (ret) {
        in_flight--;
        return ret;
    }

    std::cout << "WRITE" << std::endl;

    ret = pwrite(fi->fh, buf, size, offset);
    if (ret == -1) {
        ret = -errno;
    }

    in_flight--;
    return ret;
}

int charybde_statfs(const char *path, struct statvfs *buf)
{
    static volatile int in_flight = 0;
    in_flight++;
    int ret = error_inject(in_flight, path, "statfs");
    if (ret) {
        in_flight--;
        return ret;
    }

    ret = statvfs(path, buf);
    if (ret < 0) {
        in_flight--;
        return -errno;
    }

    in_flight--;
    return 0;
}

int charybde_flush(const char *path, struct fuse_file_info *fi)
{
    static volatile int in_flight = 0;
    in_flight++;
    int ret = error_inject(in_flight, path, "flush");
    if (ret) {
        in_flight--;
        return ret;
    }

    /* Took from fuse examples */
    ret = close(dup(fi->fh));
    if (ret < 0) {
        in_flight--;
        return -errno;
    }

    in_flight--;
    return 0;
}

int charybde_release(const char *path, struct fuse_file_info *fi)
{
    static volatile int in_flight = 0;
    in_flight++;
    int ret = error_inject(in_flight, path, "release");
    if (ret) {
        in_flight--;
        return ret;
    }

    close(fi->fh);

    in_flight--;
    return 0;    
}

int charybde_fsync(const char *path, int datasync, struct fuse_file_info *fi)
{
    static volatile int in_flight = 0;
    in_flight++;
    int ret = error_inject(in_flight, path, "fsync");
    if (ret) {
        in_flight--;
        return ret;
    }

    if (datasync) {
        ret = fdatasync(fi->fh);
        if (ret < 0) {
            in_flight--;
            return -errno;
        }
    } else {
        ret = fsync(fi->fh);
        if (ret < 0) {
            in_flight--;
            return -errno;
        }
    }

    in_flight--;
    return 0;
}

int charybde_setxattr(const char *path, const char *name,
                     const char *value, size_t size, int flags)
{
    static volatile int in_flight = 0;
    in_flight++;
    int ret = error_inject(in_flight, path, "setxattr");
    if (ret) {
        in_flight--;
        return ret;
    }

    ret = setxattr(path, name, value, size, flags);
    if (ret < 0) {
        in_flight--;
        return -errno;
    }

    in_flight--;
    return 0;
}

int charybde_getxattr(const char *path, const char *name,
                     char *value, size_t size)
{
    static volatile int in_flight = 0;
    in_flight++;
    int ret = error_inject(in_flight, path, "getxattr");
    if (ret) {
        in_flight--;
        return ret;
    }

    ret = getxattr(path, name, value, size);
    if (ret < 0) {
        in_flight--;
        return -errno;
    }
    
    in_flight--;
    return ret;
}

int charybde_listxattr(const char *path, char *list,
                      size_t size)
{
    static volatile int in_flight = 0;
    in_flight++;
    int ret = error_inject(in_flight, path, "listxattr");
    if (ret) {
        in_flight--;
        return ret;
    }

    ret = listxattr(path, list, size);
    if (ret < 0) {
        in_flight--;
        return -errno;
    }
    
    in_flight--;
    return ret;
}

int charybde_removexattr(const char *path, const char *name)
{
    static volatile int in_flight = 0;
    in_flight++;
    int ret = error_inject(in_flight, path, "removexattr");
    if (ret) {
        in_flight--;
        return ret;
    }


    ret = removexattr(path, name);
    if (ret < 0) {
        in_flight--;
        return -errno;
    }
    
    in_flight--;
    return 0;    
}

int charybde_opendir(const char *path, struct fuse_file_info *fi)
{
    static volatile int in_flight = 0;
    in_flight++;
    int ret = error_inject(in_flight, path, "opendir");
    if (ret) {
        in_flight--;
        return ret;
    }

    auto dir = opendir(path);

    if (!dir) {
        in_flight--;
        return -errno;
    }
    
    fi->fh = (int64_t) dir;
    in_flight--;
    return 0;    
}

int charybde_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                    off_t offset, struct fuse_file_info *fi)
{
    static volatile int in_flight = 0;
    in_flight++;
    int ret = error_inject(in_flight, path, "readdir");
    if (ret) {
        in_flight--;
        return ret;
    }

    DIR *dp = (DIR *) fi->fh;
    struct dirent *de;

    while ((de = readdir(dp)) != NULL) {
        struct stat st;
        memset(&st, 0, sizeof(st));
        st.st_ino = de->d_ino;
        st.st_mode = de->d_type << 12;
        if (filler(buf, de->d_name, &st, 0))
            break;
    }
    
    in_flight--;
    return 0;    
}


int charybde_releasedir(const char *path, struct fuse_file_info *fi)
{
    static volatile int in_flight = 0;
    in_flight++;
    int ret = error_inject(in_flight, path, "releasedir");
    if (ret) {
        in_flight--;
        return ret;
    }

    DIR *dir = (DIR *) fi->fh;

    ret = closedir(dir);
    if (ret < 0) {
        in_flight--;
        return -errno;
    }
    
    in_flight--;
    return 0;    
}

int charybde_fsyncdir(const char *path, int datasync, struct fuse_file_info *fi)
{
    static volatile int in_flight = 0;
    in_flight++;
    int ret = error_inject(in_flight, path, "fsyncdir");
    if (ret) {
        in_flight--;
        return ret;
    }

    auto dir = opendir(path);
    if (!dir) {
        in_flight--;
        return -errno;
    }

    if (datasync) {
        ret = fdatasync(dirfd(dir));
        if (ret < 0) {
            in_flight--;
            return -errno;
        }
    } else {
        ret = fsync(dirfd(dir));
        if (ret < 0) {
            in_flight--;
            return -errno;
        }
    }

    closedir(dir);

    in_flight--;
    return 0;
}

void *charybde_init(struct fuse_conn_info *conn)
{
   start_server_thread();
   return NULL;
}

void charybde_destroy(void *)
{
    
}

int charybde_access(const char *path, int mode)
{
    static volatile int in_flight = 0;
    in_flight++;
    int ret = error_inject(in_flight, path, "access");
    if (ret) {
        in_flight--;
        return ret;
    }

    ret = access(path, mode); 
    if (ret < 0) {
        in_flight--;
        return -errno;
    }
    
    in_flight--;
    return 0;    
}

int charybde_create(const char *path, mode_t mode,
                   struct fuse_file_info *fi)
{
    static volatile int in_flight = 0;
    in_flight++;
    int ret = error_inject(in_flight, path, "create");
    if (ret) {
        in_flight--;
        return ret;
    }

    ret = creat(path, mode);
    if (ret < 0) {
        in_flight--;
        return -errno;
    }

    fi->fh = ret;

    in_flight--;
    return 0;    
}

int charybde_ftruncate(const char *path, off_t length,
                      struct fuse_file_info *fi)
{
    static volatile int in_flight = 0;
    in_flight++;
    int ret = error_inject(in_flight, path, "ftruncate");
    if (ret) {
        in_flight--;
        return ret;
    }

    ret = truncate(path, length);
    if (ret < 0) {
        in_flight--;
        return -errno;
    }
    
    in_flight--;
    return 0;    
}

int charybde_fgetattr(const char *path, struct stat *buf, struct fuse_file_info *fi)
{
    static volatile int in_flight = 0;
    in_flight++;
    int ret = error_inject(in_flight, path, "fgetattr");
    if (ret) {
        in_flight--;
        return ret;
    }

    ret = fstat((int) fi->fh, buf);
    if (ret < 0) {
        in_flight--;
        return -errno;
    }
    
    in_flight--;
    return 0;    
}

int charybde_lock(const char *path, struct fuse_file_info *, int cmd,
                 struct flock *)
{
    static volatile int in_flight = 0;
    in_flight++;
    int ret = error_inject(in_flight, path, "lock");
    if (ret) {
        in_flight--;
        return ret;
    }

    std::cout << "charybde_lock: unimplemented." << std::endl;
    
    in_flight--;
    return 0;    
}

int charybde_utimens(const char *path, const struct timespec tv[2])
{
    static volatile int in_flight = 0;
    in_flight++;
    int ret = error_inject(in_flight, path, "bmap");
    if (ret) {
        in_flight--;
        return ret;
    }
    
    std::cout << "charybde_utimens: unimplemented." << std::endl;

    in_flight--;
    return 0;    
}

int charybde_bmap(const char *path, size_t blocksize, uint64_t *idx)
{
    static volatile int in_flight = 0;
    in_flight++;
    int ret = error_inject(in_flight, path, "bmap");
    if (ret) {
        in_flight--;
        return ret;
    }

    std::cout << "charybde_bmap: unimplemented." << std::endl;
    
    in_flight--;
    return 0;    
}

int charybde_ioctl(const char *path, int cmd, void *arg,
                  struct fuse_file_info *fi,
                  unsigned int flags, void *data)
{
    static volatile int in_flight = 0;
    in_flight++;
    int ret = error_inject(in_flight, path, "ioctl");
    if (ret) {
        in_flight--;
        return ret;
    }

    ret = ioctl(fi->fh, cmd, arg);
    if (ret < 0) {
        in_flight--;
        return -errno;
    }
    
    in_flight--;
    return 0;    
}

int charybde_poll(const char *path, struct fuse_file_info *fi,
         struct fuse_pollhandle *ph, unsigned *reventsp)
{
    static volatile int in_flight = 0;
    in_flight++;
    int ret = error_inject(in_flight, path, "poll");
    if (ret) {
        in_flight--;
        return ret;
    }

    std::cout << "charybde_poll: unimplemented." << std::endl;
    
    in_flight--;
    return 0;    
}

int charybde_write_buf(const char *path, struct fuse_bufvec *buf, off_t off,
                      struct fuse_file_info *fi)
{
    static volatile int in_flight = 0;
    in_flight++;

    int ret = error_inject(in_flight, path, "write_buf", buf->buf[0].mem, buf->buf[0].size);
    if (ret) {
        in_flight--;
        return ret;
    }

    /* Took from fuse examples */
    struct fuse_bufvec dst = FUSE_BUFVEC_INIT(fuse_buf_size(buf));

    dst.buf[0].flags = (fuse_buf_flags) (FUSE_BUF_IS_FD | FUSE_BUF_FD_SEEK);
    dst.buf[0].fd = fi->fh;
    dst.buf[0].pos = off;

    ret = fuse_buf_copy(&dst, buf, FUSE_BUF_SPLICE_NONBLOCK);

    in_flight--;
    return ret;
}

int charybde_read_buf(const char *path, struct fuse_bufvec **bufp,
                     size_t size, off_t off,
                     struct fuse_file_info *fi)
{
    static volatile int in_flight = 0;
    in_flight++;
    int ret = error_inject(in_flight, path, "read_buf");
    if (ret) {
        in_flight--;
        return ret;
    }

    /* Took from fuse examples */
    struct fuse_bufvec *src;
    
    src = (fuse_bufvec *) malloc(sizeof(struct fuse_bufvec));
    if (!src) {
        in_flight--;
        return -ENOMEM;
    }
    
    *src = FUSE_BUFVEC_INIT(size);
    
    src->buf[0].flags = (fuse_buf_flags) (FUSE_BUF_IS_FD | FUSE_BUF_FD_SEEK);
    src->buf[0].fd = fi->fh;
    src->buf[0].pos = off;
    
    *bufp = src;
    
    in_flight--;
    return 0;
}

int charybde_flock(const char *path, struct fuse_file_info *fi, int op)
{
    static volatile int in_flight = 0;
    in_flight++;
    int ret = error_inject(in_flight, path, "flock");
    if (ret) {
        in_flight--;
        return ret;
    }

    ret = flock(((int) fi->fh), op);
    if (ret < 0) {
        in_flight--;
        return -errno;
    }
    
    in_flight--;
    return 0;    
}

int charybde_fallocate(const char *path, int mode,
                      off_t offset, off_t len,
                      struct fuse_file_info *fi)
{
    static volatile int in_flight = 0;
    in_flight++;
    int ret = error_inject(in_flight, path, "fallocate");
    if (ret) {
        in_flight--;
        return ret;
    }

    ret = fallocate((int) fi->fh, mode, offset, len);
    if (ret < 0) {
        in_flight--;
        return -errno;
    }
    
    in_flight--;
    return 0;    
}

};
