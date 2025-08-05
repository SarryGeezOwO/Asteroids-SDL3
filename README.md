# Basic C++ project template

### Folder structure:
 - `external/` - This directory can be used to store third-party libraries or external dependencies.
    - `include/` - This directory holds the header files (`.h`, `.hpp`, etc.) that declare classes, functions, and data structures. These headers are often intended for public consumption by other parts of the project or by external users if the project is a library.
    - `lib/` - If the project uses or produces static libraries, this directory can store them.
 - `out/` - Contains any related content for builds and outputs of the program
    - `bin/` - This directory stores the compiled executable files or dynamic libraries (DLLs on Windows, shared objects on Linux) generated during the build process.
    - `build/` - This directory serves as a temporary location for intermediate build files, such as object files (`.o`, `.obj`) and other artifacts created by the build system (e.g., CMake, Make). This directory is usually cleaned or regenerated during a "clean" build.
- `app/` - This directory contains all contents regarding about the application creation i.e., source files and unit tests.
    - `src/` - This directory contains the primary source code files (`.cpp`, `.cxx`, etc.) that implement the project's logic.
    - `tests/` - This directory contains unit tests or integration tests for the project's components.

### files
 - `README.md` - readme file for this project
 - `makefile` - this template uses `make` and MSYS2. The makefile also uses `ccacche` for faster and convienient build time.