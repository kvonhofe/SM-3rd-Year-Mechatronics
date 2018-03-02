#include <zmq.hpp>  //provides a message queue
#include <zmq.h>
#include <Windows.h>
#include <GL.h>
#include <GLU.h>
#include <glut.h>
#include <iostream>
#include <stdio.h>
#include <cstdio>
#include <conio.h>
#include <turbojpeg.h>
#include <SMObject.h>
#include <SMStructs.h>

using namespace std;

// function prototypes 
void display();
void idle();

GLuint tex;  //created for textures
zmq::context_t context(1);
zmq::socket_t subscriber(context, ZMQ_SUB);

int a;
ProcessManagement *PM;

int main(int argc, char ** argv) {
	// initialise the window
	const int WINDOW_WIDTH = 800;
	const int WINDOW_HEIGHT = 600;
	glutInit(&argc, (char**)(argv));
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("UGV Camera");
	glutDisplayFunc(display);
	glutIdleFunc(idle);

	// Generate texture object
	glGenTextures(1, &tex);

	// Connect to server  
	subscriber.connect("tcp://192.168.1.200:26000");

	// send empty filter --> needs to be done in order to recieve data after connection
	subscriber.setsockopt(ZMQ_SUBSCRIBE, "", 0);

	//Request access to memory block
	SMObject PMObj(TEXT("Process Management"), sizeof(ProcessManagement));
	PMObj.SMAccess();
	PM = (ProcessManagement*)PMObj.pData;
	glutMainLoop();

	return 0;
}



void display() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, tex);

	glBegin(GL_QUADS);
	glTexCoord2f(0, 1); glVertex2f(-1, -1);
	glTexCoord2f(1, 1); glVertex2f(1, -1);
	glTexCoord2f(1, 0); glVertex2f(1, 1);
	glTexCoord2f(0, 0);  glVertex2f(-1, 1);
	glEnd();

	glutSwapBuffers(); // double buffer system: all drawing operations are stored in RAM i.e. the 'back buffer'
}



void idle() {
	//process management connections 
	PM->HeartBeat.Flags.Camera = 1;
	Sleep(50);
	if (PM->Shutdown.Flags.Camera == 1) exit(1);

	if (_kbhit()) exit(1);

	// Recieve from ZMQ - zero message queue, socket communication (endpoint of a two way communication link)
	zmq::message_t update;


	if (subscriber.recv(&update, ZMQ_NOBLOCK)) {
		cout << "Recieved data is: " << update.size() << endl;

		// Do JPEG decompression for a single frame
		long unsigned int _jpegSize = update.size(); //!< _jpegSize from above
		unsigned char* _compressedImage = static_cast<unsigned char*>(update.data()); //!< _compressedImage from above

		int jpegSubsamp, width, height;

		tjhandle _jpegDecompressor = tjInitDecompress();
		tjDecompressHeader2(_jpegDecompressor, _compressedImage, _jpegSize, &width, &height, &jpegSubsamp);
		unsigned char * buffer = new unsigned char[width*height * 3]; //!< will contain the decompressed image
		tjDecompress2(_jpegDecompressor, _compressedImage, _jpegSize, buffer, width, 0/*pitch*/, height, TJPF_RGB, TJFLAG_FASTDCT);
		tjDestroy(_jpegDecompressor);


		// Load texture
		glBindTexture(GL_TEXTURE_2D, tex); //Since images are 2D arrays of pixels, it will be bound to the GL_TEXTURE_2D target.

		// Wrapping
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		// Filtering
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, buffer);
		delete[] buffer; //buffer was used to temporarily store the data
	}
	else {
		cout << "No data" << endl;
	}

	display();
}