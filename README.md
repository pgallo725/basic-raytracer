# Basic Raytracer

<p float="left">
  <img src="/images/render_book_1.png" width="450" />
  <img src="/images/render_book_2.png" width="300" /> 
</p>

A simple CPU raytracer implementation based on the excellent *"Ray Tracing in One Weekend"* and *"Ray Tracing: The Next Week"* books by Peter Shirley ([no seriously, you should read them](https://raytracing.github.io)).

After completing the books, I have expanded the software with the following features:
* **Multithreading** support (scalable to an unlimited number of threads)
* JSON-based **scene files**, instead of code-generated scenes
* A **command-line interface** to provide some configurable parameters to the renderer
* Display render progress and total time to complete
* **PPM P6** binary encoding for more compact render outputs (instead of plain P3 ASCII encoding)
* A **data-oriented** rewrite of the raytracer, with additional optimizations, which is available on the `data-oriented` branch. This version runs *~10% faster* than the original version from the book.

## How to run

To build the project, clone the repository and open it in **Visual Studio 2019** (with *C++20* support enabled), from where it can be built and run without any additional configuration.

Command-line usage:
> basic-raytracer.exe \<scene\> \<output\> \<width\> \<height\> \[-s/--samples \<value\>\] \[-b/--bounces \<value\>\] \[-t/--threads \<value\>\]
