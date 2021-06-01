#ifndef CONSTANT_MEDIUM_H
#define CONSTANT_MEDIUM_H

#include "common.h"

#include "hittable.h"
#include "material.h"
#include "texture.h"

class constant_medium : public hittable {
    public:
        constant_medium(shared_ptr<hittable> b, double d, shared_ptr<texture> a)
        : boundary(b), neg_inv_density(-1/d), phase_function(make_shared<isotropic>(a))
        {}

        constant_medium(shared_ptr<hittable> b, double d, color c)
        : boundary(b), neg_inv_density(-1/d), phase_function(make_shared<isotropic>(c))
        {}

        virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;

        virtual bool bounding_box(double time0, double time1, aabb& output_box) const override {
            return boundary->bounding_box(time0, time1, output_box);
        }
    
    public:
        shared_ptr<hittable> boundary;
        shared_ptr<material> phase_function;
        double neg_inv_density;
};

bool constant_medium::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
    const bool DEBUG = false;
    const bool debugging = DEBUG && random_double() < 0.00001;

    hit_record rec1, rec2;

    if(!boundary->hit(r, -infinity, infinity, rec1))
        return false;

    if(!boundary->hit(r, rec1.t + 0.0001, infinity, rec2))
        return false;
    
    /* NOTE :: this is my attempt at making this method handle arbitrary shapes
             the book's version does not include this while loop.
             I think this should work because:
             1. If you make it to this block you have already had two intersections
             2. You slightly increment the t_min value so you are sure you are
                searching for a hit farther along the ray.
                Getting a hit overwrites the previous rec2, which makes sense because you
                don't really have a use for it.
                Basically you just keep going until you are sure there is no part of the
                object in front of you.
    while(true){
        auto tmp = rec2;
        if(!boundary->hit(r, tmp.t + 0.0001, infinity, rec2))
            break;
    }
    */

    if(debugging)
        std::cerr << "\nt_min=" << rec1.t << ", t_max=" << rec2.t << "\n";

    if(rec1.t < t_min) rec1.t = t_min;
    if(rec2.t > t_max) rec2.t = t_max;

    if(rec1.t >= rec2.t)
        return false;

    if(rec1.t < 0)
        rec1.t = 0;

    const auto ray_length = r.direction().length();
    const auto distance_inside_boundary = (rec2.t - rec1.t) * ray_length;
    const auto hit_distance = neg_inv_density * log(random_double());

    if(hit_distance > distance_inside_boundary)
        return false;

    rec.t = rec1.t + hit_distance / ray_length;
    rec.p = r.at(rec.t);

    if (debugging) {
        std::cerr << "hit_distance=" << hit_distance << "\n"
                  << "rec.t=" << rec.t << "\n"
                  << "rec.p=" << rec.p << "\n";
    }

    rec.normal = vec3(1, 0, 0); // arbitrary
    rec.front_face = true;      // arbitrary
    rec.mat_ptr = phase_function;

    return true;
};

#endif