// Simple process manager
#define MAX_PROCESSES 8

#define PROCESS_FREE    0
#define PROCESS_RUNNING 1
#define PROCESS_STOPPED 2

struct process {
    unsigned int pid;
    char name[32];
    unsigned int state;
    unsigned int ticks;   // how long it has run
};

struct process proc_table[MAX_PROCESSES];
unsigned int next_pid = 1;

// Initialize process manager
void proc_init() {
    int i = 0;
    while (i < MAX_PROCESSES) {
        proc_table[i].pid   = 0;
        proc_table[i].state = PROCESS_FREE;
        proc_table[i].ticks = 0;
        int j = 0;
        while (j < 32) { proc_table[i].name[j] = 0; j++; }
        i++;
    }

    // Create kernel process (PID 1)
    proc_table[0].pid   = 1;
    proc_table[0].state = PROCESS_RUNNING;
    proc_table[0].ticks = 0;
    // Copy name manually
    proc_table[0].name[0] = 'k';
    proc_table[0].name[1] = 'e';
    proc_table[0].name[2] = 'r';
    proc_table[0].name[3] = 'n';
    proc_table[0].name[4] = 'e';
    proc_table[0].name[5] = 'l';
    proc_table[0].name[6] = 0;
    next_pid = 2;
}

// String copy helper
void proc_strcpy(char *dst, char *src, int max) {
    int i = 0;
    while (src[i] && i < max - 1) { dst[i] = src[i]; i++; }
    dst[i] = 0;
}

// String compare helper
int proc_strcmp(char *a, char *b) {
    int i = 0;
    while (a[i] && b[i]) {
        if (a[i] != b[i]) return 0;
        i++;
    }
    return a[i] == b[i];
}

// Spawn a new process
int proc_spawn(char *name) {
    int i = 0;
    while (i < MAX_PROCESSES) {
        if (proc_table[i].state == PROCESS_FREE) {
            proc_table[i].pid   = next_pid++;
            proc_table[i].state = PROCESS_RUNNING;
            proc_table[i].ticks = 0;
            proc_strcpy(proc_table[i].name, name, 32);
            return proc_table[i].pid;
        }
        i++;
    }
    return -1;  // no free slots
}

// Kill a process by PID
int proc_kill(unsigned int pid) {
    if (pid == 1) return -2;  // cannot kill kernel
    int i = 0;
    while (i < MAX_PROCESSES) {
        if (proc_table[i].pid == pid &&
            proc_table[i].state == PROCESS_RUNNING) {
            proc_table[i].state = PROCESS_STOPPED;
            return 0;
        }
        i++;
    }
    return -1;
}

// Count running processes
int proc_count() {
    int count = 0;
    int i = 0;
    while (i < MAX_PROCESSES) {
        if (proc_table[i].state == PROCESS_RUNNING) count++;
        i++;
    }
    return count;
}

// Get process by index
struct process *proc_get(int index) {
    return &proc_table[index];
}

// Tick all running processes
void proc_tick() {
    int i = 0;
    while (i < MAX_PROCESSES) {
        if (proc_table[i].state == PROCESS_RUNNING) {
            proc_table[i].ticks++;
        }
        i++;
    }
}
