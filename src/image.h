#ifndef IMAGE_H_
#define IMAGE_H_

#include "triple.h"

#include <string>
#include <vector>

class Image {
    std::vector<Color> pixels;
public:
    unsigned width;
    unsigned height;

    Image(unsigned width = 0, unsigned height = 0);

    // Pixel accessors
    // Usage: color = img(x,y);
    //        img(x,y) = color;
    Color const &operator()(unsigned x, unsigned y) const;
    Color &operator()(unsigned x, unsigned y);

    void write_png(std::string const &filename) const;

private:
    unsigned index(unsigned x, unsigned y) const {
        return y * width + x;
  }
};

#endif
