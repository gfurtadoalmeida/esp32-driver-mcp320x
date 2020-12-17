# ESP32 Project Template

![Azure DevOps builds](https://img.shields.io/azure-devops/build/gfurtadoalmeida/GitHub/38?)  
ESP32 project template, including tests, automated build pipeline and able to work on VS Code.  

## Features

* Prepared for tests 🧪
* [Automated build pipeline](/.pipeline/) 🚀
* VS Code task for tests:
  * `Build test`
  * `Flash test`
  * `Monitor test`
  * `Build, flash and start a monitor for the tests`

Note: for VS Code it's highly recommended to install the official [ESP-IDF Extension](https://marketplace.visualstudio.com/items?itemName=espressif.esp-idf-extension), for a better experience.

## You Need to Change

* [CMakeLists.txt](/CMakeLists.txt):
  * Change the `project-name`.
* [/.vscode/settings.json](/.vscode/settings.json):
  * Change `idf.portWin` to the COM port that your device is connected.
  * Change `idf.openOcdConfigs` based on your device.
* [/test/CMakeLists.txt](/test/CMakeLists.txt):
  * Change the `component-name` on the test components.
  * Add more components to the test runner, if needed.
* [/components/component_name](/components/component_name/):
  * Change the folder name.
  * [CMakeLists.txt](/components/component_name/CMakeLists.txt):
    * Add your sources and requires.
  * [Kconfig](/components/component_name/Kconfig):
    * Add your configurations or delete the file if not needed.
  * [/private_include](/components/component_name/private_include/):
    * [check.h](/components/component_name/private_include/check.h):
      * Change `COMPONENT_NAME_TAG` name and value.
      * Change `COMP_NAME_CHECK` name.
    * [config.h](/components/component_name/private_include/config.h):
      * Add your configurations or delete the file if not needed.
      * Keep in mind that the names defined on the KConfig file are translated with a `CONFIG_` prefix (i.e.: `COMP_NAME_CONFIG_ONE` -> `CONFIG_COMP_NAME_CONFIG_ONE`).
  * [/test](/components/component_name/test/):
    * [CMakeLists.txt](/components/component_name/test/CMakeLists.txt):
      * Change the `component_name`.
    * [test.c](/components/component_name/test/test.c):
      * Add your tests.

## Building 🔨

### On VS Code

```ctrl+shif+b``` or ```ctrl+shif+p -> Task: Run Task -> Build```

### With ESP-IDF

```idf.py build``` on the root folder.

## Debugging 🧩

On the [/.debug](/.debug/) folder you'll find interface configuration files for debug boards. Choose one and copy it to the `%IDF_TOOLS_PATH%\tools\openocd-esp32\{version}\openocd-esp32\share\openocd\scripts\interface` folder.

Boards:

* [CJMCU-FTDI-2232HL](https://www.aliexpress.com/wholesale?SearchText=cjmcu+2232hl)

### Using FTDI based boards

1. Install the latest [FTDI VPC Driver](https://www.ftdichip.com/Drivers/VCP.htm).
2. Plug the debug board.
3. Open [Zadig](https://zadig.akeo.ie/) and replace the `Dual RS232-HS (Interface 0)` driver to WinUSB (`Options->List all devices`).

## Testing 🧪

### On VS Code

1. Build the test project: `ctrl+shif+p -> Task: Run Task -> Build test`
2. Flash the test project: `ctrl+shif+p -> Task: Run Task -> Flash test`
3. Monitor the test run: `ctrl+shif+p -> Task: Run Task -> Monitor test`

You can do it in one command using `ctrl+shif+p -> Task: Run Task -> Build, flash and start a monitor for the tests`

### With ESP-IDF

All commands must be run on the test runner folder.

1. Build the test project: `idf.py build`
2. Flash the test project: `idf.py flash -p COM4`
3. Monitor the test run: `idf.py monitor -p COM4`
