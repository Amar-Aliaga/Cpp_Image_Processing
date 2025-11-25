#include "image_processor.hpp"

#include <iostream>
#include <chrono>


Image *measureTime() {
    const char *filename {"/home/amar-aliaga/Downloads/pexels1.jpg"};
    auto start = std::chrono::high_resolution_clock::now();
    std::cout << "loading image..." << std::endl;

    Image *im = new Image(filename);
    im->grayscaleLuminance();
    // im->cropImage(1200, 1200, 1800, 1200);
    int i = 0;
    while(i < 3) {
        im->meanBlurr();
        ++i;
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end-start);
    std::cout << "Load time: " << duration.count() << "ms" << std::endl;
    
    // Clean up the dynamically allocated image
    return im;
}


int main() {

    // const char *filename {"/home/amar-aliaga/Downloads/pexels1.jpg"};
    
    Image *im = measureTime();
    
    auto result = im->writeFile("result/processed_image.jpg");
    if (result) {
        std::cout << "Successfully wrote image to processed_image.jpg" << std::endl;
    } else {
        std::cerr << "Failed to write image: " << result.error() << std::endl;
        return 1;
    }

    auto dimensions = im->getDimensions();
    std::cout << "Height: " << dimensions._height << std::endl;
    std::cout << "Width: " << dimensions._width << std::endl;
    std::cout << "Channels: " << dimensions._channels << std::endl;
    std::cout << "Size: " << dimensions._size / 1000000 << " MB"<< std::endl;

    delete im;

    return 0;
}