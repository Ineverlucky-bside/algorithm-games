#include <graphics.h>
#include <conio.h>
#include <stdio.h>

class Bird
{
private:
	//基础属性：位置坐标、y轴速度、碰撞半径
	double x;
	double y;
	double vy = 0;
	int radius = 15;

	double gravity = 0.6; //重力加速度
	double jump_velocity = -8.5; //起跳速度

public:
	Bird(double start_x, double start_y) : x(start_x), y(start_y) {}

	//每一帧物理更新（小鸟会随着时间下落）
	void update()
	{
		vy += gravity;
		y += vy;
	}
	//玩家按键起跳
	void jump()
	{
		vy = jump_velocity;
	}

	//绘制画面
	void draw() const
	{
		setfillcolor(GREEN);
		solidcircle(static_cast<int>(x), static_cast<int>(y), radius);
	}

	//设置只读接口（外部用来检测碰撞）
	double getX() const { return x; }
	double getY() const { return y; }
	int getRadius() const { return radius; }
};

int main()
{
	const int WINDOW_WIDTH = 800;
	const int WINDOW_HEIGHT = 600; //设置宽高

	initgraph(WINDOW_WIDTH, WINDOW_HEIGHT); //初始化画面
	BeginBatchDraw(); //双缓冲

	Bird bird(150, 200);
	
	while (true)
	{
		if (GetAsyncKeyState(VK_SPACE) & 0x8000)
		{
			bird.jump();
		}

		bird.update();
		
		if (bird.getY() + bird.getRadius() >= WINDOW_HEIGHT || bird.getY() - bird.getRadius() <= 0)
		{
			break;
		}

		cleardevice();
		bird.draw();
		FlushBatchDraw(); 
		
		Sleep(20);//设置帧率
	}
	EndBatchDraw();
	closegraph();


	return 0;
}