# Using the Component

## 1. Clone the Repository

Clone this repository and copy the folder ["components/esp32_driver_mcp320x"](../../components/esp32_driver_mcp320x/) to the components folder of your ESP-IDF project.

## 2. Add the Headers

Include `esp32_driver_mcp320x/mcp320x.h` in your code.

## No locking :warning:

The MCP320X ADC series does not allow reading multiple channels at the same time.  
Is up to the user to implement any mechanism necessary to prevent concurrent reads.  
