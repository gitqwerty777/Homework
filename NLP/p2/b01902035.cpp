#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <fcntl.h>
#include <errno.h>
#include <netdb.h>
#include <iconv.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>


void show_p2(int );
void Event_Sche(int socket_fd, FILE *fp);
int  Next_Tag(int socket_fd, FILE *fp);
int  Compare_Tag(char *tag);
void BackToMain(int socket_fd);
void toMail(int socket_fd, FILE *fp);
void toMessage(int socket_fd, FILE *fp);
void toBOARD(int socket_fd, FILE* fp);
void write_content(int socket_fd, FILE *fp);
void logout(int socket_fd);
void login(int socket_fd, FILE *fp);
void creat_bind_socket(struct sockaddr_in &srv, int &socket_fd);
void gethost(struct hostent *hp, struct sockaddr_in &srv);
void UTF8toBIG5(char inbuf[], int socket_fd);


void UTF8toBIG5(char inbuf[], int socket_fd)
{
	iconv_t cd;
	size_t in_size, out_size;
	cd = iconv_open("BIG-5", "UTF8");
	char *inptr;
	char *outbuf = NULL, *outptr;
	in_size = strlen(inbuf);
	inptr = inbuf;
	
	outbuf = (char*)malloc(in_size*3);
	out_size = in_size *3;
	outptr = outbuf;
	if( cd == (iconv_t )-1 ){
		perror( "iconv");
		exit(1);
	}
	
	if(iconv(cd, &inptr, &in_size, &outptr, &out_size) == -1){
		free(outbuf);
		write(socket_fd, inbuf, strlen(inbuf));
	}
	iconv_close(cd);
	*outptr = '\0';
	write(socket_fd, outbuf, strlen(outbuf));
	free(outbuf);
}


void gethost(struct hostent *hp, struct sockaddr_in &srv)
{
	 const char* ptt2 = "ptt2.cc";
        srv.sin_family = AF_INET;
        if((hp = gethostbyname(ptt2))== NULL){
                perror("gethostname");
		exit(1);
        }

        srv.sin_addr.s_addr = ((struct in_addr*)(hp->h_addr))->s_addr;

	//printf("Translate %s => %s\n", ptt2, inet_ntoa(srv.sin_addr));	
	return;
}

void creat_bind_socket(struct sockaddr_in &srv, int &socket_fd)
{
	socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(socket_fd < 0){
		perror("socket"); exit(1);
	}
	fprintf(stderr, "socket fd = %d\n", socket_fd);
	srv.sin_port = htons(23);	
	if(connect(socket_fd, (struct sockaddr*) &srv, sizeof(srv)) < 0){
		perror("connect"); exit(1);
	}
	//登入畫面
	show_p2(socket_fd);
	// end
}

void show_p2(int socket_fd)
{
        char buf[64];
        bzero(buf, 64);
        int len = 0;

        do{
                len = read(socket_fd, buf, sizeof(buf));
                fprintf(stderr, "%s", buf);
        }while(len >= 64);
	sleep(1);
}


void login(int socket_fd, FILE *fp)
{
	int len;
	char enter[] = "\r\n";
        int rtn;
	
	char IDsT[] = "<ID>";
	char IDeT[] = "</ID>";
	char ID[128];
	memset(ID, '\0', 128);
	
	fgets(ID, sizeof(ID), fp);
	if(ID[strlen(ID)-1] == '\n') ID[strlen(ID)-1] = '\0';
	len = strlen(ID);
	int IDs = strlen(IDsT);
	int IDe = len - IDs - strlen(IDeT)-1;
	
	for(int i=0; i< IDe; i++){
		ID[i] = ID[IDs+i];
	}ID[IDe] = '\0';
	//fprintf(stderr, "ID = %s\n", ID);
	
	char PWsT[] = "<PASS>";
	char PWeT[] = "</PASS>";
	char PW[128];
	memset(PW, '\0', 128);
	
	fgets(PW, sizeof(PW), fp);
	if(PW[strlen(PW)-1] == '\n') PW[strlen(PW)-1] = '\0';
	len = strlen(PW);
	int PWs = strlen(PWsT);
	int PWe = len - PWs - strlen(PWeT)-1;
	
	for(int i=0; i< PWe; i++){
		PW[i] = PW[PWs+i];
	}PW[PWe] = '\0';
	//fprintf(stderr, "PASSWORD = %s\n", PW);
	

	/*input ID enter*/
	rtn = write(socket_fd, ID, strlen(ID));	
	fprintf(stderr, "%s", ID);
	rtn = write(socket_fd, enter, strlen(enter));
	fprintf(stderr, "%s", enter);
	show_p2(socket_fd);
	
	/*input password enter*/
	rtn = write(socket_fd, PW, strlen(PW));
//	fprintf(stderr, "%s", PW);
	show_p2(socket_fd);
	rtn = write(socket_fd, enter, strlen(enter));
	//fprintf(stderr,"%s", enter);
	show_p2(socket_fd);

	/*check if multi connection*/
	
	char y[] = "y";
	char q[] = "q";
	char e[] = "e";
	UTF8toBIG5(y, socket_fd); //multiconnect
	UTF8toBIG5(q, socket_fd); //po wen
	write(socket_fd, enter, strlen(enter));
	UTF8toBIG5(e, socket_fd);
}

void logout(int socket_fd)
{
	char out[] = "eeeeeeeeg";
	char enter[] = "\r\n";
	char yes[] = "y";
	int rtn;
	rtn = write(socket_fd, out, sizeof(out));
	rtn = write(socket_fd, enter, sizeof(enter));
	rtn = write(socket_fd, yes, sizeof(yes));
	rtn = write(socket_fd, enter, sizeof(enter));
	rtn = write(socket_fd, enter, sizeof(enter));
}

void write_content(int socket_fd, FILE *fp)
{
	char CONetag[] = "</CONTENT>";
	char CONTENT[256];
	memset(CONTENT, '\0', 256);
	char enter[] = "\r\n";
	bool loop = true;
	int count;
	int rtn;
int lo = 0;
 	do{
	int back = 0;
                fgets(CONTENT, sizeof(CONTENT), fp);
		//if(CONTENT[strlen(CONTENT)-1]=='\n'){
		//	 CONTENT[strlen(CONTENT)-1] = '\0';
		//	back = 1;		
		//}
		/*
		//fprintf(stderr, "erw (%c)",CONTENT[strlen(CONTENT)-1]);
                if(CONTENT[strlen(CONTENT)-1] == '>'&& strlen(CONTENT)>11){
                        count = 0;
						
                        for(int i = 0; i< 10; i++){
				//fprintf(stderr, "ch(%c ,%c)", CONTENT[strlen(CONTENT)-1-i],CONetag[strlen(CONetag)-1] );
                                if(CONTENT[strlen(CONTENT)-1-i] == CONetag[strlen(CONetag)-1-i]){
                                        count++;
                                }else{
                                        i==10;
                                }
                        }
                        if(count==10){
                                int CONTENTe = strlen(CONTENT) - strlen(CONetag);
                                CONTENT[CONTENTe] = '\0';
                                loop = false;
                        }
                }
		*/
		
	int l = 0;
int ts = 0;
int te = 0;
int cc = 0;

if(l<strlen(CONTENT)-9){
while(cc!=3&&(l+9)<strlen(CONTENT)){
        if(CONTENT[l]=='<' && CONTENT[l+1]=='/' && CONTENT[l+2]=='C'&&CONTENT[l+3]=='O'&&CONTENT[l+4]=='N'&&CONTENT[l+5]=='T'&&CONTENT[l+6]=='E'&&CONTENT[l+7]=='N'&&CONTENT[l+8]=='T'&&CONTENT[l+9]=='>'){
                                cc = 3;
                                ts = l;
                                break;
                        }
                        l++;
        }
        if(cc==3){ CONTENT[l] = '\0'; loop =false;}
}

                fprintf(stderr, "%s", CONTENT);
		UTF8toBIG5(CONTENT, socket_fd);
		//rtn = write(socket_fd, CONTENT, strlen(CONTENT));
		rtn = write(socket_fd, enter, strlen(enter));
		sleep(2);

lo=cc;
fprintf(stderr, "loop = %d\n", cc);
        }while(loop&&lo!=3);	
}

void toBOARD(int socket_fd, FILE* fp)
{
	char BDeT[] = "</BOARD>"; //eT: end Tag
	char BOARD[128];
	memset(BOARD, '\0', 128);
	fgets(BOARD, sizeof(BOARD), fp);
	if(BOARD[strlen(BOARD)-1] == '\n') BOARD[strlen(BOARD)-1] = '\0';

	int len = strlen(BOARD);
	int BDe = len - strlen(BDeT);
	BOARD[BDe] = '\0';
	fprintf(stderr, "BOARD: %s\n", BOARD);
	
	char s[] = "s";	
	char enter[] = "\r\n";
	int rtn = write(socket_fd, s, strlen(s)); 
	show_p2(socket_fd);
	sleep(1);
	rtn = write(socket_fd, BOARD, strlen(BOARD));
	show_p2(socket_fd);
	sleep(2);
	rtn = write(socket_fd, enter, strlen(enter));
	sleep(2);
	show_p2(socket_fd);
	
	int nexttag = Next_Tag(socket_fd, fp);
	if(nexttag==3){
		char ctrlP[2]; ctrlP[0] = 16; ctrlP[1] = '\0';
		char ctrlX[2]; ctrlX[0] = 24; ctrlX[1] = '\0';
		char s[] = "s";
		char TITLEtag[] = "</P>";	
		char TITLE[128];
		memset(TITLE, '\0', 128);
		fgets(TITLE, sizeof(TITLE), fp);
		if(TITLE[strlen(TITLE)-1] == '\n') TITLE[strlen(TITLE)-1] = '\0';
		len = strlen(TITLE);
		int TITLEe = len - strlen(TITLEtag)-1;
		TITLE[TITLEe] = '\0';
		fprintf(stderr, "title: %s\n", TITLE);
		
		rtn = write(socket_fd, ctrlP, strlen(ctrlP));
		show_p2(socket_fd);
		rtn = write(socket_fd, enter, strlen(enter));
		show_p2(socket_fd);
		//rtn = write(socket_fd, TITLE, strlen(TITLE));
		UTF8toBIG5(TITLE, socket_fd);
		show_p2(socket_fd);
		rtn = write(socket_fd, enter, strlen(enter));
		show_p2(socket_fd);
		sleep(1);
		//next step
		//content, ^x, s, enter
		if((nexttag = Next_Tag(socket_fd, fp))==7){
			write_content(socket_fd,fp);
			show_p2(socket_fd);
			sleep(2);
			rtn = write(socket_fd, ctrlX, strlen(ctrlX));
			show_p2(socket_fd);
			sleep(3);
			rtn = write(socket_fd, s, strlen(s));
			rtn = write(socket_fd, enter, strlen(enter));
		}
	}
}


void toMessage(int socket_fd, FILE *fp)
{
	char M_USRIDeTag[] = "</W>";
	char t[] = "t";
	char enter[] = "\r\n";
	char u[] = "u";
	char w[] = "w";
	char s[] = "s";
	char y[] = "Y";
	char usrID[64];
	memset(usrID, '\0', 64);

	fgets(usrID, sizeof(usrID), fp);
	if(usrID[strlen(usrID)-1]=='\n') usrID[strlen(usrID)-1] = '\0';
	int len = strlen(usrID);
	int usrIDe = len - strlen(M_USRIDeTag)-1;
	usrID[usrIDe] = '\0'; 
	sleep(2);
	int nexttag = Next_Tag(socket_fd, fp);
	if(nexttag==7){
		int rtn = write(socket_fd, t, strlen(t));
		rtn = write(socket_fd, enter, strlen(enter));
		show_p2(socket_fd);
		rtn = write(socket_fd, u, strlen(u));
		rtn = write(socket_fd, enter, strlen(enter));
		show_p2(socket_fd);
		rtn = write(socket_fd, s, strlen(s));
		show_p2(socket_fd);
		rtn = write(socket_fd, usrID, strlen(usrID));
		show_p2(socket_fd);
		rtn = write(socket_fd, enter, strlen(enter));
		rtn = write(socket_fd, w, strlen(w));
		show_p2(socket_fd);
		write_content(socket_fd, fp);
		rtn = write(socket_fd, enter, strlen(enter));
		show_p2(socket_fd);
		rtn = write(socket_fd, y, strlen(y));
//fprintf(stderr, "==ffffff===========end message\n");
		//show_p2(socket_fd);
		rtn = write(socket_fd, enter, strlen(enter));
//fprintf(stderr, "==ffffff===========end message\n");
		//show_p2(socket_fd);
	}
//	fprintf(stderr, "==ffffff===========end message\n");
}

void toMail(int socket_fd, FILE *fp){
	char m[] = "m";
	char enter[] = "\r\n";
	char s[] = "s";
	char ctrlX[2]; ctrlX[0] = 24; ctrlX[1] = '\0';
	char y[] = "y";
	char n[] = "n";
	char IDeTag[] = "</ID>";
	char TITLEeTag[] = "</TITLE>";
	
	char usrID[64];
	char TITLE[128];
	memset(usrID, '\0', 64);
	memset(TITLE, '\0', 128);
	
	fgets(usrID, sizeof(usrID), fp);
	 if(usrID[strlen(usrID)-1]=='\n') usrID[strlen(usrID)-1] = '\0';
        int len = strlen(usrID);
        int usrIDe = len - strlen(IDeTag);
        usrID[usrIDe] = '\0';	
	
	int nexttag = Next_Tag(socket_fd, fp);
	
	if(nexttag==6){	
		fgets(TITLE, sizeof(TITLE), fp);
        	 if(TITLE[strlen(TITLE)-1]=='\n') TITLE[strlen(TITLE)-1] = '\0';
	         len = strlen(TITLE);
	        int TITLEe = len - strlen(TITLEeTag);
	        TITLE[TITLEe] = '\0';
		
		 int nexttag = Next_Tag(socket_fd, fp);
	        if(nexttag==7){
                int rtn = write(socket_fd, m, strlen(m));
                rtn = write(socket_fd, enter, strlen(enter));
                show_p2(socket_fd);
		rtn = write(socket_fd, s, strlen(s));
                rtn = write(socket_fd, enter, strlen(enter));
                show_p2(socket_fd);
		rtn = write(socket_fd, usrID, strlen(usrID));
		show_p2(socket_fd);
                rtn = write(socket_fd, enter, strlen(enter));
                //rtn = write(socket_fd, TITLE, strlen(TITLE));
		UTF8toBIG5(TITLE, socket_fd);
		show_p2(socket_fd);
		rtn = write(socket_fd, enter, strlen(enter));
                write_content(socket_fd, fp);
                show_p2(socket_fd);
		rtn = write(socket_fd, ctrlX, strlen(ctrlX));
                rtn = write(socket_fd, s, strlen(s));
                show_p2(socket_fd);
		rtn = write(socket_fd, enter, strlen(enter));
		rtn = write(socket_fd, n, strlen(n));
                show_p2(socket_fd);
		rtn = write(socket_fd, enter, strlen(enter));
		show_p2(socket_fd);
		rtn = write(socket_fd, enter, strlen(enter));
       		}
 	}	
}




void BackToMain(int socket_fd)
{
	char back[] = "eeeeeeee";
	int rtn = write(socket_fd, back, strlen(back));	
}


int Compare_Tag(char *tag)
{
	char IDtag[] = "ID";
	char PASSWORDtag[] = "PASS";
	char EXITtag[] = "EXIT";
	char BOARDtag[] = "BOARD";
	char POStag[] = "P"; 
	char CONTENTtag[] = "CONTENT";
	char MESSAGEtag[] = "W";
	char MAILUSRtag[] = "M";
	char M_TITLEtag[] = "TITLE";


	if(strcmp(tag, EXITtag)==0){
		return 1;
	}else if(strcmp(tag, BOARDtag)==0){
		return 2;
	}else if(strcmp(tag,  POStag)==0){
		return 3;
	}else if(strcmp(tag, MESSAGEtag)==0){
		return 4;
	}else if(strcmp(tag, MAILUSRtag)==0){
		return 5;
	}else if(strcmp(tag, M_TITLEtag)==0){
		return 6;
	}else if(strcmp(tag, CONTENTtag)==0){
                return 7;
        }
	
	return 0;
}

int Next_Tag(int socket_fd, FILE *fp)
{
	char ch;
	char Tag[10];
	int le = 0;
	
	do{
		ch = fgetc(fp);
		if(ch!= '<' &&ch!='>'){
			Tag[le] = ch;
			le++;
		}Tag[le] = '\0';
	}while(ch!='>');
	fprintf(stderr, "next tag = %s\n", Tag);
	//string compare switch case
	int tag_num = Compare_Tag(Tag);
	return tag_num;	
}
 
void show_p2_plus(int socket_fd, int idx[], int &num)
{
	num = 0;
        char buf[64];
        bzero(buf, 64);
        int len = 0;
	char n[4]; n[3] = '\0';
        do{
                len = read(socket_fd, buf, sizeof(buf));
		for(int i=3; i<len-2; i++){
			memset(n, '\0', 4);
			if(buf[i-1]>47&&buf[i-1]<58&&(buf[i] == ' ' && buf[i+1]=='+' && buf[i+2] == ' ')){
				int ff = 0;
//				fprintf(stderr, "\n%c %c %c %c %c %c\n", buf[i-3], buf[i-2],buf[i-1],buf[i],buf[i+1],buf[i+2]);
				if(buf[i-1]>47 && buf[i-1]<58){
					n[ff] = buf[i-1];
					ff++;	
					if(buf[i-2]>47 && buf[i-2]<58){ 
                                        	n[ff] = buf[i-2];
	                                        ff++;
        	                     		if(buf[i-3]>47 && buf[i-3]<58){ 
                                     			n[ff] = buf[i-3];
                                        		ff++;
        		                        }
					}
				}
				n[ff] = '\0';
				if(ff==1){
				}else if(ff==2){
					char temp = n[0];
					n[0] = n[1];
					n[1] = temp;
				}else if(ff==3){
					char temp = n[0];
					n[0] = n[2];
					n[2] = temp;
				}
				fprintf(stderr, "--%s--", n);
//				sleep(5);
				idx[num] = atoi(n);
        		        num++;
			}
		}
		//for(int j=0; j<len; j++){
        	  //      fprintf(stderr, "%c",buf[j]);
		//}fprintf(stderr, "\n");
		fprintf(stderr, "%s", buf);
        }while(len >= 64);
        sleep(1);
}

void check_mail(int socket_fd){
	char enter[] = "\r\n";
	char m[] = "m";
	char r[] = "r";
	char q[] = "q";
	char P[] = "PPPPP";
	char n[] = "n";
	write(socket_fd, m, strlen(m));
	//show_p2(socket_fd);
        write(socket_fd, enter, strlen(enter));
	show_p2(socket_fd);
	write(socket_fd, r, strlen(r));
	//show_p2(socket_fd);
	write(socket_fd, enter, strlen(enter));
//	write(socket_fd, enter, strlen(enter));
	int idx[200];
	memset(idx, 0, 200);
	int num = 0;
	show_p2_plus(socket_fd, idx, num);
//	fprintf(stderr, "num = %d\n", num);
	for(int i=0; i<num; i++){
		fprintf(stderr, "(%d) ", idx[i]);
		char number[5];
		sprintf(number, "%d", idx[i]);
		fprintf(stderr, "\n%s\n", number);
		write(socket_fd, number, strlen(number));
		write(socket_fd, enter, strlen(enter));
		write(socket_fd, r, strlen(r));
		show_p2(socket_fd);
		write(socket_fd, q, strlen(q));
	}

//	char buf[128];
//	memset(buf, '\0', strlen(buf));
//	write(socket_fd, P, strlen(P));
//	show_p2(socket_fd);
//	write(socket_fd, n, strlen(n));
//	sleep(1);
//	write(socket_fd, n, strlen(n));
//	sleep(1);
//	show_p2(socket_fd);
//	show_p2(socket_fd);
	
}



void check_message(int socket_fd){
	char t[] = "t";
	char d[] = "d";
	char q[] = "q";
	char c[] = "ccc";
	char y[] = "y";
	char e[] = "eee";
	char r[] = "r";
	char ctrlB[2];
	ctrlB[0] = 2; ctrlB[1] = '\0';
	char enter[] = "\r\n";
	
	write(socket_fd, t, strlen(t));
	write(socket_fd, enter, strlen(enter));
	write(socket_fd, d, strlen(d));
	write(socket_fd, enter, strlen(enter));
	//write(socket_fd, enter, strlen(enter));
	show_p2(socket_fd);
	//write(socket_fd, ctrlB, strlen(ctrlB));
	//show_p2(socket_fd);	
	write(socket_fd, q, strlen(q));
	show_p2(socket_fd);
	write(socket_fd, r, strlen(r));
	show_p2(socket_fd);
	write(socket_fd, enter, strlen(enter));
	show_p2(socket_fd);
}


void Event_Sche(int socket_fd, FILE *fp)
{
        sleep(3);
	login(socket_fd, fp);	
	int tag_num;
	while(1){
		tag_num = Next_Tag(socket_fd, fp);
		switch(tag_num){
                	case 1: // logout
                        	//show_p2(socket_fd);
				sleep(10);
				BackToMain(socket_fd);
				check_message(socket_fd);
				BackToMain(socket_fd);
				show_p2(socket_fd);
				check_mail(socket_fd);
			//	show_p2(socket_fd);
				BackToMain(socket_fd);
				show_p2(socket_fd);
				logout(socket_fd);
	                        return;
        	                break;
	                case 2: // BOARD
	                        toBOARD(socket_fd, fp);
				BackToMain(socket_fd);
				show_p2(socket_fd);
	                        break;
	                case 3: //Pos TITLE
	                        break;
	                case 4: //Message to user
				toMessage(socket_fd, fp);
				BackToMain(socket_fd);
				BackToMain(socket_fd);
	                        show_p2(socket_fd);
				break;
	                case 5: //Maile title
				toMail(socket_fd, fp);
				BackToMain(socket_fd);
				show_p2(socket_fd);
	                        break;
	                case 6: //mail title
	                        break;
	                case 7: //content
	                        break;
	                default:
	                        break;
        	}
	}
}

int main(int argc, char** argv){
	
	struct hostent *hp;
	struct sockaddr_in srv;
	
	gethost(hp, srv);
	
	int socket_fd = 0;
	creat_bind_socket(srv, socket_fd);
	
	FILE *fp = fopen(argv[1], "r");
	
	Event_Sche(socket_fd, fp);
	
	int rtn = close(socket_fd);
	rtn = fclose(fp);
	return 0;
}
