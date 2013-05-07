#define ARDUINO_HOME_VERSION "0.7"

//#define MARKUS
#define KARIM


#define INPUTMAX = 15;
#include <Arduino.h>

#include <SPI.h>
#include <Ethernet.h>

#include "Printer.h"

#include "RGBController.h"
#include "CinemaController.h"

RGBController* Lights;
CinemaController* Cinema;

#ifdef KARIM

	byte mac[] = { 0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x02 };
	IPAddress ip(10,16,0,9);
	IPAddress gateway(10,16,0, 1);
	IPAddress subnet(255, 255, 255, 0);

#endif
#ifdef MARKUS

	byte mac[] = { 0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x03 };
	IPAddress ip(192,168,1, 56);
	IPAddress gateway(192,168,1, 1);
	IPAddress subnet(255, 255, 255, 0);

#endif

bool onToggle = false;

bool lastStateOftrigger_0 = false;
bool lastStateOftrigger_1 = false;
bool lastStateOftrigger_2 = false;

bool connected = false;
bool dataReceived = false;
String inCommingCommand ="";
String actCommand ="";
Printer* out;

// telnet defaults to port 23
EthernetServer server(23);

void pDS(const String get){
	Serial.print(get);
	if(connected)
		server.print(get);
}
void pD(int get){
	Serial.print(get);
	if(connected)
		server.print(get,10);
}
void pD(bool get){
	Serial.print(get);
	if(connected)
		server.print(get);
}
void pDln(){
	Serial.println();
	if(connected)
		server.println();
}
void initHardwareKeys()
{
	/*
	 * HARDWARE ON OFF SWITCH
	 *
	 *	pin 44
	 */
	pinMode(44, INPUT);
	digitalWrite(44, HIGH);
	onToggle = false;
	/*
	 * HARDWARE ON OFF TRIGGER
	 *
	 *	pin 46
	 */	
	pinMode(46, INPUT);
	digitalWrite(46, HIGH);	 
	/*
	 * HARDWARE WHITE TRIGGER
	 *
	 *	pin 47
	 */		 
	pinMode(47, INPUT);
	digitalWrite(47, HIGH);	 
	/*
	 * HARDWARE PARTY TRIGGER
	 *
	 *	pin 48
	 */		 	 
	pinMode(48, INPUT);
	digitalWrite(48, HIGH);	 	 
}
void initPowerSupply()
{
	/*
	 * POWERSUPPLYRELAIS
	 *
	 *	pin 49
	 */	
	pinMode(49, OUTPUT);	
	digitalWrite(49, HIGH);
}
void initNetwork()
{
	pD("Trying to get an IP address using DHCP");
	pDln();

	if (Ethernet.begin(mac) == 0)
	{
		pD("Failed to configure Ethernet using DHCP");
		pDln();
		Ethernet.begin(mac, ip, gateway, subnet);
	}
	pD("My IP address: ");
	pDln();
	ip = Ethernet.localIP();
	for (byte thisByte = 0; thisByte < 4; thisByte++)
	{
		Serial.print(ip[thisByte], DEC);
		Serial.print(".");
	}
	pDln();
	// start listening for clients
	server.begin();
	connected = true;

}
void setup()
{
//	Serial.begin(115200);
	initNetwork();
	for(int i = 22; i < 36; i++)
	{
		pinMode(i, OUTPUT);	
		digitalWrite(i, HIGH);
	}
	initPowerSupply();	
	pD("add controlers");
	pDln();

	out = new Printer(&server, connected);

	Lights = new RGBController(out);
	Cinema = new CinemaController();
	pD("ready :)");
	pDln();

	Lights->setFade(true);
	Lights->setAutoNext(false);
	Lights->setUpdateTime(10);

	initHardwareKeys();

}

int decodeNumber(String number){
	int num = 0;
	if(number == "0" || number == "00" || number == "000")
	{
		return 0;
	}
	int pos = number.length()-1;
	if(pos >= 2)
	{
		num++;
	}
	unsigned int i = 0;
	for (i = 0; i< number.length(); i++)
	{
		num += (number[i]-48)*pow(10,pos--);
	}
	return num;
}
String sInt(int number)
{
	if (number == 0)
	{
		return "0";
	}
	String temp = "";
	String returnvalue = "";
	while (number > 0)
	{
		temp += (number + 48) % 10;
		number /= 10;
	}
	unsigned int i = 0;
	for (i = 0 ; i < temp.length(); i++)
	{
		returnvalue += temp[temp.length()-i-1];
	}
	return returnvalue;
}

void getHardwarInput()
{

	bool stateOftoggle = digitalRead(44);
	bool stateOftrigger_0 = digitalRead(46);
	bool stateOftrigger_1 = digitalRead(47);
	bool stateOftrigger_2 = digitalRead(48);
	
	if(onToggle != stateOftoggle )
	{
		pDS("%_GOT_HW_INPUT_POWERSWITCH");
		pDln();
		onToggle = stateOftoggle;
		if(stateOftoggle)
		{
			Lights->setProfile(3);
		}
		else
		{
			Lights->setProfile(1);			
		}
	}
	if(!lastStateOftrigger_0 && stateOftrigger_0)
	{
		pDS("%_GOT_HW_INPUT_TRIGGER_0");
		pDln();	
		Lights->toogleState(3);	
	}
	if(!lastStateOftrigger_1 && stateOftrigger_1)
	{
		pDS("%_GOT_HW_INPUT_TRIGGER_1");
		pDln();		
		Lights->setProfile(3);	
	}
	if(!lastStateOftrigger_2 && stateOftrigger_2)
	{
		pDS("%_GOT_HW_INPUT_TRIGGER_2");
		pDln();		
		Lights->setProfile(8);	
		Lights->setUpdateTime(3);	
		Lights->setWaitTime(10);
		Lights->setWaitTime(10);
		Lights->setFade(true);
		Lights->setAutoNext(true);
		Lights->setPlay(true);
		Lights->setLocked(false);
	}
	lastStateOftrigger_0 = stateOftrigger_0;
	lastStateOftrigger_1 = stateOftrigger_1;
	lastStateOftrigger_2 = stateOftrigger_2;
}

void getNewOrder()
{
	EthernetClient Client;
	Client = server.available();
	char input;
	if(Client == true)
	{
	dataReceived = true;
		if (Client.connected()) 
		{	
			while(dataReceived && Client.available())
			{
				input = Client.read();
				if((input == ';') || (input == '\n') || (actCommand.length() > 15))
				{
					dataReceived = false;
				}
				if(dataReceived)
				{
					if(((int)input >= 48) &&((int)input <= 57))
					{	//zahlen
						 actCommand += input;
					}
					else if(((int)input >= 97) &&((int)input <= 122)) 
					{	//buchstaben
						actCommand += input;
					}
				}
			}
			if((input == ';')||(input == '\n'))
			{
				switch(actCommand[0])
				{
					case 's':
						Cinema->stop();
						break;
					case 'a':
						switch(actCommand[1])
						{
							case('n'):	//on
								Lights->setProfile(3);
								break;
							case('f'):	//off
								Lights->setProfile(1);
								break;
							case 'i':	//print information
								pDS("ARDUINO HOME INFORMATION");
								pDln();
								pDS("VERSION: ");
								pDS(ARDUINO_HOME_VERSION);
								pDln();
								ip = Ethernet.localIP();
								pDS("IP: ");
								for (byte thisByte = 0; thisByte < 4; thisByte++)
								  {
									pD(ip[thisByte]);
									pDS(".");
								  }  
								break;
							case 'x':	//STOP CINEMA ACTIONS
								Cinema->stop();
								break;
						}
						break;
					case 'c':
						switch(actCommand[1])
						{
							case('b'):	//button
								pDS("BUTTON ");
								pD(decodeNumber(actCommand.substring(2)));
								pDS(" PRESSED");
								pDln();
								Cinema->pressButton(decodeNumber(actCommand.substring(2)));
								break;
							case('u'):	//window up
								pDS("WINDOW UP");
								pD(decodeNumber(actCommand.substring(2)));
								pDln();
								Cinema->windowUp(decodeNumber(actCommand.substring(2)));
								break;
							case('d'):	//window down
								pDS("WINDOW DOWN");
								pD(decodeNumber(actCommand.substring(2)));
								pDln();
								Cinema->windowDw(decodeNumber(actCommand.substring(2)));
								break;
							case('r'):	//canvas up
								pDS("CANVAS UP");
								pDln();
								Cinema->canvasUp();
								break;
							case('f'):	//canvas down
								pDS("CANVAS DOWN");
								pDln();
								Cinema->canvasDw();
								break;
						}
						break;
					case 'l':
						switch(actCommand[1])
						{
							case('a'):
								Lights->setAutoNext(decodeNumber(actCommand.substring(2))!=0);
								pDS("L AUTONEXT ");
								pD(decodeNumber(actCommand.substring(2))!=0);
								pDln();
								break;
							case('s'):	//speed
								Lights->setUpdateTime(decodeNumber(actCommand.substring(2)));
								pDS("L UPDATE TIME ");
								pD(decodeNumber(actCommand.substring(2)));
								pDln();
								break;
							case('w'):	//wait
								Lights->setWaitTime(decodeNumber(actCommand.substring(2)));
								pDS("L WAIT TIME ");
								pD(decodeNumber(actCommand.substring(2)));
								pDln();
								break;
							case('f'):	//fade
								Lights->setFade(decodeNumber(actCommand.substring(2))!=0);
								pDS("L FADE ");
								pD(decodeNumber(actCommand.substring(2))!=0);
								pDln();
								break;
							case('p'):	//play
								Lights->setPlay(decodeNumber(actCommand.substring(2))!=0);
								pDS("L PLAY ");
								pD(decodeNumber(actCommand.substring(2))!=0);
								pDln();
								break;
							case('m'):	//profile(mode)
								Lights->setProfile(decodeNumber(actCommand.substring(2)));
								pDS("L PROFILE ");
								pD(decodeNumber(actCommand.substring(2)));
								pDln();
								break;
							case('r'):	//red
								Lights->setR(decodeNumber(actCommand.substring(2)), false);
								pDS("L RED ");
								pD(decodeNumber(actCommand.substring(2)));
								pDln();
								break;
							case('g'):	//green
								Lights->setG(decodeNumber(actCommand.substring(2)), false);
								pDS("L GREEN ");
								pD(decodeNumber(actCommand.substring(2)));
								pDln();
								break;
							case('b'):	//blue
								Lights->setB(decodeNumber(actCommand.substring(2)), false);
								pDS("L BLUE ");
								pD(decodeNumber(actCommand.substring(2)));
								pDln();
								break;
							case('i'):
								Lights->setBrightness(decodeNumber(actCommand.substring(2)));
								pDS("L INTENSE ");
								pD(decodeNumber(actCommand.substring(2)));
								pDln();
								break;
							case('u'):	//forced update/sync
								Lights->sync();
								pDS("L FORCED");
								pDln();
								break;
							case('t'):	//toggle
								pDS("L TOGGLED");
								Lights->toogleState(decodeNumber(actCommand.substring(2)));
								pDln();
								break;
							case('l'):
								pDS("L LOCKED");
								Lights->setLocked(decodeNumber(actCommand.substring(2))!=0);
								pDln();
								break;
							case('d'):
								Lights->printStatus();
								break;
						}
						break;
					case 'e':
						switch(actCommand[2])
						{
							case('a'):
								Lights->getElement(decodeNumber(actCommand.substring(1,2)))->setAutoNext(decodeNumber(actCommand.substring(3))!=0);
								pDS("L AUTONEXT ");
								pD(decodeNumber(actCommand.substring(3))!=0);
								pDS(" ELMENT ");
								pD(decodeNumber(actCommand.substring(1,2)));
								pDln();
								break;
							case('s'):	//speed
								Lights->getElement(decodeNumber(actCommand.substring(1,2)))->setUpdateTime(decodeNumber(actCommand.substring(3)));
								pDS("L UPDATE TIME ");
								pD(decodeNumber(actCommand.substring(3)));
								pDS(" ELMENT ");
								pD(decodeNumber(actCommand.substring(1,2)));
								pDln();
								break;
							case('w'):	//wait
								Lights->getElement(decodeNumber(actCommand.substring(1,2)))->setWaitTime(decodeNumber(actCommand.substring(3)));
								pDS("L WAIT TIME ");
								pD(decodeNumber(actCommand.substring(3)));
								pDS(" ELMENT ");
								pD(decodeNumber(actCommand.substring(1,2)));
								pDln();
								break;
							case('f'):	//fade
								Lights->getElement(decodeNumber(actCommand.substring(1,2)))->setFade(decodeNumber(actCommand.substring(3))!=0);
								pDS("L FADE ");
								pD(decodeNumber(actCommand.substring(3))!=0);
								pDS(" ELMENT ");
								pD(decodeNumber(actCommand.substring(1,2)));
								pDln();
								break;
							case('p'):	//play
								Lights->getElement(decodeNumber(actCommand.substring(1,2)))->setPlay(decodeNumber(actCommand.substring(3))!=0);
								pDS("L PLAY ");
								pD(decodeNumber(actCommand.substring(3))!=0);
								pDS(" ELMENT ");
								pD(decodeNumber(actCommand.substring(1,2)));
								pDln();
								break;
							case('m'):	//profile(mode)
								Lights->getElement(decodeNumber(actCommand.substring(1,2)))->setProfile(decodeNumber(actCommand.substring(3)));
								pDS("L PROFILE ");
								pD(decodeNumber(actCommand.substring(3)));
								pDS(" ELMENT ");
								pD(decodeNumber(actCommand.substring(1,2)));
								pDln();
								break;
							case('r'):	//red
								Lights->getElement(decodeNumber(actCommand.substring(1,2)))->setR(decodeNumber(actCommand.substring(3)), false);
								pDS("L RED ");
								pD(decodeNumber(actCommand.substring(3)));
								pDS(" ELMENT ");
								pD(decodeNumber(actCommand.substring(1,2)));
								pDln();
								break;
							case('g'):	//green
								Lights->getElement(decodeNumber(actCommand.substring(1,2)))->setG(decodeNumber(actCommand.substring(3)), false);
								pDS("L GREEN ");
								pD(decodeNumber(actCommand.substring(3)));
								pDS(" ELMENT ");
								pD(decodeNumber(actCommand.substring(1,2)));
								pDln();
								break;
							case('b'):	//blue
								Lights->getElement(decodeNumber(actCommand.substring(1,2)))->setB(decodeNumber(actCommand.substring(3)), false);
								pDS("L BLUE ");
								pD(decodeNumber(actCommand.substring(3)));
								pDS(" ELMENT ");
								pD(decodeNumber(actCommand.substring(1,2)));
								pDln();
								break;
							case('i'):	//intense
								Lights->getElement(decodeNumber(actCommand.substring(1,2)))->setBrightness(decodeNumber(actCommand.substring(3)));
								pDS("L INTENSE ");
								pD(decodeNumber(actCommand.substring(3)));
								pDS(" ELMENT ");
								pD(decodeNumber(actCommand.substring(1,2)));
								pDln();
								break;
							case('u'):	//forced update/sync
								Lights->getElement(decodeNumber(actCommand.substring(1,2)))->sync();
								pDS("L FORCED");
								pDS(" ELMENT ");
								pD(decodeNumber(actCommand.substring(1,2)));
								pDln();
								break;
							case('t'):	//turn
								pDS("L TOGGLED");
								pDS(" ELMENT ");
								pD(decodeNumber(actCommand.substring(1,2)));
								if(actCommand[3] == '0')
								{
									Lights->getElement(decodeNumber(actCommand.substring(1,2)))->toogleState(false);
									pDS(" HARD");
								}
								else
								{
									Lights->getElement(decodeNumber(actCommand.substring(1,2)))->toogleState(true);
									pDS(" FADE");
								}
								pDln();
								break;
							case('c'): //colorTable
								{
									pDS("L ColorTable");
									pDS(" ELMENT ");
									pD(decodeNumber(actCommand.substring(1,2)));
									if(actCommand[3] == 'e') // erase all
									{
										pDS("ERASE COLOR TABLE");
										pDln();
										Lights->getElement(decodeNumber(actCommand.substring(1,2)))->getColorTable()->eraseTable();
									}
									else if(actCommand[3] == 'a')// add
									{
										pDS(" ADD COLOR R ");
										pD(decodeNumber(actCommand.substring(4,7)));
										pDS(" G ");
										pD(decodeNumber(actCommand.substring(7,10)));
										pDS(" B ");
										pD(decodeNumber(actCommand.substring(10,13)));
										pDS(" TO COLOR TABLE ");
										Lights->getElement(decodeNumber(actCommand.substring(1,2)))->getColorTable()->addNewColor(
												decodeNumber(actCommand.substring(4,7)),
												decodeNumber(actCommand.substring(7,10)),
												decodeNumber(actCommand.substring(10,13))
												);
									}
									pDln();
									break;
								}
							case('d'):
								Lights->getElement(decodeNumber(actCommand.substring(1,2)))->printStatus();
								break;
							}
						break;
					case 'p':
						switch(actCommand[1])
						{
							case('l'):	//audioLeftVolume
								Lights->setPartyVol_0(decodeNumber(actCommand.substring(2)));
							break;
							case('r'):	//audioRightVolume
								Lights->setPartyVol_1(decodeNumber(actCommand.substring(2)));
							break;
							case('c'):	//setPartyColor								
                                Lights->setPartyColor(decodeNumber(actCommand.substring(2,3)),decodeNumber(actCommand.substring(3,4)) == 1,decodeNumber(actCommand.substring(4,5)) == 1, decodeNumber(actCommand.substring(5,6)) == 1);
							break;
						}
						break;
					default:
					{
						Client.println("_%%E");// + actCommand + "%");
						actCommand = "";
					}
				}
				if(actCommand.length() > 0)
				{
					Client.println("_%%A"); // + actCommand + "%");
					actCommand = "";
				}
			}
		}
	}
}
void updatePowerSupply()
{
	bool status = false;
	/*
	*	relais
	*/
	for(int i = 22; i < 36; i++)
	{
		if(!digitalRead(i))
		{
			status = true;	
		}
	}		
	/*
	*	PWM
	*/
	if(Lights->isOn())
	{
		status = true;
	}
			
	if(status)
	{
		digitalWrite(49, LOW);		
	}
	else
	{
		digitalWrite(49, HIGH);		
	}	
}
void loop()
{
	getHardwarInput();
	getNewOrder();
	updatePowerSupply();
	Lights->update();
	Cinema->update();
}
