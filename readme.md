# PCB Tracer

PCB Tracer is an application for analyzing and documenting PCB layouts. This tool is designed to aid in reverse engineering, repair, and documentation of printed circuit boards.

This is a work in progress! I'm developing it while working on the schematic for a DPC-200. It's functional, but it can have bugs. If you find any please report it! Same thing if you can think of a new feature for it. :) 

![screen0](https://github.com/user-attachments/assets/f8b2fe3f-dae3-4976-bfea-9caabee10899)
![screen1](https://github.com/user-attachments/assets/b42f3cee-838d-4a5d-b04b-6c4ed99bfbda)
![screen4](https://github.com/user-attachments/assets/31f175ce-05c7-457d-8451-7c6b5f5a0895)


## Features

- Load and display PCB images (front and back sides)
- Trace circuit paths with adjustable track width
- Add and manage component annotations
- Generate connection lists
- Configurable color schemes

## Installing

### On Windows:

Just download the portable version for Windows from the [releases page](https://github.com/rpelorosso/pcb-tracer/releases).

### On Arch Linux:

Use the `pcb-tracer-git` package from the AUR.

## Compilation Instructions

### On Arch Linux:

1. Install the required dependencies:
   ```
   sudo pacman -S cmake qt6-base
   ```
2. Clone the repository:
   ```
   git clone https://github.com/rpelorosso/pcb-tracer.git
   cd pcb-tracer
    ```
4. Create a build directory and navigate to it:
   ```
   mkdir build
   cd build
   ```
5. Run CMake and compile:
   ```
   cmake ..
   make -j$(nproc)
   ```
7. Run the application:
   ```
   ./pcb-tracer
   ```

## Usage

1. Launch PCB Tracer
2. Load PCB images using File -> Set front side image and File -> Set back side image
3. Use the toolbar to switch between track, component, and note modes
4. Trace circuits, add components, and annotate as needed
5. Generate connection lists using PCB -> View Connections
6. Save your work using File -> Save or File -> Save As

### Tracing tracks

To trace a track, enable the Tracks mode, and click the starting pin of the trace. While pressing shift, click on the rest of the pins of the track. Pressing F and B toggles the side of the board.

https://github.com/user-attachments/assets/c98fdf31-a586-447c-b870-3e95d4f561ad

### Adding ICs and components with rows of pins

To add ICs and components with multiple pins, enable the Components mode, then click on the first pin, and while pressing shift, click the ending pin of the side row. Repeat for each side, then press enter. Then indicate the name and the number of pins.

https://github.com/user-attachments/assets/677fb8e3-78e9-42f1-ba15-c67a0845fcb9

### Adding components with single pins (resistors, diodes, transistors, etc)

Enable the Components mode, then click to add each pin. Then press enter and name the component.

https://github.com/user-attachments/assets/4ca3e80e-738e-4506-b21e-adfbdfa026d8



## Keyboard Shortcuts

- Ctrl+C: Enter Component mode
- Ctrl+T: Enter Track mode
- Ctrl+N: Enter Notes mode
- Ctrl+E: View Connections
- Ctrl+Z: Undo
- Ctrl+Y: Redo

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

## License

This project is licensed under the GNU License.

## Support

For bug reports and feature requests, please use the GitHub Issues tracker.

## Roadmap

- When two links cross in the same side, ask the user if they should effectively be joined.
- Configuration changes should trigger a new action so it appears in the undo stack.
- Automate creation of meshes and schematics in KiCad.

