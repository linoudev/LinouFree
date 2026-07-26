#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include <cstdint>
#include <winioctl.h>

// IOCTL codes for driver communication (must match driver)
#define CODE_RW                  CTL_CODE(FILE_DEVICE_UNKNOWN, 0x47536, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define CODE_BA                  CTL_CODE(FILE_DEVICE_UNKNOWN, 0x36236, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define CODE_GET_GUARDED_REGION  CTL_CODE(FILE_DEVICE_UNKNOWN, 0x13437, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define CODE_GET_DIR_BASE        CTL_CODE(FILE_DEVICE_UNKNOWN, 0x13438, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define CODE_SECURITY            0x457c1d6

// structs for driver requests (must match driver)
typedef struct _RW {
    INT32 security;
    INT32 process_id;
    ULONGLONG address;
    ULONGLONG buffer;
    ULONGLONG size;
    BOOLEAN write;
} RW, *PRW;

typedef struct _BA {
    INT32 security;
    INT32 process_id;
    ULONGLONG* address;
} BA, *PBA;

typedef struct _GA {
    INT32 security;
    ULONGLONG* address;
} GA, *PGA;

typedef struct _MEMORY_OPERATION_DATA {
    uint32_t pid;
    ULONGLONG* cr3;
} MEMORY_OPERATION_DATA, *PMEMORY_OPERATION_DATA;

class DRIVER_CLASS {
public:
    HANDLE DriverHandle;
    INT32 ProcessID;
    uintptr_t CR3;

    // connect to driver
    bool Init() {
        DriverHandle = CreateFileW(
            L"\\\\.\\{d6579ab0-c95b-4463-9135-41gbcf16e4eg}",
            GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            NULL
        );

        if (DriverHandle == INVALID_HANDLE_VALUE) {
            // try with Global\ prefix
            DriverHandle = CreateFileW(
                L"\\\\.\\Global\\{d6579ab0-c95b-4463-9135-41gbcf16e4eg}",
                GENERIC_READ | GENERIC_WRITE,
                FILE_SHARE_READ | FILE_SHARE_WRITE,
                NULL,
                OPEN_EXISTING,
                FILE_ATTRIBUTE_NORMAL,
                NULL
            );
        }

        return DriverHandle && DriverHandle != INVALID_HANDLE_VALUE;
    }

    // fetch CR3/directory base for current process (call after FindProcess)
    uintptr_t FetchCR3() {
        uintptr_t cr3_value = 0;
        MEMORY_OPERATION_DATA args = { 0 };
        args.pid = ProcessID;
        args.cr3 = (ULONGLONG*)&cr3_value;

        DWORD returned;
        DeviceIoControl(DriverHandle, CODE_GET_DIR_BASE, &args, sizeof(args), nullptr, 0, &returned, nullptr);
        CR3 = cr3_value;
        return cr3_value;
    }

    // read memory from target process
    void ReadPhysicalMemory(PVOID address, PVOID buffer, DWORD size) {
        RW args = { 0 };
        args.security = CODE_SECURITY;
        args.process_id = ProcessID;
        args.address = (ULONGLONG)address;
        args.buffer = (ULONGLONG)buffer;
        args.size = size;
        args.write = FALSE;

        DWORD returned;
        DeviceIoControl(DriverHandle, CODE_RW, &args, sizeof(args), nullptr, 0, &returned, nullptr);
    }

    // write memory to target process
    void WritePhysicalMemory(PVOID address, PVOID buffer, DWORD size) {
        RW args = { 0 };
        args.security = CODE_SECURITY;
        args.process_id = ProcessID;
        args.address = (ULONGLONG)address;
        args.buffer = (ULONGLONG)buffer;
        args.size = size;
        args.write = TRUE;

        DWORD returned;
        DeviceIoControl(DriverHandle, CODE_RW, &args, sizeof(args), nullptr, 0, &returned, nullptr);
    }

    // get base address of target process
    uintptr_t GetBase() {
        uintptr_t image_address = 0;
        BA args = { 0 };
        args.security = CODE_SECURITY;
        args.process_id = ProcessID;
        args.address = (ULONGLONG*)&image_address;

        DWORD returned;
        DeviceIoControl(DriverHandle, CODE_BA, &args, sizeof(args), nullptr, 0, &returned, nullptr);
        return image_address;
    }

    // get guarded region address
    uintptr_t GetGuardedRegion() {
        uintptr_t guarded_address = 0;
        GA args = { 0 };
        args.security = CODE_SECURITY;
        args.address = (ULONGLONG*)&guarded_address;

        DWORD returned;
        DeviceIoControl(DriverHandle, CODE_GET_GUARDED_REGION, &args, sizeof(args), nullptr, 0, &returned, nullptr);
        return guarded_address;
    }

    // find process by name, returns PID
    INT32 FindProcess(LPCTSTR name) {
        PROCESSENTRY32 entry;
        entry.dwSize = sizeof(entry);
        HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        
        if (snap == INVALID_HANDLE_VALUE)
            return 0;
        
        if (Process32First(snap, &entry)) {
            do {
                if (!lstrcmpi(entry.szExeFile, name)) {
                    ProcessID = entry.th32ProcessID;
                    CloseHandle(snap);
                    return ProcessID;
                }
            } while (Process32Next(snap, &entry));
        }
        CloseHandle(snap);
        return 0;
    }

    // cleanup
    void Close() {
        if (DriverHandle != INVALID_HANDLE_VALUE) {
            CloseHandle(DriverHandle);
            DriverHandle = INVALID_HANDLE_VALUE;
        }
    }
};

inline DRIVER_CLASS driver;

// wrapper for easy memory read/write
class MEMORY_CLASS {
public:
    ULONGLONG BaseAddress;

    template <typename T>
    T read(uint64_t addr) {
        T buf{};
        driver.ReadPhysicalMemory((PVOID)addr, &buf, sizeof(T));
        return buf;
    }

    template <typename T>
    void write(uint64_t addr, T val) {
        driver.WritePhysicalMemory((PVOID)addr, &val, sizeof(T));
    }
};

inline MEMORY_CLASS memory;
