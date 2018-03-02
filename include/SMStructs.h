#ifndef SMSTRUCTS_H
#define SMSTRUCTS_H

struct GPS {
	double Northing;  //8 bytes 
	double Easting;   //8 bytes 
	double Height;    //8 bytes 
	bool CorrectCRC;
};


struct Laser {
	double x[1100];
	double y[1100];
	int NumPoints;
	double LaserTimeStamp;
};


struct Plotting {
	double X;
	double Y;
	double Heading;
	double Steering;
	double PlotTimeStamp;
};

struct OpenGL {
	double OpenGLTimeStamp;
};


struct UnitFlags {
	unsigned short PM : 1,  //PM is the least significant bit 
		Laser : 1,
		OpenGlView : 1,
		Remote : 1,
		UGV : 1,
		Camera : 1,
		GPS : 1,
		Plot : 1,
		Simulator : 1,
		Unused : 7;
};

union ExecFlags {
	UnitFlags Flags;        // can refer to each bit separatley
	unsigned short Status;  //refers to the entire 16 bits ^^
};


struct ProcessManagement {
	ExecFlags HeartBeat;
	ExecFlags Shutdown;
	double PMTimeStamp;
};


struct Remote {
	double SetSteering;
	double SetSpeed;
	unsigned char UGV_SIM; //choose UGV or sim
	int Terminate;
	bool Connected;
	double RemoteTimeStamp;
};


struct Simulator {
	double X;
	double Y;
	double Heading;
	double SimTimeStamp;
};


struct UGV {
	double X;
	double Y;
	double Heading;
	double UGVTimeStamp;
};


struct Cam {
	double Heartbeat;
	double CamTimeStamp;
};


#endif 


