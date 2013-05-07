#include "RGBController.h"
RGBElement *RGBController::getElement(byte ID)
{
	return m_elements[ID];
}
void RGBController::writeRGB(){
	for(int i =0; i< m_size; i++)
	{
		if(m_elements[i])
		{
			m_elements[i]->setRGB(m_Ract,m_Gact,m_Bact, true);
		}
	}
}
void RGBController::fadeColor(){
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
void RGBController::switchColor()
{
	m_Ract = m_Rnext;
	m_Gact = m_Gnext;
	m_Bact = m_Bnext;
}

//public
void RGBController::stateMaschine()
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

			m_sync			= false;
			
			break;
		}
		case 1:		//OFF_FADE_DOWN ==> STATE 0
		{
			m_Rnext 		= 0;
			m_Gnext 		= 0;
			m_Bnext 		= 0;
			m_brightness 	= 1;
			m_fade 			= true;

			m_sync 			= true;

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
				m_out->pDS("_%L");
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
				m_out->pDS("_%L");
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
				m_out->pDS("_%L");
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
				m_out->pDS("_%L");
				m_out->pDS("_%R");	m_out->pD(m_Rnext);		//RED
				m_out->pDS("_%G");	m_out->pD(m_Gnext);		//GREEN
				m_out->pDS("_%B");	m_out->pD(m_Bnext);		//BLUE
				m_out->pDln();
			}
			break;
		}
		case 20: // ARDUINO AUDIO ILLUMINATOR One
		{
			audioIlluminatorBeatOfVol();
			break;
		}		
		default:	//SWTICH_OFF
		{
			m_state = 0;
		}
	}
}
void RGBController::audioIlluminatorBeatOfVol()
{
	
	if(party_color_red[0])
	{
		if(party_vol_0 > 255)
			m_elements[0]->setR(255, true);
		else
			m_elements[0]->setR(party_vol_0, true);				
	}
	if(party_color_green[0])
	{
		if(party_vol_0 > 255)
			m_elements[0]->setG(255, true);
		else
			m_elements[0]->setG(party_vol_0, true);							
	}
	if(party_color_blue[0])
	{
		if(party_vol_0 > 255)
			m_elements[0]->setB(255, true);
		else
			m_elements[0]->setB(party_vol_0, true);							
	}
	
	
	if(party_color_red[1])
	{
		if(party_vol_0 > 510)
			m_elements[1]->setR(255, true);
		else if(party_vol_0 > 255)
			m_elements[1]->setR(party_vol_0 - 255, true);							
		else
			m_elements[1]->setR(0, true);		
	}
	if(party_color_green[1])
	{
		if(party_vol_0 > 510)
			m_elements[1]->setG(255, true);
		else if(party_vol_0 > 255)
			m_elements[1]->setG(party_vol_0 - 255, true);							
		else
			m_elements[1]->setG(0, true);									
	}
	if(party_color_blue[1])
	{
		if(party_vol_0 > 510)
			m_elements[1]->setB(255, true);
		else if(party_vol_0 > 255)
			m_elements[1]->setB(party_vol_0 - 255, true);							
		else
			m_elements[1]->setB(0, true);		
	}
	
	
	if(party_color_red[2])
	{
		if(party_vol_0 > 765)
			m_elements[2]->setR(255, true);
		else if(party_vol_0 > 510)
			m_elements[2]->setR(party_vol_0 - 510, true);							
		else
			m_elements[2]->setR(0, true);		
	}
	if(party_color_green[2])
	{
		if(party_vol_0 > 765)
			m_elements[2]->setG(255, true);
		else if(party_vol_0 > 510)
			m_elements[2]->setG(party_vol_0 - 510, true);							
		else
			m_elements[2]->setG(0, true);									
	}
	if(party_color_blue[2])
	{
		if(party_vol_0 > 765)
			m_elements[2]->setB(255, true);
		else if(party_vol_0 > 510)
			m_elements[2]->setB(party_vol_0 - 510, true);							
		else
			m_elements[2]->setB(0, true);		
	}
}
void RGBController::update()
{
	if(m_locked)
	{
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
	else
	{
		for(int i =0; i< m_size; i++)
		{
			if(m_elements[i])
			{
				m_elements[i]->update();
			}
		}
	}
}
void RGBController::setR(byte value, bool force)
{
	m_Rnext = value;
	m_sync = force;

	for(int i =0; i< m_size; i++)
	{
		if(m_elements[i])
		{
			m_elements[i]->setR(value,force);
		}
	}
}
void RGBController::setG(byte value, bool force)
{
	m_Gnext = value;
	m_sync = force;

	for(int i =0; i< m_size; i++)
	{
		if(m_elements[i])
		{
			m_elements[i]->setG(value,force);
		}
	}
}	
void RGBController::setB(byte value, bool force)
{
	m_Bnext = value;
	m_sync = force;

	for(int i =0; i< m_size; i++)
	{
		if(m_elements[i])
		{
			m_elements[i]->setB(value,force);
		}
	}
}
void RGBController::setRGB(byte R, byte G, byte B, bool force)
{
	if((m_Ract != R)||(m_Gact != G)||(m_Bact !=B))
	{
		m_Rnext = R;
		m_Gnext = G;
		m_Bnext = B;
		m_play = true;
		m_sync = force;
	}

	for(int i =0; i< m_size; i++)
	{
		if(m_elements[i])
		{
			m_elements[i]->setRGB(R,G,B,force);
		}
	}
}
void RGBController::incR()
{

	if(m_Ract<255)
	{
		m_Rnext=m_Ract+1;
	}
	m_sync = true;

	for(int i =0; i< m_size; i++)
	{
		if(m_elements[i])
		{
			m_elements[i]->incR();
		}
	}
}
void RGBController::incG()
{
	if(m_Gact<255)
	{
		m_Gnext=m_Gact+1;
	}
	m_sync = true;

	for(int i =0; i< m_size; i++)
	{
		if(m_elements[i])
		{
			m_elements[i]->incG();
		}
	}
}
void RGBController::incB()
{
	if(m_Bact<255)
	{
		m_Bnext=m_Bact+1;
	}
	m_sync = true;

	for(int i =0; i< m_size; i++)
	{
		if(m_elements[i])
		{
			m_elements[i]->incB();
		}
	}
}
void RGBController::incRGB()
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

	for(int i =0; i< m_size; i++)
	{
		if(m_elements[i])
		{
			m_elements[i]->incRGB();
		}
	}
}
void RGBController::decR()
{
	if(m_Ract>0)
	{
		m_Rnext=m_Ract-1;
	}
	m_sync = true;
	for(int i =0; i< m_size; i++)
	{
		if(m_elements[i])
		{
			m_elements[i]->decR();
		}
	}
}
void RGBController::decG()
{
	if(m_Gact>0)
	{
		m_Gnext=m_Gact-1;
	}
	m_sync = true;
	for(int i =0; i< m_size; i++)
	{
		if(m_elements[i])
		{
			m_elements[i]->decG();
		}
	}
}
void RGBController::decB()
{
	if(m_Bact>0)
	{
		m_Bnext=m_Bact-1;
	}
	m_sync = true;
	for(int i =0; i< m_size; i++)
	{
		if(m_elements[i])
		{
			m_elements[i]->decB();
		}
	}
}
void RGBController::decRGB()
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
	for(int i =0; i< m_size; i++)
	{
		if(m_elements[i])
		{
			m_elements[i]->decRGB();
		}
	}
}
void RGBController::setPartyVol_0(int vol)
{
	party_vol_0 = vol;	
	m_sync = true;
}
void RGBController::setPartyVol_1(int vol)
{
	party_vol_1 = vol;
	m_sync = true;	
}
void RGBController::setPartyColor(int rec, bool red, bool green, bool blue)	
{
	if(rec > -1 && rec < ELEMENTS)
	{
		party_color_red[rec] = red;
		party_color_green[rec] = green;
		party_color_blue[rec] = blue;
	}
}
void RGBController::setBrightness(byte value)
{
	for(int i =0; i< m_size; i++)
	{
		if(m_elements[i])
		{
			m_elements[i]->setBrightness(value);
		}
	}
}
void RGBController::setFade(bool status)
{

	m_fade = status;
	m_sync	= true;

	for(int i =0; i< m_size; i++)
	{
		if(m_elements[i])
		{
			m_elements[i]->setFade(status);
		}
	}
}
void RGBController::setProfile(byte how)
{

	m_state = how;
	m_sync	= true;
	if (how > 19)
	{
		m_elements[0]->setR(0, true);
		m_elements[1]->setR(0, true);
		m_elements[2]->setR(0, true);

		m_elements[0]->setG(0, true);
		m_elements[1]->setG(0, true);
		m_elements[2]->setG(0, true);

		m_elements[0]->setB(0, true);
		m_elements[1]->setB(0, true);
		m_elements[2]->setB(0, true);
	}
	for(int i =0; i< m_size; i++)
	{
		if(m_elements[i])
		{
			m_elements[i]->setProfile(how);
		}
	}
}
void RGBController::setPlay(bool status)
{
	m_play = status;
	m_sync	= true;

	for(int i =0; i< m_size; i++)
	{
		if(m_elements[i])
		{
			m_elements[i]->setPlay(status);
		}
	}
}
void RGBController::setAutoNext(bool status)
{
	m_autoNext = status;
	m_sync	= true;

	for(int i =0; i< m_size; i++)
	{
		if(m_elements[i])
		{
			m_elements[i]->setAutoNext(status);
		}
	}
}
void RGBController::sync()
{
	for(int i =0; i< m_size; i++)
	{
		if(m_elements[i])
		{
			m_elements[i]->sync();
		}
	}
}
void RGBController::sync(long milSecTime)
{
	for(int i =0; i< m_size; i++)
	{
		if(m_elements[i])
		{
			m_elements[i]->sync(milSecTime);
		}
	}
}
void RGBController::setUpdateTime(long milSecTime)
{
	m_uTime = milSecTime;
	m_nextUpdate = millis()+ m_uTime;

	for(int i =0; i< m_size; i++)
	{
		if(m_elements[i])
		{
			m_elements[i]->setUpdateTime(milSecTime);
		}
	}
}
void RGBController::setWaitTime(long SecTime)
{
	m_wTime = SecTime;
	m_nextWait = millis() + (m_wTime * 1000);

	for(int i =0; i< m_size; i++)
	{
		if(m_elements[i])
		{
			m_elements[i]->setWaitTime(SecTime);
		}
	}
}
void RGBController::setLocked(bool status)
{
	m_locked = status;
}
/*
 *  0 toggle each fade off
 *  1 toggle each fade on
 *  2 toggle all fade off
 *  3 toggle all fade on
 */
void RGBController::toogleState(byte how)
{
	bool fade = true;
	bool on = false;

	if(m_state > 0)
	{
		if(how%2 == 1)
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
		if(how%2 == 1)
		{
			m_state = 3;
		}
		else
		{
			m_state = 2;
		}
	}
	m_sync = true;

	switch (how)
	{
		case 0:
		{
			fade = false;
		}
		case 1:
		{
			for(int i =0; i< m_size; i++)
			{
				if(m_elements[i])
				{
					m_elements[i]->toogleState(fade);
				}
			}
			break;
		}
		case 2:
		{
			fade = false;
		}
		case 3:
		{
			for(int i =0; i < m_size; i++)
			{
				if(m_elements[i])
				{
					if(m_elements[i]->isOn())
					{
						on = true;
					}
				}
			}
			
			if(on)
			{
				for(int x =0; x < m_size; x++)
				{
					if(m_elements[x])
					{
						m_elements[x]->setProfile(fade == true ? 1 : 0);
					}
				}
			}
			else
			{
				for(int x =0; x < m_size; x++)
				{
					if(m_elements[x])
					{
						m_elements[x]->setProfile(fade == true ? 3 : 2);
					}
				}
			}
			break;
		}

	}
}
void RGBController::nextColor()
{
	m_Rnext = m_colors->getNext()->getRed();
	m_Gnext = m_colors->getNext()->getGreen();
	m_Bnext = m_colors->getNext()->getBlue();
}
void RGBController::shuffleColor()
{
	byte shuffle = random (m_colors->getSize());
	m_Rnext = m_colors->getColor(shuffle)->getRed();
	m_Gnext = m_colors->getColor(shuffle)->getGreen();
	m_Bnext = m_colors->getColor(shuffle)->getBlue();
}
void RGBController::randColor()
{
	m_Rnext = random(255);
	m_Gnext = random(255);
	m_Bnext = random(255);
}
void RGBController::partColor()
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
bool RGBController::isOn()
{
	for(int i =0; i< m_size; i++)
	{
		if(m_elements[i])
		{
			if(m_elements[i]->isOn())
			{
				return true;
			}
		}
	}
	return false;
}
bool RGBController::isReady()
{
	if(m_locked)
	{
		m_ready = ((m_Ract == m_Rnext)&&(m_Gact == m_Gnext)&&(m_Bact == m_Bnext));
		return m_ready;
	}
	else
	{
		for(int i =0; i< m_size; i++)
		{
			if(m_elements[i])
			{
				if(!m_elements[i]->isReady())
				{
					return false;
				}
			}
		}
		return true;
	}
}
void RGBController::printStatus()
{
	//COLOR
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

	for(int i =0; i< m_size; i++)
	{
		if(m_elements[i])
		{
			m_elements[i]->printStatus();
		}
	}
}
