#include "DxLib.h"
#include "Utility/InputControl.h"
#include "Utility/Vector2D.h"

//=== 定数定義 ===//
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

//こんばんは
const int GROUND_HEIGHT = 400;      // 地面のY座標
const float GRAVITY = 0.5f;         // 重力加速度
const float JUMP_POWER_MIN = 5.0f;  // 最低ジャンプ力
const float JUMP_POWER_MAX = 20.0f; // 最大ジャンプ力
const float CHARGE_SPEED = 0.3f;    // ジャンプ力溜め速度
const float MOVE_SPEED = 2.0f;      // 横移動速度

//=== 構造体 ===//
struct Player {
    Vector2D pos;      // 位置
    Vector2D vel;      // 速度
    bool isJumping;    // 空中にいるか
    bool isCharging;   // ジャンプ溜め中か
    float chargePower; // 溜めジャンプ力
};

//=== プロトタイプ宣言 ===//
void Update(Player& player);
void Draw(const Player& player);

//=== メイン関数 ===//
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    ChangeWindowMode(TRUE);
    SetGraphMode(SCREEN_WIDTH, SCREEN_HEIGHT, 32);
    if (DxLib_Init() == -1) return -1;
    SetDrawScreen(DX_SCREEN_BACK);

    // プレイヤー初期化
    Player player;
    player.pos = Vector2D(300.0f, GROUND_HEIGHT - 50.0f); // 初期位置
    player.vel = Vector2D(0.0f, 0.0f);
    player.isJumping = false;
    player.isCharging = false;
    player.chargePower = 0.0f;

    // ゲームループ
    while (ProcessMessage() == 0 && ClearDrawScreen() == 0)
    {
        InputControl::GetInstance()->Update(); // 入力更新

        Update(player);
        Draw(player);

        ScreenFlip();
    }

    DxLib_End();
    return 0;
}

//=== 更新処理 ===//
void Update(Player& player)
{
    InputControl* input = InputControl::GetInstance();

    // --- 地上にいる時の処理 --- //
    if (!player.isJumping)
    {
        // 左右移動
        if (input->GetKey(KEY_INPUT_A))  player.pos.x -= MOVE_SPEED;
        if (input->GetKey(KEY_INPUT_D)) player.pos.x += MOVE_SPEED;

        // ジャンプ溜め開始
        if (input->GetKeyDown(KEY_INPUT_SPACE))
        {
            player.isCharging = true;
            player.chargePower = JUMP_POWER_MIN;
        }

        // 溜め中はジャンプ力を増加
        if (player.isCharging && input->GetKey(KEY_INPUT_SPACE))
        {
            player.chargePower += CHARGE_SPEED;
            if (player.chargePower > JUMP_POWER_MAX) player.chargePower = JUMP_POWER_MAX;
        }

        // スペースキーを離したらジャンプ
        if (player.isCharging && input->GetKeyUp(KEY_INPUT_SPACE))
        {
            player.isJumping = true;
            player.isCharging = false;
            player.vel.y = -player.chargePower;
        }
    }
    else
    {
        // --- 空中の物理処理 --- //
        player.vel.y += GRAVITY;
        player.pos.y += player.vel.y;

        // 地面に着地
        if (player.pos.y >= GROUND_HEIGHT - 50.0f)
        {
            player.pos.y = GROUND_HEIGHT - 50.0f;
            player.isJumping = false;
            player.vel.y = 0.0f;
        }
    }
}

//=== 描画処理 ===//
void Draw(const Player& player)
{
    // 地面
    DrawBox(0, GROUND_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT, GetColor(100, 255, 100), TRUE);

    // プレイヤー
    DrawBox(
        static_cast<int>(player.pos.x),
        static_cast<int>(player.pos.y),
        static_cast<int>(player.pos.x + 50),
        static_cast<int>(player.pos.y + 50),
        GetColor(255, 0, 0), TRUE
    );

    // 溜めゲージ表示
    if (player.isCharging)
    {
        int gaugeWidth = static_cast<int>((player.chargePower / JUMP_POWER_MAX) * 100);
        DrawBox(10, 10, 10 + gaugeWidth, 30, GetColor(255, 255, 0), TRUE);
    }
}
