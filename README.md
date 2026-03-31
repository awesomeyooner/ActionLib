# ActionLib
PlatformIO library designed to help with asyncronous execution of code to help avoid blocking functions in time-critical methods, particularly with communications

## Examples

### Printing Hello World 

This example prints `Hello World` Every second

```c++

// core_init()

Action say_hello = Action(1);

say_hello.link_callback(
    [](double timestamp, double time_since_last) -> StatusedValue<bool>
    {     
        Serial::info("Hello world!");

        return StatusedValue<bool>(false, StatusCode::OK);
    }
);

ActionManager::add(say_hello);
```