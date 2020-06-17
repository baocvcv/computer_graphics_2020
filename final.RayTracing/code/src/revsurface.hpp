#ifndef REVSURFACE_HPP
#define REVSURFACE_HPP

#include "object3d.hpp"
#include "curve.hpp"
#include "vec.hpp"
#include "mat44.hpp"
#include "aabb.hpp"
#include <tuple>

#define NEWTON_MAX_ITER 1000

struct RevSurface : public Object3D
{
    enum class NodeType {
        LEAF,
        INTERNAL
    };
    struct Node
    {
        NodeType type;
        Node* children[4];
        // u[2]: theta_min, theta_max; v[2]: t_min, t_max
        double u[2], v[2];
        AABB box;

        Node() {}
        Node (Node* c1, Node* c2):
            type(NodeType::INTERNAL), children{c1, c2, nullptr, nullptr},
            box(c1->box, c2->box) {}
        Node (Node* c1, Node* c2, Node* c3, Node* c4):
            type(NodeType::INTERNAL), children{c1, c2, c3, c4},
            box(c1->box, c2->box, c3->box, c4->box) {}
        Node (double u0, double u1, double v0, double v1, const AABB& _box):
            type(NodeType::LEAF), u{u0, u1}, v{v0, v1}, box(_box) {}
    };
    // TODO: extend to BSpline curve as well
    Curve *pCurve;
    Node* nodes; // quad-tree
    // AABB* boxes;
    int root;
    int node_cnt;
    const int steps = 40;
    int points_cnt;

    RevSurface(Curve *pCurve, Material* material) : pCurve(pCurve), Object3D(material) {
        // Check flat.
        for (const auto &cp : pCurve->getControls()) {
            if (cp.z != 0.0) {
                printf("Profile of revSurface must be flat on xy plane.\n");
                exit(0);
            }
        }
        // discretize curve
        std::vector<CurvePoint> points;
        pCurve->discretize(30, points);

        // create AABB quad-tree
        int num_leaf = steps * (points.size() - 1);
        int depth = log(num_leaf) / log(4) + 1;
        int num_nodes = (pow(4, depth + 1) - 1) / 3;
        // int leaf_start_pos = (pow(4, depth) - 1) / 3 + 1; // leaf start at index 0
        nodes = new Node[num_nodes*4];
        // boxes = new AABB[num_leaf];
        node_cnt = 0;
        points_cnt = points.size();
        for (unsigned int ci = 0; ci < points_cnt-1; ci++) {
            const CurvePoint& cp0 = points[ci];
            const CurvePoint& cp1 = points[ci+1];
            for (unsigned int i = 0; i < steps; i++) {
                double t[] = {
                    (double) i / steps * 2 * M_PI,
                    (double) ((i+1) % steps) / steps * 2 * M_PI
                };
                Vec3 vs[] = { // vertices of AABB
                    getPoint(cp0.V, t[0]), getPoint(cp0.V, t[1]),
                    getPoint(cp1.V, t[0]), getPoint(cp1.V, t[1])
                };
                nodes[node_cnt++] = Node(t[0], t[1], cp0.t, cp1.t,
                                         AABB(vs[0], vs[1], vs[2], vs[3]));
            }
        }
        root = createTree(0, steps+1, 0, points_cnt);
    }

    // create tree node at index using boxes in [bottom..top][left..right]
    int createTree(int left, int right, int bottom, int top) {
        int width = right - left, height = top - bottom;
        if (width == 1 && height == 1) { // leaf node
            return bottom*steps + left;
        }
        if (width <= 2 && height <= 2) { // contains leaf node
            if (width == 1)
                nodes[node_cnt] = Node(
                    &nodes[bottom*steps+left],
                    &nodes[(bottom+1)*steps+left]);
            else if (height == 1)
                nodes[node_cnt] = Node(
                    &nodes[bottom*steps+left],
                    &nodes[bottom*steps+left+1]);
            else
                nodes[node_cnt] = Node(
                    &nodes[bottom*steps+left],
                    &nodes[bottom*steps+left+1],
                    &nodes[(bottom+1)*steps+left],
                    &nodes[(bottom+1)*steps+left+1]);
            node_cnt++;
            return node_cnt-1;
        }
        // contains internal nodes
        if (width == 2) {
            int h_mid = bottom + ((top - bottom) >> 1);
            int c1 = createTree(left, right, bottom, h_mid); // bottom-left
            int c2 = createTree(left, right, h_mid, top); // top-left
            nodes[node_cnt] = Node(&nodes[c1], &nodes[c2]);
            node_cnt++;
            return node_cnt-1;
        }
        if (height == 2) {
            int w_mid = left + ((right - left) >> 1);
            int c1 = createTree(left, w_mid, bottom, top); // top-left
            int c2 = createTree(w_mid, right, bottom, top); // bottom-right
            nodes[node_cnt] = Node(&nodes[c1], &nodes[c2]);
            node_cnt++;
            return node_cnt-1;
        }
        // both > 2
        int w_mid = left + ((right - left) >> 1);
        int h_mid = bottom + ((top - bottom) >> 1);
        int c1 = createTree(left, w_mid, bottom, h_mid); // bottom-left
        int c2 = createTree(left, w_mid, h_mid, top); // top-left
        int c3 = createTree(w_mid, right, bottom, h_mid); // bottom-right
        int c4 = createTree(w_mid, right, h_mid, top); // top-right
        nodes[node_cnt] = Node(&nodes[c1], &nodes[c2], &nodes[c3], &nodes[c4]);
        node_cnt++;
        return node_cnt-1;
    }

    ~RevSurface() override {
        delete pCurve;
    }

    // acquire point on surface with v rorated rad radians
    Vec3 getPoint(Vec3 v, double rad) {
        return Vec3(v.x*cos(rad), v.x*sin(rad), v.y);
    }

    bool intersect(const Ray &r, Hit &h, double tmin) override {
        // intersect quad-tree, use (t_near + t_far) / 2 as estimate for t
        auto result = intersect_tree(&nodes[root], r);
        if (result.first == nullptr) {
            return false;
        }
        double t_near = result.second.first;
        double t_far = result.second.second;
        double t0 = (t_near + t_far) / 2;

        // estimate u, v
        Node* leaf = result.first;
        double u0 = (leaf->u[0] + leaf->u[1]) / 2;
        double v0 = (leaf->v[0] + leaf->v[1]) / 2;
        // TODO: handle ray from inside the rev volume

        // newton
        auto v_bound = pCurve->get_valid_range();
        auto x0 = Vec3(t0, u0, v0);
        if (x0.z < v_bound.first || x0.z > v_bound.second) return false;
        auto curvePoint = pCurve->evaluate(x0.z);
        auto p = curvePoint.first;
        auto dp = curvePoint.second;
        auto realPoint = getPoint(p, x0.y);
        auto f = r.pointAtParameter(x0.x) - realPoint;
        auto dt = r.dir;
        auto du = Vec3(-sin(x0.y)*p.x, cos(x0.y)*p.x, 0);
        auto dv = Vec3(cos(x0.y)*dp.x, sin(x0.y)*dp.x, dp.y);
        // { // check if derivatives are correct
        //     double u2 = x0.y * (1 + 1e-5);
        //     auto p2 = getPoint(p, u2);
        //     auto du2 = (p2 - realPoint) / (u2 - x0.y);
        //     double diff = (du2 - du).len();
        //     assert(diff < 0.1);
        //     double v2 = x0.z * (1 + 1e-5);
        //     p2 = getPoint(pCurve->evaluate(v2).first, x0.y);
        //     auto dv2 = (p2 - realPoint) / (v2 - x0.z);
        //     diff = (dv2 - dv).len();
        //     assert(diff < 0.1);
        // }
        // Mat44 jac = Mat44(dt, -du, -dv);
        Mat44 jacobian_inv = Mat44(dt, -du, -dv).inversed();
        // Mat44 id = jac.mult(jacobian_inv);
        for (int i = 0; i < NEWTON_MAX_ITER; i++) {
            //TODO: change limits
            // if (f.max() < 1e-1 && f.min() > -1e-1) { // found intersection
            if (f.max() < eps && f.min() > -eps && x0.x > tmin) { // found intersection
                Vec3 normal = du.cross(dv).normalized();
                h.set(x0.x, material, normal, Vec3(x0.y/(2*M_PI), x0.z));
                return true;
            // } else if (x0.y<leaf->u[0] || x0.y>leaf->u[1] || x0.z<leaf->v[0] || x0.z>leaf->v[1]) {
            } else if (x0.x < -.05 || x0.y<0 || x0.y>=2*M_PI) { // very loose condition
                return false;
            }
            // iter
            x0 = x0 - jacobian_inv.mult(f, false);
            if (x0.z < v_bound.first || x0.z > v_bound.second) return false;
            curvePoint = pCurve->evaluate(x0.z);
            p = curvePoint.first;
            dp = curvePoint.second;
            realPoint = getPoint(p, x0.y);
            f = r.pointAtParameter(x0.x) - realPoint;
            du = Vec3(-sin(x0.y)*p.x, cos(x0.y)*p.x, 0);
            dv = Vec3(cos(x0.y)*dp.x, sin(x0.y)*dp.x, dp.y);
            jacobian_inv = Mat44(dt, -du, -dv).inversed();
        }
        return false;
    }

    // returns <best_node, <t_near, t_far>>
    std::pair<Node*, std::pair<double, double>> intersect_tree(
        Node* node, const Ray& r) {
            double t_near, t_far;
            bool has_intersect = node->box.intersect(r, t_near, t_far);
            if (!has_intersect) {
                return {nullptr, {t_near, t_far}};
            }
            // has_intersect
            if (node->type == NodeType::LEAF) {
                return {node, {t_near, t_far}};
            } else {
                Node* res = nullptr;
                has_intersect = false; // reuse
                for (int i = 0; i < 4; i++) {
                    if (node->children[i] != nullptr) {
                        auto result = intersect_tree(node->children[i], r);
                        if (result.first != nullptr) {
                            // first intersection, or a closer intersection
                            double tn = result.second.first;
                            double tf = result.second.second;
                            bool cond1 = t_near > 0 && tn > 0 && tn < t_near;
                            bool cond2 = t_near < 0 && tn > 0;
                            bool cond3 = t_near < 0 && tn < 0 && tf < t_far;
                            if (!has_intersect || cond1 || cond2 || cond3) {
                                res = result.first;
                                t_near = tn;
                                t_far = tf;
                            }
                        }
                    }
                }
                return {res, {t_near, t_far}};
            }
        }
};

#endif //REVSURFACE_HPP
