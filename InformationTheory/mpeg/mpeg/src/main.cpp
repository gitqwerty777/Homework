#include <cstdio>
#include <bit_buf.h>
#include <sequence_header.h>
#include <group_of_pictures.h>
#include <picture.h>
#include <slice.h>
#include <macroblock.h>
#include <util.h>
#include <random>
#include <vlc.h>
#include <unistd.h>
#include <pthread.h>
#include <mutex>
#include <queue>

#ifdef __APPLE__
#include <OpenGL/gl.h> 
#include <OpenGL/glu.h> 
#include <GLUT/glut.h>
#elif __FreeBSD__ || __linux
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#elif _WIN32
#define GLUT_DISABLE_ATEXIT_HACK
#include <GL\glut.h>
#include <windows.h>
#endif
mutex gg;
volatile int v,h;
queue<uint8_t*> image_buffer;
int timer = 25;
void SystemTimer(int _value){
	glutPostRedisplay();
	glutTimerFunc(timer, SystemTimer, 1);
	return;
}
void Display(){
	glClear(GL_COLOR_BUFFER_BIT);
	uint8_t* ndata;
	{
		lock_guard<mutex> lock(gg);
		if(image_buffer.size()>=1)
		{
			ndata = image_buffer.front();
			glDrawPixels(h,v,GL_BGR,GL_UNSIGNED_BYTE,ndata);
			glutSwapBuffers();
		}
	}
	{
		lock_guard<mutex> lock(gg);
		if(image_buffer.size()>1){
			image_buffer.pop();
			delete[] ndata;
		}
	}
	return;
}

bit_buf* bb;
int slice_num=0;
int macro_num=0;
int picture_num=0;
void picture2buffer(picture& p){
	uint8_t* buf = new uint8_t[p.width*p.height*3];
	memset(buf,0,v*h*3);
	for(int i=0;i<v;i++){
		for(int j=0;j<h;j++){
			for(int c=2;c>=0;c--){
				buf[3*((v-1-i)*h+j)+(2-c)] = p.img[c][i][j];
			}
		}
	}
	while(true){
		lock_guard<mutex> lock(gg);
		if(image_buffer.size()<20){
			break;
		} else{
			//puts("queue full!! so sleep.....");
			usleep(50000);
		}
	}
	lock_guard<mutex> lock(gg);
	image_buffer.push(buf);
	//printf("image_buffer size:%lu\n",image_buffer.size());
}
void read_gop(sequence_header* sh){
	group_of_pictures gop(bb);
	gop.read();
	do{
		picture p(bb,sh->mb_width*16,sh->mb_height*16);
		p.read();
		do{
			slice sl(bb,sh);
			sl.read();
			do{
				macroblock m(bb,sh,&sl,&p,p.picture_coding_type-1);
				m.read();
			}	while(bb->nextbits(23) != 0);
			bb->next_start_code();
			slice_num++;
		} while(slice_start_code_check(bb->nextbits()));
		if(p.picture_coding_type == 1 || p.picture_coding_type == 2){
			if(sh->picture_ref0!= nullptr){
				delete (sh->picture_ref0);
			}
			if(sh->picture_ref1!= nullptr){
				sh->picture_ref0 = new picture(*(sh->picture_ref1));
			}
			sh->picture_ref1 = new picture(p);
			if(sh->picture_ref0 != nullptr){
				picture show(*(sh->picture_ref0));
				show.final_picture(h, v);
				picture2buffer(show);
			}
		}
		else{
			p.final_picture(sh->horizontal_size, sh->vertical_size);
			picture2buffer(p);
		}
	}while(bb->nextbits() == PICTURE_START_CODE);
}
void *video_sequence(void* input){
	bb->next_start_code();
	sequence_header sh(bb);
	do{
		sh.read();
		if(h == 0 && v ==	0){
			h = sh.horizontal_size, v = sh.vertical_size;
			uint8_t* buf = new uint8_t[h*v*3];
			lock_guard<mutex> lock(gg);
			image_buffer.push(buf);
		}
		do{
			read_gop(&sh);
			puts("read gop done");
		} while(bb->nextbits() == GROUP_START_CODE);	
	} while(bb->nextbits() == SEQUENCE_HEADER_CODE);
	puts("end");
	pthread_exit(NULL);
};
int main(int argc,char* argv[]){
	if(argc != 2){
		puts("input file name !!");
		return 0;
	}
	FILE* mpeg_file = fopen(argv[1],"r");

	bb= new bit_buf(mpeg_file);
	init_vlcs();
	init_idct();
	
	pthread_t thread;
	pthread_create(&thread,NULL,video_sequence,(void*)0);
		
	while(true){
		if(v>0 && h>0){
			glutInit(&argc, argv);
			glutInitWindowSize(h, v);
			glutCreateWindow("MPEG Player");
			glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_ALPHA); 
			glutDisplayFunc(Display);

			glutTimerFunc(timer, SystemTimer, 1);

			glutMainLoop();
			break;
		}
	}

	return 0;
}
