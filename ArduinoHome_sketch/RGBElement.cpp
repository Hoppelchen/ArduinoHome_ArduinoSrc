#include "RGBElement.h"

RGBElement::RGBElement(Printer* out, byte R, byte G, byte B, bool invert) :
	m_out(out),
	m_Rport(R),
	m_Gport(G),
	m_Bport(B),
	m_invert(invert)
{
	
	m_Ract  		= 0;
	m_Gact 			= 0;
	m_Bact 			= 0;
	
	m_Rnext			= 0;
	m_Gnext			= 0;
	m_Bnext			= 0;
	
	m_brightness 	= 1;

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
	
	m_colors		= new RGBColorTable(m_out);
	analogWrite(m_Rport, OUTPUT); 
	analogWrite(m_Gport, OUTPUT); 
	analogWrite(m_Bport, OUTPUT); 
	writeRGB();
}

//privat
void RGBElement::writeR()
{
	if(m_invert)
	{
		analogWrite(m_Rport, 255 - m_Ract/m_brightness);
	}
	else
	{
		analogWrite(m_Rport, m_Ract/m_brightness);
	}
}
void RGBElement::writeG()
{
	if(m_invert)
	{
		analogWrite(m_Gport, 255 -  m_Gact/m_brightness);
	}
	else
	{
		analogWrite(m_Gport, m_Gact/m_brightness);
	}
}
void RGBElement::writeB()
{
	if(m_invert)
	{
		analogWrite(m_Bport, 255 - m_Bact/m_brightness);
	}
	else
	{
		analogWrite(m_Bport, m_Bact/m_brightness);
	}
}
void RGBElement::writeRGB(){
	writeR();
	writeG();
	writeB();	
}
void RGBElement::fadeColor(){
	if(millis() > m_nextUpdate)
	{
		m_nextUpdate = millis()+ m_uTime;
		if(m_Rnext > m_Ract)
		{
			++m_Ract;
		}
		else if(m_Rnext < m_Ract)
		{
			--m_Ract;
		}
		if(m_Gnext > m_Gact)
		{
			++m_Gact;
		}
		else if(m_Gnext < m_Gact)
		{
			--m_Gact;
		}
		if(m_Bnext > m_Bact)
		{
			++m_Bact;
		}
		else if(m_Bnext < m_Bact)
		{
			--m_Bact;
		}
	}
}
void RGBElement::switchColor()
{
	m_Ract = m_Rnext;
	m_Gact = m_Gnext;
	m_Bact = m_Bnext;
}

//public
void RGBElement::stateMaschine()
{

	switch(m_state)
	{
		case 0:		//OFF
		{
			m_Rnext 		= 0;
			m_Gnext 		= 0;
			m_Bnext 		= 0;
			m_brightness 	= 1;
			m_fade 			= false;

			m_sync 			= false;
			
			break;
		}
		case 1:		//OFF_FADE_DOWN ==> STATE 0
		{
			m_Rnext 		= 0;
			m_Gnext 		= 0;
			m_Bnext 		= 0;
			m_brightness 	= 1;
			m_fade 			= true;

			m_sync			= true;

			if(isReady())
			{
				m_state = 0;
				m_out->pDln();			
				m_out->pDS("_%E");	m_out->pD(m_id);
				m_out->pDS("_%M");	m_out->pD(m_state);			//PROFILE/MODE
				m_out->pDln();		
				
			}
			break;
		}
		case 2:		//ON_WHITE
		{
			m_Rnext 		= 255;
			m_Gnext 		= 255;
			m_Bnext 		= 255;
			m_brightness 	= 1;
			m_fade 			= false;

			m_sync 			= false;

			break;
		}
		case 3:		//ON_WHITE_FADE_UP  ==> STATE 2
		{
			m_Rnext 		= 255;
			m_Gnext 		= 255;
			m_Bnext 		= 255;
			m_brightness 	= 1;
			m_fade 			= true;
			m_uTime			= 5;

			m_sync 			= true;
			
			if(isReady())
			{
				m_state = 2;	
				m_out->pDln();			
				m_out->pDS("_%E");	m_out->pD(m_id);
				m_out->pDS("_%M");	m_out->pD(m_state);			//PROFILE/MODE
				m_out->pDln();		
				
			}
			break;
		}
		case 4:		//SELECTED_COLOR
		{
			break;
		}
		case 5:		//NEXT_COLOR
		{
			if(m_autoNext && (m_sync || isReady()))
			{
				nextColor();
				m_out->pDS("_%E");	m_out->pD(m_id);
				m_out->pDS("_%R");	m_out->pD(m_Rnext);		//RED
				m_out->pDS("_%G");	m_out->pD(m_Gnext);		//GREEN
				m_out->pDS("_%B");	m_out->pD(m_Bnext);		//BLUE
				m_out->pDln();
			}
			break;
		}
		case 6:		//SHUFFLE_COLOR
		{
			if(m_autoNext && (m_sync || isReady()))
			{
				shuffleColor();
				m_out->pDS("_%E");	m_out->pD(m_id);
				m_out->pDS("_%R");	m_out->pD(m_Rnext);		//RED
				m_out->pDS("_%G");	m_out->pD(m_Gnext);		//GREEN
				m_out->pDS("_%B");	m_out->pD(m_Bnext);		//BLUE
				m_out->pDln();
			}
			break;
		}
		case 7:		//PART_COLORS
		{
			if(m_autoNext && (m_sync || isReady()))
			{
				partColor();
				m_out->pDS("_%E");	m_out->pD(m_id);
				m_out->pDS("_%R");	m_out->pD(m_Rnext);		//RED
				m_out->pDS("_%G");	m_out->pD(m_Gnext);		//GREEN
				m_out->pDS("_%B");	m_out->pD(m_Bnext);		//BLUE
				m_out->pDln();
			}
			break;
		}
		case 8:		//RAND_COLOR
		{
			if(m_autoNext && (m_sync || isReady()))
			{
				randColor();
				m_out->pDS("_%E");	m_out->pD(m_id);
				m_out->pDS("_%R");	m_out->pD(m_Rnext);		//RED
				m_out->pDS("_%G");	m_out->pD(m_Gnext);		//GREEN
				m_out->pDS("_%B");	m_out->pD(m_Bnext);		//BLUE
				m_out->pDln();
			}
			break;
		}
		default:	//SWTICH_OFF
		{
			m_state = 0;
		}
	}
}
void RGBElement::update()
{
	/*
	 * WAIT FOR NEXT WAIT TIME /
	 */
	/*
	 * IF COLOR CHANGE PRINT STATUS
	 */
	if(!isReady())
	{
		if(m_fade)
		{
			fadeColor();
		}
		else
		{
			switchColor();
		}
		writeRGB();
	}
	if(m_sync||(m_play && (millis()> m_nextWait)))
	{
		m_nextWait = millis()+ (m_wTime * 1000);
		stateMaschine();
		if(m_state > 3)
		{
			m_sync = false;
		}
	}
	else if(!m_play)
	{
		/*
		 * SAVE WAIT TIME STATE
		 *  ADD DURATION QUICKFIX
		 */
		if(m_nextWait > millis())
		{
			int diff = m_nextWait - millis();
			m_nextWait += diff;
		}
	}
}
RGBElement RGBElement::getElement()
{
	return *this;
}

void RGBElement::setR(byte value, bool force){
	m_Rnext = value;
	m_sync = force;
	if(force)
	{
		switchColor();
		writeR();
	}
}
void RGBElement::setG(byte value, bool force){
	m_Gnext = value;
	m_sync = force;
	if(force)
	{
		switchColor();
		writeG();
	}
}
void RGBElement::setB(byte value, bool force){
	m_Bnext = value;
	m_sync = force;
	if(force)
	{
		switchColor();
		writeB();
	}
}
void RGBElement::setRGB(byte R, byte G, byte B, bool force)
{
	if((m_Ract != R)||(m_Gact != G)||(m_Bact !=B))
	{
		m_Rnext = R;
		m_Gnext = G;
		m_Bnext = B;
		m_sync = force;
	}
	if(force){
		switchColor();
		writeRGB();
	}
}
void RGBElement::incR()
{
	if(m_Ract<255)
	{
		m_Rnext=m_Ract+1;
	}
	m_sync = true;
}
void RGBElement::incG()
{
	if(m_Gact<255)
	{
		m_Gnext=m_Gact+1;
	}
	m_sync = true;	
}
void RGBElement::incB()
{
	if(m_Bact<255)
	{
		m_Bnext=m_Bact+1;
	}
	m_sync = true;	
}
void RGBElement::incRGB()
{
	if(m_Ract<255)
	{
		m_Rnext=m_Ract+1;
	}
	if(m_Gact<255)
	{
		m_Gnext=m_Gact+1;	
	}
	if(m_Bact<255)
	{
		m_Bnext=m_Bact+1;	
	}
	m_sync = true;	
}
void RGBElement::decR()
{
	if(m_Ract>0)
	{
		m_Rnext=m_Ract-1;
	}
	m_sync = true;		
}
void RGBElement::decG()
{
	if(m_Gact>0)
	{
		m_Gnext=m_Gact-1;
	}
	m_sync = true;	
}
void RGBElement::decB()
{
	if(m_Bact>0)
	{
		m_Bnext=m_Bact-1;
	}
	m_sync = true;	
}
void RGBElement::decRGB()
{
	if(m_Ract>0)
	{
		m_Rnext=m_Ract-1;
	}
	if(m_Gact>0)
	{
		m_Gnext=m_Gact-1;		
	}
	if(m_Bact>0)
	{
		m_Bnext=m_Bact-1;
	}
	m_sync = true;			
}
void RGBElement::setBrightness(byte value)
{
	if(value>0)
	{
		m_brightness = value;
	}
	else
	{
		m_brightness = 1;
	}
	writeRGB();
}
void RGBElement::setFade(bool status)
{
	m_fade = status;
	m_sync	= true;
}
void RGBElement::setPlay(bool status)
{
	m_play = status;
	m_sync	= true;
}
void RGBElement::setAutoNext(bool status)
{
	m_autoNext = status;
	m_sync	= true;
}
void RGBElement::setProfile(byte how)
{
	if(how >= 20)
	{
		m_state = 4;
	}
	else
	{
		m_state = how;	
	}
	if(m_state > 4)
	{
		m_play = true;
		m_autoNext = true;
	}
	m_sync	= true;
	m_out->pDS("_%E");	m_out->pD(m_id);
	m_out->pDS("_%M");	m_out->pD(m_state);			//PROFILE/MODE
	m_out->pDln();		
}
void RGBElement::sync()
{
	m_sync = true;
}
void RGBElement::sync(int milSecTime)
{
	m_nextUpdate = millis()+ milSecTime;
}
void RGBElement::setUpdateTime(int milSecTime)
{
	m_uTime = milSecTime;
	m_nextUpdate = millis()+ m_uTime;
}
void RGBElement::setWaitTime(int SecTime)
{
	m_wTime = SecTime;
	m_nextWait = millis() + (m_wTime * 1000);
}
bool RGBElement::isReady()
{
	m_ready = ((m_Ract == m_Rnext)&&(m_Gact == m_Gnext)&&(m_Bact == m_Bnext));
	return m_ready;
}
void RGBElement::nextColor()
{
	Color* color = m_colors->getNext();
	m_Rnext = color->getRed();
	m_Gnext = color->getGreen();
	m_Bnext = color->getBlue();
}
void RGBElement::shuffleColor()
{
	byte shuffle = random (m_colors->getSize());
	Color* color =  m_colors->getColor(shuffle);
	m_Rnext = color->getRed();
	m_Gnext = color->getGreen();
	m_Bnext = color->getBlue();
}
void RGBElement::randColor()
{
	m_Rnext = random(255);
	m_Gnext = random(255);
	m_Bnext = random(255);
}
void RGBElement::partColor()
{
	if(m_colors->getSize()>1)
	{
		byte  Rdiff = 0;
		byte  Gdiff = 0;
		byte  Bdiff = 0;

		if(m_colors->getColor(0)->getRed() > m_colors->getColor(1)->getRed())
		{
			Rdiff = m_colors->getColor(0)->getRed() - m_colors->getColor(1)->getRed();
			m_Rnext = m_colors->getColor(1)->getRed();
		}
		else
		{
			Rdiff = m_colors->getColor(1)->getRed() - m_colors->getColor(0)->getRed();
			m_Rnext = m_colors->getColor(0)->getRed();
		}
		if(m_colors->getColor(0)->getGreen() > m_colors->getColor(1)->getGreen())
		{
			Gdiff = m_colors->getColor(0)->getGreen() - m_colors->getColor(1)->getGreen();
			m_Gnext = m_colors->getColor(1)->getGreen();
		}
		else
		{
			Gdiff = m_colors->getColor(1)->getGreen() - m_colors->getColor(0)->getGreen();
			m_Gnext = m_colors->getColor(0)->getGreen();
		}
		if(m_colors->getColor(0)->getBlue() > m_colors->getColor(1)->getBlue())
		{
			Bdiff = m_colors->getColor(0)->getBlue() - m_colors->getColor(1)->getBlue();
			m_Bnext = m_colors->getColor(1)->getBlue();
		}
		else
		{
			Bdiff = m_colors->getColor(1)->getBlue() - m_colors->getColor(0)->getBlue();
			m_Bnext = m_colors->getColor(0)->getBlue();
		}

		m_Rnext += random(Rdiff);
		m_Gnext += random(Gdiff);
		m_Bnext += random(Bdiff);
	}
}
RGBColorTable* RGBElement::getColorTable()
{
	return m_colors;
}
void RGBElement::setId(byte id)
{
	m_id = id;
}
void RGBElement::toogleState(bool fade)
{
	if(m_state > 0)
	{
		if(fade)
		{
			m_state = 1;
		}
		else
		{
			m_state = 0;
		}
	}
	else
	{
		if(fade)
		{
			m_state = 3;
		}
		else
		{
			m_state = 2;
		}
	}
	m_sync = true;

	m_out->pDS("_%E");	m_out->pD(m_id);
	m_out->pDS("_%M");	m_out->pD(m_state);			//PROFILE/MODE
	m_out->pDln();	
}
bool RGBElement::isOn()
{
	return m_state > 0;
}

void RGBElement::printStatus()
{
	m_out->pDS("_%E");	m_out->pD(m_id);
	m_out->pDS("_%R");	m_out->pD(m_Rnext);			//RED
	m_out->pDS("_%G");	m_out->pD(m_Gnext);			//GREEN
	m_out->pDS("_%B");	m_out->pD(m_Bnext);			//BLUE
	m_out->pDS("_%I");	m_out->pD(m_brightness);	//INTENSE
	m_out->pDln();

	m_out->pDS("_%E");	m_out->pD(m_id);
	m_out->pDS("_%S");	m_out->pD(m_uTime);			//SPEED
	m_out->pDS("_%W");	m_out->pD(m_wTime);			//WAIT
	m_out->pDS("_%M");	m_out->pD(m_state);			//PROFILE/MODE
	m_out->pDS("_%F");	m_out->pD(m_fade);			//FADE
	m_out->pDS("_%P");	m_out->pD(m_play);			//PLAY
	m_out->pDS("_%A");	m_out->pD(m_autoNext);		//AUTONEXT
	m_out->pDS("_%S");	m_out->pD(m_sync);			//SYNC/LOCKED
	m_out->pDln();
}
