#ifndef _H_Canvas_H_
#define _H_Canvas_H_

#include <Arduino.h>
class Canvas{
private:

	const int m_portOnUp;			//port on up
	const int m_portOnDw;			//port on down

	const int m_portOffUp;			//sensor stop up
	const int m_portOffDw;			//sensor stop dw

	const bool m_logicUp;			//negative or positive logic up
	const bool m_logicDw;			//negative or positive logic dw

	bool upS;						//up switch
	bool dwS;						//down switch

public:

	Canvas(int portOnUp, int portOnDw, int portOffUP, int portOffDw, bool logicU, bool logicD):
		m_portOnUp(portOnUp),
		m_portOnDw(portOnDw),
		m_portOffUp(portOffUP),
		m_portOffDw(portOffDw),
		m_logicUp(logicU),
		m_logicDw(logicD)
	{
	
		upS			= false;
		dwS 		= false;
		
		pinMode(m_portOnUp, OUTPUT);
		pinMode(m_portOnDw, OUTPUT);
	
		pinMode(m_portOffUp, INPUT);
		pinMode(m_portOffDw, INPUT);
		
		digitalWrite(m_portOffUp, HIGH);
		digitalWrite(m_portOffDw, HIGH);

		digitalWrite(m_portOnUp, HIGH);
		digitalWrite(m_portOnDw, HIGH);
	}
	void update()
	{
		if(dwS)
		{
			digitalWrite(m_portOnUp, HIGH);			//first to be save set up to low
			if((m_logicDw &&!digitalRead(m_portOffDw))||(!m_logicDw && digitalRead(m_portOffDw)))
			{			//if the stop trigger isn't hit
				digitalWrite(m_portOnDw, LOW);		//drive
			}
			else
			{
				digitalWrite(m_portOnDw, HIGH);		//stop
				dwS = false;						//reset switch
				Serial.println("CANVAS STOP BOTTOM RANGE");
			}
		}
		else if(upS)
		{
			digitalWrite(m_portOnDw, HIGH);
			if((m_logicUp &&!digitalRead(m_portOffUp))||(!m_logicUp && digitalRead(m_portOffUp)))
			{
				digitalWrite(m_portOnUp, LOW);
			}
			else
			{
				digitalWrite(m_portOnUp, HIGH);
				upS = false;
				Serial.println("CANVAS STOP UPPER RANGE");
			}
		}
		else
		{
			digitalWrite(m_portOnUp, HIGH);
			digitalWrite(m_portOnDw, HIGH);
			upS = false;
			dwS = false;
		}
	}
	void up()
	{
		dwS = false;
		if(upS != true)
		{
			upS = true;
		}
		else
		{
			upS = false;
		}
	}
	void dw()
	{
		upS = false;
		if(dwS != true)
		{
			dwS = true;
		}
		else
		{
			dwS = false;
		}
	}
	void cancel()
	{
		digitalWrite(m_portOnUp, HIGH);
		digitalWrite(m_portOnDw, HIGH);
		upS = false;
		dwS = false;
		Serial.println("CANVAS STOP CANCELD");
	}
};

#endif //_H_Canvas_H_
