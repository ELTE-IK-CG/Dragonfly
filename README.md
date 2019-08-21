# OpenGL Framework

Utility library with focus on user interface, debugging, and high performace.

## Features

 - OpenGL Glew with SDL 2
 - ImGui with docking and detachable windows (multicontext). Included addons:
    - Text Editor (with fast GLSL systax highlight)
    - Node Editor (WIP)
    - ImGui::Auto (WIP)
    - File Browser and `std::string` support
 - ElteCG opengl wrapper (very WIP, harly works)
    - OpenGL objects are (template) classes.
    - Low overhead, extra checks in Debug mode.
 - Higher level OpenGL utility libery (WIP, but going well) 
    - Classes have high level funtionality but low overhead when they don't use UI. Easy and safe to use.
    - Program classes: Shader file management, GLSL code editors, Uniform monitoring, ect. (template magic)
    - Camera (WIP)

## Install

 - Download the [GLFrameworkPack.zip](https://drive.google.com/file/d/1qlTScrB4aPtDfvpRD7IJUWWI4b4FYfZn/view?usp=sharing).
 - Find a location and create a virtual T drive, eg.: `subst T: c:\MyT_Drive\`.
 - Unzip the `GLFrameworkPack` there. There should be 3 folders in `T:\GLFrameworkPack`.
