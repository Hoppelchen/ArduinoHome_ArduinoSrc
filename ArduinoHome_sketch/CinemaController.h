#ifndef _H_CinemaController_H_
	#define _H_CinemaController_H_
	
#include <Arduino.h>
#include "Canvas.h"
class CinemaController{
	private:
		const static byte AMMOUNTOFBUTTONS = 3;
		const static byte AMMOUNTOFWINDOWS = 2;
		const static byte AMMOUNTOFCANVAS = 1;
		const static byte cyclus = 100;
		Canvas*	m_canvas[AMMOUNTOFCANVAS];
		Canvas*	m_windows[AMMOUNTOFWINDOWS];
		byte m_buttonPorts[AMMOUNTOFBUTTONS];
		byte m_buttonsCycle[AMMOUNTOFBUTTONS];
		bool m_button_toggle[AMMOUNTOFBUTTONS];
		bool m_button_toggle_status[AMMOUNTOFBUTTONS];
	public:
	
		CinemaController()
		{
			#ifdef MARKUS
				m_canvas[0] = new Canvas(22,23,36,37, false, false);
				m_windows[0] = new Canvas(26,27,40,41, false, false);
				m_windows[1] = new Canvas(24,25,38,39, false, false);
			#endif
			#ifdef KARIM
				m_canvas[0] = new Canvas(22,23,36,37, true, true);
				m_windows[0] = new Canvas(24,25,38,39, true, true);
				m_windows[1] = new Canvas(26,27,40,41, true, true);			
			#endif
			m_buttonPorts[0] = 30;
			m_buttonPorts[1] = 31;
			m_buttonPorts[2] = 32;
			pinMode(m_buttonPorts[0], OUTPUT); 
			pinMode(m_buttonPorts[1], OUTPUT); 
			pinMode(m_buttonPorts[2], OUTPUT);
			m_buttonsCycle[0] = 0;
			m_buttonsCycle[1] = 0;
			m_buttonsCycle[2] = 0;
			m_button_toggle[0] = true;
			m_button_toggle[1] = true;
			m_button_toggle[2] = true;
			m_button_toggle_status[0] = false;
			m_button_toggle_status[1] = false;
			m_button_toggle_status[2] = false;
			digitalWrite(m_buttonPorts[0], HIGH);
			digitalWrite(m_buttonPorts[1], HIGH);
			digitalWrite(m_buttonPorts[2], HIGH);
		}
		void update(){
	
			for(int i = 0; i < AMMOUNTOFBUTTONS; i++)
			{
				if(!m_button_toggle[i])
				{
					if(m_buttonsCycle[i]>1)
					{
						m_buttonsCycle[i]-=1;
						digitalWrite(m_buttonPorts[i],LOW);
					}
					else
					{
						digitalWrite(m_buttonPorts[i],HIGH);
					}	
				}
			}
			for(int i = 0; i< AMMOUNTOFWINDOWS; i++)
			{
				m_windows[i]->update();
			}
			for(int i = 0; i< AMMOUNTOFCANVAS; i++)
			{
				m_canvas[i]->update();
			}			
		}
		void windowUp(int num){
			if(num == 0)
			{
				for(int i = 0; i< AMMOUNTOFWINDOWS; i++)
				{
					m_windows[i]->up();
				}
			}
			else if(num > 0 && num <= AMMOUNTOFWINDOWS)
			{
				m_windows[num-1]->up();
			}
		}
		void windowDw(int num){
			if(num == 0)
			{
				for(int i = 0; i< AMMOUNTOFWINDOWS; i++)
				{
					m_windows[i]->dw();
				}
			}
			else if(num > 0 && num <= AMMOUNTOFWINDOWS)
			{
				m_windows[num-1]->dw();
			}
		}
		void canvasUp(){
			for(int i = 0; i< AMMOUNTOFCANVAS; i++)
			{
				m_canvas[i]->up();
			}			
		}
		void canvasDw(){
			for(int i = 0; i< AMMOUNTOFCANVAS; i++)
			{
				m_canvas[i]->dw();
			}					
		}
		void pressButton(byte which)
		{
			if(which >= 0 && which <AMMOUNTOFBUTTONS)
			{
				if(m_button_toggle[which])
				{
					m_button_toggle_status[which]  = !m_button_toggle_status[which];
					if(m_button_toggle_status[which])
					{
						digitalWrite(m_buttonPorts[which],LOW);						
					}
					else
					{
						digitalWrite(m_buttonPorts[which],HIGH);						
					}
				}
				else
				{
					m_buttonsCycle[which] = cyclus;
					Serial.print("BUTTON ");
					Serial.print(which);
					Serial.println(" received");
				}
			}
		}
		
		void setButton(byte which, bool status)
		{
			if(which >= 0 && which <AMMOUNTOFBUTTONS)
			{
				if(m_button_toggle[which])
				{
					m_button_toggle_status[which] = status;
					if(m_button_toggle_status[which])
					{
						digitalWrite(m_buttonPorts[which],LOW);						
					}
					else
					{
						digitalWrite(m_buttonPorts[which],HIGH);						
					}
				}
				else
				{
					m_buttonsCycle[which] = cyclus;
					Serial.print("BUTTON ");
					Serial.print(which);
					Serial.println(" received");
				}
			}
		}
		
		void stop()
		{
			for(int i = 0; i< AMMOUNTOFCANVAS; i++)
			{
				m_canvas[i]->cancel();
			}
			for(int i = 0; i < AMMOUNTOFBUTTONS; i++)
			{

				m_buttonsCycle[i] = 0;
				digitalWrite(m_buttonPorts[i],HIGH);
			}
		}
			
};
	
#endif //_H_CinemaController_H_
