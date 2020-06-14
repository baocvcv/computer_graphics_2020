#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cmath>
#include <vector>

#include "scene_parser.hpp"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define DegreesToRadians(x) ((M_PI * x) / 180.0f)

using namespace std;

SceneParser::SceneParser(const char *filename) {

    // initialize some reasonable default values
    group = nullptr;
    camera = nullptr;
    background_color = Vec3(0.5, 0.5, 0.5);
    num_materials = 0;
    materials = nullptr;
    current_material = nullptr;

    // parse the file
    assert(filename != nullptr);
    const char *ext = &filename[strlen(filename) - 4];

    if (strcmp(ext, ".txt") != 0) {
        printf("wrong file name extension\n");
        exit(0);
    }
    file = fopen(filename, "r");

    if (file == nullptr) {
        printf("cannot open scene file\n");
        exit(0);
    }
    parseFile();
    fclose(file);
    file = nullptr;

    if (lights.getGroupSize() == 0) {
        printf("WARNING:    No lights specified\n");
    }
}

SceneParser::~SceneParser() {

    delete group;
    delete camera;

    int i;
    for (i = 0; i < num_materials; i++) {
        delete materials[i];
    }
    delete[] materials;
}

// ====================================================================
// ====================================================================

void SceneParser::parseFile() {
    //
    // at the top level, the scene can have a camera, 
    // background color and a group of objects
    // (we add lights and other things in future assignments)
    //
    char token[MAX_PARSER_TOKEN_LENGTH];
    while (getToken(token)) {
        if (!strcmp(token, "PerspectiveCamera")) {
            parsePerspectiveCamera();
        } else if (!strcmp(token, "Background")) {
            parseBackground();
        } else if (!strcmp(token, "Materials")) {
            parseMaterials();
        } else if (!strcmp(token, "Group")) {
            group = parseGroup();
        } else {
            printf("Unknown token in parseFile: '%s'\n", token);
            exit(0);
        }
    }
    parseLights();
}

// ====================================================================
// ====================================================================

void SceneParser::parsePerspectiveCamera() {
    char token[MAX_PARSER_TOKEN_LENGTH];
    // read in the camera parameters
    getToken(token);
    assert (!strcmp(token, "{"));
    getToken(token);
    assert (!strcmp(token, "center"));
    Vec3 center = readVector3f();
    getToken(token);
    assert (!strcmp(token, "direction"));
    Vec3 direction = readVector3f();
    getToken(token);
    assert (!strcmp(token, "up"));
    Vec3 up = readVector3f();
    getToken(token);
    assert (!strcmp(token, "angle"));
    float angle_degrees = readFloat();
    float angle_radians = DegreesToRadians(angle_degrees);
    getToken(token);
    assert (!strcmp(token, "width"));
    int width = readInt();
    getToken(token);
    assert (!strcmp(token, "height"));
    int height = readInt();
    getToken(token);
    assert (!strcmp(token, "}"));
    camera = new PerspectiveCamera(center, direction, up, width, height, angle_radians);
}

void SceneParser::parseBackground() {
    char token[MAX_PARSER_TOKEN_LENGTH];
    // read in the background color
    getToken(token);
    assert (!strcmp(token, "{"));
    while (true) {
        getToken(token);
        if (!strcmp(token, "}")) {
            break;
        } else if (!strcmp(token, "color")) {
            background_color = readVector3f();
        } else {
            printf("Unknown token in parseBackground: '%s'\n", token);
            assert(0);
        }
    }
}

// ====================================================================
// ====================================================================

void SceneParser::parseLights() {
    // char token[MAX_PARSER_TOKEN_LENGTH];
    // getToken(token);
    // assert (!strcmp(token, "{"));
    // // read in the number of objects
    // getToken(token);
    // assert (!strcmp(token, "numLights"));
    // num_lights = readInt();
    // lights = new Light *[num_lights];
    // // read in the objects
    // int count = 0;
    // while (num_lights > count) {
    //     getToken(token);
    //     if (strcmp(token, "DirectionalLight") == 0) {
    //         lights[count] = parseDirectionalLight();
    //     } else if (strcmp(token, "PointLight") == 0) {
    //         lights[count] = parsePointLight();
    //     } else {
    //         printf("Unknown token in parseLight: '%s'\n", token);
    //         exit(0);
    //     }
    //     count++;
    // }
    // getToken(token);
    // assert (!strcmp(token, "}"));
    for (auto obj : group->objects) {
        if (obj->material->emission.non_zero()) {
            lights.addObject(obj);
        }
    }
}

Light *SceneParser::parseDirectionalLight() {
    char token[MAX_PARSER_TOKEN_LENGTH];
    getToken(token);
    assert (!strcmp(token, "{"));
    getToken(token);
    assert (!strcmp(token, "direction"));
    Vec3 direction = readVector3f();
    getToken(token);
    assert (!strcmp(token, "color"));
    Vec3 color = readVector3f();
    getToken(token);
    assert (!strcmp(token, "}"));
    return new DirectionalLight(direction, color);
}

Light *SceneParser::parsePointLight() {
    char token[MAX_PARSER_TOKEN_LENGTH];
    getToken(token);
    assert (!strcmp(token, "{"));
    getToken(token);
    assert (!strcmp(token, "position"));
    Vec3 position = readVector3f();
    getToken(token);
    assert (!strcmp(token, "color"));
    Vec3 color = readVector3f();
    getToken(token);
    assert (!strcmp(token, "}"));
    return new PointLight(position, color);
}
// ====================================================================
// ====================================================================

void SceneParser::parseMaterials() {
    char token[MAX_PARSER_TOKEN_LENGTH];
    getToken(token);
    assert (!strcmp(token, "{"));
    // read in the number of objects
    getToken(token);
    assert (!strcmp(token, "numMaterials"));
    num_materials = readInt();
    materials = new Material *[num_materials];
    // read in the objects
    int count = 0;
    while (num_materials > count) {
        getToken(token);
        if (!strcmp(token, "Material") ||
            !strcmp(token, "PhongMaterial")) {
            materials[count] = parseMaterial();
        } else {
            printf("Unknown token in parseMaterial: '%s'\n", token);
            exit(0);
        }
        count++;
    }
    getToken(token);
    assert (!strcmp(token, "}"));
}


Material *SceneParser::parseMaterial() {
    char token[MAX_PARSER_TOKEN_LENGTH];
    char filename[MAX_PARSER_TOKEN_LENGTH];
    filename[0] = 0;
    Vec3 color(0, 0, 0), emission(0, 0, 0);
    float n = 1.0;
    MaterialType type = MaterialType::DIFFUSE;
    getToken(token);
    assert (!strcmp(token, "{"));
    while (true) {
        getToken(token);
        if (strcmp(token, "Color") == 0) {
            color = readVector3f();
        } else if (strcmp(token, "Emission") == 0) {
            emission = readVector3f();
        } else if (strcmp(token, "Type") == 0) {
            getToken(token);
            if (strcmp(token, "specular") == 0)
                type = MaterialType::SPECULAR;
            else if (strcmp(token, "refract") == 0)
                type = MaterialType::REFRACTIVE;
        } else if (strcmp(token, "n") == 0) {
            n = readFloat();
        } else if (strcmp(token, "texture") == 0) {
            // TODO: read in texture
            getToken(filename);
        } else {
            assert (!strcmp(token, "}"));
            break;
        }
    }
    auto *answer = new Material(type, color, emission, n, filename);
    return answer;
}

// ====================================================================
// ====================================================================

Object3D *SceneParser::parseObject(char token[MAX_PARSER_TOKEN_LENGTH]) {
    Object3D *answer = nullptr;
    if (!strcmp(token, "Group")) {
        answer = (Object3D *) parseGroup();
    } else if (!strcmp(token, "Sphere")) {
        answer = (Object3D *) parseSphere();
    } else if (!strcmp(token, "Plane")) {
        answer = (Object3D *) parsePlane();
    } else if (!strcmp(token, "Triangle")) {
        answer = (Object3D *) parseTriangle();
    } else if (!strcmp(token, "TriangleMesh")) {
        answer = (Object3D *) parseTriangleMesh();
    } else if (!strcmp(token, "Transform")) {
        answer = (Object3D *) parseTransform();
    } else if (!strcmp(token, "BezierCurve")) {
        answer = (Object3D *) parseBezierCurve();
    } else if (!strcmp(token, "BsplineCurve")) {
        answer = (Object3D *) parseBsplineCurve();
    } else if (!strcmp(token, "RevSurface")) {
        answer = (Object3D *) parseRevSurface();
    } else {
        printf("Unknown token in parseObject: '%s'\n", token);
        exit(0);
    }
    return answer;
}

// ====================================================================
// ====================================================================

Group *SceneParser::parseGroup() {
    //
    // each group starts with an integer that specifies
    // the number of objects in the group
    //
    // the material index sets the material of all objects which follow,
    // until the next material index (scoping for the materials is very
    // simple, and essentially ignores any tree hierarchy)
    //
    char token[MAX_PARSER_TOKEN_LENGTH];
    getToken(token);
    assert (!strcmp(token, "{"));

    // read in the number of objects
    getToken(token);
    assert (!strcmp(token, "numObjects"));
    int num_objects = readInt();

    auto *answer = new Group();

    // read in the objects
    int count = 0;
    while (num_objects > count) {
        getToken(token);
        if (!strcmp(token, "MaterialIndex")) {
            // change the current material
            int index = readInt();
            assert (index >= 0 && index <= getNumMaterials());
            current_material = getMaterial(index);
        } else {
            Object3D *object = parseObject(token);
            assert (object != nullptr);
            answer->addObject(object);
            count++;
        }
    }
    getToken(token);
    assert (!strcmp(token, "}"));

    // return the group
    return answer;
}

// ====================================================================
// ====================================================================

Sphere *SceneParser::parseSphere() {
    char token[MAX_PARSER_TOKEN_LENGTH];
    getToken(token);
    assert (!strcmp(token, "{"));
    getToken(token);
    assert (!strcmp(token, "center"));
    Vec3 center = readVector3f();
    getToken(token);
    assert (!strcmp(token, "radius"));
    float radius = readFloat();
    getToken(token);
    assert (!strcmp(token, "}"));
    assert (current_material != nullptr);
    return new Sphere(center, radius, current_material);
}


Plane *SceneParser::parsePlane() {
    char token[MAX_PARSER_TOKEN_LENGTH];
    getToken(token);
    assert (!strcmp(token, "{"));
    getToken(token);
    assert (!strcmp(token, "normal"));
    Vec3 normal = readVector3f();
    getToken(token);
    assert (!strcmp(token, "point"));
    Vec3 p = readVector3f();
    getToken(token);
    assert (!strcmp(token, "}"));
    assert (current_material != nullptr);
    return new Plane(normal, p, current_material);
}


Triangle *SceneParser::parseTriangle() {
    char token[MAX_PARSER_TOKEN_LENGTH];
    getToken(token);
    assert (!strcmp(token, "{"));
    getToken(token);
    assert (!strcmp(token, "vertex0"));
    Vec3 v0 = readVector3f();
    getToken(token);
    assert (!strcmp(token, "vertex1"));
    Vec3 v1 = readVector3f();
    getToken(token);
    assert (!strcmp(token, "vertex2"));
    Vec3 v2 = readVector3f();
    getToken(token);
    assert (!strcmp(token, "}"));
    assert (current_material != nullptr);
    return new Triangle(v0, v1, v2, current_material);
}

Mesh *SceneParser::parseTriangleMesh() {
    char token[MAX_PARSER_TOKEN_LENGTH];
    char filename[MAX_PARSER_TOKEN_LENGTH];
    // get the filename
    getToken(token);
    assert (!strcmp(token, "{"));
    getToken(token);
    assert (!strcmp(token, "obj_file"));
    getToken(filename);
    getToken(token);
    assert (!strcmp(token, "}"));
    const char *ext = &filename[strlen(filename) - 4];
    assert(!strcmp(ext, ".obj"));
    Mesh *answer = new Mesh(filename, current_material);

    return answer;
}

Curve *SceneParser::parseBezierCurve() {
    char token[MAX_PARSER_TOKEN_LENGTH];
    getToken(token);
    assert (!strcmp(token, "{"));
    getToken(token);
    assert (!strcmp(token, "controls"));
    vector<Vec3> controls;
    while (true) {
        getToken(token);
        if (!strcmp(token, "[")) {
            controls.push_back(readVector3f());
            getToken(token);
            assert (!strcmp(token, "]"));
        } else if (!strcmp(token, "}")) {
            break;
        } else {
            printf("Incorrect format for BezierCurve!\n");
            exit(0);
        }
    }
    Curve *answer = new BezierCurve(controls);
    return answer;
}


Curve *SceneParser::parseBsplineCurve() {
    char token[MAX_PARSER_TOKEN_LENGTH];
    getToken(token);
    assert (!strcmp(token, "{"));
    getToken(token);
    assert (!strcmp(token, "controls"));
    vector<Vec3> controls;
    while (true) {
        getToken(token);
        if (!strcmp(token, "[")) {
            controls.push_back(readVector3f());
            getToken(token);
            assert (!strcmp(token, "]"));
        } else if (!strcmp(token, "}")) {
            break;
        } else {
            printf("Incorrect format for BsplineCurve!\n");
            exit(0);
        }
    }
    Curve *answer = new BsplineCurve(controls);
    return answer;
}

RevSurface *SceneParser::parseRevSurface() {
    char token[MAX_PARSER_TOKEN_LENGTH];
    getToken(token);
    assert (!strcmp(token, "{"));
    getToken(token);
    assert (!strcmp(token, "profile"));
    Curve* profile;
    getToken(token);
    if (!strcmp(token, "BezierCurve")) {
        profile = parseBezierCurve();
    } else if (!strcmp(token, "BsplineCurve")) {
        profile = parseBsplineCurve();
    } else {
        printf("Unknown profile type in parseRevSurface: '%s'\n", token);
        exit(0);
    }
    getToken(token);
    assert (!strcmp(token, "}"));
    auto *answer = new RevSurface(profile, current_material);
    return answer;
}

Transform *SceneParser::parseTransform() {
    char token[MAX_PARSER_TOKEN_LENGTH];
    Mat44 matrix = Mat44::identity();
    Object3D *object = nullptr;
    getToken(token);
    assert (!strcmp(token, "{"));
    // read in transformations: 
    // apply to the LEFT side of the current matrix (so the first
    // transform in the list is the last applied to the object)
    getToken(token);

    while (true) {
        if (!strcmp(token, "Scale")) {
            Vec3 s = readVector3f();
            matrix = matrix * Mat44::scaling(s.x, s.y, s.z);
        } else if (!strcmp(token, "UniformScale")) {
            float s = readFloat();
            matrix = matrix * Mat44::scaling(s, s, s);
        } else if (!strcmp(token, "Translate")) {
            auto v = readVector3f();
            matrix = matrix * Mat44::translation(v.x, v.y, v.z);
        } else if (!strcmp(token, "XRotate")) {
            matrix = matrix * Mat44::rot_x(DegreesToRadians(readFloat()));
        } else if (!strcmp(token, "YRotate")) {
            matrix = matrix * Mat44::rot_y(DegreesToRadians(readFloat()));
        } else if (!strcmp(token, "ZRotate")) {
            matrix = matrix * Mat44::rot_z(DegreesToRadians(readFloat()));
        } else if (!strcmp(token, "Matrix4f")) {
            Mat44 matrix2 = Mat44::identity();
            getToken(token);
            assert (!strcmp(token, "{"));
            for (int j = 0; j < 4; j++) {
                for (int i = 0; i < 4; i++) {
                    float v = readFloat();
                    matrix2[j*4 + i] = v;
                }
            }
            getToken(token);
            assert (!strcmp(token, "}"));
            matrix = matrix2 * matrix;
        } else {
            // otherwise this must be an object,
            // and there are no more transformations
            object = parseObject(token);
            break;
        }
        getToken(token);
    }

    assert(object != nullptr);
    getToken(token);
    assert (!strcmp(token, "}"));
    return new Transform(matrix, object);
}

// ====================================================================
// ====================================================================

int SceneParser::getToken(char token[MAX_PARSER_TOKEN_LENGTH]) {
    // for simplicity, tokens must be separated by whitespace
    assert (file != nullptr);
    int success = fscanf(file, "%s ", token);
    if (success == EOF) {
        token[0] = '\0';
        return 0;
    }
    return 1;
}


Vec3 SceneParser::readVector3f() {
    float x, y, z;
    int count = fscanf(file, "%f %f %f", &x, &y, &z);
    if (count != 3) {
        printf("Error trying to read 3 floats to make a Vector3f\n");
        assert (0);
    }
    return Vec3(x, y, z);
}


float SceneParser::readFloat() {
    float answer;
    int count = fscanf(file, "%f", &answer);
    if (count != 1) {
        printf("Error trying to read 1 float\n");
        assert (0);
    }
    return answer;
}


int SceneParser::readInt() {
    int answer;
    int count = fscanf(file, "%d", &answer);
    if (count != 1) {
        printf("Error trying to read 1 int\n");
        assert (0);
    }
    return answer;
}
