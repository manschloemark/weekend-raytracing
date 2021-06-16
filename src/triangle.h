#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "common.h"
#include "hittable.h"

#ifndef MT_ALG
#define MT_ALG 1
#endif

#define EPSILON 0.000001

// TODO : implement single-sided and two-sided.
class triangle : public hittable {
	public:
		triangle() {}
		triangle(const point3& _a, const point3& _b, const point3& _c, bool ss, shared_ptr<material> mat)
		: v0(_a), v1(_b), v2(_c), single_sided(ss), mp(mat) {}

		virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;
		virtual bool bounding_box(double time0, double time1, aabb& output_box) const override {
			// NOTE : the epsilon here is really only in the case where the triangle is aligned on an
			//        axis. And I'm not sure it's necessary. Peter Shirley used it in the axis aligned rectangle.
			//        It ensures the aabb will never have a width of 0 on any axis.
			vec3 epsilon = vec3(0.0001, 0.0001, 0.0001);
			point3 min = point3(fmin(v0.x(), fmin(v1.x(), v2.x())),
                          fmin(v0.y(), fmin(v1.y(), v2.y())),
                          fmin(v0.z(), fmin(v1.z(), v2.z())));
      point3 max = point3(fmax(v0.x(), fmax(v1.x(), v2.x())),
                          fmax(v0.y(), fmax(v1.y(), v2.y())),
                          fmax(v0.z(), fmax(v1.z(), v2.z())));
			output_box = aabb(min - epsilon, max + epsilon);
			return true;
		}

	public:
		shared_ptr<material> mp;
		point3 v0;
		point3 v1;
		point3 v2;
		bool single_sided;
};

bool triangle::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
#if MT_ALG
	vec3 v01 = v1 - v0;
	vec3 v02 = v2 - v0;


	// These two are stored in variables because they are used multiple times in
	// the Cramer's rule calculation.
	// The Moller Trumbore alg uses pointers more, maybe I should update my code to use them too. 
	vec3 D_x_v02 = cross(r.direction(), v02); // 'pvec' in source code
	double det = dot(v01, D_x_v02);

	// Check if parallel with plane
	if (fabs(det) < EPSILON)
		return false;

	double inv_det = 1.0 / det;

	vec3 T = r.origin() - v0;                 // 'tvec' in source code
	double u = dot(T, D_x_v02) * inv_det;
	if (u < 0.0 || u > 1.0)
		return false;

	vec3 T_x_v01 = cross(T, v01);             // 'qvec' in source code
	double v = dot(r.direction(), T_x_v01) * inv_det;
	if (v < 0.0 || u + v > 1.0)
		return false;

	double t = dot(v02, T_x_v01) * inv_det;

	rec.u = u;
	rec.v = v;
	rec.t = t;
	rec.p = r.at(t);

	rec.mat_ptr = mp;
	// NOTE : is there a better way to get the normal from all of this?
	//        it's late and I'm tired so I'll look into it tomorrow.
	rec.set_face_normal(r, cross(v01, v02));
	return true;
#else
	vec3 v01 = v1 - v0;
	vec3 v02 = v2 - v0;
	vec3 n = cross(v01, v02);
	double denom = dot(n, n);

	double n_dot_r = dot(n, r.direction());
	// Check if ray is parallel to the plane
	if (fabs(n_dot_r) < EPSILON)
		return false;

	double D = dot(n, v0);
	double t = -(dot(n, r.origin()) + D) / n_dot_r;

	// Check if triangle is behind
	if (t < 0.0)
		return false;

	vec3 p = r.at(t);

	vec3 e0 = v1 - v0;
	vec3 e1 = v2 - v1;
	vec3 e2 = v0 - v2;

	vec3 c0 = cross(e0, p - v0);
	vec3 c1 = cross(e1, p - v1);
	vec3 c2 = cross(e2, p - v2);

	// Store these in variables because they are useful for calculating they
	// barycentril coordinates later.
	// c0 is not used.
	double u = dot(n, c1);
	double v = dot(n, c2);

	if( (dot(n, c0) < 0) || (u < 0) || (v < 0) )
		return false;

	rec.set_face_normal(r, n);
	if ((!rec.front_face) && single_sided)
		return false;

	rec.t = t;
	rec.p = p;
	rec.mat_ptr = mp;
	rec.u = u / denom;
	rec.v = v / denom;

	return true;
#endif
}

#endif
