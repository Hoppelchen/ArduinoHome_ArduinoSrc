#ifndef _H_RGBColorTable_H_
	#define _H_RGBColorTable_H_
	
#include <Arduino.h>
#include "Printer.h"

class Color
{
	const byte red;
	const byte blue;
	const byte green;
	Color* next;
	
public:
	Color(byte red, byte green, byte blue):
		red(red),
		blue(blue),
		green(green),
		next(NULL)

	{}

	byte getRed()
	{
		return red;
	}
	
	byte getBlue()
	{
		return green;
	}
	
	byte getGreen()
	{
		return blue;
	}

	void setNext(Color *nextCol)
	{
		next = nextCol;
	}
	Color* getNext()
	{
		return next;
	}

};

class RGBColorTable
{
private:
	Printer* m_out;
	Color* 	first;
	Color*  actCol;
	byte 	count;
	byte 	act;

public:
	RGBColorTable(Printer* out)
	{
		m_out	= out;
		first 	= NULL;
		actCol 	= NULL;
		count 	= 0;
		fillStd();
	}
	void addNewColor(byte red, byte green, byte blue)
	{
		Color* color = new Color(red,green,blue);
		if(count == 0)
		{
			first = color;
		}
		else
		{
			actCol = first;
			while(actCol->getNext() != NULL)
			{
				actCol = actCol->getNext();
			}
			actCol->setNext(color);
		}
		
		m_out->pDS("%_COLORTABLE_ADD_POS_");
		m_out->pD(count);
		m_out->pDS("_R");		
		m_out->pD(color->getRed());
		m_out->pDS("_G");
		m_out->pD(color->getGreen());
		m_out->pDS("_B");
		m_out->pD(color->getBlue());
		m_out->pDln();
		
		count+=1;
	}

	Color* getColor(int id)
	{
		if(count < id)
		{
			return NULL;
		}
		else
		{
			actCol = first;
			for(byte i = 0; i < id; i++)
			{
				actCol = actCol->getNext();
			}
			return actCol;
			act = id;
		}
	}

	Color* getAct()
	{
		return actCol;
	}

	Color* getNext()
	{
		Color* next = actCol->getNext();
		if(next != NULL )
		{
			actCol = next;
		}
		else
		{
			actCol = first;
		}
		return actCol;
	}

	byte getSize()
	{
		return count;
	}

	void eraseTable()
	{
		Color* actPos = first;
		Color* tmp = NULL;
		while(actPos != NULL)
		{
			tmp = actPos;
			actPos = actPos->getNext();
			m_out->pDS("%_COLORTABLE_DEL_R");
			m_out->pD(tmp->getRed());
			m_out->pDS("_G");
			m_out->pD(tmp->getGreen());
			m_out->pDS("_B");
			m_out->pD(tmp->getBlue());
			m_out->pDln();
			delete(tmp);
		}
		first = NULL;
		act = 0;
		count = 0;
		actPos = NULL;
	}
	void fillStd()
	{
		addNewColor( 10, 0, 0);
		addNewColor( 0, 10, 0);
		addNewColor( 0, 0, 10);
	}

};
	
#endif //_H_RGBColorTable_H_
