#include <linux/cred.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/backing-dev.h>

/* Pretend that each entry is of this size in directory's i_size */
#define BOGO_DIRENT_SIZE 20

static struct inode* nukofs_alloc_inode(struct super_block *sb);
static void nukofs_destroy_inode(struct inode *inode);
static void nukofs_put_super(struct super_block *sb);

struct nukofs_inode_info {
    struct inode vfs_inode;
};

static struct super_operations nukofs_super_operations = {
	.alloc_inode	= nukofs_alloc_inode,
	.destroy_inode	= nukofs_destroy_inode,
	.put_super	= nukofs_put_super,
};

static struct address_space_operations nukofs_address_space_operations = {
    .readpage      = NULL,
    .writepage     = NULL,
    .write_begin   = NULL,
    .write_end     = NULL,
};

static struct inode_operations nukofs_inode_operations = {
    .getattr = NULL,
    .setattr = NULL,
};

static const struct inode_operations nukofs_dir_inode_operations = {
	.create		= NULL,
	.lookup		= simple_lookup,
	.link		= NULL,
	.unlink		= NULL,
	.symlink	= NULL,
	.mkdir		= NULL,
	.rmdir		= NULL,
	.mknod		= NULL,
	.rename		= NULL,
};

static struct file_operations nukofs_file_operations = {
    .read        = NULL,
    .open        = NULL,
    .write       = NULL,
    .release     = NULL,
    .mmap        = NULL,
    .splice_read = generic_file_splice_read,
    .aio_read    = generic_file_aio_read,
    .aio_write   = generic_file_aio_write,
    .fsync       = simple_sync_file,
    .llseek      = generic_file_llseek,
};

/* static const struct vm_operations_struct nukofs_vm_operations = { */
/* }; */

static struct backing_dev_info nukofs_backing_dev_info  __read_mostly = {
	.ra_pages	= 0,	/* No readahead */
	.capabilities	= BDI_CAP_NO_ACCT_AND_WRITEBACK | BDI_CAP_SWAP_BACKED,
	.unplug_io_fn	= default_unplug_io_fn,
};

static inline struct nukofs_inode_info *NUKOFS_INODE_INFO(struct inode *inode)
{
	return container_of(inode, struct nukofs_inode_info, vfs_inode);
}

static struct kmem_cache *nukofs_inode_cachep;

static struct inode* nukofs_alloc_inode(struct super_block *sb)
{
    struct nukofs_inode_info *p;
    p = (struct nukofs_inode_info *)kmem_cache_alloc(nukofs_inode_cachep, GFP_KERNEL);
    if (!p)
        return NULL;
    return &p->vfs_inode;
}

static void nukofs_destroy_inode(struct inode *inode)
{
    kmem_cache_free(nukofs_inode_cachep, NUKOFS_INODE_INFO(inode));
}

static void nukofs_init_once(void *foo)
{
    struct nukofs_inode_info *p = (struct nukofs_inode_info *)foo;

    inode_init_once(&p->vfs_inode);
}

static void nukofs_init_inode_cachep(void)
{
    nukofs_inode_cachep = kmem_cache_create("nukofs_inode_cache",
                      sizeof(struct nukofs_inode_info),
                      0, SLAB_PANIC, nukofs_init_once);
}

static void nukofs_destroy_inode_cachep(void)
{
    kmem_cache_destroy(nukofs_inode_cachep);
}

static void nukofs_put_super(struct super_block *sb)
{
}

static int nukofs_fill_super(struct super_block *sb, void *data, int silent)
{
    struct inode *inode;
    struct dentry *root;
    int err = -ENOSYS;

    sb->s_blocksize = 1024;
    sb->s_magic = 0xca10;
    sb->s_op = &nukofs_super_operations;

    /* init root inode */
    inode = iget_locked(sb, 0);
    if (!inode) {
        printk(KERN_INFO "ilookup failed");
        goto failed;
    }

    inode->i_mode = 0777;
    inode->i_mode |= S_IFDIR;
    inode->i_uid = 0;
    inode->i_gid = 0;
    inode->i_nlink = 1;
    inode->i_op = &nukofs_dir_inode_operations;
    inode->i_fop = &nukofs_file_operations;
    inode->i_mapping->backing_dev_info = &nukofs_backing_dev_info;
    inode->i_atime = inode->i_mtime = inode->i_ctime = CURRENT_TIME;
    inode->i_generation = get_seconds();
    root = d_alloc_root(inode);
    if(!root) {
        printk(KERN_INFO "d_alloc_root failed");
        goto failed_iput;
    }
    sb->s_root = root;

    return 0;

failed_iput:
    iput(inode);

failed:
    return err;
}

static int nukofs_get_sb(struct file_system_type *fs_type,
	int flags, const char *dev_name, void *data, struct vfsmount *mnt)
{
	return get_sb_nodev(fs_type, flags, data, nukofs_fill_super, mnt);
}

static struct file_system_type nukofs_fs_type = {
	.owner = THIS_MODULE,
	.name  = "nukofs",
	.get_sb = nukofs_get_sb,
	.kill_sb = kill_anon_super,
};

static int nukofs_init(void)
{
    int err = 0;

	printk(KERN_INFO "Hello world 1.\n");

    err = bdi_init(&nukofs_backing_dev_info);
    if (err)
        return err;

    nukofs_init_inode_cachep();
    err = register_filesystem(&nukofs_fs_type);
    if (err) {
        return err;
    }
    
	return 0;
}

static void nukofs_exit(void)
{
	bdi_destroy(&nukofs_backing_dev_info);
    nukofs_destroy_inode_cachep();
    unregister_filesystem(&nukofs_fs_type);
	printk(KERN_INFO "Goodbye world 1.\n");
}

module_init(nukofs_init);
module_exit(nukofs_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("hiroya ito");
MODULE_DESCRIPTION("A sample filesystem");
