#ifndef ___PROCESSOR_INFO__HPP___
#define ___PROCESSOR_INFO__HPP___
#include <string>

using namespace std;

class ProcessorInfo {
protected:
    int cpu_id;
    bool hyper_threads;
    unsigned logical;
    unsigned cores;
public:
    int get_cpu_id() { return cpu_id; }
    bool get_hyper_threads() { return hyper_threads; }
    unsigned get_logical() { return logical; }
    unsigned get_cores() { return cores; }

    ProcessorInfo(int cpu_id) : cpu_id(cpu_id) {
        unsigned regs[4];

        // Get vendor
        char vendor[12];
        cpuID(0, regs);
        ((unsigned *)vendor)[0] = regs[1]; // EBX
        ((unsigned *)vendor)[1] = regs[3]; // EDX
        ((unsigned *)vendor)[2] = regs[2]; // ECX
        string cpuVendor = string(vendor, 12);

        // Get CPU features
        cpuID(1, regs);
        unsigned cpuFeatures = regs[3]; // EDX

        // Detect hyper-threads  
        hyper_threads = false;
        if(cpuVendor == "GenuineIntel" && cpuFeatures & (1 << 28)) { // HTT bit
            // Logical core count per CPU
            cpuID(1, regs);
            logical = (regs[1] >> 16) & 0xff; // EBX[23:16]
            cores = logical;

            if(cpuVendor == "GenuineIntel") {
                // Get DCP cache info
                cpuID(4, regs);
                cores = ((regs[0] >> 26) & 0x3f) + 1; // EAX[31:26] + 1
            } else if(cpuVendor == "AuthenticAMD") {
                // Get NC: Number of CPU cores - 1
                cpuID(0x80000008, regs);
                cores = ((unsigned)(regs[2] & 0xff)) + 1; // ECX[7:0] + 1
            }

            if(cores < logical) hyper_threads = true;
        }
    }

    void cpuID(unsigned i, unsigned regs[4]) {
#ifdef _WIN32
    __cpuid((int *)regs, (int)i);

#else
    asm volatile
        ("cpuid" : "=a" (regs[0]), "=b" (regs[1]), "=c" (regs[2]), "=d" (regs[3])
        : "a" (i), "c" (0));
    // ECX is set to zero for CPUID function 4
#endif
    }
};

#endif
