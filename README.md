# linoufree

a free fortnite external cheet for windows, uses a kernel driver to read/write physical memory and an imgui overlay to render everything. made by [linoudev](https://github.com/linoudev)

## features

- **esp** - box, bones, snaplines, distance, player id, platform indicator (pc, xbox, playstation, switch, android, ios) with colors
- **aimbot** - customizable fov, smoothing, visible check, lock target option, configurable aim key (click to rebind)
- **radar** - 2d radar with grid, rotation modes, distance labels, opacity/size/range settings, fov cone
- **exploits** - fov changer, insta reload, silent aim, player size with custom scale, no spread, rapid fire with adjustable rate
- **misc** - fps counter with custom color, vsync toggle
- **menu** - clean dark ui with tabbed layout, toggle with insert key

## how it works

the cheet communicates with a kernel driver through deviceiocontrol. it opens a handle to a named device, sends ioctl codes for reading/writing physical memory, getting the process base address, and fetching the cr3 (directory table base) for the target process.

the driver itself is downloaded and loaded at runtime from catbox.moe using curl, then mapped with a mapper tool. this happens automatically if the driver handle fails to open on first try.

once the driver is ready, it scans for the fortnite process (fortniteclient-win64-shipping.exe), grabs the cr3 and base address, then creates a transparent directx 11 overlay window that covers the entire screen. the render loop uses imgui for drawing and handles all the esp/aimbot logic per frame.

## project structure

```
linoufree.sln              - visual studio 2022 solution
linoufree/
├── imgui/                 - dear imgui library (dx11 + win32 backend)
├── src/
│   ├── entrypoint.cpp     - main entry, driver load, game detection
│   ├── colors.h           - console color defines
│   ├── auth/              - key auth system (optional)
│   ├── memory/
│   │   └── driver.h       - driver class + memory class (ioctl communication)
│   ├── render/
│   │   ├── render.h       - d3d11 init, overlay, menu ui, render loop
│   │   └── esp.h          - esp drawing, aimbot, exploits, radar, player caching
│   ├── sdk/
│   │   ├── sdk.h          - game sdk (world to screen, bone reading, visibility)
│   │   └── offsets.h      - all the game offsets + uworld decryption + exploit offsets
│   └── util/
│       ├── math.h         - vectors, transforms, matrix math
│       └── settings.h     - default config values for everything
```

## building

open the solution in visual studio 2022, select release|x64, and build. you'll need the windows sdk and the c++ desktop workload installed. the project already links against d3d11.lib, dxgi.lib, and dwmapi.lib.

## usage

1. build the project
2. run the executable as administrator (required for the driver)
3. the cheet will download and load the driver automatically
4. launch fortnite and join a match
5. wait for "press ok in lobby" message
6. press insert to open the menu and configure settings

## notes

- this is pretty shitcoded but it works so idc
- offsets will need updating after game updates
- you can paste anything you want here, requires basic game cheeting knowledge
- stay cute

## credits

- dear imgui (https://github.com/ocornut/imgui)
- me for writing the rest while suffering from schyzophrenia

consider joining https://discord.gg/linouservices please <3

## license

you can use this code for your own purposes, but please don't claim it as your own work. i worked hard on this and would appreciate credit if you use it.
