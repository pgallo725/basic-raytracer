# Basic Raytracer

A simple CPU raytracer implementation based on the excellent *"Ray Tracing in One Weekend"* book by Peter Shirley ([no seriously, you should read it](https://raytracing.github.io/books/RayTracingInOneWeekend.html)).

After completing the book, I have expanded the software with the following features:
* **Multithreading** support (scalable to an unlimited number of threads)
* JSON-based **scene files**, instead of code-generated scenes
* A **command-line interface** to provide some configurable parameters to the renderer
* Display render progress and total time to complete
* **PPM P6** binary encoding for more compact render outputs (instead of plain P3 ASCII encoding)
* Sprinkled `const`, `noexcept` and `final` all throughout the code, to make the smart C++ people happy


![Rendered image](/images/render.png)

## How to run

To build the project, clone the repository and open it in **Visual Studio 2019** (with *C++20* support enabled), from where it can be built and run without any additional configuration.

Command-line usage:
> basic-raytracer.exe \<scene\> \<output\> \<width\> \<height\> \[-s/--samples \<value\>\] \[-b/--bounces \<value\>\] \[-t/--threads \<value\>\]
