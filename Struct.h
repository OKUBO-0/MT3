#pragma once

#include "Vector3.h"
#include "Matrix4x4.h"

struct Line {
	Vector3 origir;
	Vector3 diff;
};

struct Ray {
	Vector3 origin;
	Vector3 diff;
};

struct Segment {
	Vector3 origir;
	Vector3 diff;
};

struct Sphere {
	Vector3 center;
	float radius;
};