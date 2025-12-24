 

#include "ext2.h"
#include "../drivers/ata.h"
#include "../drivers/vga.h"

 
static bool mounted = false;
static ext2_superblock_t sb;
static ext2_bg_desc_t *bg_descs = 0;
static u32 block_size = 0;
static u32 sectors_per_block = 0;
static u32 num_bg = 0;

 
static u8 block_buffer[4096];

 
static int read_block(u32 block, void *buffer) {
    u32 lba = block * sectors_per_block;
    if (ata_read_sectors(lba, sectors_per_block, buffer) < 0) return -1;
    return 0;
}

static int write_block(u32 block, const void *buffer) {
    u32 lba = block * sectors_per_block;
    if (ata_write_sectors(lba, sectors_per_block, buffer) < 0) return -1;
    return 0;
}

static void read_inode(u32 inode, ext2_inode_t *dst) {
    u32 bg = (inode - 1) / sb.inodes_per_group;
    u32 index = (inode - 1) % sb.inodes_per_group;
    u32 table_block = bg_descs[bg].inode_table;
    
     
    u32 inode_size = sb.inode_size ? sb.inode_size : 128; 
    u32 offset = index * inode_size;
    u32 block_offset = offset / block_size;
    u32 byte_offset = offset % block_size;
    
    read_block(table_block + block_offset, block_buffer);
    
    
    u8 *ptr = block_buffer + byte_offset;
    
    
    
    
    
    *dst = *(ext2_inode_t*)ptr;
}

int ext2_init(void) {
    if (!ata_is_present()) {
        if (ata_init() < 0) return -1;
    }
    
     
     
     
     
    
    u8 buf[1024];  
    if (ata_read_sectors(2, 2, buf) < 0) return -1;
    
    ext2_superblock_t *s = (ext2_superblock_t*)buf;
    if (s->magic != EXT2_SUPER_MAGIC) {
        vga_puts("EXT2: Invalid magic\n");
        return -1;
    }
    
    sb = *s;
    block_size = 1024 << sb.log_block_size;
    sectors_per_block = block_size / 512;
    num_bg = (sb.blocks_count + sb.blocks_per_group - 1) / sb.blocks_per_group;
    
     
     
     
    u32 bg_loc = (block_size == 1024) ? 2 : 1;
    
     
    
    static ext2_bg_desc_t bg_cache[32]; 
    if (num_bg > 32) num_bg = 32; 
    
    read_block(bg_loc, block_buffer);
    ext2_bg_desc_t *src = (ext2_bg_desc_t*)block_buffer;
    for (u32 i = 0; i < num_bg; i++) {
        bg_cache[i] = src[i];
    }
    bg_descs = bg_cache;
    
    mounted = true;
    return 0;
}

 
ext2_file_t* ext2_open(const char *path) { (void)path; return 0; }
int ext2_read(ext2_file_t *file, void *buffer, u32 size) { (void)file; (void)buffer; (void)size; return -1; }
int ext2_write(ext2_file_t *file, const void *buffer, u32 size) { (void)file; (void)buffer; (void)size; return -1; }
void ext2_close(ext2_file_t *file) { (void)file; }
int ext2_create_file(const char *path) { (void)path; return -1; }
int ext2_create_dir(const char *path) { (void)path; return -1; }
int ext2_list_dir(const char *path, void (*callback)(const char *name, u32 size, bool is_dir)) { (void)path; (void)callback; return -1; }
