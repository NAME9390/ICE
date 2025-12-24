 

#include "fat32.h"
#include "../drivers/ata.h"
#include "../drivers/vga.h"

 
static bool mounted = false;
static fat32_bpb_t bpb;
static u32 fat_start_lba;
static u32 data_start_lba;
static u32 root_cluster;
static u32 sectors_per_cluster;

 
static u8 sector_buffer[512];

 
#define MAX_OPEN_FILES 8
static fat32_file_t open_files[MAX_OPEN_FILES];

 
static int write_cluster(u32 cluster, const void *buffer);
static int set_fat_entry(u32 cluster, u32 value);
static u32 find_free_cluster(void);

 
static bool name_match(const u8 *entry_name, const char *name) {
    char fat_name[12] = "           ";
    int i = 0, j = 0;
    
     
    while (name[i] && name[i] != '.' && j < 8) {
        fat_name[j++] = (name[i] >= 'a' && name[i] <= 'z') ? 
                        name[i] - 32 : name[i];
        i++;
    }
    
    if (name[i] == '.') {
        i++;
        j = 8;
        while (name[i] && j < 11) {
            fat_name[j++] = (name[i] >= 'a' && name[i] <= 'z') ? 
                            name[i] - 32 : name[i];
            i++;
        }
    }
    
     
    for (i = 0; i < 11; i++) {
        if (entry_name[i] != fat_name[i]) return false;
    }
    return true;
}

 
static int read_cluster(u32 cluster, void *buffer) {
    u32 lba = data_start_lba + (cluster - 2) * sectors_per_cluster;
    for (u32 i = 0; i < sectors_per_cluster; i++) {
        if (ata_read_sectors(lba + i, 1, (u8*)buffer + i * 512) < 0) {
            return -1;
        }
    }
    return 0;
}

 
static u32 get_next_cluster(u32 cluster) {
    u32 fat_offset = cluster * 4;
    u32 fat_sector = fat_start_lba + (fat_offset / 512);
    u32 entry_offset = fat_offset % 512;
    
    if (ata_read_sectors(fat_sector, 1, sector_buffer) < 0) {
        return 0x0FFFFFFF;   
    }
    
    u32 *fat = (u32*)sector_buffer;
    return fat[entry_offset / 4] & 0x0FFFFFFF;
}

int fat32_init(void) {
     
    if (!ata_is_present()) {
        if (ata_init() < 0) {
            vga_puts("FAT32: No disk found\n");
            mounted = false;
            return -1;
        }
    }
    
     
    if (ata_read_sectors(0, 1, sector_buffer) < 0) {
        vga_puts("FAT32: Failed to read boot sector\n");
        mounted = false;
        return -1;
    }
    
     
    fat32_bpb_t *boot = (fat32_bpb_t*)sector_buffer;
    bpb = *boot;
    
     
    if (bpb.bytes_per_sector != 512) {
        vga_puts("FAT32: Unsupported sector size\n");
        mounted = false;
        return -1;
    }
    
     
    fat_start_lba = bpb.reserved_sectors;
    data_start_lba = fat_start_lba + (bpb.num_fats * bpb.fat_size_32);
    root_cluster = bpb.root_cluster;
    sectors_per_cluster = bpb.sectors_per_cluster;
    
     
    for (int i = 0; i < MAX_OPEN_FILES; i++) {
        open_files[i].valid = false;
    }
    
    mounted = true;
    return 0;
}

fat32_file_t* fat32_open(const char *path) {
    if (!mounted) return 0;
    
     
    fat32_file_t *file = 0;
    for (int i = 0; i < MAX_OPEN_FILES; i++) {
        if (!open_files[i].valid) {
            file = &open_files[i];
            break;
        }
    }
    if (!file) return 0;
    
     
    if (*path == '/') path++;
    
    u32 cluster = root_cluster;
    const char *name = path;
    
     
    u8 cluster_buffer[512 * 8];   
    
    while (*name) {
         
        if (read_cluster(cluster, cluster_buffer) < 0) {
            return 0;
        }
        
         
        fat32_dir_entry_t *entries = (fat32_dir_entry_t*)cluster_buffer;
        int entries_count = (sectors_per_cluster * 512) / sizeof(fat32_dir_entry_t);
        bool found = false;
        
        for (int i = 0; i < entries_count; i++) {
            if (entries[i].name[0] == 0) break;   
            if (entries[i].name[0] == 0xE5) continue;   
            if (entries[i].attr == FAT_ATTR_LFN) continue;   
            
             
            char component[64];
            int j = 0;
            const char *p = name;
            while (*p && *p != '/' && j < 63) {
                component[j++] = *p++;
            }
            component[j] = 0;
            
            if (name_match(entries[i].name, component)) {
                cluster = (entries[i].cluster_high << 16) | entries[i].cluster_low;
                
                if (*p == '/') {
                     
                    name = p + 1;
                } else {
                     
                    file->cluster = cluster;
                    file->size = entries[i].file_size;
                    file->position = 0;
                    file->valid = true;
                    return file;
                }
                found = true;
                break;
            }
        }
        
        if (!found) {
             
            cluster = get_next_cluster(cluster);
            if (cluster >= 0x0FFFFFF8) {
                return 0;   
            }
        }
    }
    
    return 0;
}

int fat32_read(fat32_file_t *file, void *buffer, u32 size) {
    if (!file || !file->valid) return -1;
    
    u32 bytes_read = 0;
    u8 *buf = (u8*)buffer;
    u32 cluster = file->cluster;
    u32 cluster_size = sectors_per_cluster * 512;
    
     
    u32 skip_clusters = file->position / cluster_size;
    for (u32 i = 0; i < skip_clusters && cluster < 0x0FFFFFF8; i++) {
        cluster = get_next_cluster(cluster);
    }
    
    u32 offset_in_cluster = file->position % cluster_size;
    u8 cluster_buffer[4096];
    
    while (bytes_read < size && file->position < file->size) {
        if (cluster >= 0x0FFFFFF8) break;
        
        if (read_cluster(cluster, cluster_buffer) < 0) {
            return bytes_read > 0 ? (int)bytes_read : -1;
        }
        
        u32 copy_size = cluster_size - offset_in_cluster;
        if (copy_size > size - bytes_read) copy_size = size - bytes_read;
        if (copy_size > file->size - file->position) {
            copy_size = file->size - file->position;
        }
        
        for (u32 i = 0; i < copy_size; i++) {
            buf[bytes_read + i] = cluster_buffer[offset_in_cluster + i];
        }
        
        bytes_read += copy_size;
        file->position += copy_size;
        offset_in_cluster = 0;
        
        cluster = get_next_cluster(cluster);
    }
    
    return bytes_read;
}

int fat32_write(fat32_file_t *file, const void *buffer, u32 size) {
    if (!file || !file->valid) return -1;
    
    u32 bytes_written = 0;
    const u8 *buf = (const u8*)buffer;
    u32 cluster = file->cluster;
    u32 cluster_size = sectors_per_cluster * 512;
    
     
    u32 skip_clusters = file->position / cluster_size;
    for (u32 i = 0; i < skip_clusters; i++) {
        u32 next = get_next_cluster(cluster);
        if (next >= 0x0FFFFFF8) {
             
             
             u32 new_cluster = find_free_cluster();
             if (new_cluster == 0) return -1;
             if (set_fat_entry(cluster, new_cluster) < 0) return -1;
             if (set_fat_entry(new_cluster, 0x0FFFFFFF) < 0) return -1;
             
              
             u8 zero[4096];
             for(int z=0; z<4096; z++) zero[z] = 0;
             write_cluster(new_cluster, zero);
             
             next = new_cluster;
        }
        cluster = next;
    }
    
    u32 offset_in_cluster = file->position % cluster_size;
    u8 cluster_buffer[4096];
    
    while (bytes_written < size) {
         
        if (read_cluster(cluster, cluster_buffer) < 0) {
             
             for(int z=0; z<4096; z++) cluster_buffer[z] = 0;
        }
        
        u32 copy_size = cluster_size - offset_in_cluster;
        if (copy_size > size - bytes_written) copy_size = size - bytes_written;
        
        for (u32 i = 0; i < copy_size; i++) {
            cluster_buffer[offset_in_cluster + i] = buf[bytes_written + i];
        }
        
        if (write_cluster(cluster, cluster_buffer) < 0) return -1;
        
        bytes_written += copy_size;
        file->position += copy_size;
        if (file->position > file->size) file->size = file->position;
        
        offset_in_cluster = 0;
        
        if (bytes_written < size) {
             
            u32 next = get_next_cluster(cluster);
            if (next >= 0x0FFFFFF8) {
                u32 new_cluster = find_free_cluster();
                if (new_cluster == 0) return -1;
                if (set_fat_entry(cluster, new_cluster) < 0) return -1;
                if (set_fat_entry(new_cluster, 0x0FFFFFFF) < 0) return -1;
                
                u8 zero[4096];
                for(int z=0; z<4096; z++) zero[z] = 0;
                write_cluster(new_cluster, zero);
                
                next = new_cluster;
            }
            cluster = next;
        }
    }
    
     
     
     
     
     
    
    return bytes_written;
}

 
static int write_cluster(u32 cluster, const void *buffer) {
    u32 lba = data_start_lba + (cluster - 2) * sectors_per_cluster;
    for (u32 i = 0; i < sectors_per_cluster; i++) {
        if (ata_write_sectors(lba + i, 1, (const u8*)buffer + i * 512) < 0) {
            return -1;
        }
    }
    return 0;
}

 
static int set_fat_entry(u32 cluster, u32 value) {
    u32 fat_offset = cluster * 4;
    u32 fat_sector = fat_start_lba + (fat_offset / 512);
    u32 entry_offset = fat_offset % 512;
    
    if (ata_read_sectors(fat_sector, 1, sector_buffer) < 0) return -1;
    
    u32 *fat = (u32*)sector_buffer;
    fat[entry_offset / 4] = value;
    
    if (ata_write_sectors(fat_sector, 1, sector_buffer) < 0) return -1;
    
    return 0;
}

 
static u32 find_free_cluster(void) {
    u32 total_clusters = bpb.total_sectors_32 / bpb.sectors_per_cluster;
    u32 fat_sector = -1;
    
     
    for (u32 i = 2; i < total_clusters; i++) {
        u32 current_fat_sector = fat_start_lba + ((i * 4) / 512);
        
        if (current_fat_sector != fat_sector) {
            fat_sector = current_fat_sector;
            if (ata_read_sectors(fat_sector, 1, sector_buffer) < 0) return 0;
        }
        
        u32 *fat = (u32*)sector_buffer;
        u32 entry = fat[(i * 4) % 512 / 4] & 0x0FFFFFFF;
        
        if (entry == 0) return i;
    }
    return 0;
}

 
static void update_file_size(const char *path, u32 new_size) {
     
    (void)path; (void)new_size;
}

int fat32_create_file(const char *path) {
    if (!mounted) return -1;
    
     
    u32 parent_cluster = root_cluster;
    
     
    const char *name = path;
    const char *p = path;
    while (*p) {
        if (*p == '/') name = p + 1;
        p++;
    }
    if (!*name) return -1;
    
     
    u32 file_cluster = find_free_cluster();
    if (file_cluster == 0) return -2;  
    
    if (set_fat_entry(file_cluster, 0x0FFFFFFF) < 0) return -1;
    
     
    u8 cluster_buffer[4096];  
    if (read_cluster(parent_cluster, cluster_buffer) < 0) return -1;
    
    fat32_dir_entry_t *entries = (fat32_dir_entry_t*)cluster_buffer;
    int entries_count = (sectors_per_cluster * 512) / sizeof(fat32_dir_entry_t);
    int free_idx = -1;
    
    for (int i = 0; i < entries_count; i++) {
        if (entries[i].name[0] == 0 || entries[i].name[0] == 0xE5) {
            free_idx = i;
            break;
        }
    }
    
    if (free_idx == -1) return -2;  
    
     
    fat32_dir_entry_t *entry = &entries[free_idx];
    
     
    for (int k = 0; k < 8; k++) entry->name[k] = ' ';
    for (int k = 0; k < 3; k++) entry->ext[k] = ' ';
    
    int i = 0, j = 0;
     
    while (name[i] && name[i] != '.' && j < 8) {
        entry->name[j++] = (name[i] >= 'a' && name[i] <= 'z') ? name[i]-32 : name[i];
        i++;
    }
     
    if (name[i] == '.') {
        i++;
        j = 0;
        while (name[i] && j < 3) {
            entry->ext[j++] = (name[i] >= 'a' && name[i] <= 'z') ? name[i]-32 : name[i];
            i++;
        }
    }
    
    entry->attr = FAT_ATTR_ARCHIVE;
    entry->cluster_high = (file_cluster >> 16) & 0xFFFF;
    entry->cluster_low = file_cluster & 0xFFFF;
    entry->file_size = 0;
    
     
    if (write_cluster(parent_cluster, cluster_buffer) < 0) return -1;
    
     
    for (int k = 0; k < 4096; k++) cluster_buffer[k] = 0;
    if (write_cluster(file_cluster, cluster_buffer) < 0) return -1;
    
    return 0;
}

 
int fat32_create_dir(const char *path) {
    if (!mounted) return -1;
    u32 parent_cluster = root_cluster;

     
    const char *name = path;
    const char *p = path;
    while (*p) {
        if (*p == '/') name = p + 1;
        p++;
    }
    if (!*name) return -1;

     
    u32 dir_cluster = find_free_cluster();
    if (dir_cluster == 0) return -2;
    
    if (set_fat_entry(dir_cluster, 0x0FFFFFFF) < 0) return -1;

     
    u8 cluster_buffer[4096];
    for(int i=0; i<4096; i++) cluster_buffer[i] = 0;
    
    fat32_dir_entry_t *sub = (fat32_dir_entry_t*)cluster_buffer;
    
     
    for(int k=0; k<8; k++) sub[0].name[k] = ' ';
    for(int k=0; k<3; k++) sub[0].ext[k] = ' ';
    sub[0].name[0] = '.';
    sub[0].attr = FAT_ATTR_DIRECTORY;
    sub[0].cluster_high = (dir_cluster >> 16) & 0xFFFF;
    sub[0].cluster_low = dir_cluster & 0xFFFF;
    
     
    for(int k=0; k<8; k++) sub[1].name[k] = ' ';
    for(int k=0; k<3; k++) sub[1].ext[k] = ' ';
    sub[1].name[0] = '.';
    sub[1].name[1] = '.';
    sub[1].attr = FAT_ATTR_DIRECTORY;
     
    sub[1].cluster_high = 0;
    sub[1].cluster_low = 0;
    
    if (write_cluster(dir_cluster, cluster_buffer) < 0) return -1;

     
    if (read_cluster(parent_cluster, cluster_buffer) < 0) return -1;
    fat32_dir_entry_t *entries = (fat32_dir_entry_t*)cluster_buffer;
    int entries_count = (sectors_per_cluster * 512) / sizeof(fat32_dir_entry_t);
    int free_idx = -1;
    
    for (int i = 0; i < entries_count; i++) {
        if (entries[i].name[0] == 0 || entries[i].name[0] == 0xE5) {
            free_idx = i;
            break;
        }
    }
    
    if (free_idx == -1) return -2;
    
    fat32_dir_entry_t *entry = &entries[free_idx];
    for (int k = 0; k < 8; k++) entry->name[k] = ' ';
    for (int k = 0; k < 3; k++) entry->ext[k] = ' ';
    
    int i = 0, j = 0;
    while (name[i] && name[i] != '.' && j < 8) {
        entry->name[j++] = (name[i] >= 'a' && name[i] <= 'z') ? name[i]-32 : name[i];
        i++;
    }
    if (name[i] == '.') {
        i++; j = 0;
        while (name[i] && j < 3) {
            entry->ext[j++] = (name[i] >= 'a' && name[i] <= 'z') ? name[i]-32 : name[i];
            i++;
        }
    }
    
    entry->attr = FAT_ATTR_DIRECTORY;
    entry->cluster_high = (dir_cluster >> 16) & 0xFFFF;
    entry->cluster_low = dir_cluster & 0xFFFF;
    entry->file_size = 0;
    
    if (write_cluster(parent_cluster, cluster_buffer) < 0) return -1;
    
    return 0;
}

void fat32_close(fat32_file_t *file) {
    if (file) {
         
        file->valid = false;
    }
}

bool fat32_is_mounted(void) {
    return mounted;
}

int fat32_list_dir(const char *path, void (*callback)(fat32_dir_entry_t *entry)) {
    if (!mounted) return -1;
    
    u32 cluster = root_cluster;
    (void)path;  
    
    u8 cluster_buffer[4096];
    int count = 0;
    
    while (cluster < 0x0FFFFFF8) {
        if (read_cluster(cluster, cluster_buffer) < 0) return count;
        
        fat32_dir_entry_t *entries = (fat32_dir_entry_t*)cluster_buffer;
        int entries_count = (sectors_per_cluster * 512) / sizeof(fat32_dir_entry_t);
        
        for (int i = 0; i < entries_count; i++) {
            if (entries[i].name[0] == 0) return count;
            if (entries[i].name[0] == 0xE5) continue;
            if (entries[i].attr == FAT_ATTR_LFN) continue;
            
            callback(&entries[i]);
            count++;
        }
        
        cluster = get_next_cluster(cluster);
    }
    return count;
}
