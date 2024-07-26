#pragma once
#include "struct.h"
#include <cmath>

/// <summary>
/// 描画関数系
/// </summary>
class MyDraw {
public:
	/// <summary>
	/// 平面の描画
	/// </summary>
	/// <param name="plane">平面</param>
	/// <param name="viewProjectionMatrix">ワールドビュープロジェクション行列</param>
	/// <param name="viewportMatrix">ビューポート行列</param>
	/// <param name="color">色</param>
	static void DrawPlane(const Plane& plane, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color);

	/// <summary>
	/// グリッドの描画
	/// </summary>
	/// <param name="viewProjectionMatrix">ワールドビュープロジェクション行列</param>
	/// <param name="viewportMatrix">ビューポート行列</param>
	static void DrawGrid(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix);

	/// <summary>
	/// 球の描画
	/// </summary>
	/// <param name="sphere">球</param>
	/// <param name="viewProjectionMatrix">ワールドビュープロジェクション行列</param>
	/// <param name="viewportMatrix">ビューポート行列</param>
	/// <param name="color">色</param>
	static void DrawShere(const Sphere& sphere, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color);

	/// <summary>
	/// 線分の描画
	/// </summary>
	/// <param name="seg">線分</param>
	/// <param name="viewProjectionMatrix">ワールドビュープロジェクション行列</param>
	/// <param name="viewportMatrix">ビューポート行列</param>
	/// <param name="color">色</param>
	static void DrawLine(const Segment& seg, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color);

	/// <summary>
	/// 半直線の描画
	/// </summary>
	/// <param name="ray">半直線</param>
	/// <param name="viewProjectionMatrix">ワールドビュープロジェクション行列</param>
	/// <param name="viewportMatrix">ビューポート行列</param>
	/// <param name="color">色</param>
	static void DrawLine(const Ray& ray, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color);


	/// <summary>
	/// 直線の描画
	/// </summary>
	/// <param name="line">直線</param>
	/// <param name="viewProjectionMatrix">ワールドビュープロジェクション行列</param>
	/// <param name="viewportMatrix">ビューポート行列</param>
	/// <param name="color">色</param>
	static void DrawLine(const Line& line, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color);

	/// <summary>
	/// 三角形の描画
	/// </summary>
	/// <param name="triangle">三角形</param>
	/// <param name="viewProjectionMatrix">ワールドビュープロジェクション行列</param>
	/// <param name="viewportMatrix">ビューポート行列</param>
	/// <param name="color">色</param>
	static void DrawTriangle(const Triangle& triangle, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color);

	/// <summary>
	/// AABBの描画
	/// </summary>
	/// <param name="aabb">AABB(四角形)</param>
	/// <param name="viewProjectionMatrix"></param>
	/// <param name="viewportMatrix"></param>
	/// <param name="color"></param>
	static void DrawAABB(const AABB& aabb, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color);
};