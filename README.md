# Interactive Mandelbrot Render In SDL

Good way to stress test your PC: How many MAX_ITER can you smoothly run.

Use mouse scroll wheel to go in/out.

![Render1](https://github.com/cjrt/mandelbrot-render/blob/main/pics/mandelbrot.png)

# Formulas
"pixel_to_complex" function

![Formula1](https://github.com/cjrt/mandelbrot-render/blob/main/pics/mandelbrotiterationformula.png)

"mandelbrot" function

![Formula2](https://github.com/cjrt/mandelbrot-render/blob/main/pics/pixeltocomplexformula.png)

# Building 

Requires SDL2 & OpenMP for multiprocessing.

```bash
# Install dependencies [Ubuntu/Debian]
sudo apt update
sudo apt install build-essential libsdl2-dev libomp-dev

# Install dependencies [Fedora]
sudo dnf install @development-tools SDL2-devel libomp

# Install dependencies [Arch Linux]
sudo pacman -S base-devel sdl2 libomp

# Build the project
make run
```

# Images
![Render2](https://github.com/cjrt/mandelbrot-render/blob/main/pics/mandelbrot2.png)
![Render3](https://github.com/cjrt/mandelbrot-render/blob/main/pics/mandelbrot3.png)
![Render4](https://github.com/cjrt/mandelbrot-render/blob/main/pics/mandelbrot4.png)
