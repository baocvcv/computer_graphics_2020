#ifndef IMAGE_H
#define IMAGE_H

#include <cassert>
#include <vecmath.h>

// Simple image class
class Image {

public:

    Image() : width(0), height(0), data(nullptr) {}

    Image(int w, int h) {
        width = w;
        height = h;
        data = new Vec3[width * height];
    }

    ~Image() {
        delete[] data;
    }

    void SetSize(int w, int h) {
        assert(w > 0 && h > 0);
        if (data == nullptr)
            delete[] data;
        width = w;
        height = h;
        data = new Vec3[width * height];
    }

    int Width() const {
        return width;
    }

    int Height() const {
        return height;
    }

    const Vec3 &GetPixel(int x, int y) const {
        assert(x >= 0 && x < width);
        assert(y >= 0 && y < height);
        return data[y * width + x];
    }

    void SetAllPixels(const Vec3 &color) {
        for (int i = 0; i < width * height; ++i) {
            data[i] = color;
        }
    }

    void SetPixel(int x, int y, const Vec3 &color) {
        assert(x >= 0 && x < width);
        assert(y >= 0 && y < height);
        data[y * width + x] = color;
    }

    void IncrementPixel(int x, int y, const Vec3 &color) {
        assert(x >= 0 && x < width);
        assert(y >= 0 && y < height);
        data[y * width + x] += color;
    }

    static Image *LoadPPM(const char *filename);

    void SavePPM(const char *filename) const;

    static Image *LoadTGA(const char *filename);

    void SaveTGA(const char *filename) const;

    int SaveBMP(const char *filename);

    void SaveImage(const char *filename);

private:

    int width;
    int height;
    Vec3 *data;

};

#endif // IMAGE_H