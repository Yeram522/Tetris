#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "GameManager.h"
#include <algorithm>


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

	void update()
	{

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

	void move() override
	{
		this->setPos({ this->getPos().x , this->getPos().y + 1 });
	}

	void rotateBlock()
	{
		//block이 I형일 경우에는 따로 처리 해준다.
		if (isBlockI()) return;
		rotate();
	}

	vector<Position> getColliderPoses(WORD virtualKeyCode)
	{
		vector<Position> colliderposes; //blocl의 가로길이가 곧 바닥과 부딫힐 블록의 수/길이이다.

		switch (virtualKeyCode)
		{
		case VK_RIGHT:
			colliderposes.resize(getDim().y);//바뀐 dimesion에 따라 targetpos도 조정.targetpos 값 갱신 후 반환한다.	
			for (int i = 0; i < colliderposes.size(); i++)
				colliderposes[i] = { getPos().x + getDim().x - 1, getPos().y + i };
			break;
		case VK_LEFT:
			colliderposes.resize(getDim().y);//바뀐 dimesion에 따라 targetpos도 조정.targetpos 값 갱신 후 반환한다.	
			for (int i = 0; i < colliderposes.size(); i++)
				colliderposes[i] = { getPos().x , getPos().y + i };
			break;
		case VK_DOWN:
			colliderposes.resize(getDim().x);//바뀐 dimesion에 따라 targetpos도 조정.targetpos 값 갱신 후 반환한다.	
			for (int i = 0; i < colliderposes.size(); i++)
				colliderposes[i] = { getPos().x + i, getPos().y + getDim().y - 1 };
			break;
		}
		
			
		return colliderposes;
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
	int pastDim;
	int linepoint;

public:
	Tetris()
		:GameObject("", { 0,0 }, { 30,30 }), block(Block::GetInstance())
		, gamescreen({0,0} ,{12,22}), scoreboard({ 13, 1 }, { 7,3 }), nextblockcard( {13,5} , {6,4} )
	{
		stackedblocks.setFace("          ");
		stackedblocks.setDim({ 10, 1 });
		stackedblocks.setPos({gamescreen.getPos().x + 1, gamescreen.getDim().y - stackedblocks.getDim().y - 1});
		pastDim = stackedblocks.getDim().y;
	}

	//clear blocks 
	void clearBlocks()
	{
		//stackedblocks의 dim.y 만큼 검사한다.
		int deletecount = 0;
		string strblocks(stackedblocks.getFace());
		for (int dh = 0; dh < stackedblocks.getDim().y; dh++)
			if (strblocks.substr(dh*10, 10) == "**********")
			{
				strblocks.erase(dh * 10, dh + 9);
				deletecount++;
			}
		stackedblocks.setFace(strblocks.c_str());
		if(stackedblocks.getDim().y - deletecount == 0) stackedblocks.setDim({ 10, 1 });
		else
			stackedblocks.setDim({ 10, stackedblocks.getDim().y - deletecount });
		stackedblocks.setPos({ stackedblocks.getPos().x , stackedblocks.getPos().y + deletecount});
		linepoint += deletecount;
	}
	
	//save the block when crash with other stacked blocks.
	void stackBlocks()
	{
		int newDim;
		int hp;
		if (pastDim < gamescreen.getDim().y - block->getPos().y + 1)
		{
			newDim = gamescreen.getDim().y - block->getPos().y + 1;
			pastDim = newDim;
			hp = block->getPos().y;
		}
		else
		{
			newDim = pastDim;
			hp = stackedblocks.getPos().y;
		}
		
		//resize h capacity
		stackedblocks.setDim({ 10, newDim }); 
		stackedblocks.setPos({ gamescreen.getPos().x + 1, hp });
		string staticblocks;
	   //just get the screen canvas and save the blocks status.
		for (hp ; hp <= 22; hp++)
			for (int wp = stackedblocks.getPos().x; wp < stackedblocks.getPos().x + 10; wp++)
				staticblocks.push_back(screen->readCanvas()[screen->pos2Offset({wp,hp})]);

		
		const char* newshape = staticblocks.c_str();
		stackedblocks.setFace(newshape);

		block->setPos({ 5,1 });//resetposition of singlton block.
		//set different shape of block of random.	
	}


	//process collision by key down condition
	bool checkLeftCollision()
	{
		int dw = block->getColliderPoses(VK_LEFT).front().x;
		for (int dh = block->getColliderPoses(VK_LEFT).front().y; dh <= block->getColliderPoses(VK_LEFT).back().y; dh++)
			if (screen->readCanvas()[screen->pos2Offset({ dw - 1 ,dh })] == '*' /*target stacked block*/
				&& screen->readCanvas()[screen->pos2Offset({ dw ,dh })] == '*') /*singleton Block, can move*/
			{
				return true;
				Borland::gotoxy(0, 36);
				printf("LEFT 충돌");
				Borland::gotoxy(0, 0);
			}
		return false;
	}

	bool checkRightCollision()
	{
		int dw = block->getColliderPoses(VK_RIGHT).front().x;
		for (int dh = block->getColliderPoses(VK_RIGHT).front().y; dh <= block->getColliderPoses(VK_RIGHT).back().y; dh++)
			if (screen->readCanvas()[screen->pos2Offset({ dw + 1 ,dh })] == '*' /*target stacked block*/
				&& screen->readCanvas()[screen->pos2Offset({ dw ,dh })] == '*') /*singleton Block, can move*/
			{
				return true;
				Borland::gotoxy(0, 36);
				printf("RIGHT 충돌");
				Borland::gotoxy(0, 0);
			}
		return false;
	}

	bool checkDownCollision()
	{
		int dh = block->getColliderPoses(VK_DOWN).front().y;
		for (int dw = block->getColliderPoses(VK_DOWN).front().x; dw <= block->getColliderPoses(VK_DOWN).back().x; dw++)
			if ((screen->readCanvas()[screen->pos2Offset({ dw ,dh + 1 })] == '*' || screen->readCanvas()[screen->pos2Offset({ dw ,dh + 1 })] == '-') /*target stacked block*/
				&& screen->readCanvas()[screen->pos2Offset({ dw ,dh })] == '*') /*singleton Block, can move*/
			{
				stackBlocks();
				
				Borland::gotoxy(0, 36);
				printf("충돌");
				Borland::gotoxy(0, 0);

				return true;
			}

		return false;

	}

	void autoMoveBlock()
	{
		if (checkDownCollision()) return;
		block->move();
	}
	
	//check overlap '*'point per prame and recover hided shape.
	void overlapBlock()
	{
		vector<Position> stackedblocksScreenposes;
	    for (int h = stackedblocks.getPos().y; h < stackedblocks.getPos().y + stackedblocks.getDim().y; h++)
			for (int w = stackedblocks.getPos().x; w < stackedblocks.getPos().x + stackedblocks.getDim().x; w++)
				stackedblocksScreenposes.push_back({ w,h });

		for (int h = this->getPos().y; h < this->getPos().y + this->getDim().y; h++)
			for (int w = this->getPos().x; w < this->getPos().x + this->getDim().x; w++)
				if (screen->readCanvas()[screen->pos2Offset({ w ,h })] == ' ' &&
					stackedblocks.getFace()[findLocalIndex(stackedblocksScreenposes, { w ,h })] == '*')
				{
					screen->writeCanvas(screen->pos2Offset({ w ,h }), '*');
					//스크린엔 빈칸으로 보이지만,ㅡ 그위치에 쌓인 블록정보가 있으면 겹친것!
					Borland::gotoxy(0, 36);
					printf("겹침");
					Borland::gotoxy(0, 0);
					
				}	

		Borland::gotoxy(0, 36);
		printf("     ");
		Borland::gotoxy(0, 0);
		
	}

	int findLocalIndex(const vector<Position> &arr, const Position pos)
	{
		for (auto i = 0; i < arr.size(); i++) {
			if (arr[i].x == pos.x && arr[i].y == pos.y)
			{
				return i;
			}
		}
		return -1;
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
		overlapBlock();

		if (input->getKey(VK_LEFT)) {
			if (gamescreen.getPos().x+1 >= block->getPos().x) return;
			if (checkLeftCollision()) return;
			block->setPos({ (block->getPos().x - 1) % (screen->getWidth()), block->getPos().y });
		}
		if (input->getKey(VK_RIGHT)) {
			if (gamescreen.getPos().x + gamescreen.getDim().x - block->getDim().x - 1 <= block->getPos().x) return;
			if (checkRightCollision()) return;
			block->setPos({ (block->getPos().x + 1) % (screen->getWidth()), block->getPos().y });
		}
		if (input->getKey(VK_UP)) {
			block->rotateBlock();
		}
		if (input->getKey(VK_DOWN)) {
			if (checkDownCollision()) return;
			if (gamescreen.getDim().y - block->getDim().y - 1 <= block->getPos().y) return;
			block->setPos({ block->getPos().x , block->getPos().y+1 });
		}
		if (input->getKey(VK_SPACE)) {
			// immediately move to the bottom
		}

		clearBlocks();
	}

};


Block* Block::Instance = nullptr;


int main()
{
	Screen* screen = Screen::GetInstance();
	Input* input = Input::GetInstance();
	Block* block = Block::GetInstance();

	Tetris tetris = Tetris();

	block->changeBlockShape(BlockShape::L);

	// Game loop
	bool isLooping = true;
	while (isLooping) {

		screen->clear();

		tetris.draw();

		//tetris.processCollisionEnter();
		//--------------------------
		input->readInputs();

		tetris.update();

		screen->render();

		tetris.autoMoveBlock();

		Sleep(100);

	}
	printf("\nGame Over\n");

	return 0;
}

