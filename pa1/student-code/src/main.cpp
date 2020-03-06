#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <iostream>

#include "scene_parser.hpp"
#include "image.hpp"
#include "camera.hpp"
#include "group.hpp"
#include "light.hpp"

#include <string>

using namespace std;

int main(int argc, char *argv[]) {
    for (int argNum = 1; argNum < argc; ++argNum) {
        std::cout << "Argument " << argNum << " is: " << argv[argNum] << std::endl;
    }

    if (argc != 3) {
        cout << "Usage: ./bin/PA1 <input scene file> <output bmp file>" << endl;
        return 1;
    }
    string inputFile = argv[1];
    string outputFile = argv[2];  // only bmp is allowed.

    // TODO: Main RayCasting Logic
    // First, parse the scene using SceneParser.
    SceneParser sp(inputFile.c_str());

    // Then loop over each pixel in the image, shooting a ray
    // through that pixel and finding its intersection with
    // the scene.  Write the color at the intersection to that
    // pixel in your output image.
    Group* group = sp.getGroup();
    Camera* cam = sp.getCamera();
    int width = cam->getWidth();
    int height = cam->getHeight();

    int numLights = sp.getNumLights();
    vector<Light*> lights;
    for (int i = 0; i < numLights; i++)
        lights.push_back(sp.getLight(i));

    Image outImage(width, height);
    Vector3f backGroundColor = sp.getBackgroundColor();
    float tmin = 1e-6;
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            Hit h;
            Ray r = cam->generateRay(Vector2f(x, y));
            if (group->intersect(r, h, tmin)) {
                Vector3f color = Vector3f::ZERO;
                for (auto light: lights) {
                    Vector3f lightColor, dirToLight;
                    Vector3f p = r.pointAtParameter(h.getT());
                    light->getIllumination(p, dirToLight, lightColor);
                    color += h.getMaterial()->Shade(r, h, dirToLight, lightColor);
                }
                outImage.SetPixel(x, y, color);
            } else {
                outImage.SetPixel(x, y, backGroundColor);
            }
        }
    }
    outImage.SaveImage(outputFile.c_str());

    cout << "Hello! Computer Graphics!" << endl;
    return 0;
}
