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

	//SerialWrapper�N���X
	SerialWrapper serialwrapper(3);
	
	//string �ϐ�
	std::string str;
	
	//����s������ϐ�
	int avoid = 0;
	
	//�V���A����M�{��Q�����
	serialwrapper.begin(9600);
	
	//�Z���T�l���i�[
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

		//Esc�L�[�����ƃv���O�����I��
        if (key == 0x1b) break;

		//�X�y�[�X�L�[������ Take off / Landing 
		if (key == ' ') {
			if (ardrone.onGround()) ardrone.takeoff();
			else                    ardrone.landing();
		}

        // Get an image
        cv::Mat image = ardrone.getImage();

		//�h���[���̃o�b�e���[��10%�ȉ��ɂȂ�ƒ������v���O�����I��
		if (ardrone.getBatteryPercentage() <= 10) {
			if (ardrone.onGround()) {
				printf("�[�d���ĂˁI");
				break;
			}
			else {
				ardrone.landing();
			}
		}

		//��M�����Z���T�l�ǂݍ���
		read = serialwrapper.read();

		//��M�����Z���T�l�� ���� or = �Ȃ� str �ɒǉ�
		if (((read >= '0') && (read <= '9')) || (read == '=')) {
			str += read;
		}

		//��M�����Z���T�l�� \n �Ȃ�h���[���̉���s��������
		if (read == '\n') {
			avoid = ardrone.TakeDistance(str);
			str.erase(str.begin(), str.end());
			printf( "�������@avoid = %d�@",avoid);
		}

		//�ړ��p�ϐ�������
		double vx = 0.0, vy = 0.0, vz = 0.0, vr = 0.0;

		if (avoid == 1) {			//����(�O�̃Z���T������)
			vx = -0.2;
			printf( "����\n" );
		}
		else if (avoid == 2) {		//���� �i�E�̃Z���T�������j
			vy = 0.2;
			printf( "����\n" );
		}
		else if (avoid == 3) {		//�O�� �i���̃Z���T�������j
			vx = 0.2;
			printf( "�O��\n" );
		}
		else if (avoid == 4) {		//�E��  (���̃Z���T������)
			vy = -0.2;
			printf( "�E��\n" );
		}
		else if (avoid == 5) {		//�����ցi�O�ƉE�̃Z���T�������j 
			vx = -0.2;
			vy = 0.2;
			printf( "������\n" );
		}
		else if (avoid == 6) {		//���O�ց@�i�E�ƌ��̃Z���T�������j
			vx = 0.2;
			vy = 0.2;
			printf( "���O��\n" );
		}
		else if (avoid == 7) {		//�E�O�ց@�i���ƌ��̃Z���T�������j
			vx = 0.2;
			vy = -0.2;
			printf( "�E�O��\n" );
		}
		else if (avoid == 8) {		//�E���ց@�i�O�ƍ��̃Z���T�������j 
			vx = -0.2;
			vy = -0.2;
			printf( "�E����\n" );
		}
		else if (avoid == 9) {		//���(�O�ƌ��Z���T������) 
			vz = 0.5;
			printf( "���\n" );
		}
		else if (avoid == 10) {		//��� �i�E�ƍ��̃Z���T�������j 
			vz = 0.5;
			printf( "���\n" );
		}
		else if (avoid == 11) {		//�����(�O�ƉE�ƌ��̃Z���T������)
			vy = 0.2;
			vz = 0.5;
			printf( "�����\n" );
		}
		else if (avoid == 12) {		//�O��� �i�E�ƌ��ƍ��̃Z���T�������j
			vx = 0.2;
			vz = 0.5;
			printf( "�O���\n" );
		}
		else if (avoid == 13) {		//�E��� �i���ƍ��ƑO�̃Z���T�������j
			vy = -0.2;
			vz = 0.5;
			printf( "�E���\n" );
		}
		else if (avoid == 14) {		//�����  (���ƑO�ƉE�̃Z���T������)
			vx = -0.2;
			vz = 0.5;
			printf( "�����\n" );
		}
		else if (avoid == 15) {		//��ցi�O�ƉE�ƌ��ƍ��̃Z���T�������j
			vz = 0.5;
			printf( "���\n" );
		}
		else if (avoid == 16) {		//���ցi��̃Z���T�������j
			vz = -0.5;
			printf("����\n");
		}
		else {
			printf( "���c�p�L�[\n" );
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

		/*if (avoid == 1) {				//������(�E�O�̃Z���T������)
			vx = -0.5; 
			vy = 0.5; 
			std::cout << "������" << std::endl;
		} else if (avoid == 2) {		//���O�� �i�E���̃Z���T�������j
			vx = 0.5;
			vy = 0.5; 
			std::cout << "���O��" << std::endl;
		} else if (avoid == 3) {		//�E�O�� �i�����̃Z���T�������j
			vx = 0.5; 
			vy = -0.5; 
			std::cout << "�E�O��" << std::endl;
		} else if (avoid == 4) {		//�E����  (���O�̃Z���T������)
			vx = -0.5; 
			vy = -0.5; 
			std::cout << "�E����" << std::endl;
		} else if (avoid == 5) {		//���ցi�E�O�ƉE���̃Z���T�������j 
			vy = 0.5; 
			std::cout << "����" << std::endl;
		} else if (avoid == 6) {		//�O�ց@�i�E���ƍ����̃Z���T�������j
			vx = 0.5; 
			std::cout << "�O��" << std::endl;
		} else if (avoid == 7) {		//�E�ց@�i���O�ƍ����̃Z���T�������j
			vy = -0.5; 
			std::cout << "�E��" << std::endl; 
		} else if (avoid == 8) {		//���ց@�i�E�O�ƍ��O�̃Z���T�������j 
			vx = -0.5; 
			std::cout << "����" << std::endl; 
		} else if (avoid == 9) {		//���(�E�O�ƍ����Z���T������) 
			vz = 0.5; 
			std::cout << "���" << std::endl; 
		} else if (avoid == 10) {		//��� �i�E���ƍ��O�̃Z���T�������j 
			vz = 0.5;
			std::cout << "���" << std::endl;
		} else if (avoid == 11) {		//���O���(�E�O�ƉE���ƍ����̃Z���T������)
			vx = 0.5; 
			vy = 0.5; 
			vz = 0.5; 
			std::cout << "���O���" << std::endl;
		} else if (avoid == 12) {		//�E�O��� �i�E���ƍ����ƍ��O�̃Z���T�������j
			vx = 0.5;
			vy = -0.5;
			vz = 0.5; 
			std::cout << "�E�O���" << std::endl;
		} else if (avoid == 13) {		//�E����� �i�����ƍ��O�ƉE�O�̃Z���T�������j
			vx = -0.5;
			vy = -0.5; 
			vz = 0.5; 
			std::cout << "�E�����" << std::endl;
		} else if (avoid == 14) {		//�������  (���O�ƉE�O�ƉE���̃Z���T������)
			vx = -0.5;
			vy = 0.5;
			vz = 0.5; 
			std::cout << "�������" << std::endl;
		} else if (avoid == 15) {		//��ցi�E�O�ƉE���ƍ����ƍ��O�̃Z���T�������j
			vz = 0.5;
			std::cout << "���" << std::endl;
		} else {
			std::cout << "" << std::endl;
		}*/

		/*if (avoid == 1)  vx = -0.7;            //����(�O�̃Z���T������)
		else if (avoid == 2) { vy = -0.7; }          //���� �i�E�̃Z���T�������j
		else if (avoid == 3)  vx = 0.7;            //�O�� �i���̃Z���T�������j
		else if (avoid == 4) { vy = 0.7; }          //�E��  (���̃Z���T������)

		else if (avoid == 5) { vx = -0.5; vy = -0.5; }  //������ցi�O�ƉE�̃Z���T�������j
		else if (avoid == 6) { vx = 0.5; vy = -0.5; }  //���O�ց@�i�E�ƌ��̃Z���T�������j
		else if (avoid == 7) { vx = 0.5; vy = 0.5; }  //�E�O�ց@�i���ƌ��̃Z���T�������j
		else if (avoid == 8) { vx = -0.5; vy = 0.5; }  //�E����ց@�i�O�ƍ��̃Z���T�������j

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
		
		//�h���[���ړ�
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