#include "header.h"

int mdir(void)
{
    int i = 0;
    MINODE *parent;
    if(name[0] == NULL)
    {    
        printf("Usage: mkdir requires pathname\n");
        return -1;
    }
    // Get last value for name of actual new dir
    while(name[i])
    {
        i++;
    }
    i = i - 1;
    char bname[64];
    // Copy found name to bname
    strcpy(bname, name[i]);
    // Set name[i] to NULL so it doesn't cause 
    // errors later when path is searched for location
    name[i] = NULL;
    // check if trying to make new root, don't allow
    if(!strcmp(bname, "/"))
    {
        printf("mkdir: cannot create directory '/': File exists\n");
        return -1;
    }
    // find the value of parent minode.
    if(name[0] == NULL)
    {
        parent = findval(running->cwd);
    }
    else if(!strcmp(name[0], "/"))
    {
        parent = findval(root);
    }
    else
    {
        parent = findval(running->cwd);
    }
    // ensure parent is a directory
    if(checktype(parent))
    {
        // search parents inode for bname
        // to ensure parent doesn't already hold
        // file by this name
        INODE *pip = &parent->INODE;
        if(search(pip, bname) == -1)
        {
            // run smkdir on parent and bname, 
            // increment counts of parent by one
            // since new file added and put to disk
            // and return.
            int ret = smkdir(parent, bname);
            pip->i_links_count++;
            parent->dirty = 1;
            iput(parent);
            return ret;
        }
        printf("mkdir: cannot create directory '%s': File exists\n", bname);
        return -1;
    }
    printf("Path must be of type directory\n");
    return -1;
}

/*
    allocated ino and block for new directory, essentially creates
    the new directory and then passes it along to enter_child() so
    that the directory can be added to it's parents block.
*/
int smkdir(MINODE *mip, char *bname)
{
    // allocate new inode and block.
    int ino = ialloc(mip->dev), i;
    int block = balloc(mip->dev);
    char buf[BLKSIZE];
    char *cp;
    DIR *ddp;
    if(ino != 0 && block != 0)
    {
        // get new minode
        MINODE *new = iget(mip->dev, ino);
        INODE *pip = &(new->INODE), *PMip = &(mip->INODE);
        // 004 => directory, 755 => permissions
        // Write all the information to new inode
        // assign block allocated to [0] and give mode of 
        // directory.
        pip->i_mode = 0040755;
        pip->i_block[0] = block;
        // make remaining i_blocks 0
        for(i = 1; i < 12; i++)
        {
            pip->i_block[i] = 0;
        }
        // Set inode properties
        pip->i_uid = running->uid;
        pip->i_gid = running->gid;
        pip->i_size = BLKSIZE;
        pip->i_links_count = 2;
        pip->i_atime = time(NULL);
        pip->i_ctime = time(NULL);
        pip->i_mtime = time(NULL);
        pip->i_blocks = 2;
        // Change new to dirty and write to disk
        new->dirty = 1;
        iput(new);
        // get newly assigned block 0 and write '.' and '..' to it.
        get_block(new->dev, pip->i_block[0], buf);
        ddp = (DIR*)buf;
        cp = buf;
        ddp->inode = ino;
        // EXT2_FT_DIR = 2, represents directory file.
        ddp->file_type = EXT2_FT_DIR;
        // Ideal rec length, this is for all except last entry
        // Last entry makes distance to BLKSIZE.
        ddp->rec_len = 4 * ((8 + 1 + 3) / 4);
        ddp->name_len = strlen(".");
        strcpy(ddp->name, ".");

        cp += ddp->rec_len;
        ddp = (DIR*)cp;
        
        ddp->inode = mip->ino;
        // BLKSIZE - previous directories record length
        ddp->rec_len = BLKSIZE - 12;
        // EXT2_FT_DIR = 2, represents directory file.
        ddp->file_type = EXT2_FT_DIR;
        ddp->name_len = strlen("..");
        strcpy(ddp->name, "..");
        // Write block back to disk block
        put_block(new->dev, block, buf);
        // add newly created directory to parent
        // so it exists.
        enter_child(mip, ino, bname);

        return 1;
    }
    printf("mkdir: cannot create directory '%s': No available inodes/blocks\n", bname);
    return -1;
}

/*
    Add newly created child directory to parent.
*/
int enter_child(MINODE *mip, int ino, char *bname)
{
    int i = 0, remain, len, need_len, ideal;
    INODE *Pinode = &mip->INODE;
    char buf[BLKSIZE];
    char *cp;
    DIR *ddp;
    len = strlen(bname);
    // needed minimum rec_len to fit new directory
    // into block.
    need_len = 4 * ( ( 8 + len + 3 ) / 4);
    // Check all direct blocks.
    for(i; i < 12; i++)
    {
        // if block = 0 stop checking current blocks
        // will break to allocate a new block to parent
        // directory.
        if(!Pinode->i_block[i])
            break;
        
        get_block(mip->dev, Pinode->i_block[i], buf);

        ddp = (DIR*)buf;
        cp = buf;
        // Traverses to last entry in given block.
        while(cp + ddp->rec_len < buf + BLKSIZE)
        {
            cp += ddp->rec_len;
            ddp = (DIR*)cp;
        }
        // gets remainder of ideal block length for ddp and it's
        // actual block length to know how much it can give up.
        ideal = 4 * ( ( 8 + ddp->name_len + 3 ) / 4);
        remain = ddp->rec_len - ideal;
        // if that remainder is greater or equal to needed
        // rec_len of new entry we can add it.
        if(remain >= need_len)
        {
            ddp->rec_len = ideal;

            cp += ddp->rec_len;
            ddp = (DIR*)cp;

            ddp->inode = ino;
            // EXT2_FT_DIR = 2, represents directory file.
            // EXT2_FT_REG_FILE = 1, represents regular file.
            ddp->file_type = EXT2_FT_DIR;
            ddp->rec_len = remain;
            ddp->name_len = len;
            strcpy(ddp->name, bname);

            put_block(mip->dev, Pinode->i_block[i], buf);

            return 1;
        }
    }
    // If no block found allocate a new block
    // and enter it there. 
    int block = balloc(mip->dev);

    Pinode->i_block[i] = block;
    Pinode->i_size += BLKSIZE;

    mip->dirty = 1;

    get_block(mip->dev, block, buf);

    dp = (DIR*)buf;
    cp = buf;

    ddp->inode = ino;
    // EXT2_FT_DIR = 2, represents directory file.
    ddp->file_type = EXT2_FT_DIR;
    // Full block because it has it all to itself
    ddp->rec_len = BLKSIZE;
    ddp->name_len = len;
    strcpy(ddp->name, bname);
    
    put_block(mip->dev, block, buf);
}