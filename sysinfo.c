// CPU vendor string buffer
char cpu_vendor[13];
unsigned int cpu_family;
unsigned int cpu_model;

// Read CPU info using CPUID instruction
void cpu_detect() {
    unsigned int eax, ebx, ecx, edx;

    // CPUID with eax=0 returns vendor string
    __asm__(
        "cpuid"
        : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx)
        : "a"(0)
    );

    // Vendor string is in EBX, EDX, ECX (in that order)
    cpu_vendor[0]  = (ebx >>  0) & 0xFF;
    cpu_vendor[1]  = (ebx >>  8) & 0xFF;
    cpu_vendor[2]  = (ebx >> 16) & 0xFF;
    cpu_vendor[3]  = (ebx >> 24) & 0xFF;
    cpu_vendor[4]  = (edx >>  0) & 0xFF;
    cpu_vendor[5]  = (edx >>  8) & 0xFF;
    cpu_vendor[6]  = (edx >> 16) & 0xFF;
    cpu_vendor[7]  = (edx >> 24) & 0xFF;
    cpu_vendor[8]  = (ecx >>  0) & 0xFF;
    cpu_vendor[9]  = (ecx >>  8) & 0xFF;
    cpu_vendor[10] = (ecx >> 16) & 0xFF;
    cpu_vendor[11] = (ecx >> 24) & 0xFF;
    cpu_vendor[12] = 0;

    // CPUID with eax=1 returns family/model
    __asm__(
        "cpuid"
        : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx)
        : "a"(1)
    );

    cpu_family = (eax >> 8) & 0xF;
    cpu_model  = (eax >> 4) & 0xF;
}

// Get vendor string
char *cpu_get_vendor() {
    return cpu_vendor;
}

unsigned int cpu_get_family() { return cpu_family; }
unsigned int cpu_get_model()  { return cpu_model;  }
