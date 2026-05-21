#include "image.h"

#include "lode/lodepng.h"

using namespace std;

Image::Image(unsigned width, unsigned height)
:   pixels(width * height),
    width(width),
    height(height)
{}

// Pixel accessors
// Usage: color = img(x,y);
//        img(x,y) = color;
Color const &Image::operator()(unsigned x, unsigned y) const {
    return pixels.at(index(x, y));
}

Color &Image::operator()(unsigned x, unsigned y) {
    return pixels.at(index(x, y));
}

void Image::write_png(std::string const &filename) const {
    vector<unsigned char> image;
    image.reserve(pixels.size() * 4); // reserves size (fewer allocations)
    for (Color pixel : pixels) {
        image.push_back(static_cast<unsigned char>(pixel.r * 255.0));
        image.push_back(static_cast<unsigned char>(pixel.g * 255.0));
        image.push_back(static_cast<unsigned char>(pixel.b * 255.0));
        image.push_back(255); // alpha is always 1
    }

    lodepng::encode(filename, image, width, height);
}
