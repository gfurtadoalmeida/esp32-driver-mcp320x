# ESP32 MCP3204/3208 12-bit ADC Driver

![Azure DevOps builds](https://img.shields.io/azure-devops/build/gfurtadoalmeida/GitHub/44?)  
ESP32 driver for Microchip [MCP3204](https://www.microchip.com/wwwproducts/en/MCP3204) and [MCP3208](https://www.microchip.com/wwwproducts/en/MCP3208) 12-bit ADCs.

## Characteristics

* Written in **C** using the [ESP-IDF](https://github.com/espressif/esp-idf) framework.
* Uses the SPI driver instead of raw GPIO manipulation.

## Installation

1. Clone this repository and copy the folder ["components/mcp320x"](/components/mcp320x/) to the components folder of your ESP-IDF project.
2. Include `mcp320x.h` in your code.

## Project Structure

```text
/
|-- components
    |-- mcp320x             : The driver
        | external          : External libraries
        |-- include         : Includes that can be used by dependent projects
        |-- private_include : Includes that only the driver can use
        |-- src             : Where ".c" files resides
        |-- test            : Tests
|-- examples                : Usage examples
|-- main                    : Where debugging happens
|-- test                    : Test runner
```

## Development Guidelines

* The code must run on any ESP32+ hardware.
* Create tests for every new functionality.

## Building 🔨

### Requirements

* [CMake](https://cmake.org/): 3.5+, must be on PATH environment variable.
* [ESP-IDF](https://github.com/espressif/esp-idf): use the the last stable one.

### On VS Code

It's highly recommended to install the official [ESP-IDF Extension](https://marketplace.visualstudio.com/items?itemName=espressif.esp-idf-extension), for a better experience.

```ctrl+shif+b``` or ```ctrl+shif+p -> Task: Run Task -> Build```

### With ESP-IDF

```idf.py build``` on the root folder.

## Debugging 🧩

On the [/.debug](/.debug/) folder you'll find interface configuration files for debug boards. Choose one and copy it to the `%IDF_TOOLS_PATH%\tools\openocd-esp32\{version}\openocd-esp32\share\openocd\scripts\interface` folder.

Boards:

* [CJMCU-FTDI-2232HL](https://www.aliexpress.com/wholesale?SearchText=cjmcu+2232hl)

Driver configuration:

1. Install the latest [FTDI VPC Driver](https://www.ftdichip.com/Drivers/VCP.htm).
2. Plug the debug board.
3. Open [Zadig](https://zadig.akeo.ie/) and replace the `Dual RS232-HS (Interface 0)` driver with WinUSB (`Options->List all devices`).

## Testing 🧪

The task watchdog for CPU0 is disabled on the test project. It is needed so we can interact with the test tool.  

### Hardware Setup

* MCP320X:
  * Reference: 5V
  * Channel 3: 2.5V
* ESP32:
  * SPI host = SPI3_HOST
  * SPI Chip Select pin: GPIO_NUM_5

### On VS Code

1. Build the test project: `ctrl+shif+p -> Task: Run Task -> Build test`
2. Flash the test project: `ctrl+shif+p -> Task: Run Task -> Flash test`
3. Monitor the test run: `ctrl+shif+p -> Task: Run Task -> Monitor test`

You can do it in one command using `ctrl+shif+p -> Task: Run Task -> Build, flash and start a monitor for the tests`

### With ESP-IDF

All commands must be run on the test runner folder.
Change the COM port to the one you're using.

1. Build the test project: `idf.py build`
2. Flash the test project: `idf.py flash -p COM4`
3. Monitor the test run: `idf.py monitor -p COM4`
