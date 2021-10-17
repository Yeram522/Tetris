#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "GameManager.h"


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

class Block : public GameObject
{
private:
	//넣는 값에 따라 다른 종류의 블록이 만들어진다.
	//움직일 수 있는 블록은 단 한개 존재함으로 싱클톤 형식으로 만들어본다.
	Block() :GameObject(){ }

	static Block* Instance;//싱글톤 static 변수 생성

	Metrix offset2mt(int offset)
	{
		if (offset < getDim().x) return { 0 , offset };
		else return { offset/ getDim().x , offset% getDim().x};
	}

	Metrix offset2mt(int offset,int N)
	{
		if (offset < N) return { 0 , offset };
		else return { offset / N , offset % N };
	}

	int mt2Offset(const Metrix& mt) 
	{
		if ((getDim().x < getDim().y) ? true : false) return (size_t)(getDim().x) * mt.x + mt.y-1;
		
		return (size_t)(getDim().x) * mt.x + mt.y;
	}

	int sizeN(const Dimension& dim)//NXM의 값 중 더 큰 값을 반환한다. 함수명 다시 짓기.
	{ return (dim.x < dim.y) ? dim.y : dim.x; }

	bool isBlockI()
	{
		if ((getDim().x == 4 && getDim().y == 1) || (getDim().x == 1 && getDim().y == 4))
		{
			this->setDim({ getDim().y, getDim().x }); //회전하면 행,열의 크기가 바뀜
			return true;
		}
		return false;
	}

	void rotate()
	{
		vector<Metrix> metrix; //블록을 구성하는 *은 무조건 4개.
		//N X M 행렬에서 N < M 일때 크기 조절 해서 저장해야 함.
		//해당 shape 검사. *가 있는 행렬 값을 metrix에 저장한다.
		for (int i = 0; i < (size_t)(this->getDim().x * this->getDim().y); i++)
			if (this->getFace()[i] == '*') metrix.push_back(offset2mt(i));
		
		for (int i = 0; i < 4; i++)//만약 1*4 2*3 일 경우에는 큰값으로 만들어줘야됨.
			metrix.push_back({ metrix[i].y,(sizeN(getDim())-1) - metrix[i].x });//N X N 크기의 배열에서 회전연산.

		metrix.erase(metrix.begin(),metrix.begin()+4);//회전 전의 값 삭제.

		this->setDim({ getDim().y, getDim().x }); //회전하면 행,열의 크기가 바뀜.

		//회전후 다시 shape 구성.
		string newshape;
		newshape.append((size_t)(this->getDim().x * this->getDim().y),' ');//빈 문자로 채운 후
		
		for (int i = 0; i < metrix.size(); i++)//vetor에 담긴 값에 따라 * 셋팅.
		{ newshape.replace(mt2Offset(metrix[i]), 1, "*"); }
		
		const char* rotatedshape = newshape.c_str();
		this->setFace(rotatedshape);//부모클래스 face에 복사.		
	}

public:
	static Block* GetInstance() {
		if (Instance == nullptr) {
			Instance = new Block();
		}
		return Instance;
	}

	void rotateBlock()
	{
		//block이 I형일 경우에는 따로 처리 해준다.
		if (isBlockI()) return;
		rotate();
	}

	vector<Position> getTargetPos2Stacked()
	{
		vector<Position> targetpos; //blocl의 가로길이가 곧 바닥과 부딫힐 블록의 수/길이이다.
		targetpos.resize(getDim().x);//바뀐 dimesion에 따라 targetpos도 조정.
		//targetpos 값 갱신 후 반환한다.	
		//int temp = 0;
		/*for (auto iter = targetpos.begin(); iter <= targetpos.end(); iter++)
		{
			*iter = { getPos().x + temp, getPos().y + getDim().y - 1 };
			temp++;
		}*/

		for (int i = 0; i < targetpos.size(); i++)
		{
			targetpos[i] = { getPos().x + i, getPos().y + getDim().y - 1 };
		}
			
		return targetpos;
	}

	void changeBlockShape(BlockShape enumshape)
	{
		switch (enumshape)
		{
		case I:
			setFace("****");
			setDim({ 1,4 });
			break;
		case L:
			setFace("*  ***");
			setDim({ 3,2 });
			break;
		case J:
			setFace("  ****");
			setDim({ 3,2 });
			break;
		case T:
			setFace(" * ***");
			setDim({ 3,2 });
			break;
		case O:
			setFace("****");
			setDim({ 2,2 });
			break;
		case S:
			setFace(" **** ");
			setDim({ 3,2 });
			break;
		case Z:
			setFace("**  **");
			setDim({ 3,2 });
			break;
		}		
	}
};

class Tetris : public GameObject
{
private:
	UI gamescreen;
	UI scoreboard;
	UI nextblockcard;
	Block* block;
	GameObject stackedblocks; // 밑에 적재된 블록. vector 사용. 가변적임.

public:
	Tetris()
		:GameObject("",{0,0},{30,30}) , block(Block::GetInstance())
		, gamescreen({0,0} ,{12,22}), scoreboard({ 13, 1 }, { 7,3 }), nextblockcard( {13,5} , {6,4} )
	{
		stackedblocks.setFace("**********");
		stackedblocks.setPos({gamescreen.getPos().x + 1, gamescreen.getDim().y - 2});
		stackedblocks.setDim({ 10, 1 });
	}

	void stackBlock()
	{
		//string을 char*로 변환해서 gameobject face를 변경한다.
	}

	void isCollisionEnter() override //충돌은 world position에 기반된다.
	{
		//vector<Position> screenposes = stackedblocks.getScreenPoses();
		//stackedblocks의 크기 pos값의 
		//Block::getTargetPos2Stacked()의 타겟범위내에서 screen[offset]== '*'을 찾는다.
		for (int dh = block->getTargetPos2Stacked().front().y+1; dh <= block->getTargetPos2Stacked().back().y+1; dh++)
			for (int dw = block->getTargetPos2Stacked().front().x; dw <= block->getTargetPos2Stacked().back().x; dw++)
				if (screen->readCanvas()[screen->pos2Offset({ dw,dh })] == '*')
				{
					Borland::gotoxy(0, 36);
					printf("충돌! %d %d", dw, dh);
					Borland::gotoxy(0, 0);
					return;
				}
				else {
					Borland::gotoxy(0, 37);
					printf("scrren:%d %d", dw, dh);
					Borland::gotoxy(0, 38);
					printf("block: %d %d", block->getPos().x, block->getPos().y);
					Borland::gotoxy(0, 0);
				}
		
	}

	void draw() override
	{
		gamescreen.draw();
		scoreboard.draw();
		nextblockcard.draw();
		stackedblocks.draw();
		block->draw();
		
	}

	void update() override
	{
		if (input->getKey(VK_LEFT)) {
			if (gamescreen.getPos().x+1 >= block->getPos().x) return;
			block->setPos({ (block->getPos().x - 1) % (screen->getWidth()), block->getPos().y });
		}
		if (input->getKey(VK_RIGHT)) {
			if (gamescreen.getPos().x + gamescreen.getDim().x - block->getDim().x - 1 <= block->getPos().x) return;
			block->setPos({ (block->getPos().x + 1) % (screen->getWidth()), block->getPos().y });
		}
		if (input->getKey(VK_UP)) {
			block->rotateBlock();
		}
		if (input->getKey(VK_DOWN)) {
			if (gamescreen.getDim().y - block->getDim().y - 1 <= block->getPos().y) return;
			block->setPos({ block->getPos().x , block->getPos().y+1 });
		}
		if (input->getKey(VK_SPACE)) {
			
		}

		isCollisionEnter();
	}

};


Block* Block::Instance = nullptr;


int main()
{
	Screen* screen = Screen::GetInstance();
	Input* input = Input::GetInstance();
	Block* block = Block::GetInstance();

	Tetris tetris = Tetris();

	block->changeBlockShape(BlockShape::Z);

	// Game loop
	bool isLooping = true;
	while (isLooping) {

		screen->clear();

		tetris.draw();

		input->readInputs();

		tetris.update();

		screen->render();
		Sleep(100);

	}
	printf("\nGame Over\n");

	return 0;
}

