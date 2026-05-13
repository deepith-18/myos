#define MAX_FILES     16
#define MAX_FILENAME  16
#define MAX_FILESIZE  256

struct file {
    char name[MAX_FILENAME];
    char data[MAX_FILESIZE];
    unsigned int size;
    unsigned int used;
};

struct file fs_table[MAX_FILES];

void fs_init() {
    int i = 0;
    while (i < MAX_FILES) {
        fs_table[i].used = 0;
        fs_table[i].size = 0;
        int j = 0;
        while (j < MAX_FILENAME) { fs_table[i].name[j] = 0; j++; }
        int k = 0;
        while (k < MAX_FILESIZE) { fs_table[i].data[k] = 0; k++; }
        i++;
    }
}

int fs_strcmp(char *a, char *b) {
    int i = 0;
    while (a[i] && b[i]) {
        if (a[i] != b[i]) return 0;
        i++;
    }
    return a[i] == b[i];
}

void fs_strcpy(char *dst, char *src, int max) {
    int i = 0;
    while (src[i] && i < max - 1) { dst[i] = src[i]; i++; }
    dst[i] = 0;
}

int fs_strlen(char *str) {
    int i = 0;
    while (str[i]) i++;
    return i;
}

int fs_create(char *name) {
    int i = 0;
    while (i < MAX_FILES) {
        if (fs_table[i].used && fs_strcmp(fs_table[i].name, name)) return -1;
        i++;
    }
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
    return -2;
}

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
    return -1;
}

char *fs_read(char *name) {
    int i = 0;
    while (i < MAX_FILES) {
        if (fs_table[i].used && fs_strcmp(fs_table[i].name, name))
            return fs_table[i].data;
        i++;
    }
    return 0;
}

// Delete a file
int fs_delete(char *name) {
    int i = 0;
    while (i < MAX_FILES) {
        if (fs_table[i].used && fs_strcmp(fs_table[i].name, name)) {
            fs_table[i].used = 0;
            fs_table[i].size = 0;
            int j = 0;
            while (j < MAX_FILENAME) { fs_table[i].name[j] = 0; j++; }
            int k = 0;
            while (k < MAX_FILESIZE) { fs_table[i].data[k] = 0; k++; }
            return 0;
        }
        i++;
    }
    return -1;
}

// Append text to existing file
int fs_append(char *name, char *data) {
    int i = 0;
    while (i < MAX_FILES) {
        if (fs_table[i].used && fs_strcmp(fs_table[i].name, name)) {
            int current_size = fs_table[i].size;
            int j = 0;
            while (data[j] && current_size + j < MAX_FILESIZE - 1) {
                fs_table[i].data[current_size + j] = data[j];
                j++;
            }
            fs_table[i].data[current_size + j] = 0;
            fs_table[i].size = current_size + j;
            return 0;
        }
        i++;
    }
    return -1;
}

// Rename a file
int fs_rename(char *old_name, char *new_name) {
    // Check new name not already taken
    int i = 0;
    while (i < MAX_FILES) {
        if (fs_table[i].used && fs_strcmp(fs_table[i].name, new_name)) return -2;
        i++;
    }
    i = 0;
    while (i < MAX_FILES) {
        if (fs_table[i].used && fs_strcmp(fs_table[i].name, old_name)) {
            fs_strcpy(fs_table[i].name, new_name, MAX_FILENAME);
            return 0;
        }
        i++;
    }
    return -1;
}

struct file *fs_get(int index) {
    return &fs_table[index];
}
