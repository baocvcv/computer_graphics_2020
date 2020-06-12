#ifndef SCENE_PARSER_H
#define SCENE_PARSER_H

#include "group.hpp"
#include <cassert>
#include <vecmath.h>

class Camera;
class Light;
class Material;
class Object3D;
class Sphere;
class Plane;
class Triangle;
class Transform;
class Mesh;
class Curve;
class RevSurface;

#define MAX_PARSER_TOKEN_LENGTH 1024

class SceneParser {
public:

    SceneParser() = delete;
    SceneParser(const char *filename);

    ~SceneParser();

    Camera *getCamera() const {
        return camera;
    }

    Vector3f getBackgroundColor() const {
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

    Vector3f readVector3f();

    float readFloat();
    int readInt();

    FILE *file;
    Camera *camera;
    Group lights;
    int num_materials;
    Material **materials;
    Material *current_material;
    Group *group;

    Vector3f background_color;
};

#endif // SCENE_PARSER_H
