#ifndef _Z_BUFFER_HPP_
#define _Z_BUFFER_HPP_

#include <cmath>
#include <limits>

#include <iostream>

#include <vector>

class Zbuffer {
    size_t width;
    size_t heigth;
    std::vector<std::vector<float>> buffer;

    public:

    Zbuffer(size_t _width, size_t _heigth) {
        width = _width;
        heigth = _heigth;
        std::vector<float> buf_row(heigth, std::numeric_limits<float>::max());
        for(size_t i = 0; i < width; i++) {
            buffer.push_back(buf_row);
        }
    };

    void make_matrix(size_t _w, size_t _h) {
        width = _w;
        heigth = _h;
        std::vector<std::vector<float>> tmp_buffer;
        std::vector<float> buf_row(width, std::numeric_limits<float>::max());
        for(size_t i = 0; i < heigth; i++) {
            buffer.push_back(buf_row);
        }
        buffer = tmp_buffer;
    }

    size_t getWidth() {
        return width;
    }

    size_t getHeigth() {
        return heigth;
    }

    void clear() {
        for(size_t x = 0; x < width; x++) {
            for(size_t y = 0; y < heigth; y++) {
                buffer[x][y] = std::numeric_limits<float>::max();
            }
        }
    }

    void setValue(size_t x, size_t y, float value) {
        buffer[x][y] = value;
    }

    float getValue(size_t x, size_t y) {
        return buffer[x][y];
    }

};

#endif // !_Z_BUFFER_HPP_