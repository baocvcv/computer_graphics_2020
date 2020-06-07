#pragma once

#include <image.hpp>
#include <algorithm>
#include <queue>
#include <cstdio>

class Element {
public:
    virtual void draw(Image &img) = 0;
    virtual ~Element() = default;
};

class Line : public Element {

public:
    int xA, yA;
    int xB, yB;
    Vector3f color;
    void draw(Image &img) override {
        printf("Draw a line from (%d, %d) to (%d, %d) using color (%f, %f, %f)\n", xA, yA, xB, yB,
                color.x(), color.y(), color.z());
        
        if (xA == xB) { // vertical line
            for (int y = yA; y <= yB; y++) {
                img.SetPixel(xA, y, color);
            }
            return;
        }
        if (yA == yB) { // horizontal line
            for (int x = xA; x <= xB; x++) {
                img.SetPixel(x, yA, color);
            }
            return;
        }

        int dx = xB - xA;
        int dy = yB - yA;
        double k = dy * 1.0 / dx;
        if (-1 <= k && k <= 1) {
            if (xB < xA) {
                std::swap(xA, xB);
                std::swap(yA, yB);
            }
            if (k > 0) {
                double e = -.5;
                for (int x = xA, y = yA; x <= xB; x++) {
                    img.SetPixel(x, y, color);
                    e += k;
                    if (e >= 0) {
                        y++, e--;
                    }
                }
            } else {
                double e = .5;
                for (int x = xA, y = yA; x <= xB; x++) {
                    img.SetPixel(x, y, color);
                    e += k;
                    if (e <= 0) {
                        y--, e++;
                    }
                }
            }
        } else {
            if (yB < yA) {
                std::swap(xA, xB);
                std::swap(yA, yB);
            }
            k = dx * 1. / dy;
            if (k > 0) {
                double e = -.5;
                for (int y = yA, x = xA; y <= yB; y++) {
                    img.SetPixel(x, y, color);
                    e += k;
                    if (e >= 0) {
                        x++, e--;
                    }
                }
            } else {
                double e = .5;
                for (int y = yA, x = xA; y <= yB; y++) {
                    img.SetPixel(x, y, color);
                    e += k;
                    if (e <= 0) {
                        x--, e++;
                    }
                }
            }
        }
        
    }
};

class Circle : public Element {

public:
    int cx, cy;
    int radius;
    Vector3f color;
    void draw(Image &img) override {
        printf("Draw a circle with center (%d, %d) and radius %d using color (%f, %f, %f)\n", cx, cy, radius,
               color.x(), color.y(), color.z());

        int d = 5 - 4 * radius;
        draw_circle_points(0, radius, img);
        for (int x = 0, y = radius; x <= y; x++) {
            if (d < 0) {
                d += 8 * x + 12;
            } else {
                d += 8 * (x - y) + 20;
                y--;
            }
            draw_circle_points(x, y, img);
        }
    }

    void draw_circle_points(int x, int y, Image& img) {
        img.SetPixel(x + cx, y + cy, color); img.SetPixel(y + cx, x + cy, color);
        img.SetPixel(-x + cx, y + cy, color); img.SetPixel(y + cx, -x + cy, color);
        img.SetPixel(x + cx, -y + cy, color); img.SetPixel(-y + cx, x + cy, color);
        img.SetPixel(-x + cx, -y + cy, color); img.SetPixel(-y + cx, -x + cy, color);
    }
};

class Fill : public Element {

public:
    int cx, cy;
    Vector3f color;
    void draw(Image &img) override {
        printf("Flood fill source point = (%d, %d) using color (%f, %f, %f)\n", cx, cy,
                color.x(), color.y(), color.z());
        
        int dx[] = { 1, 0,-1, 0 };
        int dy[] = { 0, 1, 0,-1 };
        int width = img.Width();
        int height = img.Height();

        std::queue<std::pair<int, int>> que;
        auto old_color = img.GetPixel(cx, cy);
        img.SetPixel(cx, cy, color);
        que.emplace(cx, cy);
        while (!que.empty()) {
            auto& pt = que.front();
            for (int dir = 0; dir < 4; dir++) {
                int x = pt.first + dx[dir];
                int y = pt.second + dy[dir];
                bool in_bound = 0 <= x && x < width && 0 <= y && y < height;
                if (in_bound && img.GetPixel(x, y) == old_color) {
                    img.SetPixel(x, y, color);
                    que.emplace(x, y);
                }
            }
            que.pop();
        }
    }
};