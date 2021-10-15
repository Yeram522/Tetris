#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "GameManager.h"
#include <string>

using namespace std;

class UI;//속도 및 박스 렌더 및 계산
class Block; //테트리스 블록 게임오브젝트
class Tetris;//규칙

class UI : public GameObject {
public: //배치 할 위치, 생성할 박스 크기를 넣으면 스크린에 박스UI를 그린다.
	UI(const Position& pos, const Dimension& sz) :GameObject(pos,sz)
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
		setFace(testline);//부모클래스 face에 복사.
	}
};

class Block : public GameObject
{
private:
	enum BlockShape
	{   
		I = 73,
		L = 108,
		J = 106,
		T = 116,
		O = 111,
		S = 115,
		Z = 122 //문자를 10진수 아스키코드로 표현
	};
	//넣는 값에 따라 다른 종류의 블록이 만들어진다.
	//움직일 수 있는 블록은 단 한개 존재함으로 싱클톤 형식으로 만들어본다.
	//매개변수는 shape/shape은 변경되지 않으므로 const
	Block(const char* blockshape) :GameObject(blockshape)
	{
		setFace(blockshape);
		setPos({ 5,1 });//UI 중간값으로 일단 설정
		setDim({ 4,4 });
	}

	static Block* Instance;//싱글톤 static 변수 생성

public:
	static Block* GetInstance() {
		if (Instance == nullptr) {
			Instance = new Block("");
		}
		return Instance;
	}

	void changeBlockShape()
	{
		setFace("*   *   *   *   ");
	}
};

class Tetris : public GameObject
{
private:
	UI gamescreen;
	UI scoreboard;
	UI nextblockcard;
	Block* block;

public:
	Tetris()
		:GameObject("",{0,0},{30,30}) , block(Block::GetInstance())
		, gamescreen({0,0} ,{10,10}), scoreboard({ 13, 1 }, { 7,3 }), nextblockcard( {13,5} , {6,4} )
	{

	}

	void update() override
	{
	}

};


Block* Block::Instance = nullptr;


int main()
{
	Screen* screen = Screen::GetInstance();
	Input* input = Input::GetInstance();
	Block* block = Block::GetInstance();

	Position pos{ 0, 0 };

	UI tetrisScreen{ { 0, 0 } , {10,20} };
	UI scoreboard{ { 13, 1 } , {7,3} };
	UI nextblock{ { 13, 5 } , {6,4} };
	block->changeBlockShape();

	// Game loop
	bool isLooping = true;
	while (isLooping) {

		screen->clear();

		tetrisScreen.draw();
		//scoreboard.draw();
		//nextblock.draw();

		block->draw();

		input->readInputs();

		screen->render();
		Sleep(100);

	}
	printf("\nGame Over\n");

	return 0;
}

