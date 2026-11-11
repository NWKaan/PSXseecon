# PSXseecon

PSXseecon connects Aerowinx Precision Simulator 10 (PSX) to OpenCockpits SIOC. It exchanges data with PSX over the Aerowinx Network Protocol and with the SIOC Server over IOCP, both using TCP/IP.

## Build and package

1. Open `PSXseecon.sln` in Visual Studio and build the **Production | x64** configuration. The project uses the dependencies listed in `vcpkg.json`.
2. Create a ZIP file containing the resulting `PSXseecon.exe`, both font files from `PSXseecon/assets/fonts/`, the [manual](PSXseecon/docs/PSXseecon_Manual.pdf), and the [EULA](PSXseecon/docs/EULA.pdf).
3. Place `awnxmcduS_101.TTF` and `awnxmcduL_101.TTF` in the same folder as `PSXseecon.exe` inside the ZIP. PSXseecon loads these fonts from beside the executable.

The ZIP is the installation package. Extract it to a folder before running `PSXseecon.exe`.

## Configuration

You can configure PSXseecon further with a `PSXseecon.cfg` file. See the [manual](PSXseecon/docs/PSXseecon_Manual.pdf) for the available settings and instructions. The local configuration file is excluded from Git.

## Generate the SIOC variable list

To include `SIOC_variables_by_Qcode.txt` in the installation ZIP:

1. Enable `#define LIST_SIOC_VARS` in `PSXseecon/include/condcomp.hpp` by removing the `//` at the start of that line.
2. Build the **Development | x64** configuration and start that build of PSXseecon.
3. Find the generated `SIOC_variables_by_Qcode.txt` in the program's working directory and add it to the ZIP.

The variable-list build generates the file at startup and then exits.
