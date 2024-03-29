
#ifndef APPLICATION_H
#define APPLICATION_H

#include "timer.h"
class Application
{
public:
	static Application& GetInstance()
	{
		static Application app;
		return app;
	}
	void Init();
	void Run();
	void Exit();

	static bool GetKeyDown(unsigned short key);
	static bool GetMouseUpdate();
	static float GetMousePosX();
	static float GetMousePosY();
	static bool GetMouseDown(unsigned short key);
	//Declare variables to store the last and current mouse position
	static double mouse_last_x, mouse_last_y, mouse_current_x, mouse_current_y, mouse_diff_x, mouse_diff_y;
	static double camera_yaw, camera_pitch;

	static int GetWindowHeight();
	static int GetWindowWidth();

	static bool GetShouldUpdate();
	static void SetShouldUpdate(bool update);

private:
	Application();
	~Application();

	// Declare the window width and height as constant integer
	const static int m_window_deadzone = 100;
	const static int m_window_width = 1920;
	const static int m_window_height = 1080;

	static bool m_bShouldUpdate;
	//Declare a window object
	StopWatch m_timer;
};

#endif