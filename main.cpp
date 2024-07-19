#include <Novice.h>
#include "MyMath.h"
#include "ImGuiManager.h"

const char kWindowTitle[] = "LE2C_06_オオクボ_タク";

const int kWindowWidth = 1280;
const int kWindowHeight = 720;


// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, kWindowWidth, kWindowHeight);

	Vector3 cameraTranslate{ 0.0f, 1.9f, -6.49f };
	Vector3 cameraRotate{ 0.26f, 0.0f, 0.0f };

	Sphere sphere{
		{0.0f, 0.0f, 0.0f},
		0.5f
	};

	Segment segment({ -2.0f, -1.0f, 0.0f }, { 3.0f, 2.0f, 2.0f });
	Vector3 point{ -1.5f, 0.6f, 0.6f };

	Vector3 project = MyMath::Project(MyMath::Subtract(point, segment.origin), segment.diff);
	Vector3 clossestPoint = MyMath::ClosestPoint(point, segment);

	Sphere pointSphere{ point, 0.01f };
	Sphere clossestPointSphere{ clossestPoint, 0.01f };


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


		Matrix4x4 worldMatrix = MyMath::MakeAffineMatrix({ 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f });
		Matrix4x4 cameraMatrix = MyMath::MakeAffineMatrix({ 1.0f, 1.0f, 1.0f }, cameraRotate, cameraTranslate);
		Matrix4x4 viewMatrix = MyMath::Inverse(cameraMatrix);
		Matrix4x4 projectionMatrix = MyMath::MakePerspectiveFovMatrix(0.45f, float(kWindowWidth) / float(kWindowHeight), 0.1f, 100.0f);
		Matrix4x4 worldViewProjectionMatrix = MyMath::Multiply(worldMatrix, MyMath::Multiply(viewMatrix, projectionMatrix));
		Matrix4x4 viewportMatrix = MyMath::MakeViewPortMatrix(0, 0, float(kWindowWidth), float(kWindowHeight), 0.0f, 1.0f);



		///
		/// ↑更新処理ここまで
		///


		///
		/// ↓描画処理ここから
		///



		MyMath::DrawGrid(worldViewProjectionMatrix, viewportMatrix);

		MyMath::DrawShere(pointSphere, worldViewProjectionMatrix, viewportMatrix, RED);
		MyMath::DrawShere(clossestPointSphere, worldViewProjectionMatrix, viewportMatrix, BLACK);

		Vector3 start = MyMath::TransformCoord(MyMath::TransformCoord(segment.origin, worldViewProjectionMatrix), viewportMatrix);
		Vector3 end = MyMath::TransformCoord(MyMath::TransformCoord(MyMath::Add(segment.origin, segment.diff), worldViewProjectionMatrix), viewportMatrix);

		Novice::DrawLine(int(start.x), int(start.y), int(end.x), int(end.y), WHITE);

		ImGui::Begin("Debug");
		ImGui::DragFloat3("cameraTRa", &cameraTranslate.x, 0.1f, -50.0f, 50.0f);
		ImGui::DragFloat3("cameraRot", &cameraRotate.x, 0.1f, -50.0f, 50.0f);
		ImGui::InputFloat3("Project", &project.x, "%.3f", ImGuiInputTextFlags_ReadOnly);

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