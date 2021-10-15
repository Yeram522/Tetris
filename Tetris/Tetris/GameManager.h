#pragma once
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <iostream>
#include <conio.h> // console io
#include <cstring> // string.h
#include <cstdlib> // stdlib.h
#include <string>
#include <string> // c++ string class
#include <Windows.h>
#include "Input.h"

class Screen;
class GameObject;
class Input;

class Screen {
private:
	int		width; // visible width
	int		height;
	int		size;
	char* canvas;

	// constructor (생성자 함수) 메모리공간상에 적재되는 순간 호출되는
	Screen(int width = 10, int height = 10)
		: width(width), height(height), canvas(new char[(width + 1) * height])
	{
		bool faultyInput = false;
		if (this->width <= 0) {
			this->width = 10;
			faultyInput = true;
		}
		if (this->height <= 0) {
			this->height = 10;
			faultyInput = true;
		}
		size = (this->width + 1) * this->height;
		if (faultyInput == true) {
			delete canvas;
			canvas = new char[size];
		}
	}
	// destructor (소멸자 함수) 메모리공간상에서 없어지는 순간 호출되는 함수
	virtual ~Screen()
	{
		delete[] canvas;
		canvas = nullptr;
		width = 0; height = 0;
	}

	static Screen* Instance;


public:

	static Screen* GetInstance() {
		if (Instance == nullptr) {
			Instance = new Screen(20,20);
		}
		return Instance;
	}

	int getWidth() const
	{
		return width;
	}

	void setWidth(int width)
	{
		this->width = width;
	}

	void clear()
	{
		memset(canvas, ' ', size);
	}

	Position offset2Pos(int offset) const
	{

	}

	int pos2Offset(const Position& pos) const
	{
		return (width+1) * pos.y + pos.x;
	}

	void draw(const Position& pos, const char* shape, const Dimension& sz = Position{ 1, 1 })
	{
		int offset = pos2Offset(pos);
		for (int h = 0; h < sz.y; h++)
			strncpy(&canvas[offset + (width + 1) * h], &shape[h * sz.x], sz.x);
	}

	void render()
	{
		Borland::gotoxy(0, 0);
		for (int h = 0; h < height; h++)
			canvas[(width + 1) * (h + 1) - 1] = '\n';
		canvas[size - 1] = '\0';
		printf("%s", canvas);
	}

};

Screen* Screen::Instance = nullptr;



class GameObject
{
private:
	char		face[100];
	Position	pos;
	Dimension	dim;
	
protected:
	Screen* screen;
	Input* input;

public:

	GameObject(const char* face, const Position& pos, const Dimension& dim)
		: pos(pos), screen(Screen::GetInstance()), dim(dim), input(Input::GetInstance())
	{
		setFace(face);
	}

	GameObject(const Position& pos, const Dimension& dim)
		: pos(pos), screen(Screen::GetInstance()), dim(dim), input(Input::GetInstance())
	{

	}

	virtual ~GameObject() {}

	void move(int direction)
	{
	}
	void move()
	{
	}
	virtual void draw()
	{
		screen->draw(pos, face, dim);
	}

	virtual void update()
	{
		if (input->getKey(VK_LEFT)) {
			if (pos.x <= 0) return;
			pos.x = (pos.x - 1) % (screen->getWidth());
		}
		if (input->getKey(VK_RIGHT)) {
			if (pos.x >= (screen->getWidth() - 1)) return;
			pos.x = (pos.x + 1) % (screen->getWidth());
		}
	}

	Position getPos() { return pos; } // getter function
	void setPos(const Position& pos) { this->pos = pos; } // setter function

	const char* getFace() { return face; }
	void setFace(const char* face) { strcpy(this->face, face); }

	Dimension getDim() { return dim; }

	Screen* getScreen() { return screen; }
};