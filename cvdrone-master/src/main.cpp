#define GLOBAL_DEFINE_EXIT_VALUE
#include "ardrone/ardrone.h"
#include "ardrone/SerialWrapper.h"
#include <iostream>
#include <cstring>
using namespace std;

// --------------------------------------------------------------------------
// main(Number of arguments, Argument values)
// Description  : This is the entry point of the program.
// Return value : SUCCESS:0  ERROR:-1
// --------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    // AR.Drone class
    ARDrone ardrone;

	//SerialWrapperクラス
	SerialWrapper serialwrapper(3);
	
	//string 変数
	std::string str;
	
	//回避行動決定変数
	int avoid = 0;
	
	//シリアル受信＋障害物回避
	serialwrapper.begin(9600);
	
	//センサ値を格納
	char read;

    // Initialize
    if (!ardrone.open()) {
        std::cout << "Failed to initialize." << std::endl;
        return -1;
    }

    // Battery
    std::cout << "Battery = " << ardrone.getBatteryPercentage() << "[%]" << std::endl;

    // Instructions
    std::cout << "********************************************" << std::endl;
    std::cout << "*         CV Drone sample program          *" << std::endl;
    std::cout << "*             - How to play -              *" << std::endl;
    std::cout << "********************************************" << std::endl;
    std::cout << "*                                          *" << std::endl;
    std::cout << "* - Controls -                             *" << std::endl;
    std::cout << "*    'Space'        -- Takeoff/Landing     *" << std::endl;
    std::cout << "*    'Up' or 'I'    -- Move forward        *" << std::endl;
    std::cout << "*    'Down'  or 'K' -- Move backward       *" << std::endl;
    std::cout << "*    'Left'  or 'U' -- Move left           *" << std::endl;
    std::cout << "*    'Right' or 'O' -- Move right          *" << std::endl;
	std::cout << "*    'J'            -- Turn left           *" << std::endl;
	std::cout << "*    'L'            -- Turn right          *" << std::endl;
    std::cout << "*    'Q'            -- Move upward         *" << std::endl;
    std::cout << "*    'A'            -- Move downward       *" << std::endl;
    std::cout << "*                                          *" << std::endl;
    std::cout << "* - Others -                               *" << std::endl;
    std::cout << "*    'C'     -- Change camera              *" << std::endl;
    std::cout << "*    'Esc'   -- Exit                       *" << std::endl;
    std::cout << "*                                          *" << std::endl;
    std::cout << "********************************************" << std::endl;

    while (1) {
        // Key input
        int key = cv::waitKey(33);

		//Escキー押すとプログラム終了
        if (key == 0x1b) break;

		//スペースキー押すと Take off / Landing 
		if (key == ' ') {
			if (ardrone.onGround()) ardrone.takeoff();
			else                    ardrone.landing();
		}

        // Get an image
        cv::Mat image = ardrone.getImage();

		//ドローンのバッテリーが10%以下になると着陸＆プログラム終了
		if (ardrone.getBatteryPercentage() <= 10) {
			if (ardrone.onGround()) {
				printf("充電してね！");
				break;
			}
			else {
				ardrone.landing();
			}
		}

		//受信したセンサ値読み込む
		read = serialwrapper.read();

		//受信したセンサ値が 数字 or = なら str に追加
		if (((read >= '0') && (read <= '9')) || (read == '=')) {
			str += read;
		}

		//受信したセンサ値が \n ならドローンの回避行動を決定
		if (read == '\n') {
			avoid = ardrone.TakeDistance(str);
			str.erase(str.begin(), str.end());
			printf( "回避方向　avoid = %d　",avoid);
		}

		//移動用変数初期化
		double vx = 0.0, vy = 0.0, vz = 0.0, vr = 0.0;

		if (avoid == 1) {			//後ろへ(前のセンサが反応)
			vx = -0.2;
			printf( "後ろへ\n" );
		}
		else if (avoid == 2) {		//左へ （右のセンサが反応）
			vy = 0.2;
			printf( "左へ\n" );
		}
		else if (avoid == 3) {		//前へ （後ろのセンサが反応）
			vx = 0.2;
			printf( "前へ\n" );
		}
		else if (avoid == 4) {		//右へ  (左のセンサが反応)
			vy = -0.2;
			printf( "右へ\n" );
		}
		else if (avoid == 5) {		//左後ろへ（前と右のセンサが反応） 
			vx = -0.2;
			vy = 0.2;
			printf( "左後ろへ\n" );
		}
		else if (avoid == 6) {		//左前へ　（右と後ろのセンサが反応）
			vx = 0.2;
			vy = 0.2;
			printf( "左前へ\n" );
		}
		else if (avoid == 7) {		//右前へ　（左と後ろのセンサが反応）
			vx = 0.2;
			vy = -0.2;
			printf( "右前へ\n" );
		}
		else if (avoid == 8) {		//右後ろへ　（前と左のセンサが反応） 
			vx = -0.2;
			vy = -0.2;
			printf( "右後ろへ\n" );
		}
		else if (avoid == 9) {		//上へ(前と後ろセンサが反応) 
			vz = 0.5;
			printf( "上へ\n" );
		}
		else if (avoid == 10) {		//上へ （右と左のセンサが反応） 
			vz = 0.5;
			printf( "上へ\n" );
		}
		else if (avoid == 11) {		//左上へ(前と右と後ろのセンサが反応)
			vy = 0.2;
			vz = 0.5;
			printf( "左上へ\n" );
		}
		else if (avoid == 12) {		//前上へ （右と後ろと左のセンサが反応）
			vx = 0.2;
			vz = 0.5;
			printf( "前上へ\n" );
		}
		else if (avoid == 13) {		//右上へ （後ろと左と前のセンサが反応）
			vy = -0.2;
			vz = 0.5;
			printf( "右上へ\n" );
		}
		else if (avoid == 14) {		//後ろ上へ  (左と前と右のセンサが反応)
			vx = -0.2;
			vz = 0.5;
			printf( "後ろ上へ\n" );
		}
		else if (avoid == 15) {		//上へ（前と右と後ろと左のセンサが反応）
			vz = 0.5;
			printf( "上へ\n" );
		}
		else if (avoid == 16) {		//下へ（上のセンサが反応）
			vz = -0.5;
			printf("下へ\n");
		}
		else {
			printf( "操縦用キー\n" );
			if (key == 'i' || key == CV_VK_UP)    vx = 0.2;
			if (key == 'k' || key == CV_VK_DOWN)  vx = -0.2;
			if (key == 'u' || key == CV_VK_LEFT)  vy = 0.2;
			if (key == 'o' || key == CV_VK_RIGHT) vy = -0.2;
			if (key == 'j') vr = 1.0;
			if (key == 'l') vr = -1.0;
			if (key == 'q') vz = 0.5;
			if (key == 'a') vz = -0.5;
		}

		/*if (flag == 1) {
			if (key == 'i' || key == CV_VK_UP)    vx = 0.1;
			if (key == 'k' || key == CV_VK_DOWN)  vx = -0.1;
			if (key == 'u' || key == CV_VK_LEFT)  vy = 0.1;
			if (key == 'o' || key == CV_VK_RIGHT) vy = -0.1;
			if (key == 'j') vr = 1.0;
			if (key == 'l') vr = -1.0;
			if (key == 'q') vz = 0.1;
			if (key == 'a') vz = -0.1;
		}*/

		/*if (avoid == 1) {				//左後ろへ(右前のセンサが反応)
			vx = -0.5; 
			vy = 0.5; 
			std::cout << "左後ろへ" << std::endl;
		} else if (avoid == 2) {		//左前へ （右後ろのセンサが反応）
			vx = 0.5;
			vy = 0.5; 
			std::cout << "左前へ" << std::endl;
		} else if (avoid == 3) {		//右前へ （左後ろのセンサが反応）
			vx = 0.5; 
			vy = -0.5; 
			std::cout << "右前へ" << std::endl;
		} else if (avoid == 4) {		//右後ろへ  (左前のセンサが反応)
			vx = -0.5; 
			vy = -0.5; 
			std::cout << "右後ろへ" << std::endl;
		} else if (avoid == 5) {		//左へ（右前と右後ろのセンサが反応） 
			vy = 0.5; 
			std::cout << "左へ" << std::endl;
		} else if (avoid == 6) {		//前へ　（右後ろと左後ろのセンサが反応）
			vx = 0.5; 
			std::cout << "前へ" << std::endl;
		} else if (avoid == 7) {		//右へ　（左前と左後ろのセンサが反応）
			vy = -0.5; 
			std::cout << "右へ" << std::endl; 
		} else if (avoid == 8) {		//後ろへ　（右前と左前のセンサが反応） 
			vx = -0.5; 
			std::cout << "後ろへ" << std::endl; 
		} else if (avoid == 9) {		//上へ(右前と左後ろセンサが反応) 
			vz = 0.5; 
			std::cout << "上へ" << std::endl; 
		} else if (avoid == 10) {		//上へ （右後ろと左前のセンサが反応） 
			vz = 0.5;
			std::cout << "上へ" << std::endl;
		} else if (avoid == 11) {		//左前上へ(右前と右後ろと左後ろのセンサが反応)
			vx = 0.5; 
			vy = 0.5; 
			vz = 0.5; 
			std::cout << "左前上へ" << std::endl;
		} else if (avoid == 12) {		//右前上へ （右後ろと左後ろと左前のセンサが反応）
			vx = 0.5;
			vy = -0.5;
			vz = 0.5; 
			std::cout << "右前上へ" << std::endl;
		} else if (avoid == 13) {		//右後ろ上へ （左後ろと左前と右前のセンサが反応）
			vx = -0.5;
			vy = -0.5; 
			vz = 0.5; 
			std::cout << "右後ろ上へ" << std::endl;
		} else if (avoid == 14) {		//左後ろ上へ  (左前と右前と右後ろのセンサが反応)
			vx = -0.5;
			vy = 0.5;
			vz = 0.5; 
			std::cout << "左後ろ上へ" << std::endl;
		} else if (avoid == 15) {		//上へ（右前と右後ろと左後ろと左前のセンサが反応）
			vz = 0.5;
			std::cout << "上へ" << std::endl;
		} else {
			std::cout << "" << std::endl;
		}*/

		/*if (avoid == 1)  vx = -0.7;            //後ろへ(前のセンサが反応)
		else if (avoid == 2) { vy = -0.7; }          //左へ （右のセンサが反応）
		else if (avoid == 3)  vx = 0.7;            //前へ （後ろのセンサが反応）
		else if (avoid == 4) { vy = 0.7; }          //右へ  (左のセンサが反応)

		else if (avoid == 5) { vx = -0.5; vy = -0.5; }  //左後方へ（前と右のセンサが反応）
		else if (avoid == 6) { vx = 0.5; vy = -0.5; }  //左前へ　（右と後ろのセンサが反応）
		else if (avoid == 7) { vx = 0.5; vy = 0.5; }  //右前へ　（左と後ろのセンサが反応）
		else if (avoid == 8) { vx = -0.5; vy = 0.5; }  //右後方へ　（前と左のセンサが反応）

		else {
			if (key == 'i' || key == CV_VK_UP)    vx = 1.0;
			if (key == 'k' || key == CV_VK_DOWN)  vx = -1.0;
			if (key == 'u' || key == CV_VK_LEFT)  vy = 1.0;
			if (key == 'o' || key == CV_VK_RIGHT) vy = -1.0;
			if (key == 'j') vr = 1.0;
			if (key == 'l') vr = -1.0;
			if (key == 'q') vz = 1.0;
			if (key == 'a') vz = -1.0;
		}*/
		
		//ドローン移動
		ardrone.move3D(vx, vy, vz, vr);
		
        // Change camera
        static int mode = 0;
        if (key == 'c') ardrone.setCamera(++mode % 4);

        // Display the image
        cv::imshow("camera", image);
    }

    // See you
    ardrone.close();

    return 0;
}