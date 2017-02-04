#include <WPILib.h>
#include "CANTalon.h"
#include <cmath>
#include <thread>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <CameraServer.h>

//ARCADE DRIVE
/**
 * This is a demo program showing the use of the RobotDrive class.
 * The SampleRobot class is the base of a robot application that will automatically call your
 * Autonomous and OperatorControl methods at the right time as controlled by the switches on
 * the driver station or the field controls.
 *
 * WARNING: While it may look like a good choice to use for your code if you're inexperienced,
 * don't. Unless you know what you are doing, complex code will be much more difficult under
 * this system. Use IterativeRobot or Command-Based instead if you're new.
 */
class Robot: public frc::SampleRobot {

	Joystick *leftJoyStick;
	Joystick *rightJoyStick;

	CANTalon *MotorR1;
	CANTalon *MotorR2;
	CANTalon *MotorL1;
	CANTalon *MotorL2;
	double thresh;
	double throttlePre;
	double steerPre;

	double throttle;
	double steer;
	int invert;
	bool invertTest;

public:
	static void VisionThread()
	{
		        cs::UsbCamera camera = CameraServer::GetInstance()->StartAutomaticCapture();
		        camera.SetResolution(320, 240);
		        cs::CvSink cvSink = CameraServer::GetInstance()->GetVideo();
		        cs::CvSource outputStreamStd = CameraServer::GetInstance()->PutVideo("Gray", 640, 480);
		        cv::Mat source;
		        cv::Mat output;
		        while(true) {
		            cvSink.GrabFrame(source);
		            cvtColor(source, output, cv::COLOR_BGR2GRAY);
		            outputStreamStd.PutFrame(output);
		        }
		    }
		   void RobotInit(){
		        std::thread visionThread(VisionThread);
		        visionThread.detach();
		    }

	Robot() {

		this -> thresh = .05;
		this -> throttle = 0;
		this -> steer = 0;
		this -> throttlePre = 0;
		this -> steerPre = 0;
		this -> invert = 1;
		this -> invertTest = true;

		leftJoyStick = new Joystick(0);
		rightJoyStick = new Joystick(1);

		MotorR1 = new CANTalon(1);
		MotorR2 = new CANTalon(2);
		MotorL1 = new CANTalon(3);
		MotorL2 = new CANTalon(4);

		MotorR1->Set(0);
		MotorR2->Set(0);
		MotorL1->Set(0);
		MotorL2->Set(0);

	}
	void OperatorControl() {
		while (true) {

			throttlePre = leftJoyStick->GetY();
			steerPre = rightJoyStick->GetX();

			throttle = pow(2, throttlePre) - 1;
			steer = pow(2, steerPre) - 1;

			SmartDashboard::PutNumber("DB/Slider 0", leftJoyStick -> GetY());
			SmartDashboard::PutNumber("DB/Slider 1", rightJoyStick -> GetY());

			SmartDashboard::PutNumber("DB/Slider 2", throttlePre);
			SmartDashboard::PutNumber("DB/Slider 3", steerPre);

			if(rightJoyStick -> GetRawButton(2)&invertTest) {
				invert*=-1;
				invertTest=false;
			}
			if(rightJoyStick -> GetRawButton(2)==false){
				invertTest=true;
			}

			MotorR1->Set(throttlePre*invert + steerPre);
			MotorR2->Set(throttlePre*invert + steerPre);
			MotorL1->Set(-throttlePre*invert + steerPre);
			MotorL2->Set(-throttlePre*invert + steerPre);

			frc::Wait(0.005);
		}
	}
};

START_ROBOT_CLASS(Robot)
