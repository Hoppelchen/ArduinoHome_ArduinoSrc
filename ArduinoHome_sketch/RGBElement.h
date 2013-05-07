#ifndef _H_RGBElement_H_
	#define _H_RGBElement_H_
#include <Arduino.h>
#include "Printer.h"
#include "RGBColorTable.h"
class RGBElement{
	private:
	
		const byte m_Rport;
		const byte m_Gport;
		const byte m_Bport;

		bool m_invert;

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
		bool m_autoNext;

		byte m_state;

		byte m_id;

		Printer* m_out;

		void writeR();
		void writeG();
		void writeB();	
		void writeRGB();
		void fadeColor();
		void switchColor();
		void stateMaschine();
		
	public:
		RGBElement(Printer* out, byte R, byte G, byte B, bool invert);
		RGBElement getElement();
		RGBColorTable* getColorTable();
		void update();
		void setR(byte value, bool force);
		void setG(byte value, bool force);
		void setB(byte value, bool force);
		void setId(byte id);
		void setRGB(byte R, byte G, byte B, bool force);
		void incR();
		void incG();
		void incB();
		void incRGB();
		void decR();
		void decG();
		void decB();
		void decRGB();		
		void nextColor();
		void shuffleColor();
		void randColor();
		void partColor();
		void setBrightness(byte value);
		void setFade(bool status);
		void setPlay(bool status);
		void setAutoNext(bool status);
		void setProfile(byte how);
		void sync();
		void sync(int milSecTime);
		void setUpdateTime(int milSecTime);
		void setWaitTime(int SecTime);
		void toogleState(bool fade);
		void printStatus();
		bool isOn();
		bool isReady();

};

#endif //_H_RGBElement_H_
