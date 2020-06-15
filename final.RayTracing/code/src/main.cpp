#include "common.hpp"
#include "scenes.hpp"
#include "image.hpp"
#include "camera.hpp"
#include "object3d.hpp"
#include "group.hpp"
#include "ray_tracer.hpp"
#include "scene_parser.hpp"

using namespace std;

int main(int argc, char *argv[]) {
    // for (int argNum = 1; argNum < argc; ++argNum) {
    //     std::cout << "Argument " << argNum << " is: " << argv[argNum] << std::endl;
    // }

    // if (argc != 3) {
    //     cout << "Usage: ./bin/PA1 <input scene file> <output bmp file>" << endl;
    //     return 1;
    // }
    // string inputFile = argv[1];
    if (argc != 2) {
        cout << "Usage: ./bin/FINAL <output file>" << endl;
        return 1;
    }
    string outputFile = argv[1];  // only bmp is allowed.

    // SceneParser sp(inputFile.c_str());

    Image outImg;
    Scene sc = getScene2();
    renderFrame(sc, outImg, 10);
    // auto sp("../testcases/scene01_basic.txt");
    // renderFrame(sp, outImg, 40);
    outImg.SaveImage(outputFile.c_str());

    // cout << "Hello! Computer Graphics!" << endl;
    return 0;
}

