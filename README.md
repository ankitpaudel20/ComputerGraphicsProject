# Computer Graphics Project

OBJ renderer made with software renderer from scratch.

![Top Language][Top Language]&nbsp;
![GitHub Repo Size][GitHub Repo Size]&nbsp;
![Git][Git]&nbsp;
![Visual Studio Code][Visual Studio Code]&nbsp;
![Visual Studio 2019][Visual Studio 2019]&nbsp;

![Sample][Sample]

## Dependencies
    glfw, glew
### Linux
    Install it from package managers of rexpective distros.
### Windows
#### MSVC
    * Install it from vgpkg to use in visual studio with msvc.
    OR
    * You can manually download includes and library files from their respective sites 
        and manually link them by editing include and library directories in CMakeLists.txt.
#### GCC
    *If you are using GCC, then you are well off using msys2 for both compiler and dependencies as it has pacman built in for all the developmental libraries
    

## Building/Running
    Works best on GCC with -O3 optimization
    * Clone this repo to your PC.
    * Go to its project directory.
    * Configure CMake with Visual Studio Code (Vscode must have installed `Cmake tools` and `Cmake` extensions), Visual Studi oor any other Ide supporting Cmake.
    * Build and run the project.

## Documentation

The detailed documentation is available [here.][Documentation]

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

[Top Language]: https://img.shields.io/github/languages/top/ankitpaudel20/ComputerGraphicsProject?style=flat-square
[GitHub Repo Size]: https://img.shields.io/github/repo-size/ankitpaudel20/ComputerGraphicsProject?style=flat-square&logo=GitHub
[Git]: https://img.shields.io/badge/-Git-F05032?style=flat-square&logo=git&logoColor=ffffff
[Visual Studio Code]: https://img.shields.io/badge/-Visual%20Studio%20Code-007ACC?style=flat-square&logo=visual-studio-code&logoColor=ffffff
[Visual Studio 2019]: https://img.shields.io/badge/-Visual%20Studio%202019-5C2D91?style=flat-square&logo=visual-studio&logoColor=ffffff
[Documentation]: https://ankitpaudel20.github.io/ComputerGraphicsProject/structengine.html
[Sample]: ./demo/demo.gif "Sample Output"
