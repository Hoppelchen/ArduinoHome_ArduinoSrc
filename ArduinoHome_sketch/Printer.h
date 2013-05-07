/*
 * Printer.h
 *
 *  Created on: 19.10.2012
 *      Author: Hopeless
 */

#ifndef PRINTER_H_
#define PRINTER_H_
#include <EthernetServer.h>
class Printer {
private:
	EthernetServer* 	m_eth;
	bool 				connected;

public:
	Printer(EthernetServer* eth, bool connected) :
		m_eth(eth),
		connected(connected)
	{	}

	void pDS(const String get)
	{
		Serial.print(get);
		m_eth->print(get);
	}
	void pD(int get)
	{
		Serial.print(get);
		if(connected)
			m_eth->print(get,10);
	}
	void pD(bool get)
	{
		Serial.print(get);
		if(connected)
			m_eth->print(get);
	}
	void pDln()
	{
		Serial.println();
		if(connected)
			m_eth->println();
	}
};

#endif /* PRINTER_H_ */
