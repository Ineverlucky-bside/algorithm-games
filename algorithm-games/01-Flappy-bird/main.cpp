#include <graphics.h>
#include <conio.h>
#include <stdio.h>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <string>
#include "EasyXPng.h"

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

	IMAGE* pBirdImage = nullptr;//保存小鸟图片的指针

public:
	Bird(double start_x, double start_y, IMAGE* img = nullptr) : x(start_x), y(start_y), pBirdImage(img){}

	//每一帧物理更新（小鸟会随着时间下落）
	void update()
	{
		vy += gravity; //匀加速直线运动的写法
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
		if (pBirdImage != nullptr)
		{
			//图片中心对齐小鸟
			int drawX = static_cast<int>(x - pBirdImage->getwidth() / 2.0);
			int drawY = static_cast<int>(y - pBirdImage->getwidth() / 2.0);

			putimagePng(drawX, drawY, pBirdImage);
		}
		else
		{
			setfillcolor(YELLOW);
			solidcircle(static_cast<int>(x), static_cast<int>(y), radius);
		}
	}

	//设置只读接口（外部用来检测碰撞）
	double getX() const { return x; }
	double getY() const { return y; }
	int getRadius() const { return radius; }
};

class Pipe
{
private:
	double x; //水管横坐标
	int width = 65; //水管宽度
	int gap = 160; //上下水管之间的空隙
	int topHeight; //上水管的高度
	double speed = 3.0; // 向左平移的速度
	int windowHeight; //窗口总高度
	bool passed = false; // 检查这根水管是否参与几分

public:
	Pipe(double start_x, int win_height) : x(start_x), windowHeight(win_height)
	{
		reset(start_x);
	}
	//重置上水管的高度在80到max之间随机
	void reset(double new_x) 
	{
		x = new_x;
		int minHeight = 80;
		int maxHeight = windowHeight - gap - 80;
		topHeight = minHeight + rand() % (maxHeight - minHeight);
		passed = false; 
	}
	//帧更新向左移动
	void update()
	{
		x -= speed; //匀速直线运动
	}
	//绘制上下水管
	void draw() const
	{
		setfillcolor(GREEN);
		solidrectangle(static_cast<int>(x), 0, static_cast<int>(x + width), topHeight);
		solidrectangle(static_cast<int>(x), topHeight + gap, static_cast<int>(x + width), windowHeight);
	}
	//判断是否飞出了屏幕左侧
	bool isOutOfScreen() const
	{
		return (x + width) < 0;
	}

	bool checkPassed()
	{
		if (!passed && isOutOfScreen())
		{
			passed = true;
			return true;
		}
		return false;
	}
	//碰撞检测：判断是否碰到了小鸟 AABB碰撞
	bool checkCollision(const Bird& bird) const
	{
		double bx = bird.getX();
		double by = bird.getY();
		double br = bird.getRadius();

		if (bx + br > x && bx - br < x + width) //在水管的区间
		{
			if (by - br < topHeight || by + br > topHeight + gap)
			{
				return true;
			}
		}
		return false;
	}
	//只读接口
	double getX() const { return x; }
};

//按键辅助函数
inline bool isKeyPressed(int virtualKey)
{
	return (GetAsyncKeyState(virtualKey) & 0x8000) != 0;
}

int main()
{
	bool is_running = true;
	//初始化随机数种子
	srand(static_cast<unsigned int>(time(nullptr)));
	const int WINDOW_WIDTH = 800;
	const int WINDOW_HEIGHT = 600; //设置宽高
	const int PIPE_COUNT = 3; //水管数量
	const int PIPE_SPACE = 300; //水管间距

	initgraph(WINDOW_WIDTH, WINDOW_HEIGHT); //初始化画面
	BeginBatchDraw(); //双缓冲

	IMAGE imBackground; // 定义图像对象
	IMAGE imBird;
	loadimage(&imBackground, _T("背景.png"), WINDOW_WIDTH, WINDOW_HEIGHT);
	loadimage(&imBird, _T("鸟.png"));

	//设置文字样式
	setbkmode(TRANSPARENT);
	settextstyle(32, 0, _T("Consolas"));
	settextcolor(BLUE);

	int score = 0; // 记录分数
	Bird bird(150, 200, &imBird);
	std::vector<Pipe> pipes;
	pipes.reserve(PIPE_COUNT); //预分配内存，避免动态扩容的额外开销

	for (int i = 0; i < PIPE_COUNT; i++)
	{
		pipes.emplace_back(WINDOW_WIDTH + i * PIPE_SPACE, WINDOW_HEIGHT); // 构造对象
	}
	
	while (is_running)
	{
		//处理输入
		if (isKeyPressed(VK_SPACE))
		{
			bird.jump();
		}

		//状态更新
		bird.update(); 
		for (auto& pipe : pipes)
		{
			pipe.update();
			//记录分数
			if (pipe.checkPassed())
			{
				score++;
			}
			//障碍物离开屏幕后从右侧刷新
			if (pipe.isOutOfScreen())
			{
				pipe.reset(pipe.getX() + PIPE_COUNT * PIPE_SPACE);
			}
		}

		//碰撞判定
		if ( bird.getY() + bird.getRadius() >= WINDOW_HEIGHT || bird.getY() - bird.getRadius() <= 0 )
		{
			is_running = false;
			break;
		}

		for (const auto& pipe : pipes)
		{
			if (pipe.checkCollision(bird))
			{
				is_running = false;
				break;
			}
		}
			
		cleardevice();
		putimage(0, 0, &imBackground);
		bird.draw();
		for (const auto& pipe : pipes)
		{
			pipe.draw();
		}

		//绘制分数
		std::wstring scoreStr = L"Score: " + std::to_wstring(score);
		outtextxy(30, 20, scoreStr.c_str());
		
		FlushBatchDraw(); 
		
		Sleep(20);//设置帧率
	}
	EndBatchDraw();
	closegraph();


	return 0;
}