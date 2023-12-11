#ifndef HITTABLE_H
#define HITTABLE_H

#include <vector>
#include <memory>

#include "primitive.h"

class scene{
public:
    scene() {}
    scene(std::shared_ptr<primitive> object) { add(object); }

    bool intersect(const ray& r, interval ray_t, intersection& inter) const{
        intersection temp;
        bool hitting = false;
        double closest_so_far = ray_t.max;
        for (const auto& prim : nodes){
            if (prim->intersect(r, ray_t, temp)){
                hitting = true;
                ray_t.max = temp.t;
                inter = temp;
            }
        }
        return hitting;
    }

    void add(std::shared_ptr<primitive> prim) { nodes.push_back(prim); }
    void clear() { nodes.clear(); }

private:
    std::vector<std::shared_ptr<primitive>> nodes;
};
#endif //HITTABLE_H
