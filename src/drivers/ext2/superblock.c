#include "drivers/ext2/superblock.h"
#include "system/mm/allocator.h"

int ext2_superblock_init(struct ext2* fs) {

    struct Superblock* superblock;
    if ((superblock = kalloc(SUPERBLOCK_SIZE)) == NULL) {
        return -1;
    }

    if (read_sectors(fs, SUPERBLOCK_START, SUPERBLOCK_SECTORS, superblock) != 0) {
        return -1;
    }

    fs->sb = superblock;

    return 0;
}

int ext2_superblock_end(struct ext2* fs) {
    ext2_superblock_write(fs);
    //TODO Free Superblock
    return 0;
}

int ext2_get_total_block_groups(struct Superblock* superblock) {
    return (superblock->totalBlocks / superblock->blocksPerBlockGroup) + 1;
}

int ext2_superblock_write(struct ext2* fs) {
    return write_sectors(fs, SUPERBLOCK_START, SUPERBLOCK_SECTORS, fs->sb);
}

