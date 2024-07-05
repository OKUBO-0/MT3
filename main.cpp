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

	//平面
	AABB aabb1{
		.min{-0.5f, -0.5f, -0.5f},
		.max{ 0.0f, 0.0f, 0.0f}
	};
	AABB aabb2{
		.min{0.2f, 0.2f, 0.2f},
		.max{ 1.0f, 1.0f, 1.0f}
	};

	//色
	uint32_t colorS1 = WHITE;
	uint32_t colorS2 = WHITE;

	Matrix4x4 worldMatrix = MyMath::MakeAffineMatrix({ 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f });
	Matrix4x4 cameraMatrix = MyMath::MakeAffineMatrix({ 1.0f, 1.0f, 1.0f }, cameraRotate, cameraTranslate);
	Matrix4x4 viewMatrix = MyMath::Inverse(cameraMatrix);
	Matrix4x4 projectionMatrix = MyMath::MakePerspectiveFovMatrix(0.45f, float(kWindowWidth) / float(kWindowHeight), 0.1f, 100.0f);
	Matrix4x4 worldViewProjectionMatrix = MyMath::Multiply(worldMatrix, MyMath::Multiply(viewMatrix, projectionMatrix));
	Matrix4x4 viewportMatrix = MyMath::MakeViewPortMatrix(0, 0, float(kWindowWidth), float(kWindowHeight), 0.0f, 1.0f);

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

		Vector3 move{};
		Matrix4x4 trans = MyMath::MakeTranslateMatrix(cameraTranslate);

		if (keys[DIK_W]) {
			move.z += 0.1f;
		}
		if (keys[DIK_S]) {
			move.z -= 0.1f;
		}
		if (keys[DIK_A]) {
			move.x -= 0.1f;
		}
		if (keys[DIK_D]) {
			move.x += 0.1f;
		}
		if (keys[DIK_RIGHTARROW]) {
			cameraRotate.y += 0.1f;
		}
		if (keys[DIK_LEFTARROW]) {
			cameraRotate.y -= 0.1f;
		}

		cameraTranslate = MyMath::TransformCoord(move, trans);
		//諸々の変換
		worldMatrix = MyMath::MakeAffineMatrix({ 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f });
		cameraMatrix = MyMath::MakeAffineMatrix({ 1.0f, 1.0f, 1.0f }, cameraRotate, cameraTranslate);
		viewMatrix = MyMath::Inverse(cameraMatrix);
		projectionMatrix = MyMath::MakePerspectiveFovMatrix(0.45f, float(kWindowWidth) / float(kWindowHeight), 0.1f, 100.0f);
		worldViewProjectionMatrix = MyMath::Multiply(worldMatrix, MyMath::Multiply(viewMatrix, projectionMatrix));
		viewportMatrix = MyMath::MakeViewPortMatrix(0, 0, float(kWindowWidth), float(kWindowHeight), 0.0f, 1.0f);

		//当たり判定
		if (MyMath::IsCollision(aabb1, aabb2)) {
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

		
		MyDraw::DrawGrid(worldViewProjectionMatrix, viewportMatrix);
		MyDraw::DrawAABB(aabb1, worldViewProjectionMatrix, viewportMatrix, colorS1);
		MyDraw::DrawAABB(aabb2, worldViewProjectionMatrix, viewportMatrix, colorS2);

		ImGui::Begin("Debug");

		if (ImGui::CollapsingHeader("camera")) {
			ImGui::DragFloat3("cameraTRa", &cameraTranslate.x, 0.1f, -50.0f, 50.0f);
			ImGui::DragFloat3("cameraRot", &cameraRotate.x, 0.1f, -50.0f, 50.0f);
		}
		ImGui::Separator();

		if (ImGui::CollapsingHeader("AABB")) {
			ImGui::DragFloat3("AABB1min", &aabb1.min.x, 0.1f, -1.0f, 5.0f);
			ImGui::DragFloat3("AABB1max", &aabb1.max.x, 0.1f, -1.0f, 5.0f);
			ImGui::DragFloat3("AABB2min", &aabb2.min.x, 0.1f, -1.0f, 5.0f);
			ImGui::DragFloat3("AABB2max", &aabb2.max.x, 0.1f, -1.0f, 5.0f);
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