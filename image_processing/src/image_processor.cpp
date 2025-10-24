#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "image_processor.hpp"
#include "stb_image.h"
#include "stb_image_write.h"

#include <iostream>
#include <expected>
#include <algorithm>


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


Image::Image(int width, int height, int channels) :_width(width), _height(height), _channels(channels) {
    _size = _width * _height * _channels;
    _data = new uint8_t[_size];
}


Image::Image(const Image &image) : Image(image._width, image._height, image._channels) {
    if(image._data) {
        _data = new uint8_t[_size];
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
