# Basic Raytracer

A simple CPU raytracer implementation based on the excellent *"Ray Tracing in One Weekend"* book by Peter Shirley ([no seriously, you should read it](https://raytracing.github.io/books/RayTracingInOneWeekend.html)).

After completing the book, I have expanded the software with the following features:
* A **command-line interface** to provide some configurable parameters to the renderer
* **Multithreading** support (unlimited number of threads)
* **PPM P6** binary encoding for more compact render outputs (instead of plain P3 ASCII encoding)
* Display render progress and total time to complete

Some additional features are planned but haven't been yet implemented, such as:
* YAML or JSON-based **scene file** support
* Refactoring and porting to pure **C++20**

![Rendered image](/render.png)

## How to run

Currently, no build system has been integrated. 
To build the project, clone the repository and open it in **Visual Studio 2019** (with *C++20* support enabled), from where it can be built and run without any additional configuration.

Command-line usage:
> basic-raytracer.exe \<width\> \<height\> \[-s/--samples \<value\>\] \[-b/--bounces \<value\>\] \[-t/--threads \<value\>\]
