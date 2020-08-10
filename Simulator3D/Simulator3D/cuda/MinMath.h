#ifndef MINMATH_H
#define MINMATH_H

#ifdef __CUDACC__
#pragma message("Min Math: Cuda compilation code")
#define DEVICE_FUNC __device__
#else 
#define DEVICE_FUNC
#endif

#include <stdio.h>

namespace mm {

	struct vec4
	{
		float x, y, z, w;
	};


	struct vec3
	{
		float x, y, z;

		DEVICE_FUNC
		vec3() = default;

		DEVICE_FUNC
		vec3(float x, float y, float z) : x(x), y(y), z(z) {}

		DEVICE_FUNC
		vec3(const vec4& v) {
			x = v.x;
			y = v.y;
			z = v.z;
		}
	};


	struct mat3
	{
		float m[3][3];
	};

	// ADD
	DEVICE_FUNC
		void add_to(vec3* a, const vec3& b);

	DEVICE_FUNC
		void add_to(vec4* a, const vec3& b);

	DEVICE_FUNC
		void add_to(vec4* a, const vec4& b);

	DEVICE_FUNC
		void add_to(vec3* a, float f);

	DEVICE_FUNC
		void add_to(vec4* a, float f);

	// Add to the diagonal
	DEVICE_FUNC
		void add_to(mat3* m, float f);

	DEVICE_FUNC
		void add_to(mat3* m, const mat3& b);

	DEVICE_FUNC
		vec3 add(const vec3& a, const vec3& b);

	DEVICE_FUNC
		vec4 add(const vec4& a, const vec4& b);

	DEVICE_FUNC
		vec3 add(const vec3& v, float f);

	// DIFF
	DEVICE_FUNC
		vec3 sub(float f, const vec3& v);

	DEVICE_FUNC
		vec3 sub(const vec3& a, const vec3& b);

	DEVICE_FUNC
		void sub_in(float f, vec3* v);

	// FLOOR
	DEVICE_FUNC
		vec3 floor(const vec3& v);

	// MULTIPLICATION

	DEVICE_FUNC
		mat3 mul(const mat3& a, const mat3& b);

	DEVICE_FUNC
		vec3 mul(const mat3& a, const vec3& b);

	DEVICE_FUNC
		mat3 mul(const mat3& m, float f);

	DEVICE_FUNC
		vec4 mul(const vec4& v, float f);

	// compute a * transposed(a)
	DEVICE_FUNC
		mat3 mul_trans(const mat3& a);

	DEVICE_FUNC
		vec3 mul(const vec3& v, float f);

	DEVICE_FUNC
		void mul_in(vec3* v, float f);

	DEVICE_FUNC
		void mul_in(mat3* v, float f);


	// SQUARE
	DEVICE_FUNC
		void square_in(vec3* v);


	// SET ZERO
	DEVICE_FUNC
		void set_zero(vec3* a);

	DEVICE_FUNC
		void set_zero(vec4* a);

	DEVICE_FUNC
		void set_zero(mat3* a);

	// DETERMINANT
	DEVICE_FUNC
	float determinant(const mat3& m);

	// DOT
	DEVICE_FUNC
		float dot3(const float* a, const float* b);

	DEVICE_FUNC
		float dot3(const vec3 a, const vec3 b);

	DEVICE_FUNC
		float dot3(const vec4 a, const vec3 b);

	// ADD OUTTER PRODUCT TO MATRIX
	DEVICE_FUNC
	void add_outter_product(mat3* r, const vec3& a, vec3& b);

	DEVICE_FUNC
		vec3 col0(const mat3& m);

	DEVICE_FUNC
		vec3 col1(const mat3& m);

	DEVICE_FUNC
		vec3 col2(const mat3& m);

#ifdef GENERATE_IMPLEMENTATION_MM

	// ADD
	DEVICE_FUNC 
	void add_to(vec3* a, const vec3& b) {
		a->x += b.x;
		a->y += b.y;
		a->z += b.z;
	}

	DEVICE_FUNC
	void add_to(vec4* a, const vec3& b) {
		a->x += b.x;
		a->y += b.y;
		a->z += b.z;
	}

	DEVICE_FUNC
	void add_to(vec4* a, const vec4& b) {
		a->x += b.x;
		a->y += b.y;
		a->z += b.z;
		a->w += b.w;
	}

	DEVICE_FUNC
	void add_to(vec3* a, float f) {
		a->x += f;
		a->y += f;
		a->z += f;
	}

	DEVICE_FUNC
	void add_to(vec4* a, float f) {
		a->x += f;
		a->y += f;
		a->z += f;
		a->w += f;
	}

	DEVICE_FUNC
	void add_to(mat3* m, float f) {
		m->m[0][0] += f;
		m->m[1][1] += f;
		m->m[2][2] += f;
	}

	DEVICE_FUNC
	void add_to(mat3* m, const mat3& b)
	{
#pragma unroll
		for (int i = 0; i < 3; ++i) {
#pragma unroll
			for (int j = 0; j < 3; ++j) {
				m->m[i][j] += b.m[i][j];
			}
		}
	}

	DEVICE_FUNC
	vec3 add(const vec3& a, const vec3& b) {
		vec3 res;
		res.x = a.x + b.x;
		res.y = a.y + b.y;
		res.z = a.z + b.z;
		return res;
	}

	DEVICE_FUNC
	vec4 add(const vec4& a, const vec4& b) {
		vec4 res;
		res.x = a.x + b.x;
		res.y = a.y + b.y;
		res.z = a.z + b.z;
		res.w = a.w + b.w;

		return res;
	}

	DEVICE_FUNC
		vec3 add(const vec3& v, float f)
	{
		vec3 r;
		r.x = v.x + f;
		r.y = v.y + f;
		r.z = v.z + f;
		
		return r;
	}

	// DIFF
	DEVICE_FUNC
	vec3 sub(float f, const vec3& v) {
		vec3 r;
		r.x = f - v.x;
		r.y = f - v.y;
		r.z = f - v.z;

		return r;
	}

	DEVICE_FUNC
	vec3 sub(const vec3& a, const vec3& b) {
		vec3 r;
		r.x = a.x - b.x;
		r.y = a.y - b.y;
		r.z = a.z - b.z;

		return r;
	}

	DEVICE_FUNC
	void sub_in(float f, vec3* v) {
		v->x = f - v->x;
		v->y = f - v->y;
		v->z = f - v->z;
	}

	// FLOOR
	DEVICE_FUNC
	vec3 floor(const vec3& v) {
		vec3 r;
		r.x = std::floor(v.x);
		r.y = std::floor(v.y);
		r.z = std::floor(v.z);

		return r;
	}

	// MULTIPLICATION

	DEVICE_FUNC
	vec3 mul(const mat3& a, const vec3& b) {
		vec3 r;
		float* p = reinterpret_cast<float*>(&r);
		const float* pa = reinterpret_cast<const float*>(a.m);
		const float* pb = reinterpret_cast<const float*>(&b);

#pragma unroll
		for (int i = 0; i < 3; ++i) {
			p[i] = dot3(pa + 3 * i, pb);
		}

		return r;
	}

	DEVICE_FUNC
	mat3 mul(const mat3& a, const mat3& b) {
		mat3 res;
#pragma unroll
		for (int i = 0; i < 3; ++i) {
#pragma unroll
			for (int j = 0; j < 3; ++j) {
				float acc = 0.0f;
#pragma unroll
				for (int k = 0; k < 3; ++k) {
					acc += a.m[i][k] * b.m[k][j];
				}
				res.m[i][j] = acc;
			}
		}

		return res;
	}

	// compute a * transposed(a)
	DEVICE_FUNC 
	mat3 mul_trans(const mat3& a) {
		mat3 res;
#pragma unroll
		for (int i = 0; i < 3; ++i) {
#pragma unroll
			for (int j = 0; j < 3; ++j) {
				float acc = 0.0f;
#pragma unroll
				for (int k = 0; k < 3; ++k) {
					acc += a.m[i][k] * a.m[i][k];
				}
				res.m[i][j] = acc;
			}
		}

		return res;
	}

	DEVICE_FUNC
	vec3 mul(const vec3& v, float f) {
		vec3 r;
		r.x = v.x * f;
		r.y = v.y * f;
		r.z = v.z * f;

		return r;
	}

	DEVICE_FUNC
	void mul_in(vec3* v, float f) {
		v->x *= f;
		v->y *= f;
		v->z *= f;
	}

	DEVICE_FUNC
	void mul_in(mat3* m, float f) {
		float* p = reinterpret_cast<float*>(m->m);
#pragma unroll
		for (int i = 0; i < 9; ++i) {
			p[i] *= f;
		}
	}

	DEVICE_FUNC
	mat3 mul(const mat3& m, float f) {
		mat3 r;
		float* pr = reinterpret_cast<float*>(r.m);
		const float* p = reinterpret_cast<const float*>(m.m);
#pragma unroll
		for (int i = 0; i < 9; ++i) {
			pr[i] = p[i] * f;
		}

		return r;
	}

	DEVICE_FUNC
	vec4 mul(const vec4& v, float f) {
		vec4 r;
		r.x = v.x * f;
		r.y = v.y * f;
		r.z = v.z * f;
		r.w = v.w * f;

		return r;
	}

	// SQUARE
	DEVICE_FUNC
	void square_in(vec3* v) {
		v->x *= v->x;
		v->y *= v->y;
		v->z *= v->z;
	}


	// SET ZERO
	DEVICE_FUNC
	void set_zero(vec3* a) {
		a->x = 0; a->y = 0; a->z = 0;
	}

	DEVICE_FUNC
	void set_zero(vec4* a) {
		a->x = 0; a->y = 0; a->z = 0; a->w = 0;
	}
	DEVICE_FUNC
	void set_zero(mat3* a) {
		float* pr = reinterpret_cast<float*>(a->m);
#pragma unroll
		for (int i = 0; i < 9; ++i) {
			pr[i] = 0;
		}
	}

	// DETERMINANT
	DEVICE_FUNC
	float determinant(const mat3& m) {
		float q0 = m.m[1][1] * m.m[2][2] - m.m[1][2] * m.m[2][1];
		float q1 = m.m[1][0] * m.m[2][2] - m.m[1][2] * m.m[2][0];
		float q2 = m.m[1][0] * m.m[2][1] - m.m[1][1] * m.m[2][0];
		return m.m[0][0] * q0 - m.m[0][1] * q1 + m.m[0][2] * q2;
	}

	// DOT
	DEVICE_FUNC
	float dot3(const float* a, const float* b) {
		return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
	}

	DEVICE_FUNC
	float dot3(const vec3 a, const vec3 b) {
		return dot3(
			reinterpret_cast<const float*>(&a),
			reinterpret_cast<const float*>(&b));
	}

	DEVICE_FUNC
		float dot3(const vec4 a, const vec3 b) {
		return dot3(
			reinterpret_cast<const float*>(&a),
			reinterpret_cast<const float*>(&b));
	}

	// OUTTER PRODUCT
	DEVICE_FUNC
	void add_outter_product(mat3* r, const vec3& a, vec3& b)
	{
		// Totally unrolled outer product
		r->m[0][0] += a.x * b.x;
		r->m[0][1] += a.x * b.y;
		r->m[0][2] += a.x * b.z;

		r->m[1][0] += a.y * b.x;
		r->m[1][1] += a.y * b.y;
		r->m[1][2] += a.y * b.z;

		r->m[2][0] += a.z * b.x;
		r->m[2][1] += a.z * b.y;
		r->m[2][2] += a.z * b.z;
	}


	DEVICE_FUNC
	vec3 col0(const mat3& m) {
		vec3 v;
		v.x = m.m[0][0];
		v.y = m.m[1][0];
		v.z = m.m[2][0];

		return v;
	}

	DEVICE_FUNC
	vec3 col1(const mat3& m) {
		vec3 v;
		v.x = m.m[0][1];
		v.y = m.m[1][1];
		v.z = m.m[2][1];

		return v;
	}

	DEVICE_FUNC
	vec3 col2(const mat3& m) {
		vec3 v;
		v.x = m.m[0][2];
		v.y = m.m[1][2];
		v.z = m.m[2][2];

		return v;
	}


#endif // GENERATE_IMPLEMENTATION_MM

}

#endif // !MINMATH_H


