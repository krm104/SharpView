
#ifndef __PUPIL_ZMQ__
#define __PUPIL_ZMQ__

/*********************************************************************
This code pulls messages from the Pupil Labs ZMQ networking server, 
extracts the pupil diameter and normalized gaze positions, and sends
these values to the Epson Moverio via Bluetooth.

That is all that this program does. This program should be activated
first after the Moverio App is running.
*********************************************************************/
//ZMQ Networking Includes//
#include <zmq.hpp>
#include <zmq_utils.h>

//Standard Includes//
#include <string>
#include <iostream>
#include <time.h>
#include <ctime>

using namespace std;

#define MINCONFIDENCE 0.55

//Class for Storing and Calling ZMQ related communication functions//
class ZMQ {
	public:
		ZMQ()
		{
			context = new zmq::context_t(1);
			socket = new zmq::socket_t(*context, ZMQ_SUB);
			smsg = "";
			strpos = 0;
			sz = 0;
		}

		~ZMQ()
		{	}

		/********************************************************************
		The receiver socket for the ZMQ networking interface is created
		********************************************************************/
		void SetupSocket()
		{
			socket->setsockopt(ZMQ_SUBSCRIBE, "", 0);
		}

		/********************************************************************
		A connection to the ZMQ server established
		********************************************************************/
		void Connect()
		{
			socket->connect ("tcp://127.0.0.1:5000");
		}

		/********************************************************************
		Receive a message from the Pupil ZMQ transmission and store the results
		in the reference parameters
		********************************************************************/
		void getMsg(double& pdiam, double& normx, double& normy, clock_t& mtime)
		{
			//Message Object//
			zmq::message_t reply;

			//Receive Message//
			socket->recv (&reply);

			//Check Message Size//
			if ( reply.size() > 1 )
			{
				//Copy char string into c++ string//
				smsg.clear();
				smsg.append((char*)reply.data());
				
				//Check for confidence//
				strpos = smsg.find("confidence:");
				{
					if ( strpos != string::npos )
					{
						strpos += 11;
						double confidence = std::stod((char*)reply.data()+strpos);
			
						if ( confidence >= MINCONFIDENCE )
						{
							mtime = clock();

							//Check for pupil diameter//
							strpos = smsg.find ("diameter:");
							if ( strpos != string::npos)
							{
								//compensate for 'diameter:'//
								strpos += 9;

								//get diameter value//
								pdiam = std::stod((char*)reply.data()+strpos);
							}

							//Check for normalized screen position//
							strpos = smsg.find ("norm_pos:(");
							if ( strpos != string::npos)
							{
								//compensate for "norm_pos:("//
								strpos += 10;
				
								//get norm_x and norm_y value//
								normx = std::stod((char*)reply.data()+strpos, &sz);
								normy = std::stod((char*)reply.data()+strpos+sz+1);
							}
						}
					}
				}
			}
		}

	private:
		//Message storage array//
		char msg[10000];
		//Message Storage string for searching//
		string smsg;
		//search pos location//
		int strpos;
		//location for end of double conversion//
		std::string::size_type sz; 

		//ZMQ context//
		zmq::context_t* context;
		//ZMQ socket//
		zmq::socket_t* socket;
};

class PUPILZMQ
{
	//////////Public Members//////////
	public:
		//Double value for pupil diameter in pixels//
		double pdiam;
		//Double value for normalized horiz gaze-screen location (percent of screen width)//
		double normx;
		//Double value for normalized vert gaze-screen location (percent of screen height)//
		double normy;
		//Time of last successful message
		clock_t mtime;
	
	//////////Private Members//////////
	private:	
		
		//ZMQ Connection Object//
		ZMQ pzmq;

	//////////Public Methods//////////
	public:
		//Default Consstructor//
		PUPILZMQ() : pdiam(0.0), normx(0.0), normy(0.0), mtime(0)
		{	}

		//Default Destructor//
		~PUPILZMQ()
		{	}

		/********************************************************************
		The receiver socket for the ZMQ networking interface is created
		********************************************************************/
		void SetupSocketConnection()
		{
			pzmq.SetupSocket();
			pzmq.Connect();
		}

		/********************************************************************
		A message is received from the Pupil ZMQ transmission and the results
		stored within the pdiam, normx and normy variables (pupil diamter,
		normalized horizontal, normalized vertical values).
		********************************************************************/
		void processMessage()
		{
			pzmq.getMsg(pdiam, normx, normy, mtime);
		}

	//////////Private Methods//////////
};

#endif //__PUPIL_ZMQ__//