#pragma once

#include <vector>
#include <expected>
#include <cstdint>
#include <string>


class Image {
    private:
        static constexpr float _grayscale_Rlinear = 0.2126;
        static constexpr float _grayscale_Glinear = 0.7152;
        static constexpr float _grayscale_Blinear = 0.0722;

        int _width {}, _height {}, _channels {};
        uint8_t *_data {nullptr};
        std::size_t _size {};

    public:
        enum class ImageFormat : uint8_t {
            kJPG = 0,
            kPNG,
            kBMP,
            kTGA, 
            kUNKNOWN
        };

        struct Dimensions {
            int   _width  {};
            int   _height {};
            int _channels {};
            std::size_t _size {};
        };

        Image(const char *filename);
        Image(int width, int height, int channels);
        Image(const Image &Image);
        ~Image();

        Dimensions getDimensions() const noexcept;

        std::expected<uint8_t, std::string>  readFile(const char *filename);
        std::expected<uint8_t, std::string> writeFile(const char *filename);

        ImageFormat detectImageFormat(const char *filename);

};