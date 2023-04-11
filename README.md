# ESP32 Project Template

[![Azure DevOps Build Status](https://dev.azure.com/gfurtadoalmeida/GitHub/_apis/build/status%2Fesp32%2Fproject-template)](https://dev.azure.com/gfurtadoalmeida/GitHub/_build/latest?definitionId=52) ![GitHub Build Status](https://github.com/gfurtadoalmeida/esp32-project-template/actions/workflows/build.yml/badge.svg) [![Maintainability Rating](https://sonarcloud.io/api/project_badges/measure?project=esp32_project_template&metric=sqale_rating)](https://sonarcloud.io/summary/new_code?id=esp32_project_template) [![Code Smells](https://sonarcloud.io/api/project_badges/measure?project=esp32_project_template&metric=code_smells)](https://sonarcloud.io/summary/new_code?id=esp32_project_template)   
ESP32 project template with build pipeline, enhanced VS Code support and more.  

## Characteristics

* Build pipelines with [Sonar Cloud](https://sonarcloud.io/) integration: 🚀
  * Azure DevOps
  * GitHub Action
* [VS Code task](https://code.visualstudio.com/docs/editor/tasks) for tests (requires [ESP-IDF extension](https://marketplace.visualstudio.com/items?itemName=espressif.esp-idf-extension)): 🧪
  * Build test
  * Flash test
  * Monitor test
  * Build, flash and start a monitor for the tests
* [Newlib nano formating](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/performance/size.html?#newlib-nano-formatting) enabled for `printf/scanf`, with the following advantages:
  * Binary size reduction (25KB~50KB) as they are already in ESP32 ROM.
  * Stack usage reduction for functions that call string formating functions.
  * Increased performance as functions in ROM run faster than functions from flash. 
  * Functions in ROM can run when flash instruction cache is disabled.
* Sample [Kconfig](/components/component_name/Kconfig) file for component configuration through [menuconfig](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/kconfig.html).
* Watchdog disabled when testing (`CONFIG_ESP_TASK_WDT_CHECK_IDLE_TASK_CPU0` not set).

## Documentation

Everything is on the [wiki](https://github.com/gfurtadoalmeida/esp32-project-template/wiki).
