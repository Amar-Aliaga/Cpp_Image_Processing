#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "image_processor.hpp"
#include "stb_image.h"
#include "stb_image_write.h"

#include <iostream>
#include <expected>
#include <algorithm>
#include <memory>


std::expected<uint8_t, std::string> Image::readFile(const char *filename) {
    if(filename == nullptr || filename[0] == '\0') {
        return std::unexpected("Couldn't load file.");
    }

    if(_data) {
        stbi_image_free(_data);
    }

    _data = stbi_load(filename, &_width, &_height, &_channels, 0);
    
    if(_data == nullptr) {
        return std::unexpected("Failed to load image.\n");
    }

    return static_cast<uint8_t>(1);
}


Image::Image(const char *filename) : _width(0), _height(0), _channels(0), _size(0), _data(nullptr) {

    auto result = readFile(filename);
    if(result) {
        std::cout << "Successfully loaded " << filename << "\n";
        _size = _width * _height * _channels;
    } else {
        std::cerr << "Failed to read from file: " << result.error() << "\n";
    }
}


Image::Image(int width, int height, int channels) :
    _width(width), _height(height), _channels(channels), _size(_width * _height * _channels) {

    _data = new uint8_t[_size];
}


Image::Image(const Image &image) : Image(image._width, image._height, image._channels) {
    if(image._data) {
        _data = new uint8_t[_size];
        std::copy(image._data, image._data + _size, _data);
    }else {
        _data = nullptr;
    }
}


Image::~Image() {
    stbi_image_free(_data);
}


Image::Dimensions Image::getDimensions() const noexcept {
    return {_width, _height, _channels, _size};
}


Image::ImageFormat Image::detectImageFormat(const char *filename) {
    if(!filename) {
        return ImageFormat::kUNKNOWN;
    }

    std::string file(filename);

    auto pos = file.rfind('.');
    if(pos == std::string::npos) {
        return ImageFormat::kUNKNOWN;
    }

    std::string format = file.substr(pos + 1);
    std::transform(format.begin(), format.end(), format.begin(), ::tolower);

    if(format == "jpg" || format == "jpeg") {
        return ImageFormat::kJPG;
    } 
    if(format == "png") {
        return ImageFormat::kPNG;
    }
    if(format == "bmp") {
        return ImageFormat::kBMP;
    }
    if(format == "tga") {
        return ImageFormat::kTGA;
    }

    return ImageFormat::kUNKNOWN;
}


std::expected<uint8_t, std::string> Image::writeFile(const char *filename) {
    if(filename == nullptr || filename[0] == '\0') {
        return std::unexpected("Couldn't load file.");
    }

    if(_data == nullptr) {
        return std::unexpected("No image data to write.");
    }

    ImageFormat image_format = detectImageFormat(filename);
    int result {0};

    switch (image_format) {
    case ImageFormat::kJPG:
        result = stbi_write_jpg(filename, _width, _height, _channels, _data, 100);
        break;
    case ImageFormat::kPNG:
        result = stbi_write_png(filename, _width, _height, _channels, _data, _width * _channels);
        break;
    case ImageFormat::kBMP:
        result = stbi_write_bmp(filename, _width, _height, _channels, _data);
        break;
    case ImageFormat::kTGA:
        result = stbi_write_tga(filename, _width, _height, _channels, _data);
        break;
    case ImageFormat::kUNKNOWN:
        return std::unexpected("Unknown image format.");
    }
    
    if(result == 0) {
        return std::unexpected("Failed to write image file.");
    }
    
    return static_cast<uint8_t>(1);
}


Image &Image::grayscaleAverage() {
    if (_channels < 3) {
        std::cerr << "Image has less than 3 channels." << "\n";
        return *this;
    }

    for(size_t i = 0; i < _size; i += _channels) {
        uint8_t rgb = (_data[i] + _data[i+1] + _data[i+2]) / 3;
        _data[i]   = rgb;
        _data[i+1] = rgb;
        _data[i+2] = rgb;
    }
    return *this;
}


Image &Image::grayscaleLuminance() {
    if (_channels < 3) {
        std::cerr << "Image has less than 3 channels." << "\n";
        return *this;
    }

    for(size_t i = 0; i < _size; i += _channels) {
        uint8_t rgb = ((0.299 * _data[i]) + (0.587 * _data[i+1]) + (0.114 * _data[i+2]));
        _data[i]   = rgb;
        _data[i+1] = rgb;
        _data[i+2] = rgb;
    }
    return *this;
}


Image &Image::colorMask(uint8_t r, uint8_t g, uint8_t b) {
    if (_channels < 3) {
        std::cerr << "Image has less than 3 channels." << "\n";
        return *this;
    }

    for(size_t i = 0; i < _size; i += _channels) {
        _data[i]   *= r;
        _data[i+1] *= g;
        _data[i+2] *= b;
    }
    return *this;
}


Image &Image::flipX() {
    if(_channels < 3) {
        return *this;
    }

    for(int row = 0; row < _height; ++row) {
        for(int col = 0; col < _width/2; ++col) {
            auto left_side  = (row * _width + col) * _channels;
            auto right_side = (row * _width + (_width - 1 - col)) * _channels;
            for(uint8_t rgb = 0; rgb < _channels; ++rgb) {
                std::swap((_data[left_side + rgb]), _data[right_side + rgb]);
            }
        }
    }
    return *this;
}


Image &Image::flipY() {
    if(_channels < 3) {
        return *this;
    }

    for(int row = 0; row < _height/2; ++row) {
        for(int col = 0; col < _width; ++col) {
            auto upper_side = (row * _width + col) * _channels;
            auto lower_side = ((_height - 1 - row) * _width + col) * _channels;
            for(uint8_t rgb = 0; rgb < _channels; ++rgb) {
                std::swap(_data[upper_side + rgb], _data[lower_side + rgb]);
            }
        }
    }
    return *this;
}


Image &Image::rotate90() {
    if(_channels < 3) {
        return *this;
    }

    //uint8_t *new_data = new uint8_t[_size];
    auto new_data = std::make_unique<uint8_t[]>(_size);
    int height  = _height;
    int new_height = _width;

    for(int row = 0; row < _height; ++row) {
        for(int col = 0; col < _width; ++col) {
            auto source_pixel = (row * _width + col) * _channels;
            auto destination_pixel = (col * height + (height - 1 - row)) * _channels;
            for(int rgb = 0; rgb < _channels; ++rgb) {
                new_data[destination_pixel + rgb] = _data[source_pixel + rgb];
            }
        }
    }

    stbi_image_free(_data);
    _data = new_data.release();
    _width = height;
    _height = new_height;

    return *this;
}


Image &Image::invert() {
    if(_channels < 3) {
        return *this;
    }

    for(size_t i = 0; i < _size; i += _channels) {
        _data[i]   = 255 - _data[i];
        _data[i+1] = 255 - _data[i+1];
        _data[i+2] = 255 - _data[i+2];
    }

    return *this;
}


Image &Image::sepiaTone() {
    if(_channels < 3) {
        return *this;
    }

    for(size_t i = 0; i < _size; i += _channels) {
        float r = (0.393 * _data[i]) + (0.769 * _data[i+1]) + (0.189 * _data[i+2]);
        float g = (0.349 * _data[i]) + (0.686 * _data[i+1]) + (0.168 * _data[i+2]);
        float b = (0.272 * _data[i]) + (0.534 * _data[i+1]) + (0.131 * _data[i+2]);

        _data[i]   = std::clamp(static_cast<int>(r), 0, 255);
        _data[i+1] = std::clamp(static_cast<int>(g), 0, 255);
        _data[i+2] = std::clamp(static_cast<int>(b), 0, 255);
    }

    return *this;
}


Image &Image::cropImage(uint16_t x_cord, uint16_t y_cord, uint16_t width, uint16_t height) {
    if(_channels < 3) {
        return *this;
    }

    std::size_t new_size = width * height * _channels;
    auto new_data = std::make_unique<uint8_t[]>(new_size);

    for(int row = 0; row < height; ++row) {
        for(int col = 0; col < width; ++col) {
            auto destination = ((row + y_cord) * _width + col + x_cord) * _channels; 
            auto source = (row * width + col) * _channels;                     
            for(uint8_t rgb = 0; rgb < _channels; ++rgb) {
                new_data[source + rgb] = _data[destination + rgb];
            }
        }
    }

    stbi_image_free(_data);
    _data = new_data.release();
    _width  = width;
    _height = height;
    _size = new_size;

    return *this;
}


Image &Image::meanBlurr() {
    auto new_data = std::make_unique<uint8_t[]>(_size);

    for(int row = 0; row < _height; ++row) {
        for(int col = 0; col < _width; ++col) {
            // Process each channel separately
            for(int ch = 0; ch < _channels; ++ch) {
                int sum = 0;
                int count = 0;
                
                // 3x3 kernel
                for(int i = -1; i <= 1; ++i) {
                    for(int j = -1; j <= 1; ++j) {
                        int neighbor_row = row + i;
                        int neighbor_col = col + j;
                        
                        // Boundary check
                        if(neighbor_row >= 0 && neighbor_row < _height && 
                           neighbor_col >= 0 && neighbor_col < _width) {
                            int idx = (neighbor_row * _width + neighbor_col) * _channels + ch;
                            sum += _data[idx];
                            count++;
                        }
                    }
                }
                
                int dest_idx = (row * _width + col) * _channels + ch;
                new_data[dest_idx] = static_cast<uint8_t>(sum / count);
            }
        }
    }
    
    stbi_image_free(_data);
    _data = new_data.release();

    return *this;
}

//inference, RTI, UCX, Tensor, holohub all in docker
// Image &Image::testImage() {
//     return *this;
// }
