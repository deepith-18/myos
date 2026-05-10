// Simple in-memory filesystem
// Supports up to 16 files, 256 bytes each

#define MAX_FILES     16
#define MAX_FILENAME  16
#define MAX_FILESIZE  256

// File structure
struct file {
    char name[MAX_FILENAME];
    char data[MAX_FILESIZE];
    unsigned int size;
    unsigned int used;
};

// Filesystem table
struct file fs_table[MAX_FILES];

// Initialize filesystem
void fs_init() {
    int i = 0;
    while (i < MAX_FILES) {
        fs_table[i].used = 0;
        fs_table[i].size = 0;
        int j = 0;
        while (j < MAX_FILENAME) {
            fs_table[i].name[j] = 0;
            j++;
        }
        int k = 0;
        while (k < MAX_FILESIZE) {
            fs_table[i].data[k] = 0;
            k++;
        }
        i++;
    }
}

// Compare strings helper
int fs_strcmp(char *a, char *b) {
    int i = 0;
    while (a[i] && b[i]) {
        if (a[i] != b[i]) return 0;
        i++;
    }
    return a[i] == b[i];
}

// Copy string helper
void fs_strcpy(char *dst, char *src, int max) {
    int i = 0;
    while (src[i] && i < max - 1) {
        dst[i] = src[i];
        i++;
    }
    dst[i] = 0;
}

// Get string length
int fs_strlen(char *str) {
    int i = 0;
    while (str[i]) i++;
    return i;
}

// Create a new file
int fs_create(char *name) {
    // Check if already exists
    int i = 0;
    while (i < MAX_FILES) {
        if (fs_table[i].used && fs_strcmp(fs_table[i].name, name)) {
            return -1; // already exists
        }
        i++;
    }

    // Find empty slot
    i = 0;
    while (i < MAX_FILES) {
        if (!fs_table[i].used) {
            fs_strcpy(fs_table[i].name, name, MAX_FILENAME);
            fs_table[i].used = 1;
            fs_table[i].size = 0;
            return i;
        }
        i++;
    }
    return -2; // filesystem full
}

// Write data to file
int fs_write(char *name, char *data) {
    int i = 0;
    while (i < MAX_FILES) {
        if (fs_table[i].used && fs_strcmp(fs_table[i].name, name)) {
            fs_strcpy(fs_table[i].data, data, MAX_FILESIZE);
            fs_table[i].size = fs_strlen(data);
            return 0;
        }
        i++;
    }
    return -1; // not found
}

// Read data from file — returns pointer to data
char *fs_read(char *name) {
    int i = 0;
    while (i < MAX_FILES) {
        if (fs_table[i].used && fs_strcmp(fs_table[i].name, name)) {
            return fs_table[i].data;
        }
        i++;
    }
    return 0; // not found
}

// Count used files
int fs_count() {
    int count = 0;
    int i = 0;
    while (i < MAX_FILES) {
        if (fs_table[i].used) count++;
        i++;
    }
    return count;
}

// Get file by index (for listing)
struct file *fs_get(int index) {
    return &fs_table[index];
}

