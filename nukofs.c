#include <linux/cred.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/backing-dev.h>
#include <linux/pagemap.h>

/* Pretend that each entry is of this size in directory's i_size */
#define BOGO_DIRENT_SIZE 20

static struct inode *nukofs_alloc_inode(struct super_block *sb);
static void nukofs_destroy_inode(struct inode *inode);
static void nukofs_put_super(struct super_block *sb);
static int nukofs_mknod(struct inode *dir, struct dentry *dentry,
			int mode, dev_t dev);
static int nukofs_create(struct inode *dir, struct dentry *dentry,
			 int mode, struct nameidata *nd);
static int nukofs_mkdir(struct inode *inode, struct dentry *dentry, int mode);

struct nukofs_inode_info {
	struct inode vfs_inode;
};

static struct super_operations nukofs_super_operations = {
	.statfs		= simple_statfs,
	.alloc_inode	= nukofs_alloc_inode,
	.destroy_inode	= nukofs_destroy_inode,
	.put_super	= nukofs_put_super,
};

static int simple_commit_write(struct file *file, struct page *page,
			       unsigned from, unsigned to)
{
	struct inode *inode = page->mapping->host;
	loff_t pos = ((loff_t)page->index << PAGE_CACHE_SHIFT) + to;

	if (!PageUptodate(page))
		SetPageUptodate(page);
	/*
	 * No need to use i_size_read() here, the i_size
	 * cannot change under us because we hold the i_mutex.
	 */
	if (pos > inode->i_size)
		i_size_write(inode, pos);
	set_page_dirty(page);
	return 0;
}

int simple_write_end(struct file *file, struct address_space *mapping,
			loff_t pos, unsigned len, unsigned copied,
			struct page *page, void *fsdata)
{
	unsigned from = pos & (PAGE_CACHE_SIZE - 1);

	/* zero the stale part of the page if we did a short copy */
	if (copied < len) {
		void *kaddr = kmap_atomic(page, KM_USER0);
		memset(kaddr + from + copied, 0, len - copied);
		flush_dcache_page(page);
		kunmap_atomic(kaddr, KM_USER0);
	}

	simple_commit_write(file, page, from, from+copied);

	unlock_page(page);
	page_cache_release(page);

	return copied;
}

static const struct address_space_operations nukofs_address_space_operations = {
	.readpage    = simple_readpage,
	.writepage   = NULL,
	.write_begin = simple_write_begin,
	.write_end   = simple_write_end,
};

static struct inode_operations nukofs_file_inode_operations = {
	.getattr = NULL,
	.setattr = NULL,
};

static const struct inode_operations nukofs_dir_inode_operations = {
	.create		= nukofs_create,
	.lookup		= simple_lookup,
	.link		= simple_link,
	.unlink		= simple_unlink,
	.symlink	= NULL,
	.mkdir		= nukofs_mkdir,
	.rmdir		= simple_rmdir,
	.mknod		= nukofs_mknod,
	.rename		= simple_rename,
};

static const struct file_operations nukofs_file_operations = {
	.read        = do_sync_read,
	.write       = do_sync_write,
	.aio_write   = generic_file_aio_write,
	.release     = NULL,
	.mmap        = NULL,
	.splice_read = NULL,
	.aio_read    = generic_file_aio_read,
	.fsync       = NULL,
	.llseek      = NULL,
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

/* taken from ramfs_get_inode */
struct inode *nukofs_get_inode(struct super_block *sb, int mode, dev_t dev)
{
	struct inode *inode = new_inode(sb);
	if (inode) {
		inode->i_mode = mode;
		inode->i_uid = current_fsuid();
		inode->i_gid = current_fsgid();
		inode->i_mapping->a_ops = &nukofs_address_space_operations;
		inode->i_mapping->backing_dev_info = &nukofs_backing_dev_info;
		inode->i_atime = inode->i_mtime = inode->i_ctime = CURRENT_TIME;
		switch (mode & S_IFMT) {
		default:
			/* TODO */
			break;
		case S_IFREG:
			inode->i_op = &nukofs_file_inode_operations;
			inode->i_fop = &nukofs_file_operations;
			break;
		case S_IFDIR:
			inode->i_op = &nukofs_dir_inode_operations;
			inode->i_fop = &simple_dir_operations;
			/*
			 * directory inodes start off with
			 * i_nlink == 2 (for "." entry)
			 */
			inc_nlink(inode);
			break;
		case S_IFLNK:
			/* TODO */
			break;
		}
	}
	return inode;
}

static struct inode *nukofs_alloc_inode(struct super_block *sb)
{
	struct nukofs_inode_info *p;

	p = (struct nukofs_inode_info *)
		kmem_cache_alloc(nukofs_inode_cachep, GFP_KERNEL);
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
	int err = -ENOMEM;

	sb->s_blocksize = 1024;
	sb->s_magic = 0xca10;
	sb->s_op = &nukofs_super_operations;

	/* init root inode */
	inode = nukofs_get_inode(sb, 0755|S_IFDIR, 0);
	if (!inode)
		goto failed;

	root = d_alloc_root(inode);
	if (!root)
		goto failed_iput;

	sb->s_root = root;

	return 0;

failed_iput:
	iput(inode);

failed:
	return err;
}

static int nukofs_get_sb(struct file_system_type *fs_type,
			 int flags, const char *dev_name,
			 void *data, struct vfsmount *mnt)
{
	return get_sb_nodev(fs_type, flags, data, nukofs_fill_super, mnt);
}

static int nukofs_mknod(struct inode *dir, struct dentry *dentry,
			int mode, dev_t dev)
{
	struct inode *inode = nukofs_get_inode(dir->i_sb, mode, dev);
	/*
	 * inodeが割り当てられない場合、RAMベースのファイルシステム的にはENOMEM
	 * なのかもしれないけど、ユーザランドから見ればファイルシステムの空きス
	 * ペースが無い状態に等しい?
	 *
	 * でも、open(2), mknod(2) で ENOMEM を返しうると man に書いてはある
	 */
	int error = -ENOSPC;

	if (inode) {
		d_instantiate(dentry, inode);
		dget(dentry);
		error = 0;
		inode->i_mtime = inode->i_ctime = CURRENT_TIME;
	}

	return 0;
}

static int nukofs_create(struct inode *dir, struct dentry *dentry,
			 int mode, struct nameidata *nd)
{
	return nukofs_mknod(dir, dentry, mode | S_IFREG, 0);
}

static int nukofs_mkdir(struct inode *dir, struct dentry *dentry, int mode)
{
	return nukofs_mknod(dir, dentry, mode | S_IFDIR, 0);
}

static struct file_system_type nukofs_fs_type = {
	.owner   = THIS_MODULE,
	.name    = "nukofs",
	.get_sb  = nukofs_get_sb,
	.kill_sb = kill_litter_super,
};

static int nukofs_init(void)
{
	int err = 0;

	err = bdi_init(&nukofs_backing_dev_info);
	if (err)
		return err;

	err = register_filesystem(&nukofs_fs_type);
	if (err) {
		bdi_destroy(&nukofs_backing_dev_info);
		return err;
	}

	nukofs_init_inode_cachep();
	return 0;
}

static void nukofs_exit(void)
{
	bdi_destroy(&nukofs_backing_dev_info);
	unregister_filesystem(&nukofs_fs_type);
	nukofs_destroy_inode_cachep();
}

module_init(nukofs_init);
module_exit(nukofs_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("hiroya ito");
MODULE_DESCRIPTION("A sample filesystem");
