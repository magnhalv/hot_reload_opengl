# Hot reload OpenGL


## Build

This project is built using cmake. Recommended is using ninja and git bash. Example below using visual studio build tools: 

```
cmd.exe /c "\"C:\\Program Files (x86)\\Microsoft Visual Studio\\2022\\BuildTools\\VC\\Auxiliary\\Build\\vcvars64.bat\" && bash"
cmake -S . -B build -G "Ninja"
cmake --build build
cmake --build build --target <hot_reload_opengl|engine_dyn|tests>
```

An example project for how to set up OpenGl with hot reloading.

## TODO 

* Select assets to "build":
    * Click on button
    * See tranparent asset under your cursor
    * Left mouse to place
    * Click "r" to rotate 90 degrees

* Global MemoryArena handling
* Make shaders global

### GUI

* Window

### Cli

* Add separate color for input text
* Add auto-complete
* Add help
