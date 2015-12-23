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

#define FUSE_USE_VERSION 29

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

#include "charybde_ops.h"

#include "server.hh"

static struct fuse_operations charybde_oper = {
	.getattr     = charybde_getattr,
    .readlink    = charybde_readlink,
    .mknod       = charybde_mknod,
    .mkdir       = charybde_mkdir,
    .unlink      = charybde_unlink,
    .rmdir       = charybde_rmdir,
    .symlink     = charybde_symlink,
    .rename      = charybde_rename,
    .link        = charybde_link,
    .chmod       = charybde_chmod,
    .chown       = charybde_chown,
    .truncate    = charybde_truncate,
	.open		 = charybde_open,
	.read		 = charybde_read,
    .write       = charybde_write,
    .statfs      = charybde_statfs,
    .flush       = charybde_flush,
    .release     = charybde_release,
    .fsync       = charybde_fsync,
    .setxattr    = charybde_setxattr,
    .getxattr    = charybde_getxattr,
    .listxattr   = charybde_listxattr,
    .removexattr = charybde_removexattr,
    .opendir     = charybde_opendir,
    .readdir     = charybde_readdir,
    .releasedir  = charybde_releasedir,
    .fsyncdir    = charybde_fsyncdir,

    .init        = charybde_init,
    .destroy     = charybde_destroy,

    .access      = charybde_access,
    .create      = charybde_create,
    .ftruncate   = charybde_ftruncate,
    .fgetattr    = charybde_fgetattr,
    .lock        = charybde_lock,
    .utimens     = charybde_utimens,
    .bmap        = charybde_bmap,
    .ioctl       = charybde_ioctl,
    .poll        = charybde_poll,
    .write_buf   = charybde_write_buf,
    .read_buf    = charybde_read_buf,
    .flock       = charybde_flock,
    .fallocate   = charybde_fallocate,
};

int main(int argc, char *argv[])
{
    printf("starting fuse filesystem\n");
	return fuse_main(argc, argv, &charybde_oper, NULL);
}
