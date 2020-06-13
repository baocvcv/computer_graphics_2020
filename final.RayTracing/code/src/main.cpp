#include "scene_parser.hpp"
#include "image.hpp"
#include "camera.hpp"
#include "object3d.hpp"
#include "light.hpp"
#include "group.hpp"

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <iostream>
#include <vector>
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

    SceneParser sp(inputFile.c_str());
    // Group *group = sp.getGroup();
    Camera *cam = sp.getCamera();
    // int width = cam->getWidth();
    // int height = cam->getHeight();
    // int numlights = sp.getNumLights();
    // vector<Light*> lights;
    // for (int i = 0; i < numlights; i++) lights.push_back(sp.getLight(i));

    // TODO: put this function in Camera?
    // TODO: allow selection of different algorithms
    Image outImg;
    // Vector3f bkgColor = sp.getBackgroundColor();
    // double tmin = 1e-6;
    // for (int x = 0; x < width; x++) {
    //     for (int y = 0; y < height; y++) {
    //         Hit h;
    //         Ray r = cam->generateRay(Vector2f(x, y));
    //         if (group->intersect(r, h, tmin)) {
    //             Vector3f color = Vector3f::ZERO;
    //             for (auto light: lights) {
    //                 Vector3f lightColor, dirToLight;
    //                 Vector3f p = r.pointAtParameter(h.t);
    //                 light->getIllumination(p, dirToLight, lightColor);
    //                 color += h.material->Shade(r, h, dirToLight, lightColor);
    //             }
    //             outImg.SetPixel(x, y, color);
    //         } else { outImg.SetPixel(x, y, bkgColor); }
    //     }
    // }

    cam->renderFrame(sp, outImg, 400);
    outImg.SaveImage(outputFile.c_str());

    // cout << "Hello! Computer Graphics!" << endl;
    return 0;
}

