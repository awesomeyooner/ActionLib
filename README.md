# ActionLib
CMake library designed to help with asyncronous execution of code to help avoid blocking functions in time-critical methods, particularly with communications

## Setup
To include this library, please `git clone` or `git submodule add` this repo into the `\lib` folder of your project

Then add this to link the library

```cmake
# Add User Libraries
add_subdirectory(lib/EmbeddedLib)
add_subdirectory(lib/WireLib)
add_subdirectory(lib/ActionLib)

add_library(shared_library INTERFACE)

target_include_directories(shared_library INTERFACE
    ${CMAKE_SOURCE_DIR}/Core/Inc
    ${CMAKE_SOURCE_DIR}/USB_DEVICE/App
    ${CMAKE_SOURCE_DIR}/USB_DEVICE/Target
    ${CMAKE_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Inc
    ${CMAKE_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy
    ${CMAKE_SOURCE_DIR}/Middlewares/ST/STM32_USB_Device_Library/Core/Inc
    ${CMAKE_SOURCE_DIR}/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc
    ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/Device/ST/STM32F4xx/Include
    ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/Include
)

target_compile_definitions(shared_library INTERFACE
    STM32CUBE
    STM32F446xx
)

target_link_libraries(EmbeddedLib PRIVATE shared_library)
target_link_libraries(WireLib PRIVATE shared_library)
target_link_libraries(ActionLib PRIVATE shared_library)
```

## Examples

### Printing Hello World 

This example prints `Hello World` Every second

```c++
void init()
{
    // Initialize the timestamps for ActionManager
    ActionManager::init();

    // ...

    // Create the Action
    Action say_hello = Action(1);

    // Link its actual callback method
    say_hello.link_callback(
        [](double timestamp, double time_since_last) -> StatusedValue<bool>
        {     
            Serial::info("Hello world!");

            return StatusedValue<bool>(false, StatusCode::OK);
        }
    );

    // Add it to the list of actions
    ActionManager::add(say_hello);

} // end of "init()"


void update()
{
    ActionManager::update();
    // ...

} // end of "update()"
```