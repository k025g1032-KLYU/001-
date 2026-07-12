#include <Novice.h>
#include <Math.h>
#include<time.h> 

const char kWindowTitle[] = "GC1A＿11＿ヨ＿カンリン＿タイトル";

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = {0};
	char preKeys[256] = {0};
	 
	//start 
	int startGame = false;
	int startBar = true;
	int startBarTimer = 90;
	//背景
	int sBG = Novice::LoadTexture("./images/BG3v2.png");

	//BIG TIMER
	clock_t start, end;
	double bigtimer = 0;
	double BT = 0;

	// player座標 移動速度 變數宣言
	int grHandle = Novice::LoadTexture("./images/player.png");
	int playerX = 400;
	int playerY = 400;

	// bullet座標 移動速度 變數宣言
	int bulletPic = Novice::LoadTexture("./images/bullet.png");

	int bulletX = 0;
	int bulletY = 1000;
	int bulletR = 5;

	//enemy
	int enemy = Novice::LoadTexture("./images/enemy2.png");
	int enemyX = 600;
	int enemyY = 100;
	int enemyR = 25;
	int enemyXSpd = 10;
	int enemyYSpd = 10;
	int enemyHp = 3;

	int R = enemyR + bulletR;
	int ifHit = false;

	int explodenemyX = 0;
	int explodenemyY = 0;

	//distance
	int distanceX = bulletX - enemyX;
	int distanceY = bulletY - enemyY;
	float distanceZ = 0;

	// bullet狀態保存旗變數宣言
	int explode = Novice::LoadTexture("./images/explode.png");
	int isBulletShot = false;

	//respone timer
	int respawnTimer = 120;

	//生存狀態管理flag變數
	int alive = true;

	//爆炸中狀態管理flag變數
	int boom = false;

	//背景
	int BG = Novice::LoadTexture("./images/BG2.png");

	//Sound Effect
	int audioBoom = Novice::LoadAudio("./sounds/boom2.wav");
	int audioShoot = Novice::LoadAudio("./sounds/sound-5.mp3");
	int audioBG = Novice::LoadAudio("./sounds/BGmusic.mp3");
	int playAudioBG = -1;
	start = clock();
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
		
		if (!startGame)
		{
			startBarTimer -= 1;
			if(startBar)
			{
				Novice::ScreenPrintf(500, 350, "press Space to start the game");
			}
			if(startBarTimer==45)
			{
				startBar = false;
			}
			if (startBarTimer == 0)
			{
				startBar = true;
				startBarTimer = 90;
			}
			Novice::DrawSprite(
				0, 0,	// 描画する画像の左上座標
				sBG,	// テクスチャハンドル
				1.0f, 1.0f,	// 倍率
				0.0f,		// 回転
				0xFFFFFFFF	// 色

			);

			if (keys[DIK_SPACE])
			{
				startGame = true;
				start = clock();
			}
		}
		
		if (startGame)
		{
			
			//audioBG
			if (Novice::IsPlayingAudio(playAudioBG) == 0 || playAudioBG == -1) {

				playAudioBG = Novice::PlayAudio(audioBG, 0, 1);

			}


			// enemy 移動
			enemyX = enemyX + enemyXSpd;
			enemyY = enemyY + enemyYSpd;
			if (enemyX >= 1200) {
				enemyXSpd = -16;

			}
			if (enemyX <= 100) {
				enemyXSpd = 10;
			}
			if (enemyY >= 300) {
				enemyYSpd = -14;
			}
			if (enemyY <= 30) {
				enemyYSpd = 8;
			}

			// player 移動處裡
			if (playerX + 32 >= 1280)
			{
				playerX = 1280 - 32;
			}
			if (playerX - 32 <= 0)
			{
				playerX = 30;
			}
			if (playerY + 64 >= 720)
			{
				playerY = 720 - 64;
			}
			if (playerY <= 0)
			{
				playerY = 0;
			}

			// 1.輸入移動座標
			//例)WASD
			if (keys[DIK_W] || keys[DIK_UP])
			{
				playerY -= 3;
			}
			if (keys[DIK_S] || keys[DIK_DOWN])
			{
				playerY += 10;
			}
			if (keys[DIK_D] || keys[DIK_RIGHT])
			{
				playerX += 10;
			}
			if (keys[DIK_A] || keys[DIK_LEFT])
			{
				playerX -= 10;
			}


			// 2. 按鍵子彈發射處裡
			// 用keys現在的輸入情報 用preKeys取得一偵前的輸入情報
			if (keys[DIK_SPACE] && !preKeys[DIK_SPACE]/*keys為ture同時prekeys為false的時候*/)
			{
				//如果子彈沒射出
				if (!isBulletShot && enemyHp > 0)
				{
					//狀態變為子彈發射中
					isBulletShot = true;

					//生成於player位置
					bulletX = playerX;
					bulletY = playerY;
					Novice::PlayAudio(audioShoot, 0, 2);
				}
			}
			// 3.不出畫面外處裡
			//例)posX>=1280 posY<=0

			/*子彈座標移動處裡 狀態管理*/
			// 1.子彈發射狀態的場合 座標前進
			if (isBulletShot)
			{
				//bullet的座標以speed前進
				bulletY -= 7;
			}

			/* 2.子彈完全離開畫面*/
			if (bulletY <= -10/*子彈在畫面外*/ || ifHit/*命中*/)
			{
				//變成未發射狀態
				isBulletShot = false;
			}



			distanceX = abs(bulletX - enemyX);
			distanceY = abs(bulletY - enemyY);
			float fdistanceX = static_cast<float>(distanceX);
			float fdistanceY = static_cast<float>(distanceY);
			distanceZ = sqrtf(static_cast<float>(fdistanceX * fdistanceX + fdistanceY * fdistanceY));

			if (distanceZ <= R)
			{
				ifHit = true;
			}
			if (distanceZ > R)
			{
				ifHit = false;
			}

			//子彈命中enemy死掉(爆炸)
			if (ifHit && respawnTimer == 120)
			{
				alive = false;
				boom = true;  //生存flag變false 爆炸flag便true
				enemyHp -= 1;
				explodenemyX = enemyX - 32;
				explodenemyY = enemyY - 24;
				Novice::PlayAudio(audioBoom, 0, 1);
				bulletX = 0;
				bulletY = 1000;
			}

			//timer計算(只算enemy死掉的時候)

			if (alive == false  /*enemy死掉*/)
			{
				respawnTimer -= 1;

				if (enemyHp > 0)
				{
					Novice::ScreenPrintf(550, 290, "Respawn in: %d", respawnTimer);

				}
				if (respawnTimer <= 90  /*死後過了0.5秒*/)
				{
					boom = false;  //爆炸結束
				}
				if (respawnTimer <= 0 && enemyHp > 0/*死後2秒*/)
				{
					respawnTimer = 120;  //timer reset
					alive = true;  //復活
				}
			}

			if (enemyHp == 0)
			{
				BT = bigtimer;
				enemyHp = -1;
			}
			end = clock();
			bigtimer = end - start;
			
		}
		
		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///
		/// 
		if (keys[DIK_Z]) 
		{
			Novice::ScreenPrintf(10, 530, "BigTimer%lf", bigtimer);
		}
		
		if (startGame)
		{
			
			Novice::DrawSprite(
				0, 0,	// 描画する画像の左上座標
				BG,	// テクスチャハンドル
				1.0f, 1.0f,	// 倍率
				0.0f,		// 回転
				0xFFFFFFFF	// 色

			);



			if (keys[DIK_Z])
			{
				Novice::ScreenPrintf(10, 10, "Up:W  Down:S  Right:D  Left:A");
				Novice::ScreenPrintf(10, 30, "Shoot:Space ");
				Novice::ScreenPrintf(10, 60, "PlayerPosX: %d   PlayerPosY: %d", playerX, playerY);
				Novice::ScreenPrintf(10, 80, "bulletPosX: %d   BulletPosY: %d  IsBulletShot: %d", bulletX, bulletY, isBulletShot);
				Novice::ScreenPrintf(10, 100, "bulletd to targrt(x): %d", distanceX);
				Novice::ScreenPrintf(10, 120, "bullet to targrt(y): %d", distanceY);
				Novice::ScreenPrintf(10, 150, "bullet to targrt(distance): %f", distanceZ);
				Novice::ScreenPrintf(10, 170, "bullet radius + targrt radius: %d", enemyR + bulletR);
				if (ifHit == 0)
				{
					Novice::ScreenPrintf(10, 190, "IfHit:False");
				}
				if (ifHit == 1)
				{
					Novice::ScreenPrintf(10, 190, "IfHit:True");
				}
				Novice::ScreenPrintf(10, 530, "BigTimer%lf", bigtimer);
				Novice::ScreenPrintf(200, 650, "Timer:%d", respawnTimer);
				if (alive)
				{
					Novice::ScreenPrintf(50, 650, "eAlive:True");
				}
				if (!alive)
				{
					Novice::ScreenPrintf(50, 650, "eAlive:False");
				}
				Novice::ScreenPrintf(10, 210, "Enemy HP: %d", enemyHp);
				Novice::DrawEllipse(
					enemyX, enemyY,
					enemyR, enemyR,
					0.0f, WHITE,
					kFillModeWireFrame
				);
				Novice::DrawEllipse(
					bulletX, bulletY,
					bulletR, bulletR,
					0.0f, WHITE,
					kFillModeWireFrame
				);
				Novice::DrawTriangle(
					enemyX, enemyY,
					playerX, playerY,
					playerX, enemyY,
					WHITE,
					kFillModeWireFrame
				);
			}

			/*if (!keys[DIK_W] && !keys[DIK_S] && !keys[DIK_D] && !keys[DIK_A] && !keys[DIK_UP] && !keys[DIK_DOWN] && !keys[DIK_LEFT] && !keys[DIK_RIGHT])
			{
				Novice::ScreenPrintf(playerX - 50, playerY -50, "Enemy HP: %d", enemyHp);
			}*/
			if (enemyHp > 0)
			{
				Novice::ScreenPrintf(playerX - 50, playerY + 70, "Enemy HP: %d", enemyHp);
			}
			
			if (enemyHp <= 0)
			{
				Novice::StopAudio(audioBG);
				Novice::ScreenPrintf(550, 300, "YOU WIN");
				Novice::ScreenPrintf(550, 330, "Time:%0.2lfsec", BT / CLOCKS_PER_SEC);
				Novice::ScreenPrintf(520, 500, "press R to Retry");
				if (keys[DIK_R])
				{
					enemyHp = 3;
					respawnTimer = 1;
					start = clock();
				}
				Novice::ScreenPrintf(520, 530, "press B to close the game");
				if (keys[DIK_B])
				{
					break;
				}

			}


			Novice::DrawSprite(
				playerX - 32, playerY,	// 描画する画像の左上座標
				grHandle,	// テクスチャハンドル
				1.0f, 1.0f,	// 倍率
				0.0f,		// 回転
				0xFFFFFFFF	// 色

			);

			if (isBulletShot)
			{

				Novice::DrawSprite(
					bulletX - 16, bulletY - 4,  // 描画する画像の左上座標
					bulletPic,	// テクスチャハンドル
					1.0f, 1.0f,	// 倍率
					0.0f,		// 回転
					0xFFFFFFFF	// 色

				);


			}



			if (alive)
			{
				Novice::DrawSprite(
					enemyX - 32, enemyY - 24,  // 描画する画像の左上座標
					enemy,	// テクスチャハンドル
					1.0f, 1.0f,	// 倍率
					0.0f,		// 回転
					0xFFFFFFFF	// 色
				);
			}


			if (boom)
			{
				Novice::DrawSprite(
					explodenemyX, explodenemyY,	// 描画する画像の左上座標
					explode,	// テクスチャハンドル
					1.0f, 1.0f,	// 倍率
					0.0f,		// 回転
					0xFFFFFFFF	// 色

				);
			}
		}
		
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
