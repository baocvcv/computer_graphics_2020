#ifndef SCENES_HPP
#define SCENES_HPP

#include "common.hpp"
#include "helpers.hpp"
#include "object3d.hpp"
#include "vec.hpp"
#include "mat44.hpp"
#include "camera.hpp"
#include "curve.hpp"
#include "revsurface.hpp"
#include "mesh.hpp"

std::string textures[] = {
    
};

Material materials[] = {
    Material(MaterialType::DIFFUSE, Vec3(1, 1, 1)), // white wall 0
    Material(MaterialType::DIFFUSE, Vec3()), // black wall 1
    Material(MaterialType::DIFFUSE, Vec3(0.75, 0.25, 0.25)), // red wall 2
    Material(MaterialType::DIFFUSE, Vec3(0.25, 0.75, 0.25)), // green wall 3
    Material(MaterialType::DIFFUSE, Vec3(0.25, 0.25, 0.75)), // blue wall 4
    Material(MaterialType::SPECULAR, Vec3(1, 1, 1) * 0.999), // mirror 5
    Material(MaterialType::REFRACTIVE, Vec3(1, 1, 1) * 0.999, 1.5f), // glass 6
    Material(MaterialType::DIFFUSE, Vec3(), Vec3(12, 12, 12)) // light 7
};

PerspectiveCamera camera(Vec3(10), Vec3(-1), Vec3(0, 0, 1), 1920, 1080, M_PI/3);

struct Scene {
    PerspectiveCamera* camera;
    Group* group; // group of all the objects
    // Group lights; // group of lights

    Scene(PerspectiveCamera* c_, Group* g_): camera(c_), group(g_) {}
};

Scene getScene1() {
    Group* g = new Group;
    // walls
    g->addObject(new Plane(Vec3(1), Vec3(-10), &materials[0]));
    g->addObject(new Plane(Vec3(-1), Vec3(-10), &materials[1]));
    g->addObject(new Plane(Vec3(0, 1), Vec3(-20), &materials[2]));
    g->addObject(new Plane(Vec3(0,-1), Vec3(-20), &materials[3]));
    g->addObject(new Plane(Vec3(0, 0, 1), Vec3(-15), &materials[4]));
    g->addObject(new Plane(Vec3(0, 0,-1), Vec3(-15), &materials[4]));
    // balls
    g->addObject(new Sphere(Vec3(-0.5, -3), 1.f, &materials[5]));
    g->addObject(new Sphere(Vec3(0.5, 3), 1.f, &materials[6]));
    // light
    g->addObject(new Sphere(Vec3(1, 0, 8), 2.f, &materials[7]));

    return Scene(&camera, g);
}
#endif