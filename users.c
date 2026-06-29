// users.c - Robust Multi-User Subsystem for DeepithOS

// External filesystem operations from your kernel/fs modules
int fs_create(char *name);
int fs_write(char *name, char *data);
char *fs_read(char *name);

// External printing functions from your kernel video modules
void print_string(char *str, unsigned char color);
void print_newline();

// Structure definition for runtime memory tracking
struct user_profile {
    char username[32];
    char password[32];
    int active;
};

// Allocate a database structure layout in memory for up to 8 profiles
static struct user_profile user_db[8];
static int user_count = 0;
static char current_username[32] = "deepith";

static void mystrcpy(char *dest, char *src, int max_len) {
    int i = 0;
    while (src[i] && i < (max_len - 1)) {
        dest[i] = src[i];
        i++;
    }
    dest[i] = 0;
}

static int mystrcmp(char *a, char *b) {
    int i = 0;
    while (a[i] && b[i]) {
        if (a[i] != b[i]) return 0;
        i++;
    }
    return a[i] == b[i];
}

// Seeds default database profiles directly into memory segments
void users_init() {
    int i;
    for (i = 0; i < 8; i++) {
        user_db[i].active = 0;
    }

    // Seed primary administrator account profile index
    mystrcpy(user_db[0].username, "deepith", 32);
    mystrcpy(user_db[0].password, "deepith", 32);
    user_db[0].active = 1;
    user_count = 1;
}

char *users_get_current() {
    return current_username;
}

// Authenticates against the memory array database registers instantly
int users_authenticate(char *username, char *password) {
    int i;
    for (i = 0; i < 8; i++) {
        if (user_db[i].active) {
            if (mystrcmp(user_db[i].username, username) && mystrcmp(user_db[i].password, password)) {
                mystrcpy(current_username, username, 32);
                return 1; // Verified successfully!
            }
        }
    }
    return 0; // Invalid credentials
}

// Registers a completely new account context dynamically inside RAM bounds
int users_add_user(char *username, char *password) {
    if (!username || username[0] == 0 || !password || password[0] == 0) return -1;
    
    // Check if user already exists, update their password if they do
    int i;
    for (i = 0; i < 8; i++) {
        if (user_db[i].active && mystrcmp(user_db[i].username, username)) {
            mystrcpy(user_db[i].password, password, 32);
            return 0;
        }
    }

    // Find a free slot in the array context space
    for (i = 0; i < 8; i++) {
        if (!user_db[i].active) {
            mystrcpy(user_db[i].username, username, 32);
            mystrcpy(user_db[i].password, password, 32);
            user_db[i].active = 1;
            user_count++;
            return 0; // Successfully registered into system space bounds!
        }
    }
    return -2; // Database array is full
}

int users_change_password(char *new_password) {
    if (!new_password || new_password[0] == 0) return -1;
    return users_add_user(current_username, new_password);
}

// Debug function to see who is currently active in memory
void users_list_all() {
    print_string("  Registered Users in Memory:", 0x0E);
    print_newline();
    int i;
    for (i = 0; i < 8; i++) {
        if (user_db[i].active) {
            print_string("  - ", 0x07);
            print_string(user_db[i].username, 0x0A);
            print_newline();
        }
    }
}
