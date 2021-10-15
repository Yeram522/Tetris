#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "GameManager.h"
#include <string>

using namespace std;

class UI;//속도 및 박스 렌더 및 계산
class Tetris;//규칙

class UI : public GameObject {
//그릴 위치, 박스크기를 넣으면 스크린에 박스를 그린다.
public:
	UI(const Position& pos, const Dimension& sz)
		:GameObject(pos,sz)
	{ 
		//┌─────────────────┐
		string line("+");
		line.append((size_t)(getDim().x - 2), '-').append("+"); 

		//│                 │
		string column("|");
		column.append((size_t)(getDim().x - 2), ' ').append("|"); 

		//└─────────────────┘
		string bottom("+");
		bottom.append((size_t)(getDim().x - 2), '-').append("+");


		for (int i = 0; i < sz.y - 2; i++) line.append(column);
		line.append(bottom);

		const char* testline = line.c_str();
		setFace(line.c_str());//부모클래스 face에 복사.
	}

	void drawSquare()
	{
		this->draw();
	}
};

int main()
{
	Screen* screen = Screen::GetInstance();
	Input* input = Input::GetInstance();
	Position pos{ 0, 0 };
	//std::string test("┌────────┐");
	//const char* shape = test.c_str();
	//Dimension sz{ (int)strlen(shape), 1};
	//GameObject one{ test.c_str(), pos,  sz };
	//GameObject two{ "│────────│",{ 1, 3 },  sz };
	UI tetrisScreen{ pos , {10,10} };

	// Get the standard inp

	bool isLooping = true;
	while (isLooping) {

		screen->clear();
		//one.draw();
		//two.draw();
		tetrisScreen.draw();

		input->readInputs();

		//one.update();
		//two.update();
		tetrisScreen.update();

		screen->render();
		Sleep(100);

	}
	printf("\nGame Over\n");

	return 0;
}

