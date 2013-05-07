#ifndef _H_RGBController_H_
	#define _H_RGBController_H_
#include <Arduino.h>
#include "RGBElement.h"
#include "Printer.h"

#define ELEMENTS 3
class RGBController{
	private:
		byte m_size;
		bool m_locked;

		int  party_vol_0;
		int  party_vol_1;
		
		bool party_color_red[ELEMENTS];
		bool party_color_green[ELEMENTS];
		bool party_color_blue[ELEMENTS];
		
		RGBElement* m_elements[3];
		
		byte m_Ract;
		byte m_Gact;
		byte m_Bact;

		byte m_Rnext;
		byte m_Gnext;
		byte m_Bnext;

		RGBColorTable* m_colors;

		byte m_brightness;

		unsigned long m_nextUpdate;
		int m_uTime;

		unsigned long m_nextWait;
		int m_wTime;

		bool m_play;
		bool m_ready;
		bool m_sync;
		bool m_fade;
		bool m_stopfade;
		bool m_autoNext;

		byte m_state;

		byte m_id;

		Printer* m_out;

		void writeRGB();
		void fadeColor();
		void switchColor();
		void stateMaschine();
		void audioIlluminatorBeatOfVol();
		void nextColor();
		void shuffleColor();
		void partColor();
		void randColor();

	public:
		RGBController(Printer* out)
		{
			m_size = 3;
			m_out = out;
			#ifdef MARKUS
				m_elements[0] = new RGBElement(out, 4, 	2, 		3, 	true);
				m_elements[1] = new RGBElement(out, 7, 	5, 		6, 	true);
				m_elements[2] = new RGBElement(out, 12,	8,	   9, 	true);
				m_out->pDS("LOAD RGB MARKUS");
			#endif //MARKUS
			#ifdef KARIM
				m_elements[0] = new RGBElement(out, 6, 	5, 		4, 	true);
				m_elements[1] = new RGBElement(out, 9, 	8, 		7, 	true);
				m_elements[2] = new RGBElement(out,11,	3,		2, 	true);
				m_out->pDS("LOAD RGB KARIM");				
			#endif //KARIM			
			m_out->pDln();
			
			m_elements[0]->setId(1);
			m_elements[1]->setId(2);
			m_elements[2]->setId(3);

			m_colors = new RGBColorTable(m_out);
			
			m_locked = false;

			party_vol_0 = 0;
			party_vol_1 = 0;

			party_color_red[0]		= false;
			party_color_green[0]	= false;
			party_color_blue[0]		= false;

			party_color_red[1]		= false;
			party_color_green[1]	= false;
			party_color_blue[1]		= false;

			party_color_red[2]		= false;
			party_color_green[2]	= false;
			party_color_blue[2]		= false;

			m_Ract  		= 0;
			m_Gact 			= 0;
			m_Bact 			= 0;

			m_Rnext			= 0;
			m_Gnext			= 0;
			m_Bnext			= 0;

			m_nextUpdate 	= 0;
			m_uTime			= 5;

			m_nextWait 		= 0;
			m_wTime 		= 60;

			m_play			= true;
			m_sync			= true;
			m_fade			= true;

			m_autoNext 		= true;
			m_ready 		= true;

			m_id 			= 0;
			m_state 		= 0;

		}
		RGBElement *getElement(byte ID);
		int getSize(){ return m_size;}
		void update();		
		void setR(byte value, bool force);
		void setG(byte value, bool force);
		void setB(byte value, bool force);
		void setRGB(byte R, byte G, byte B, bool force);
		void incR();
		void incG();
		void incB();
		void incRGB();
		void decR();
		void decG();
		void decB();
		void decRGB();				

		void setPartyVol_0(int vol);
		void setPartyVol_1(int vol);
		void setPartyColor(int rec, bool red, bool green, bool blue);		
		void setBrightness(byte value);
		void setFade(bool status);
		void setPlay(bool status);
		void setProfile(byte how);		
		void setAutoNext(bool status);
		void setLocked(bool satus);
		void sync();
		void sync(long milSecTime);
		void setUpdateTime(long milSecTime);
		void setWaitTime(long SecTime);
		void toogleState(byte how);
		void printStatus();
		bool isOn();
		bool isReady();
};

#endif //_H_RGBController_H_
