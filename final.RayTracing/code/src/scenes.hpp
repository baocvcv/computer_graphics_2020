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
    Material(MaterialType::REFRACTIVE, Vec3(1, 1, 1) * 0.999, Vec3(), 1.5f), // glass 6
    Material(MaterialType::DIFFUSE, Vec3(), Vec3(12, 12, 12)), // light 7
    Material(MaterialType::DIFFUSE, Vec3(), Vec3(), 1., "./resources/desert.png"), // desert wall 8
};

PerspectiveCamera camera(Vec3(10), Vec3(-1), Vec3(0, 0, 1), 1920, 1080, M_PI/3);

std::vector<Vec3> bspline_wineglass = {
    { 0.000000,-0.459543,0.0 },
    { 0.000000,-0.459544,0.0 },
    { 0.000000,-0.459545,0.0 },
    { -0.351882,-0.426747,0.0 },
    { -0.848656,-0.278898,0.0 },
    { -1.112097,0.084005,0.0 },
    { -1.164785,1.105511,0.0 },
    { -0.991667,2.328629,0.0 },
    { -1.029301,2.503360,0.0 },
    { -1.088800,2.345600,0.0 },
    { -1.278000,1.162800,0.0 },
    { -1.214800,0.055200,0.0 },
    { -0.915600,-0.381200,0.0 },
    { -0.380400,-0.622000,0.0 },
    { -0.144000,-0.968400,0.0 },
    { -0.096800,-1.480000,0.0 },
    { -0.128400,-2.112400,0.0 },
    { -0.317200,-2.202800,0.0 },
    { -0.994400,-2.262800,0.0 },
    { -1.214800,-2.323200,0.0 },
    { -1.199200,-2.398400,0.0 },
    { -1.057600,-2.458800,0.0 },
    { -0.711200,-2.458800,0.0 },
    { 0.000000,-2.458800,0.0 },
    { 0.000000,-2.458801,0.0 },
    { 0.000000,-2.458802,0.0 }
};

struct Scene {
    Camera* camera;
    Group* group; // group of all the objects
    // Group lights; // group of lights

    Scene(Camera* c_, Group* g_): camera(c_), group(g_) {}
};

Scene getScene1() {
    Group* g = new Group;
    // walls
    g->addObject(new Plane(Vec3(1), Vec3(-50), &materials[0]));
    g->addObject(new Plane(Vec3(-1), Vec3(10), &materials[1]));
    g->addObject(new Plane(Vec3(0, 1), Vec3(0, -15), &materials[2]));
    g->addObject(new Plane(Vec3(0,-1), Vec3(0, 15), &materials[3]));
    g->addObject(new Plane(Vec3(0, 0, 1), Vec3(0, 0, -10), &materials[4]));
    g->addObject(new Plane(Vec3(0, 0,-1), Vec3(0, 0, 10), &materials[4]));
    // balls
    g->addObject(new Sphere(Vec3(-0.5, -2, -2), 1.f, &materials[5]));
    g->addObject(new Sphere(Vec3(0.3, 2, -1.5), 1.f, &materials[6]));
    // light
    g->addObject(new Sphere(Vec3(-4, 0, 128.8f), 100.f, &materials[7]));
    // g->addObject(new Circle(Vec3(-4, 0, 9.99), 3.f, Vec3(0, 0, -1), &materials[7]));
    // bkg
    g->addObject(new Plane(Vec3(1), Vec3(-49), &materials[8]));

    return Scene(&camera, g);
}

Scene getScene2() {
    Group* g = new Group;
    PerspectiveCamera* cam = new PerspectiveCamera(
        Vec3(0, 0, 10),
        Vec3(0, 0, -1),
        Vec3(0, 1, 0),
        1280, 1000, M_PI/3.2
    );
    // DoFCamera* cam = new DoFCamera(
    //     Vec3(0, 0, 10),
    //     Vec3(0, 0, -1),
    //     Vec3(0, 1, 0),
    //     800, 600, M_PI/3.2,
    //     .3, 12
    // );

    // walls

    // walls
    g->addObject(new Plane(Vec3(1), Vec3(-10), &materials[0]));
    g->addObject(new Plane(Vec3(-1), Vec3(10), &materials[0]));
    g->addObject(new Plane(Vec3(0, 1), Vec3(0, -2), &materials[2]));
    g->addObject(new Plane(Vec3(0,-1), Vec3(0, 10), &materials[3]));
    g->addObject(new Plane(Vec3(0, 0, 1), Vec3(0, 0, -13), &materials[4]));
    g->addObject(new Plane(Vec3(0, 0,-1), Vec3(0, 0, 10), &materials[1]));
    // balls
    g->addObject(new Sphere(Vec3(0, 0, -2), 1.8f, &materials[6]));
    g->addObject(new Sphere(Vec3(-2.5, -1, 2), .75f, &materials[6]));
    g->addObject(new Sphere(Vec3(3, -1, -5), .75f, &materials[5]));
    g->addObject(new Transform(
        Mat44::translation(3, 0, -1),
        new Sphere(Vec3(1, -1, -1), .75f, &materials[6])));
    // light
    g->addObject(new Sphere(Vec3(0, 7, 4), 3.f, &materials[7]));
    // bkg
    g->addObject(new Rectangle(Vec3(-10, 10, -12.5), Vec3(20), Vec3(0, -12), &materials[8]));
    // g->addObject(new Rectangle(Vec3(0, 1), Vec3(0, -1), Vec3(1), &materials[3]));
    // g->addObject(new Rectangle(Vec3(1, 0), Vec3(1), Vec3(0, 1), &materials[3]));
    // g->addObject(new Circle(Vec3(-2, 2), 1.f, Vec3(0, 0, 1), &materials[3]));

    return Scene(cam, g);
}

Scene getScene3() {
    Group* g = new Group;
    PerspectiveCamera* cam = new PerspectiveCamera(
        Vec3(0, 0, 10),
        Vec3(0, 0, -1),
        Vec3(0, 1, 0),
        600, 400, M_PI/3.2
    );

    // walls
    g->addObject(new Plane(Vec3(1), Vec3(-10), &materials[0]));
    g->addObject(new Plane(Vec3(-1), Vec3(10), &materials[0]));
    g->addObject(new Plane(Vec3(0, 1), Vec3(0, -2), &materials[2]));
    g->addObject(new Plane(Vec3(0,-1), Vec3(0, 10), &materials[3]));
    g->addObject(new Plane(Vec3(0, 0, 1), Vec3(0, 0, -13), &materials[4]));
    g->addObject(new Plane(Vec3(0, 0,-1), Vec3(0, 0, 10), &materials[1]));
    // balls
    // g->addObject(new Sphere(Vec3(), 1.f, &materials[6]));
    // g->addObject(new Sphere(Vec3(-1, -1, 1), .75f, &materials[5]));
    // g->addObject(new Sphere(Vec3(1, -1, -1), .75f, &materials[6]));
    // g->addObject(new Transform(
    //     Mat44::translation(3, 0, 0),
    //     new Sphere(Vec3(1, -1, -1), .75f, &materials[6])));
    g->addObject(
        new Transform(
            Mat44::translation(0, 3, 0).mult(Mat44::rot_x(-M_PI / 2)),
            new RevSurface(new BsplineCurve(bspline_wineglass), &materials[6]))
        );
    // light
    g->addObject(new Sphere(Vec3(0, 7, 7), 3.f, &materials[7]));
    // bkg
    g->addObject(new Rectangle(Vec3(-10, 10, -12.5), Vec3(20), Vec3(0, -12), &materials[8]));

    return Scene(cam, g);
}
#endif