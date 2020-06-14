#ifndef SCENE_PARSER_H
#define SCENE_PARSER_H

#include "camera.hpp"
#include "object3d.hpp"
#include "helpers.hpp"
#include "group.hpp"
#include "mesh.hpp"
#include "curve.hpp"
#include "revsurface.hpp"
#include "json.hpp"
#include "vec.hpp"
#include "mat44.hpp"

#include <cassert>
#include <vecmath.h>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>

#define MAX_PARSER_TOKEN_LENGTH 1024

class MySceneParser {
public:
    PerspectiveCamera* camera;
    std::vector<Material> materials;
    Group group;
    Group lights;
    nlohmann::json scene;

    MySceneParser() = delete;
    MySceneParser(std::string filename) {
        std::ifstream infile(filename);
        if (!infile.is_open()) {
            std::cout << "Cannot open scene file\n";
        }
        infile >> scene;
    }
};

class SceneParser {
public:

    SceneParser() = delete;
    SceneParser(const char *filename);

    ~SceneParser();

    Camera *getCamera() const {
        return camera;
    }

    Vec3 getBackgroundColor() const {
        return background_color;
    }

    int getNumMaterials() const {
        return num_materials;
    }

    Material *getMaterial(int i) const {
        assert(i >= 0 && i < num_materials);
        return materials[i];
    }

    Group *getGroup() const {
        return group;
    }

    void parseFile();
    void parsePerspectiveCamera();
    void parseBackground();
    void parseLights();
    Light *parsePointLight();
    Light *parseDirectionalLight();
    void parseMaterials();
    Material *parseMaterial();
    Object3D *parseObject(char token[MAX_PARSER_TOKEN_LENGTH]);
    Group *parseGroup();
    Sphere *parseSphere();
    Plane *parsePlane();
    Triangle *parseTriangle();
    Mesh *parseTriangleMesh();
    Transform *parseTransform();
    Curve *parseBezierCurve();
    Curve *parseBsplineCurve();
    RevSurface *parseRevSurface();

    int getToken(char token[MAX_PARSER_TOKEN_LENGTH]);

    Vec3 readVector3f();

    float readFloat();
    int readInt();

    FILE *file;
    Camera *camera;
    Group lights;
    int num_materials;
    Material **materials;
    Material *current_material;
    Group *group;

    Vec3 background_color;
};

#endif // SCENE_PARSER_H
