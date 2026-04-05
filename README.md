# ActionLib
PlatformIO library designed to help with asyncronous execution of code to help avoid blocking functions in time-critical methods, particularly with communications

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