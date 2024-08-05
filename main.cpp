
#include <Novice.h>
#define _USE_MATH_DEFINES
#include <cmath>
#include <assert.h>
#include <imgui.h>
#include <algorithm>

struct Vec3 {
	float x;
	float y;
	float z;
};

struct Matrix4x4 {
	float m[4][4];
};

struct Sphere {
	Vec3 center;
	float radius;
};

struct Plane {
	Vec3 normal;
	float distance;
};

struct Segment {
	Vec3 origin;
	Vec3 diff;
};

struct Triangle {
	Vec3 vertices[3];
};

struct AABB {
	Vec3 min;
	Vec3 max;
};

//加算
Vec3 Add(const Vec3& v1, const Vec3& v2) {
	Vec3 result;
	result.x = v1.x + v2.x;
	result.y = v1.y + v2.y;
	result.z = v1.z + v2.z;
	return result;
}

//減算
Vec3 Subtract(const Vec3& v1, const Vec3& v2) {
	Vec3 result;
	result.x = v1.x - v2.x;
	result.y = v1.y - v2.y;
	result.z = v1.z - v2.z;
	return result;
}

//スカラー倍
Vec3 MultiplyVec3(float scaler, const Vec3& v) {
	Vec3 result;
	result.x = v.x * scaler;
	result.y = v.y * scaler;
	result.z = v.z * scaler;
	return result;
}

Vec3 MultiplyV(const Vec3& v0, const Vec3& v1) {
	Vec3 result;
	result.x = v0.x * v1.x;
	result.y = v0.y * v1.y;
	result.z = v0.z * v1.z;
	return result;
}

float Dot(const Vec3& v1, const Vec3& v2) {
	float result;
	result = v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
	return result;
}

//単位行列の作成
Matrix4x4 MakeIdentity4x4() {
	Matrix4x4 result;

	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			if (i == j) {
				result.m[i][j] = 1.0f;
			}
			else {
				result.m[i][j] = 0.0f;
			}
		}
	}

	return result;
}

//行列の積
Matrix4x4 Multiply(const Matrix4x4& matrix1, const Matrix4x4& matrix2) {
	Matrix4x4 result;

	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			result.m[i][j] = 0;
			for (int k = 0; k < 4; ++k) {
				result.m[i][j] += matrix1.m[i][k] * matrix2.m[k][j];
			}
		}
	}

	return result;
}

//Scale
Matrix4x4 MakeScaleMatrix(const Vec3& scale) {
	Matrix4x4 matrix;

	matrix = MakeIdentity4x4();

	matrix.m[0][0] = scale.x;
	matrix.m[1][1] = scale.y;
	matrix.m[2][2] = scale.z;

	return matrix;
}

//Rotate
Matrix4x4 MakeRotateXMatrix(float radian) {
	Matrix4x4 result;

	result = MakeIdentity4x4();

	result.m[1][1] = std::cos(radian);
	result.m[1][2] = std::sin(radian);
	result.m[2][1] = -std::sin(radian);
	result.m[2][2] = std::cos(radian);

	return result;
}

Matrix4x4 MakeRotateYMatrix(float radian) {
	Matrix4x4 result;

	result = MakeIdentity4x4();

	result.m[0][0] = std::cos(radian);
	result.m[0][2] = -std::sin(radian);
	result.m[2][0] = std::sin(radian);
	result.m[2][2] = std::cos(radian);

	return result;
}

Matrix4x4 MakeRotateZMatrix(float radian) {
	Matrix4x4 result;

	result = MakeIdentity4x4();

	result.m[0][0] = std::cos(radian);
	result.m[0][1] = std::sin(radian);
	result.m[1][0] = -std::sin(radian);
	result.m[1][1] = std::cos(radian);

	return result;
}

Matrix4x4 MakeRotateMatrix(const Vec3& rotate) {
	Matrix4x4 rotateXMatrix = MakeRotateXMatrix(rotate.x);
	Matrix4x4 rotateYMatrix = MakeRotateYMatrix(rotate.y);
	Matrix4x4 rotateZMatrix = MakeRotateZMatrix(rotate.z);
	Matrix4x4 matrix = Multiply(rotateXMatrix, Multiply(rotateYMatrix, rotateZMatrix));
	return matrix;
}

//Translate
Matrix4x4 MakeTranslateMatrix(const Vec3& translate) {
	Matrix4x4 matrix;
	matrix = MakeIdentity4x4();

	matrix.m[3][0] = translate.x;
	matrix.m[3][1] = translate.y;
	matrix.m[3][2] = translate.z;

	return matrix;
}

//Transform
Vec3 Transform(const Vec3& vector, const Matrix4x4& matrix) {
	Vec3 result;
	// 各成分を計算
	result.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0] + matrix.m[3][0];
	result.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1] + matrix.m[3][1];
	result.z = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2] + matrix.m[3][2];
	float w = vector.x * matrix.m[0][3] + vector.y * matrix.m[1][3] + vector.z * matrix.m[2][3] + matrix.m[3][3];

	if (w != 0.0f) {
		result.x /= w;
		result.y /= w;
		result.z /= w;
	}

	return result;
}

//Affine
Matrix4x4 MakeAffineMatrix(const Vec3& scale, const Vec3& rotate, const Vec3& translate) {
	Matrix4x4 scaleMa = MakeScaleMatrix(scale);
	Matrix4x4 rotateMa = MakeRotateMatrix(rotate);
	Matrix4x4 translateMa = MakeTranslateMatrix(translate);

	Matrix4x4 matrix = Multiply(scaleMa, Multiply(rotateMa, translateMa));
	return matrix;
}

//透視投影行列
Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip) {
	Matrix4x4 matrix;
	matrix.m[0][0] = 1.0f / aspectRatio * (1.0f / std::tan(fovY / 2.0f));
	matrix.m[0][1] = 0.0f;
	matrix.m[0][2] = 0.0f;
	matrix.m[0][3] = 0.0f;
	matrix.m[1][0] = 0.0f;
	matrix.m[1][1] = 1.0f / std::tan(fovY / 2.0f);
	matrix.m[1][2] = 0.0f;
	matrix.m[1][3] = 0.0f;
	matrix.m[2][0] = 0.0f;
	matrix.m[2][1] = 0.0f;
	matrix.m[2][2] = farClip / (farClip - nearClip);
	matrix.m[2][3] = 1.0f;
	matrix.m[3][0] = 0.0f;
	matrix.m[3][1] = 0.0f;
	matrix.m[3][2] = (-nearClip * farClip) / (farClip - nearClip);
	matrix.m[3][3] = 0.0f;

	return matrix;
}

//正射影行列
Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip) {
	Matrix4x4 matrix;
	matrix.m[0][0] = 2.0f / (right - left);
	matrix.m[0][1] = 0.0f;
	matrix.m[0][2] = 0.0f;
	matrix.m[0][3] = 0.0f;
	matrix.m[1][0] = 0.0f;
	matrix.m[1][1] = 2.0f / (top - bottom);
	matrix.m[1][2] = 0.0f;
	matrix.m[1][3] = 0.0f;
	matrix.m[2][0] = 0.0f;
	matrix.m[2][1] = 0.0f;
	matrix.m[2][2] = 1.0f / (farClip - nearClip);
	matrix.m[2][3] = 0.0f;
	matrix.m[3][0] = (left + right) / (left - right);
	matrix.m[3][1] = (top + bottom) / (bottom - top);
	matrix.m[3][2] = nearClip / (nearClip - farClip);
	matrix.m[3][3] = 1.0f;

	return matrix;
}

//ビューポート変換行列
Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth) {
	Matrix4x4 matrix;
	matrix.m[0][0] = width / 2.0f;
	matrix.m[0][1] = 0.0f;
	matrix.m[0][2] = 0.0f;
	matrix.m[0][3] = 0.0f;
	matrix.m[1][0] = 0.0f;
	matrix.m[1][1] = -(height / 2.0f);
	matrix.m[1][2] = 0.0f;
	matrix.m[1][3] = 0.0f;
	matrix.m[2][0] = 0.0f;
	matrix.m[2][1] = 0.0f;
	matrix.m[2][2] = maxDepth - minDepth;
	matrix.m[2][3] = 0.0f;
	matrix.m[3][0] = left + (width / 2.0f);
	matrix.m[3][1] = top + (height / 2.0f);
	matrix.m[3][2] = minDepth;
	matrix.m[3][3] = 1.0f;

	return matrix;
}

// 行列式を計算する関数
float Determinant(const Matrix4x4& matrix) {
	float det =
		matrix.m[0][0] * (matrix.m[1][1] * matrix.m[2][2] * matrix.m[3][3] +
			matrix.m[1][2] * matrix.m[2][3] * matrix.m[3][1] +
			matrix.m[1][3] * matrix.m[2][1] * matrix.m[3][2] -
			matrix.m[1][3] * matrix.m[2][2] * matrix.m[3][1] -
			matrix.m[1][1] * matrix.m[2][3] * matrix.m[3][2] -
			matrix.m[1][2] * matrix.m[2][1] * matrix.m[3][3]) -
		matrix.m[0][1] * (matrix.m[1][0] * matrix.m[2][2] * matrix.m[3][3] +
			matrix.m[1][2] * matrix.m[2][3] * matrix.m[3][0] +
			matrix.m[1][3] * matrix.m[2][0] * matrix.m[3][2] -
			matrix.m[1][3] * matrix.m[2][2] * matrix.m[3][0] -
			matrix.m[1][0] * matrix.m[2][3] * matrix.m[3][2] -
			matrix.m[1][2] * matrix.m[2][0] * matrix.m[3][3]) +
		matrix.m[0][2] * (matrix.m[1][0] * matrix.m[2][1] * matrix.m[3][3] +
			matrix.m[1][1] * matrix.m[2][3] * matrix.m[3][0] +
			matrix.m[1][3] * matrix.m[2][0] * matrix.m[3][1] -
			matrix.m[1][3] * matrix.m[2][1] * matrix.m[3][0] -
			matrix.m[1][0] * matrix.m[2][3] * matrix.m[3][1] -
			matrix.m[1][1] * matrix.m[2][0] * matrix.m[3][3]) -
		matrix.m[0][3] * (matrix.m[1][0] * matrix.m[2][1] * matrix.m[3][2] +
			matrix.m[1][1] * matrix.m[2][2] * matrix.m[3][0] +
			matrix.m[1][2] * matrix.m[2][0] * matrix.m[3][1] -
			matrix.m[1][2] * matrix.m[2][1] * matrix.m[3][0] -
			matrix.m[1][0] * matrix.m[2][2] * matrix.m[3][1] -
			matrix.m[1][1] * matrix.m[2][0] * matrix.m[3][2]);

	return det;
}

//逆行列
Matrix4x4 Inverse(const Matrix4x4& matrix) {
	Matrix4x4 result;

	result.m[0][0] = (matrix.m[1][1] * matrix.m[2][2] * matrix.m[3][3] +
		matrix.m[1][2] * matrix.m[2][3] * matrix.m[3][1] +
		matrix.m[1][3] * matrix.m[2][1] * matrix.m[3][2] -
		matrix.m[1][3] * matrix.m[2][2] * matrix.m[3][1] -
		matrix.m[1][2] * matrix.m[2][1] * matrix.m[3][3] -
		matrix.m[1][1] * matrix.m[2][3] * matrix.m[3][2]);

	result.m[0][1] = (-matrix.m[0][1] * matrix.m[2][2] * matrix.m[3][3] -
		matrix.m[0][2] * matrix.m[2][3] * matrix.m[3][1] -
		matrix.m[0][3] * matrix.m[2][1] * matrix.m[3][2] +
		matrix.m[0][3] * matrix.m[2][2] * matrix.m[3][1] +
		matrix.m[0][2] * matrix.m[2][1] * matrix.m[3][3] +
		matrix.m[0][1] * matrix.m[2][3] * matrix.m[3][2]);

	result.m[0][2] = (matrix.m[0][1] * matrix.m[1][2] * matrix.m[3][3] +
		matrix.m[0][2] * matrix.m[1][3] * matrix.m[3][1] +
		matrix.m[0][3] * matrix.m[1][1] * matrix.m[3][2] -
		matrix.m[0][3] * matrix.m[1][2] * matrix.m[3][1] -
		matrix.m[0][2] * matrix.m[1][1] * matrix.m[3][3] -
		matrix.m[0][1] * matrix.m[1][3] * matrix.m[3][2]);

	result.m[0][3] = (-matrix.m[0][1] * matrix.m[1][2] * matrix.m[2][3] -
		matrix.m[0][2] * matrix.m[1][3] * matrix.m[2][1] -
		matrix.m[0][3] * matrix.m[1][1] * matrix.m[2][2] +
		matrix.m[0][3] * matrix.m[1][2] * matrix.m[2][1] +
		matrix.m[0][2] * matrix.m[1][1] * matrix.m[2][3] +
		matrix.m[0][1] * matrix.m[1][3] * matrix.m[2][2]);

	result.m[1][0] = (-matrix.m[1][0] * matrix.m[2][2] * matrix.m[3][3] -
		matrix.m[1][2] * matrix.m[2][3] * matrix.m[3][0] -
		matrix.m[1][3] * matrix.m[2][0] * matrix.m[3][2] +
		matrix.m[1][3] * matrix.m[2][2] * matrix.m[3][0] +
		matrix.m[1][2] * matrix.m[2][0] * matrix.m[3][3] +
		matrix.m[1][0] * matrix.m[2][3] * matrix.m[3][2]);

	result.m[1][1] = (matrix.m[0][0] * matrix.m[2][2] * matrix.m[3][3] +
		matrix.m[0][2] * matrix.m[2][3] * matrix.m[3][0] +
		matrix.m[0][3] * matrix.m[2][0] * matrix.m[3][2] -
		matrix.m[0][3] * matrix.m[2][2] * matrix.m[3][0] -
		matrix.m[0][2] * matrix.m[2][0] * matrix.m[3][3] -
		matrix.m[0][0] * matrix.m[2][3] * matrix.m[3][2]);

	result.m[1][2] = (-matrix.m[0][0] * matrix.m[1][2] * matrix.m[3][0] -
		matrix.m[0][2] * matrix.m[1][3] * matrix.m[3][0] -
		matrix.m[0][3] * matrix.m[1][0] * matrix.m[3][2] +
		matrix.m[0][3] * matrix.m[1][2] * matrix.m[3][0] +
		matrix.m[0][2] * matrix.m[1][0] * matrix.m[3][3] +
		matrix.m[0][0] * matrix.m[1][3] * matrix.m[3][2]);

	result.m[1][3] = (matrix.m[0][0] * matrix.m[1][2] * matrix.m[2][3] +
		matrix.m[0][2] * matrix.m[1][3] * matrix.m[2][0] +
		matrix.m[0][3] * matrix.m[1][0] * matrix.m[2][2] -
		matrix.m[0][3] * matrix.m[1][2] * matrix.m[2][0] -
		matrix.m[0][2] * matrix.m[1][0] * matrix.m[2][3] -
		matrix.m[0][0] * matrix.m[1][3] * matrix.m[2][2]);

	result.m[2][0] = (matrix.m[1][0] * matrix.m[2][1] * matrix.m[3][3] +
		matrix.m[1][1] * matrix.m[2][3] * matrix.m[3][0] +
		matrix.m[1][3] * matrix.m[2][0] * matrix.m[3][1] -
		matrix.m[1][3] * matrix.m[2][1] * matrix.m[3][0] -
		matrix.m[1][1] * matrix.m[2][0] * matrix.m[3][3] -
		matrix.m[1][0] * matrix.m[2][3] * matrix.m[3][1]);

	result.m[2][1] = (-matrix.m[0][0] * matrix.m[2][1] * matrix.m[3][3] -
		matrix.m[0][1] * matrix.m[2][3] * matrix.m[3][0] -
		matrix.m[0][3] * matrix.m[2][0] * matrix.m[3][1] +
		matrix.m[0][3] * matrix.m[2][1] * matrix.m[3][0] +
		matrix.m[0][1] * matrix.m[2][0] * matrix.m[3][3] +
		matrix.m[0][0] * matrix.m[2][3] * matrix.m[3][1]);

	result.m[2][2] = (matrix.m[0][0] * matrix.m[1][1] * matrix.m[3][3] +
		matrix.m[0][1] * matrix.m[1][3] * matrix.m[3][0] +
		matrix.m[0][3] * matrix.m[1][0] * matrix.m[3][1] -
		matrix.m[0][3] * matrix.m[1][1] * matrix.m[3][0] -
		matrix.m[0][1] * matrix.m[1][0] * matrix.m[3][3] -
		matrix.m[0][0] * matrix.m[1][3] * matrix.m[3][1]);

	result.m[2][3] = (-matrix.m[0][0] * matrix.m[1][1] * matrix.m[2][3] -
		matrix.m[0][1] * matrix.m[1][3] * matrix.m[2][0] -
		matrix.m[0][3] * matrix.m[1][0] * matrix.m[2][1] +
		matrix.m[0][3] * matrix.m[1][1] * matrix.m[2][0] +
		matrix.m[0][1] * matrix.m[1][0] * matrix.m[2][3] +
		matrix.m[0][0] * matrix.m[1][3] * matrix.m[2][1]);

	result.m[3][0] = (-matrix.m[1][0] * matrix.m[2][1] * matrix.m[3][2] -
		matrix.m[1][1] * matrix.m[2][2] * matrix.m[3][0] -
		matrix.m[1][2] * matrix.m[2][0] * matrix.m[3][1] +
		matrix.m[1][2] * matrix.m[2][1] * matrix.m[3][0] +
		matrix.m[1][1] * matrix.m[2][0] * matrix.m[3][2] +
		matrix.m[1][0] * matrix.m[2][2] * matrix.m[3][1]);

	result.m[3][1] = (matrix.m[0][0] * matrix.m[2][1] * matrix.m[3][2] +
		matrix.m[0][1] * matrix.m[2][2] * matrix.m[3][0] +
		matrix.m[0][2] * matrix.m[2][0] * matrix.m[3][1] -
		matrix.m[0][2] * matrix.m[2][1] * matrix.m[3][0] -
		matrix.m[0][1] * matrix.m[2][0] * matrix.m[3][2] -
		matrix.m[0][0] * matrix.m[2][2] * matrix.m[3][1]);

	result.m[3][2] = (-matrix.m[0][0] * matrix.m[1][1] * matrix.m[3][2] -
		matrix.m[0][1] * matrix.m[1][2] * matrix.m[3][0] -
		matrix.m[0][2] * matrix.m[1][0] * matrix.m[3][1] +
		matrix.m[0][2] * matrix.m[1][1] * matrix.m[3][0] +
		matrix.m[0][1] * matrix.m[1][0] * matrix.m[3][2] +
		matrix.m[0][0] * matrix.m[1][2] * matrix.m[3][1]);

	result.m[3][3] = (matrix.m[0][0] * matrix.m[1][1] * matrix.m[2][2] +
		matrix.m[0][1] * matrix.m[1][2] * matrix.m[2][0] +
		matrix.m[0][2] * matrix.m[1][0] * matrix.m[2][1] -
		matrix.m[0][2] * matrix.m[1][1] * matrix.m[2][0] -
		matrix.m[0][1] * matrix.m[1][0] * matrix.m[2][2] -
		matrix.m[0][0] * matrix.m[1][2] * matrix.m[2][1]);

	return result;
}

Vec3 camaraTranslate{ 0.0f, 1.9f, -6.49f };
Vec3 camaraRotate{ 0.26f, 0.0f, 0.0f };

//Grid
void DrawGrid(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix) {
	const float kGridHalfWidth = 2.0f;
	const uint32_t kSubdivision = 10;
	const float kGridEvery = (kGridHalfWidth * 2.0f) / float(kSubdivision);

	//Grid横線
	for (uint32_t xIndex = 0; xIndex <= kSubdivision; ++xIndex) {
		Vec3 kLocalVerticse[2] = {
			{-kGridHalfWidth + (kGridEvery * xIndex), 0.0f, -kGridHalfWidth},
			{-kGridHalfWidth + (kGridEvery * xIndex), 0.0f, +kGridHalfWidth}
		};

		Matrix4x4 worldMatrix = MakeAffineMatrix({ 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f });
		Matrix4x4 camaraMatrix = MakeAffineMatrix({ 1.0f, 1.0f, 1.0f }, camaraRotate, camaraTranslate);
		Matrix4x4 viewMatrix = Inverse(camaraMatrix);
		Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, viewProjectionMatrix));

		Vec3 screenVertices[2];
		for (int i = 0; i < 2; ++i) {
			Vec3 ndcVertex = Transform(kLocalVerticse[i], worldViewProjectionMatrix);
			screenVertices[i] = Transform(ndcVertex, viewportMatrix);
		}

		if (xIndex == 5) {
			Novice::DrawLine(
				int(screenVertices[0].x), int(screenVertices[0].y),
				int(screenVertices[1].x), int(screenVertices[1].y),
				BLACK
			);
		}
		else {
			Novice::DrawLine(
				int(screenVertices[0].x), int(screenVertices[0].y),
				int(screenVertices[1].x), int(screenVertices[1].y),
				WHITE
			);
		}
	}

	//Grid縦線
	for (uint32_t xIndex = 0; xIndex <= kSubdivision; ++xIndex) {
		Vec3 kLocalVerticse[2] = {
			{-kGridHalfWidth, 0.0f, -kGridHalfWidth + (kGridEvery * xIndex)},
			{+kGridHalfWidth, 0.0f, -kGridHalfWidth + (kGridEvery * xIndex)}
		};

		Matrix4x4 worldMatrix = MakeAffineMatrix({ 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f });
		Matrix4x4 camaraMatrix = MakeAffineMatrix({ 1.0f, 1.0f, 1.0f }, camaraRotate, camaraTranslate);
		Matrix4x4 viewMatrix = Inverse(camaraMatrix);
		Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, viewProjectionMatrix));

		Vec3 screenVertices[2];
		for (int i = 0; i < 2; ++i) {
			Vec3 ndcVertex = Transform(kLocalVerticse[i], worldViewProjectionMatrix);
			screenVertices[i] = Transform(ndcVertex, viewportMatrix);
		}

		if (xIndex == 5) {
			Novice::DrawLine(
				int(screenVertices[0].x), int(screenVertices[0].y),
				int(screenVertices[1].x), int(screenVertices[1].y),
				BLACK
			);
		}
		else {
			Novice::DrawLine(
				int(screenVertices[0].x), int(screenVertices[0].y),
				int(screenVertices[1].x), int(screenVertices[1].y),
				WHITE
			);
		}
	}
}

//Sphere
void DrawSphere(const Sphere& sphere, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	const uint32_t kSubDivision = 12;
	const float kLonEvery = (2.0f * float(M_PI)) / float(kSubDivision);
	const float kLatEvery = float(M_PI) / float(kSubDivision);

	for (uint32_t latIndex = 0; latIndex < kSubDivision; ++latIndex) {
		float lat = float(M_PI) / 2.0f + kLatEvery * latIndex; //緯度

		for (uint32_t lonIndex = 0; lonIndex < kSubDivision; ++lonIndex) {
			float lon = lonIndex * kLonEvery; //経度

			Vec3 kLocalVerticse[3] = {
				{sphere.radius * std::cos(lat) * std::cos(lon), sphere.radius * std::sin(lat), sphere.radius * std::cos(lat) * std::sin(lon)},
				{sphere.radius * std::cos(lat + kLatEvery) * std::cos(lon), sphere.radius * std::sin(lat + kLatEvery), sphere.radius * std::cos(lat + kLatEvery) * std::sin(lon)},
				{sphere.radius * std::cos(lat) * std::cos(lon + kLonEvery), sphere.radius * std::sin(lat), sphere.radius * std::cos(lat) * std::sin(lon + kLonEvery)}
			};

			Matrix4x4 worldMatrix = MakeAffineMatrix({ 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, sphere.center);
			Matrix4x4 camaraMatrix = MakeAffineMatrix({ 1.0f, 1.0f, 1.0f }, camaraRotate, camaraTranslate);
			Matrix4x4 viewMatrix = Inverse(camaraMatrix);
			Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, viewProjectionMatrix));

			Vec3 screenVertices[3];
			for (int i = 0; i < 3; ++i) {
				Vec3 ndcVertex = Transform(kLocalVerticse[i], worldViewProjectionMatrix);
				screenVertices[i] = Transform(ndcVertex, viewportMatrix);
			}

			//a,b
			Novice::DrawLine(
				int(screenVertices[0].x), int(screenVertices[0].y),
				int(screenVertices[1].x), int(screenVertices[1].y),
				color
			);

			//a, c
			Novice::DrawLine(
				int(screenVertices[0].x), int(screenVertices[0].y),
				int(screenVertices[2].x), int(screenVertices[2].y),
				color
			);
		}
	}
}

void DrawAddSphere(const float radius, const Matrix4x4 worldMatrix, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	const uint32_t kSubDivision = 12;
	const float kLonEvery = (2.0f * float(M_PI)) / float(kSubDivision);
	const float kLatEvery = float(M_PI) / float(kSubDivision);

	for (uint32_t latIndex = 0; latIndex < kSubDivision; ++latIndex) {
		float lat = float(M_PI) / 2.0f + kLatEvery * latIndex; //緯度

		for (uint32_t lonIndex = 0; lonIndex < kSubDivision; ++lonIndex) {
			float lon = lonIndex * kLonEvery; //経度

			Vec3 kLocalVerticse[3] = {
				{radius * std::cos(lat) * std::cos(lon), radius * std::sin(lat), radius * std::cos(lat) * std::sin(lon)},
				{radius * std::cos(lat + kLatEvery) * std::cos(lon), radius * std::sin(lat + kLatEvery), radius * std::cos(lat + kLatEvery) * std::sin(lon)},
				{radius * std::cos(lat) * std::cos(lon + kLonEvery), radius * std::sin(lat), radius * std::cos(lat) * std::sin(lon + kLonEvery)}
			};

			Matrix4x4 camaraMatrix = MakeAffineMatrix({ 1.0f, 1.0f, 1.0f }, camaraRotate, camaraTranslate);
			Matrix4x4 viewMatrix = Inverse(camaraMatrix);
			Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, viewProjectionMatrix));

			Vec3 screenVertices[3];
			for (int i = 0; i < 3; ++i) {
				Vec3 ndcVertex = Transform(kLocalVerticse[i], worldViewProjectionMatrix);
				screenVertices[i] = Transform(ndcVertex, viewportMatrix);
			}

			//a,b
			Novice::DrawLine(
				int(screenVertices[0].x), int(screenVertices[0].y),
				int(screenVertices[1].x), int(screenVertices[1].y),
				color
			);

			//a, c
			Novice::DrawLine(
				int(screenVertices[0].x), int(screenVertices[0].y),
				int(screenVertices[2].x), int(screenVertices[2].y),
				color
			);
		}
	}
}

//
Vec3 Perpendicular(const Vec3& vector) {
	if (vector.x != 0.0f || vector.y != 0.0f) {
		return { -vector.y, vector.x, 0.0f };
	}
	return { 0.0f, -vector.z, vector.y };
}

float Length(const Vec3& v) {
	float result;
	result = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
	return result;
}

Vec3 Normalize(const Vec3& v) {
	Vec3 result;
	float length = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
	return result = { v.x / length, v.y / length, v.z / length };
}

//クロス積
Vec3 Cross(const Vec3& v1, const Vec3& v2) {
	Vec3 result;
	result.x = (v1.y * v2.z) - (v1.z * v2.y);
	result.y = (v1.z * v2.x) - (v1.x * v2.z);
	result.z = (v1.x * v2.y) - (v1.y * v2.x);
	return result;
}

//平面描画
void DrawPlane(const Plane& plane, const  Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	Vec3 center = MultiplyVec3(plane.distance, plane.normal);
	Vec3 perpendiculars[4];
	perpendiculars[0] = Normalize(Perpendicular(plane.normal));
	perpendiculars[1] = { -perpendiculars[0].x, -perpendiculars[0].y, -perpendiculars[0].z };
	perpendiculars[2] = Cross(plane.normal, perpendiculars[0]);
	perpendiculars[3] = { -perpendiculars[2].x, -perpendiculars[2].y, -perpendiculars[2].z };

	Vec3 points[4];
	for (int32_t index = 0; index < 4; ++index) {
		Matrix4x4 worldMatrix = MakeAffineMatrix({ 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, center);
		Matrix4x4 camaraMatrix = MakeAffineMatrix({ 1.0f, 1.0f, 1.0f }, camaraRotate, camaraTranslate);
		Matrix4x4 viewMatrix = Inverse(camaraMatrix);
		Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, viewProjectionMatrix));

		Vec3 extend = MultiplyVec3(2.0f, perpendiculars[index]);
		Vec3 point = Add(center, extend);
		points[index] = Transform(Transform(point, worldViewProjectionMatrix), viewportMatrix);
	}

	Novice::DrawLine(
		int(points[1].x), int(points[1].y),
		int(points[3].x), int(points[3].y),
		color
	);

	Novice::DrawLine(
		int(points[1].x), int(points[1].y),
		int(points[2].x), int(points[2].y),
		color
	);

	Novice::DrawLine(
		int(points[2].x), int(points[2].y),
		int(points[0].x), int(points[0].y),
		color
	);

	Novice::DrawLine(
		int(points[3].x), int(points[3].y),
		int(points[0].x), int(points[0].y),
		color
	);
}

//ライン描画
void DrawLine(const Segment& segment, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	Vec3 kLocalVerticse[2] = {
		{segment.origin},
		{Add(segment.origin, segment.diff)}
	};

	Matrix4x4 worldMatrix = MakeAffineMatrix({ 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f });
	Matrix4x4 camaraMatrix = MakeAffineMatrix({ 1.0f, 1.0f, 1.0f }, camaraRotate, camaraTranslate);
	Matrix4x4 viewMatrix = Inverse(camaraMatrix);
	Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, viewProjectionMatrix));

	Vec3 screenVertices[2];
	for (int i = 0; i < 2; ++i) {
		Vec3 ndcVertex = Transform(kLocalVerticse[i], worldViewProjectionMatrix);
		screenVertices[i] = Transform(ndcVertex, viewportMatrix);
	}

	//a,b
	Novice::DrawLine(
		int(screenVertices[0].x), int(screenVertices[0].y),
		int(screenVertices[1].x), int(screenVertices[1].y),
		color
	);
}

//トライアングルの描画
void DrawTriangle(const Triangle& triangle, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	Vec3 kLocalVerticse[3] = {
		triangle.vertices[0],
		triangle.vertices[1],
		triangle.vertices[2]
	};

	Vec3 screenVertices[3];
	for (int i = 0; i < 3; ++i) {
		Matrix4x4 worldMatrix = MakeAffineMatrix({ 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, kLocalVerticse[i]);
		Matrix4x4 camaraMatrix = MakeAffineMatrix({ 1.0f, 1.0f, 1.0f }, camaraRotate, camaraTranslate);
		Matrix4x4 viewMatrix = Inverse(camaraMatrix);
		Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, viewProjectionMatrix));

		Vec3 ndcVertex = Transform(kLocalVerticse[i], worldViewProjectionMatrix);
		screenVertices[i] = Transform(ndcVertex, viewportMatrix);
	}

	Novice::DrawTriangle(
		int(screenVertices[0].x), int(screenVertices[0].y),
		int(screenVertices[1].x), int(screenVertices[1].y),
		int(screenVertices[2].x), int(screenVertices[2].y),
		color,
		kFillModeWireFrame
	);
}

//立方体の描画
void DrawAABB(const AABB& aabb, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	Vec3 kLocalVerticse[8] = {
		aabb.min, //0
		{aabb.max.x, aabb.min.y, aabb.min.z},//1
		{aabb.max.x, aabb.max.y, aabb.min.z},//2
		{aabb.min.x, aabb.max.y, aabb.min.z},//3
		{aabb.min.x, aabb.max.y, aabb.max.z},//4
		{aabb.min.x, aabb.min.y, aabb.max.z},//5
		{aabb.max.x, aabb.min.y, aabb.max.z},//6
		aabb.max,//7
	};

	Vec3 screenVertices[8];
	for (int i = 0; i < 8; ++i) {
		Matrix4x4 worldMatrix = MakeAffineMatrix({ 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, kLocalVerticse[i]);
		Matrix4x4 camaraMatrix = MakeAffineMatrix({ 1.0f, 1.0f, 1.0f }, camaraRotate, camaraTranslate);
		Matrix4x4 viewMatrix = Inverse(camaraMatrix);
		Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, viewProjectionMatrix));

		Vec3 ndcVertex = Transform(kLocalVerticse[i], worldViewProjectionMatrix);
		screenVertices[i] = Transform(ndcVertex, viewportMatrix);
	}

	for (int i = 0; i < 7; ++i) {
		Novice::DrawLine(
			int(screenVertices[i].x), int(screenVertices[i].y),
			int(screenVertices[i + 1].x), int(screenVertices[i + 1].y),
			color
		);
	}

	Novice::DrawLine(
		int(screenVertices[3].x), int(screenVertices[3].y),
		int(screenVertices[0].x), int(screenVertices[0].y),
		color
	);

	Novice::DrawLine(
		int(screenVertices[7].x), int(screenVertices[7].y),
		int(screenVertices[4].x), int(screenVertices[4].y),
		color
	);

	Novice::DrawLine(
		int(screenVertices[0].x), int(screenVertices[0].y),
		int(screenVertices[5].x), int(screenVertices[5].y),
		color
	);

	Novice::DrawLine(
		int(screenVertices[1].x), int(screenVertices[1].y),
		int(screenVertices[6].x), int(screenVertices[6].y),
		color
	);

	Novice::DrawLine(
		int(screenVertices[2].x), int(screenVertices[2].y),
		int(screenVertices[7].x), int(screenVertices[7].y),
		color
	);
}

//球と球の衝突判定
bool IsCollision(const Sphere& s1, const Sphere& s2) {
	float x = s1.center.x - s2.center.x;
	float y = s1.center.y - s2.center.y;
	float z = s1.center.z - s2.center.z;
	float length = sqrtf(x * x + y * y + z * z);
	if (s1.radius + s2.radius >= length) {
		return true;
	}
	return false;
}

//球と平面の衝突判定
bool IsSphereToPlaneCollision(const Sphere& sphere, const Plane& plane) {
	Normalize(plane.normal);

	float S2PLength = sphere.center.x * plane.normal.x + sphere.center.y * plane.normal.y + sphere.center.z * plane.normal.z - plane.distance;
	if (sphere.radius >= fabs(S2PLength) * 2.0f) {
		return true;
	}
	return false;
}

//線と平面の衝突判定
bool IsSegmentToPlaneCollision(const Segment& segment, const Plane& plane) {
	float dot = Dot(plane.normal, segment.diff);

	if (dot == 0.0f) {
		return false;
	}

	float t = (plane.distance - Dot(segment.origin, plane.normal)) / dot;

	if (t >= 0.0f && t <= 1.0f) {
		return true;
	}
	return false;
}

//三角形と線の衝突判定
bool IsTriangleToSegmentCollision(const Triangle& triangle, const Segment& segment) {
	Vec3 normal = Normalize(Cross(Subtract(triangle.vertices[1], triangle.vertices[0]), Subtract(triangle.vertices[2], triangle.vertices[1])));
	float dot = Dot(normal, segment.diff);

	if (dot == 0.0f) {
		return false;
	}

	float t = (Dot(normal, Subtract(triangle.vertices[0], segment.origin))) / dot;

	Vec3 p = Add(segment.origin, MultiplyVec3(t, segment.diff));

	Vec3 v0 = triangle.vertices[0];
	Vec3 v1 = triangle.vertices[1];
	Vec3 v2 = triangle.vertices[2];

	Vec3 v0p = Subtract(p, v0);
	Vec3 v1p = Subtract(p, v1);
	Vec3 v2p = Subtract(p, v2);

	Vec3 v01 = Subtract(v1, v0);
	Vec3 v12 = Subtract(v2, v1);
	Vec3 v20 = Subtract(v0, v2);

	Vec3 cross01 = Cross(v01, v1p);
	Vec3 cross12 = Cross(v12, v2p);
	Vec3 cross20 = Cross(v20, v0p);

	if (Dot(cross01, normal) >= 0.0f &&
		Dot(cross12, normal) >= 0.0f &&
		Dot(cross20, normal) >= 0.0f) {
		return true;
	}
	return false;
}

//立方体と立方体の衝突判定
bool IsAABBtoAABBCollision(const AABB& aabb1, const AABB& aabb2) {
	if ((aabb1.min.x <= aabb2.max.x && aabb1.max.x >= aabb2.min.x) &&
		(aabb1.min.y <= aabb2.max.y && aabb1.max.y >= aabb2.min.y) &&
		(aabb1.min.z <= aabb2.max.z && aabb1.max.z >= aabb2.min.z)) {
		return true;
	}
	return false;
}

//立方体と球の衝突判定
bool IsAABBtoSphereCollision(const AABB& aabb, const Sphere& sphere) {
	Vec3 closestPoint{ std::clamp(sphere.center.x, aabb.min.x, aabb.max.x),
		std::clamp(sphere.center.y, aabb.min.y, aabb.max.y),
		std::clamp(sphere.center.z, aabb.min.z, aabb.max.z), };

	float distance = Length(Subtract(closestPoint, sphere.center));

	if (distance <= sphere.radius) {
		return true;
	}
	return false;
}

//立方体と線の衝突判定
bool IsAABBtoLineCollision(const AABB& aabb, const Segment& segment) {
	Vec3 dir = segment.diff;
	Vec3 invDir = { 1.0f / dir.x, 1.0f / dir.y, 1.0f / dir.z };
	Vec3 tMin = {
		(aabb.min.x - segment.origin.x) * invDir.x,
		(aabb.min.y - segment.origin.y) * invDir.y,
		(aabb.min.z - segment.origin.z) * invDir.z
	};
	Vec3 tMax = {
		(aabb.max.x - segment.origin.x) * invDir.x,
		(aabb.max.y - segment.origin.y) * invDir.y,
		(aabb.max.z - segment.origin.z) * invDir.z
	};

	float tNearX = min(tMin.x, tMax.x);
	float tNearY = min(tMin.y, tMax.y);
	float tNearZ = min(tMin.z, tMax.z);
	float tFarX = max(tMin.x, tMax.x);
	float tFarY = max(tMin.y, tMax.y);
	float tFarZ = max(tMin.z, tMax.z);

	float tmin = max(max(tNearX, tNearY), tNearZ);
	float tmax = min(min(tFarX, tFarY), tFarZ);

	if (tmin <= tmax && tmax >= 0.0f && tmin <= 1.0f) {
		return true;
	}
	return false;
}

Vec3 Lerp(const Vec3& v1, const Vec3& v2, float t) {
	Vec3 result;
	result.x = v1.x * (1.0f - t) + v2.x * t;
	result.y = v1.y * (1.0f - t) + v2.y * t;
	result.z = v1.z * (1.0f - t) + v2.z * t;
	return result;
}

Vec3 Bezier(const Vec3& p0, const Vec3& p1, const Vec3& p2, float t) {
	Vec3 p0p1 = Lerp(p0, p1, t);
	Vec3 p1p2 = Lerp(p1, p2, t);
	Vec3 p = Lerp(p0p1, p1p2, t);
	return p;
}

void DrawBezier(const Vec3& controlPoint0, const Vec3& controlPoint1, const Vec3& controlPoint2,
	const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	for (int index = 0; index < 32; index++) {
		float t0 = index / 32.0f;
		float t1 = (index + 1) / 32.0f;

		Vec3 bezier0 = Bezier(controlPoint0, controlPoint1, controlPoint2, t0);
		Vec3 bezier1 = Bezier(controlPoint0, controlPoint1, controlPoint2, t1);

		Vec3 kLocalVerticse[2] = {
			{bezier0},
			{bezier1}
		};

		Matrix4x4 worldMatrix = MakeAffineMatrix({ 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f });
		Matrix4x4 camaraMatrix = MakeAffineMatrix({ 1.0f, 1.0f, 1.0f }, camaraRotate, camaraTranslate);
		Matrix4x4 viewMatrix = Inverse(camaraMatrix);
		Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, viewProjectionMatrix));

		Vec3 screenVertices[2];
		for (int i = 0; i < 2; ++i) {
			Vec3 ndcVertex = Transform(kLocalVerticse[i], worldViewProjectionMatrix);
			screenVertices[i] = Transform(ndcVertex, viewportMatrix);
		}

		//a,b
		Novice::DrawLine(
			int(screenVertices[0].x), int(screenVertices[0].y),
			int(screenVertices[1].x), int(screenVertices[1].y),
			color
		);
	}
}



const char kWindowTitle[] = "LE2C_06_オオクボ_タク";

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, 1280.0f / 720.0f, 0.1f, 100.0f);
	Matrix4x4 viewportMatrix = MakeViewportMatrix(0, 0, 1280.0f, 720.0f, 0.0f, 1.0f);

	Vec3 translates[3] = {
		{0.2f, 1.0f, 0.0f},
		{0.4f, 0.0f, 0.0f},
		{0.3f, 0.0f, 0.0f}
	};

	Vec3 rotates[3] = {
		{0.0f, 0.0f, -6.8f},
		{0.0f, 0.0f, -1.4f},
		{0.0f, 0.0f, 0.0f}
	};

	Vec3 scales[3] = {
		{1.0f, 1.0f, 1.0f},
		{1.0f, 1.0f, 1.0f},
		{1.0f, 1.0f, 1.0f}
	};

	/*unsigned int color = WHITE;*/

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		///
		/// ↓更新処理ここから
		///

		Matrix4x4 sWM = MakeAffineMatrix(scales[0], rotates[0], translates[0]);
		Matrix4x4 eWM = Multiply(MakeAffineMatrix(scales[1], rotates[1], translates[1]), sWM);
		Matrix4x4 hWM = Multiply(MakeAffineMatrix(scales[2], rotates[2], translates[2]), eWM);

		ImGui::DragFloat3("CamaraTranslate", &camaraTranslate.x, 0.01f);
		ImGui::DragFloat3("CamaraRotate", &camaraRotate.x, 0.01f);
		ImGui::DragFloat3("p0T", &translates[0].x, 0.01f);
		ImGui::DragFloat3("p0R", &rotates[0].x, 0.01f);
		ImGui::DragFloat3("p0S", &scales[0].x, 0.01f);
		ImGui::DragFloat3("p1T", &translates[1].x, 0.01f);
		ImGui::DragFloat3("p1R", &rotates[1].x, 0.01f);
		ImGui::DragFloat3("p1S", &scales[1].x, 0.01f);
		ImGui::DragFloat3("p2T", &translates[2].x, 0.01f);
		ImGui::DragFloat3("p2R", &rotates[2].x, 0.01f);
		ImGui::DragFloat3("p2S", &scales[2].x, 0.01f);

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///
		DrawGrid(projectionMatrix, viewportMatrix);

		DrawAddSphere(0.1f, sWM, projectionMatrix, viewportMatrix, RED);
		DrawAddSphere(0.1f, eWM, projectionMatrix, viewportMatrix, GREEN);
		DrawAddSphere(0.1f, hWM, projectionMatrix, viewportMatrix, BLUE);

		DrawLine({ {sWM.m[3][0],sWM.m[3][1],sWM.m[3][2]}, {eWM.m[3][0] - sWM.m[3][0],eWM.m[3][1] - sWM.m[3][1],eWM.m[3][2] - sWM.m[3][2]} }, projectionMatrix, viewportMatrix, WHITE);
		DrawLine({ {eWM.m[3][0],eWM.m[3][1],eWM.m[3][2]}, {hWM.m[3][0] - eWM.m[3][0],hWM.m[3][1] - eWM.m[3][1],hWM.m[3][2] - eWM.m[3][2]} }, projectionMatrix, viewportMatrix, WHITE);
		///
		/// ↑描画処理ここまで
		///

		// フレームの終了
		Novice::EndFrame();

		// ESCキーが押されたらループを抜ける
		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	}

	// ライブラリの終了
	Novice::Finalize();
	return 0;
}