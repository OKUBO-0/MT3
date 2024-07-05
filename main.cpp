#include <Novice.h>
#include "MyMath.h"
#include "MyDraw.h"
#include "ImGuiManager.h"

const char kWindowTitle[] = "LE2C_06_オオクボ_タク";

const int kWindowWidth = 1280;
const int kWindowHeight = 720;

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	//カメラ関係
	Vector3 cameraTranslate{ 0.0f, 1.9f, -6.49f };
	Vector3 cameraRotate{ 0.26f, 0.0f, 0.0f };

	Segment segment
	{
		{0.0f, 0.0f, 0.0f},
		{1.0f, 1.0f, 1.0f}
	};

	//平面
	Triangle triangle;
	triangle.vertices[0] = { 0.0f, 1.0f, 0.0f };
	triangle.vertices[1] = { 1.0f, 0.0f, 0.0f };
	triangle.vertices[2] = { -1.0f, 0.0f, 0.0f };

	//色
	uint32_t colorS1 = WHITE;
	uint32_t colorS2 = WHITE;

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

		if (keys[DIK_W])
		{
			cameraTranslate.z += 0.1f;
		}
		if (keys[DIK_S])
		{
			cameraTranslate.z -= 0.1f;
		}
		if (keys[DIK_A])
		{
			cameraTranslate.x -= 0.1f;
		}
		if (keys[DIK_D])
		{
			cameraTranslate.x += 0.1f;
		}
		if (keys[DIK_Q])
		{
			cameraRotate.y += 0.1f;
		}
		if (keys[DIK_E])
		{
			cameraRotate.y -= 0.1f;
		}

		//変換
		Matrix4x4 worldMatrix = MyMath::MakeAffineMatrix({ 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f });
		Matrix4x4 cameraMatrix = MyMath::MakeAffineMatrix({ 1.0f, 1.0f, 1.0f }, cameraRotate, cameraTranslate);
		Matrix4x4 viewMatrix = MyMath::Inverse(cameraMatrix);
		Matrix4x4 projectionMatrix = MyMath::MakePerspectiveFovMatrix(0.45f, float(kWindowWidth) / float(kWindowHeight), 0.1f, 100.0f);
		Matrix4x4 worldViewProjectionMatrix = MyMath::Multiply(worldMatrix, MyMath::Multiply(viewMatrix, projectionMatrix));
		Matrix4x4 viewportMatrix = MyMath::MakeViewPortMatrix(0, 0, float(kWindowWidth), float(kWindowHeight), 0.0f, 1.0f);

		//当たり判定
		if (MyMath::IsCollision(triangle, segment)) {
			colorS1 = RED;
		}
		else {
			colorS1 = WHITE;
		}

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		//描画
		MyDraw::DrawLine(segment, worldViewProjectionMatrix, viewportMatrix, colorS1);
		MyDraw::DrawTriangle(triangle, worldViewProjectionMatrix, viewportMatrix, colorS2);
		MyDraw::DrawGrid(worldViewProjectionMatrix, viewportMatrix);

		ImGui::Begin("Debug");
		if (ImGui::CollapsingHeader("camera")) {
			ImGui::DragFloat3("cameraTRa", &cameraTranslate.x, 0.1f, -50.0f, 50.0f);
			ImGui::DragFloat3("cameraRot", &cameraRotate.x, 0.1f, -50.0f, 50.0f);
		}
		ImGui::Separator();

		if (ImGui::CollapsingHeader("segment")) {
			ImGui::DragFloat3("segmentOrigin", &segment.origin.x, 0.1f, -1.0f, 1.0f);
			ImGui::DragFloat3("segmentDiff", &segment.diff.x, 0.1f, -1.0f, 1.0f);
		}
		ImGui::Separator();

		if (ImGui::CollapsingHeader("triangle")) {
			ImGui::DragFloat3("triangle0", &triangle.vertices[0].x, 0.1f, -1.0f, 5.0f);
			ImGui::DragFloat3("triangle1", &triangle.vertices[1].x, 0.1f, -1.0f, 5.0f);
			ImGui::DragFloat3("triangle2", &triangle.vertices[2].x, 0.1f, -1.0f, 5.0f);
		}
		ImGui::Separator();

		ImGui::End();

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