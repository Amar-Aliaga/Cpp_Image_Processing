# Cpp_Image_Processing

A modern C++23 project for basic image processing operations (grayscale, blur, crop, flip, etc.) using stb_image and stb_image_write for image I/O. The project is modular, CMake-based, and easy to extend.

---

## Project Structure

```
Cpp_Image_Processing/
├── app/
│   ├── main.cpp              # Example application using the image processing library
│   └── CMakeLists.txt        # Build instructions for the app
├── build/                    # CMake build output (ignored in VCS)
├── external/
│   └── stb/
│       ├── stb_image.h       # stb_image header (image loading)
│       └── stb_image_write.h # stb_image_write header (image saving)
├── image_processing/
│   ├── include/
│   │   └── image_processor.hpp # Image class and API declarations
│   ├── src/
│   │   └── image_processor.cpp # Image class implementation (filters, I/O, etc.)
│   └── CMakeLists.txt          # Build instructions for the library
├── result/
│   └── processed_image.jpg   # Output image after processing
├── CMakeLists.txt            # Top-level CMake configuration
├── LICENSE                   # Project license
└── README.md                 # This file
```

---

## Features

- **Image loading/saving**: Supports JPG, PNG, BMP, TGA via stb_image.
- **Image filters**:
  - Grayscale (average & luminance)
  - Mean blur (box blur)
  - Color mask
  - Flip (horizontal/vertical)
  - Rotate 90°
  - Invert colors
  - Sepia tone
  - Crop
- **Easy to extend**: Add your own filters in `image_processor.cpp`.

---

## How to Build

1. **Clone the repo**  
   ```bash
   git clone https://github.com/Amar-Aliaga/Cpp_Image_Processing.git
   cd Cpp_Image_Processing
   ```

2. **Configure and build with CMake**  
   ```bash
   cmake -S . -B build
   cmake --build build --parallel
   ```

3. **Run the example app**  
   ```bash
   ./build/app/image_app
   ```

   The processed image will be saved as `result/processed_image.jpg`.

---

## How it Works

- The main application (`app/main.cpp`) loads an image, applies grayscale and blur filters, and writes the result.
- All image operations are implemented in the `Image` class (`image_processing/include/image_processor.hpp` and `src/image_processor.cpp`).
- The project uses C++23 features and is organized as a static library for easy reuse.

---

## Adding New Filters

1. Implement your filter as a member function in `Image` (in `image_processor.cpp`).
2. Declare it in `image_processor.hpp`.
3. Call it from `main.cpp` or your own app.

---

## Dependencies

- [stb_image](https://github.com/nothings/stb) and [stb_image_write](https://github.com/nothings/stb) (included in `external/stb/`)
- C++23 compatible compiler
- CMake 3.20+

---

## License

See [LICENSE](LICENSE).

---
