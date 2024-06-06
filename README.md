# Telegraph

## Compilation

### Toolchain

1) Windows: Install the toolchain from arm's [website](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads)
Install [Make](http://gnuwin32.sourceforge.net/packages/make.htm)

2) Linux: Install **gcc-arm-none-eabi** / **gdb-multiarch** / **libnewlib-arm-none-eabi**.
Install **Make**

## For VSC usage

### OpenOCD

1) Windows: install [from here](https://gnutoolchains.com/arm-eabi/openocd/)

2) Linux: instal from local repo 

### Visual Studio Code

That.. install that

#### Extensions

1) Install C/C++ extension from [here](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools)

2) Install Stm32-for_vsc from [here](https://marketplace.visualstudio.com/items?itemName=bmd.stm32-for-vscode)

### Configuration

By default CubeMX generates projects in a format called EWARM. Unfortunately, EWARM is currently not supported by the VSCode extension, but the more generic Makefile structure is. In order to configure CubeMX to support VSCode you have to navigate to **Project Manager->Project->Toolchain/IDE** and set it to **Makefile**.

Under the Code generator tab, enable the **Copy all used libraries into the project folder** option. This step is needed because stm32-for-vscode doesn't support the implicit inclusion of libraries yet.
After doing that, click on GENERATE CODE; You should see the new Makefile project structure being created.

# For vanilla STM32CubeIDE usage

1) In STM32CUBEMX: Reload this project file (**telegraph.ioc**) goto **Project Manager->Project->Toolchain/IDE** and set it to **EWARM**, then regenerate. 

2) Load project into STM32CUBEIDE

## Wiring and Hardware

### BOM
1) STM32F103 (or, Blue Pill)
2) Joystick Module
3) Led + resistor for 3V3
4) Cap: 100nF ceramic, 1uF electrolytic
5) USB - TTL convertor 

### Wiring

Use the following schemematic:

![Schematic](/Documents/schematic.png)

## Usage

1) Connect through some terminal application config as 8N1 115200 bauds. 
2) Plug device
3) First screen:

![Welcom-screen](/Documents/Welcome-screen.png)

4) Tipe **MORSE** codes with the switch. 
Example:

![Example](/Documents/Example.png)

5) Commands: 
* STn: Set unit Time
* GT: Get unit Time
* CB: Clear Buffer
* SB: Show Buffer




