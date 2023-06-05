#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>
#include<conio.h>
#include<graphics.h>
#include<windows.h>

#define ESC      27
#define MaxSprites 19

/* variáveis globais */
int FPS = 60;
int GWW, GWH; //Global Window Dimensions
int GBW, GBH; // Global Button Dimensions

/* funções */
void MainMenu();
void Game();
void CarregandoSons();
void WalkingSound();
bool KillColide();
bool Colide();
int STKDirection();

struct MainCharacter {
	int r; //hitbox radius
	int frame;
	int idleDirection;
	double stmn, maxstmn, stmnrgn; //stamina and max stamina
	double px, py, vm, vx, vy, dcl, acl; // x and y position, movement speed, x and y velocity, desaceleration and acceleration
	double rpy, rpx; //Relative Position
	double sprtvm, sprtam; //sprint velocity multiplier, sprint accel multiplier
	bool sprint, exausted; //states
	bool up, down, right, left; // direções 
};

struct Stalker {
	int r; //hitbox radius
	int frame;
	int direction;
	double px, py, vm, vx, vy, acl; // x and y position, movement speed, desaceleration and acceleration
	bool OnScreen, Chase; //states
};

struct BoundBox {
	int x1, x2, y1, y2;
	int r;
};

struct Fragment {
	int x, y;
};

bool KillColide(double sx, double sy, double sr, double px, double py, double prx, double pry, double pr) { // s = stalker / p = player / prx = player relative pos x
	double dx, dy, dist;
	
	dx = (sx + prx) - px;
	dy = (sy + pry) - py;
	dist = sqrt((dx * dx) + (dy * dy));
	if (dist < sr + pr) return true;
	else return false;
}

int STKDirection(double sx, double sy, double px, double py, double prx, double pry) { // s = stalker / p = player / prx = player relative pos x
	double dx, dy;
	
	dx = (sx + prx) - px;
	dy = (sy + pry) - py;
	
	if (dx >= 0 and dy >= 0) {
		if (dx > dy) return 3;
		else return 0;
	}
	else if (dx < 0 and dy >= 0) {
		if (dx * -1 > dy) return 2;
		else return 0;
	}
	else if (dx < 0 and dy < 0) {
		if (dx < dy) return 2;
		else return 1;
	}
	else if (dx >= 0 and dy < 0) {
		if (dx * -1 < dy) return 3;
		else return 1;
	}
}

bool FragmentColide(double px, double py, double prx, double pry, double pr, Fragment fragments[], int FC) { // s = stalker / p = player / prx = player relative pos x
	double dx, dy, dist;
	
	dx = (fragments[FC].x + prx) - px;
	dy = (fragments[FC].y + pry) - py;
	dist = sqrt((dx * dx) + (dy * dy));
	if (dist < 10 + pr) return true;
	else return false;
}

int Colide(BoundBox w[], double prx, double pry, double px, double py, double pr, double pvx, double pvy, int i, int tamBB) {
	bool result = false;
	for (int x=0;x<tamBB;x++) {
		if (i == 0) {
			if (px - 20 + pvx > w[x].x1 + prx and py > w[x].y1 + pry and px - 20 + pvx < w[x].x2 + prx and py < w[x].y2 + pry and pvx < 0) result = true;
		}
		else if (i == 1) {
			if (px + 20 + pvx > w[x].x1 + prx and py > w[x].y1 + pry and px + 20 + pvx < w[x].x2 + prx and py < w[x].y2 + pry and pvx > 0) result = true;
		}
		else if (i == 2) {
			if (px  > w[x].x1 + prx and py + 20 + pvy > w[x].y1 + pry and px < w[x].x2 + prx and py + 20 + pvy < w[x].y2 + pry and pvy > 0) result = true;
		}
		else if (i == 3) {
			if (px  > w[x].x1 + prx and py - 20 + pvy > w[x].y1 + pry and px < w[x].x2 + prx and py - 20 + pvy < w[x].y2 + pry and pvy < 0) result = true;
		}
		else if (i == 4) {
			if (px + 15 + pvx > w[x].x1 + prx and py - 15 + pvy > w[x].y1 + pry 
			and px + 15 + pvx < w[x].x2 + prx and py - 15 + pvy < w[x].y2 + pry 
			and (pvy < 0 or pvx > 0)) result = true;
		}
		else if (i == 5) {
			if (px - 15 + pvx > w[x].x1 + prx and py - 15 + pvy > w[x].y1 + pry 
			and px - 15 + pvx < w[x].x2 + prx and py - 15 + pvy < w[x].y2 + pry 
			and (pvy < 0 or pvx < 0)) result = true;
		}
		else if (i == 6) {
			if (px - 15 + pvx > w[x].x1 + prx and py + 15 + pvy > w[x].y1 + pry 
			and px - 15 + pvx < w[x].x2 + prx and py + 15 + pvy < w[x].y2 + pry 
			and (pvy > 0 or pvx < 0)) result = true;
		}
		else if (i == 7) {
			if (px + 15 + pvx > w[x].x1 + prx and py + 15 + pvy > w[x].y1 + pry 
			and px + 15 + pvx < w[x].x2 + prx and py + 15 + pvy < w[x].y2 + pry 
			and (pvy > 0 or pvx > 0)) result = true;
		}
	}
	return result;
}

void CarregandoSons() {
	mciSendString("open .\\assets\\AUDIO\\Jumpscare.mp3 type MPEGVideo alias jumpscare", NULL, 0, 0);
	mciSendString("open .\\assets\\AUDIO\\Running.mp3 type MPEGVideo alias running", NULL, 0, 0);
	mciSendString("open .\\assets\\AUDIO\\Walking.mp3 type MPEGVideo alias walking", NULL, 0, 0);
	mciSendString("open .\\assets\\AUDIO\\Menu.mp3 type MPEGVideo alias menu", NULL, 0, 0);
	mciSendString("open .\\assets\\AUDIO\\Chasing.mp3 type MPEGVideo alias chasing", NULL, 0, 0);
	mciSendString("open .\\assets\\AUDIO\\FoundYou.mp3 type MPEGVideo alias foundyou", NULL, 0, 0);
}

int main() {
    
    GWW = 1360; 
	GWH = 720;
	GBW = 377;
	GBH = 90;
	
	initwindow(GWW, GWH, "Darkest Dreams", 0, 0);
	
	CarregandoSons();
	
	MainMenu();
	
	closegraph();
	return 0;
}

void MainMenu() {
	
	bool stop = false;
	bool restart = false;
	int pg = 1;
	POINT P;
	HWND window;
	
	bool btnPlaySelected = false;
	bool btnQuitSelected = false;
	
	int tam;
	void *Bg;
    void *btnPlay, *btnPlaySel;
    void *btnQuit, *btnQuitSel;
	
	// carregando a imagem de fundo
    tam = imagesize(0, 0, GWW-1, GWH-1);
    Bg = malloc(tam);
    readimagefile(".\\assets\\Menu2.bmp", 0, 0, GWW - 1, GWH-1);
    getimage(0, 0, GWW-1, GWH-1, Bg);

    // carregando as imagens dos botões
    cleardevice();
    	
	window = GetForegroundWindow(); // Obtém o Handle da janela gráfica	
	
    waveOutSetVolume(0,0x88888888); //volume no médio
	mciSendString("seek menu to start", NULL, 0, 0);
	mciSendString("play menu", NULL, 0, 0);
	
	while(!stop) {
	    if (pg == 1) pg = 2; else pg = 1;
	    setactivepage(pg);
	    cleardevice();
	    putimage(0, 0, Bg, COPY_PUT);		
	    setvisualpage(pg);
	    
	    if (GetCursorPos(&P))
            if (ScreenToClient(window, &P)) {
        
            if(GetKeyState(VK_LBUTTON)&0x80) {
                if (770 <= P.x && P.x <= 770+GBW-1 && 437 <= P.y && P.y <= 437+GBH-1) {
                mciSendString("stop menu", NULL, 0, 0);
                Game();
                restart = true;
				}
            else if (909 <= P.x && P.x <= 909+GBW-1 && 556 <= P.y && P.y <= 556+GBH-1)
                stop = true;
        }
      }
      if (restart == true) {
      	restart = false;
      	waveOutSetVolume(0,0x88888888); //volume no médio
		mciSendString("seek menu to start", NULL, 0, 0);
		mciSendString("play menu", NULL, 0, 0);
	  }
	}
}

void Game() {
	
	bool stop = false;
	bool khit = false;
	bool hitUP = false, hitDO = false, hitRI = false, hitLE = false, hitSH = false; 
	bool killColide = false;
	bool SHB = false; //Show Hit Box
	bool points[8] = {false, false, false, false, false, false, false, false};
	bool RRNGLock = false;
	bool fragColide = false;
	bool winCondition = false;
	bool wlock = false;
	bool rlock = false;
	char keyp;
	unsigned long long clk1, clk1a; /* clock x / clock x auxiliar */
	int tam, tamBB;
	int pg = 1;
	int i, iy, ie = 0; 
	int colide;
	int FXPL[10] = {195, 2316, 1860, 2460, 1260, 900, 190, 1140, 1615, 815}; //Fragments X Possition List
	int FYPL[10] = {1360, 80, 360, 1580, 650, 530, 100, 450, 935, 1180}; //Fragments Y Possition List
	int RNG = 0;
	int RRNG[5]; //variável para evitar a repetição do RNG na geração de fragmentos *Repeated Random Number Generation* :)
	int FC = 0; //Fragementos coletados
	int FSM = 1; //Fragment Speed Multiplier
	int FAM = 0.2; //Fragment Acceleration Multiplier
	int flapsed = 0;
	int chaseEffectControl = 0;
	void *FBG[8];
	void *END[11];
	void *GOS;
	FILE *TXT;
	BoundBox *walls;
	BoundBox aux;
	Fragment fragments[5];
  	void *R;  // esta variável é um vetor dinâmico que contém o sprite
  	void *Down[MaxSprites+1];  // esta variável é um vetor dinâmico que contém o sprite
  	void *Up[MaxSprites+1];
  	void *Left[MaxSprites+1];
  	void *Right[MaxSprites+1];
  	void *DownMask[MaxSprites+1]; 
  	void *UpMask[MaxSprites+1];
  	void *LeftMask[MaxSprites+1];
  	void *RightMask[MaxSprites+1];
  	void *stkDown[MaxSprites+1];
  	void *stkUp[MaxSprites+1];
  	void *stkLeft[MaxSprites+1];
  	void *stkRight[MaxSprites+1];
  	void *stkDownMask[MaxSprites+1];
  	void *stkUpMask[MaxSprites+1];
  	void *stkLeftMask[MaxSprites+1];
  	void *stkRightMask[MaxSprites+1];
	
	srand(time(NULL));
	
	for (i=0;i<5;i++) {
		if (i==0) RNG = 4;
		else RNG = rand() % 10;
		RRNG[i] = RNG;
		for (iy=0;iy<i;iy++){
			if (RNG == RRNG[iy]) RRNGLock = true;
		}
		while (RRNGLock == true) {
			RRNGLock = false;
			srand(rand());
			RNG = rand() % 10;
			for (iy=0;iy<i;iy++){
				if (RNG == RRNG[iy]) RRNGLock = true;
			}
		}
		fragments[i].x = FXPL[RNG];
		fragments[i].y = FYPL[RNG];
	}
	
	//Definições do personagem principal
	MainCharacter MC;
	
	MC.r = 20; // radio da hb
	MC.px = GWW * 0.5; // posição x
	MC.py = GWH * 0.5; // posição y
	MC.dcl = 0.85; // desaceleração
	MC.acl = 0.5; // aceleração
	MC.vm = 8; // velocidade máxima
	MC.vx = 0; // velocidade x
	MC.vy = 0; // velocidade y
	MC.rpx = -580;
	MC.rpy = -1200;
	MC.frame = 0;
	MC.idleDirection = 0;
	MC.maxstmn = 100;
	MC.stmn = MC.maxstmn;
	MC.stmnrgn = 0.1;
	MC.sprtvm = 1.8;
	MC.sprtam = 1.2;
	MC.sprint = false;
	MC.exausted = false;
	MC.up = false;
	MC.down = false;
	MC.right = false;
	MC.left = false;
	
	//Definições do Stalker
	Stalker STK;
	
	STK.r = 20;
	STK.px = -500;
	STK.py = -500;
	STK.acl = 0.5;
	STK.vm = 5;
	STK.vx = 0;
	STK.vy = 0;
	STK.frame = 0;
	STK.direction = 0;
	STK.OnScreen = false;
	STK.Chase = false;
	
	keyp = 0;
	pg = 1;
	i = 0;
	clk1 = GetTickCount();
	clk1a = clk1 + 1200;
	
	//carregando áreas de colisão
	TXT = fopen(".//assets//FBG//bounds.txt", "r");
	tamBB = 0;
	walls = NULL;
 	while (fscanf(TXT, "%d;%d;%d;%d\n", &aux.x1, &aux.y1, &aux.x2, &aux.y2) != EOF) {
 		tamBB++;
    	walls = (BoundBox *)realloc(walls, sizeof(BoundBox) * tamBB);
    	walls[tamBB-1] = aux;
 	}
	fclose(TXT);
	
	
	//carregando imagens
	readimagefile(".\\assets\\load.bmp", 0,0,1360,720);
	setactivepage(3);
	
	tam = imagesize(0,0,1260,410);
	FBG[0] = malloc(tam);
	readimagefile(".\\assets\\FBG\\layout1.bmp", 0,0,1260,410);
	getimage(0,0,1260,410,FBG[0]);
	FBG[1] = malloc(tam);
	readimagefile(".\\assets\\FBG\\layout2.bmp", 0,0,1260,410);
	getimage(0,0,1260,410,FBG[1]);
	FBG[2] = malloc(tam);
	readimagefile(".\\assets\\FBG\\layout3.bmp", 0,0,1260,410);
	getimage(0,0,1260,410,FBG[2]);
	FBG[3] = malloc(tam);
	readimagefile(".\\assets\\FBG\\layout4.bmp", 0,0,1260,410);
	getimage(0,0,1260,410,FBG[3]);
	FBG[4] = malloc(tam);
	readimagefile(".\\assets\\FBG\\layout5.bmp", 0,0,1260,410);
	getimage(0,0,1260,410,FBG[4]);
	FBG[5] = malloc(tam);
	readimagefile(".\\assets\\FBG\\layout6.bmp", 0,0,1260,410);
	getimage(0,0,1260,410,FBG[5]);
	FBG[6] = malloc(tam);
	readimagefile(".\\assets\\FBG\\layout7.bmp", 0,0,1260,410);
	getimage(0,0,1260,410,FBG[6]);
	FBG[7] = malloc(tam);
	readimagefile(".\\assets\\FBG\\layout8.bmp", 0,0,1260,410);
	getimage(0,0,1260,410,FBG[7]);
	
	tam = imagesize(0,0,1360,720);
	GOS = malloc(tam);
	readimagefile(".\\assets\\Game_Over2.bmp", 0, 0, GWW - 1, GWH-1);
    getimage(0, 0, GWW-1, GWH-1, GOS);
    
    END[0] = malloc(tam);
    readimagefile(".\\assets\\END\\0.bmp", 0,0,1360,720);
    getimage(0,0,1360,720,END[0]);
    
    END[1] = malloc(tam);
    readimagefile(".\\assets\\END\\1.bmp", 0,0,1360,720);
    getimage(0,0,1360,720,END[1]);
    
    END[2] = malloc(tam);
    readimagefile(".\\assets\\END\\2.bmp", 0,0,1360,720);
    getimage(0,0,1360,720,END[2]);
    
    END[3] = malloc(tam);
    readimagefile(".\\assets\\END\\3.bmp", 0,0,1360,720);
    getimage(0,0,1360,720,END[3]);
    
    END[4] = malloc(tam);
    readimagefile(".\\assets\\END\\4.bmp", 0,0,1360,720);
    getimage(0,0,1360,720,END[4]);
    
    END[5] = malloc(tam);
    readimagefile(".\\assets\\END\\5.bmp", 0,0,1360,720);
    getimage(0,0,1360,720,END[5]);
    
    END[6] = malloc(tam);
    readimagefile(".\\assets\\END\\6.bmp", 0,0,1360,720);
    getimage(0,0,1360,720,END[6]);
    
    END[7] = malloc(tam);
    readimagefile(".\\assets\\END\\7.bmp", 0,0,1360,720);
    getimage(0,0,1360,720,END[7]);
    
    END[8] = malloc(tam);
    readimagefile(".\\assets\\END\\8.bmp", 0,0,1360,720);
    getimage(0,0,1360,720,END[8]);
    
    END[9] = malloc(tam);
    readimagefile(".\\assets\\END\\9.bmp", 0,0,1360,720);
    getimage(0,0,1360,720,END[9]);
    
    END[10] = malloc(tam);
    readimagefile(".\\assets\\END\\10.bmp", 0,0,1360,720);
    getimage(0,0,1360,720,END[10]);
    
    // carregando spritesheet Main Character
    tam = imagesize(0, 0, 160, 160);
    for(i = 0; i <= MaxSprites; i++) {
      Down[i] = malloc(tam);
    }
    for(i = 0; i <= MaxSprites; i++) {
      Up[i] = malloc(tam);
    }
    for(i = 0; i <= MaxSprites; i++) {
      Right[i] = malloc(tam);
    }
    for(i = 0; i <= MaxSprites; i++) {
      Left[i] = malloc(tam);
    }
    
    for(i = 0; i <= MaxSprites; i++) {
      DownMask[i] = malloc(tam);
    }
    for(i = 0; i <= MaxSprites; i++) {
      UpMask[i] = malloc(tam);
    }
    for(i = 0; i <= MaxSprites; i++) {
      RightMask[i] = malloc(tam);
    }
    for(i = 0; i <= MaxSprites; i++) {
      LeftMask[i] = malloc(tam);
    }
  
    readimagefile(".\\assets\\PP\\r5.bmp", 0, 0, 800, 640);
    for(i = 0; i <= MaxSprites; i++) { 
    	getimage(160*i, 0, 159+160*i, 159, Down[i]); // primeira fileira
	    if (i > 4) {
	    	getimage(160*i-800, 160, (159+160*i)-800, 319, Down[i]); // segunda fileira
		}
		if (i > 9) {
	    	getimage(160*i-1600, 320, (159+160*i)-1600, 479, Down[i]); // terceira fileira
		}
		if (i > 14 && i < 20) {
	    	getimage(160*i-2400, 480, (159+160*i)-2400, 640, Down[i]); // quarta fileira
		}
	}
	
	readimagefile(".\\assets\\PP\\m5.bmp", 0, 0, 800, 640);
    for(i = 0; i <= MaxSprites; i++) { 
    	getimage(160*i, 0, 159+160*i, 159, DownMask[i]); // primeira fileira
	    if (i > 4) {
	    	getimage(160*i-800, 160, (159+160*i)-800, 319, DownMask[i]); // segunda fileira
		}
		if (i > 9) {
	    	getimage(160*i-1600, 320, (159+160*i)-1600, 479, DownMask[i]); // terceira fileira
		}
		if (i > 14 && i < 20) {
	    	getimage(160*i-2400, 480, (159+160*i)-2400, 640, DownMask[i]); // quarta fileira
		}
	}
	
	readimagefile(".\\assets\\PP\\r1.bmp", 0, 0, 800, 640);
    for(i = 0; i <= MaxSprites; i++) { 
    	getimage(160*i, 0, 159+160*i, 159, Up[i]); // primeira fileira
	    if (i > 4) {
	    	getimage(160*i-800, 160, (159+160*i)-800, 319, Up[i]); // segunda fileira
		}
		if (i > 9) {
	    	getimage(160*i-1600, 320, (159+160*i)-1600, 479, Up[i]); // terceira fileira
		}
		if (i > 14 && i < 20) {
	    	getimage(160*i-2400, 480, (159+160*i)-2400, 640, Up[i]); // quarta fileira
		}
	}
	
	readimagefile(".\\assets\\PP\\m1.bmp", 0, 0, 800, 640);
    for(i = 0; i <= MaxSprites; i++) { 
    	getimage(160*i, 0, 159+160*i, 159, UpMask[i]); // primeira fileira
	    if (i > 4) {
	    	getimage(160*i-800, 160, (159+160*i)-800, 319, UpMask[i]); // segunda fileira
		}
		if (i > 9) {
	    	getimage(160*i-1600, 320, (159+160*i)-1600, 479, UpMask[i]); // terceira fileira
		}
		if (i > 14 && i < 20) {
	    	getimage(160*i-2400, 480, (159+160*i)-2400, 640, UpMask[i]); // quarta fileira
		}
	}
  
	readimagefile(".\\assets\\PP\\r7.bmp", 0, 0, 800, 640);
    for(i = 0; i <= MaxSprites; i++) { 
    	getimage(160*i, 0, 159+160*i, 159, Right[i]); // primeira fileira
	    if (i > 4) {
	    	getimage(160*i-800, 160, (159+160*i)-800, 319, Right[i]); // segunda fileira
		}
		if (i > 9) {
	    	getimage(160*i-1600, 320, (159+160*i)-1600, 479, Right[i]); // terceira fileira
		}
		if (i > 14 && i < 20) {
	    	getimage(160*i-2400, 480, (159+160*i)-2400, 640, Right[i]); // quarta fileira
		}
	}
	
	readimagefile(".\\assets\\PP\\m7.bmp", 0, 0, 800, 640);
    for(i = 0; i <= MaxSprites; i++) { 
    	getimage(160*i, 0, 159+160*i, 159, RightMask[i]); // primeira fileira
	    if (i > 4) {
	    	getimage(160*i-800, 160, (159+160*i)-800, 319, RightMask[i]); // segunda fileira
		}
		if (i > 9) {
	    	getimage(160*i-1600, 320, (159+160*i)-1600, 479, RightMask[i]); // terceira fileira
		}
		if (i > 14 && i < 20) {
	    	getimage(160*i-2400, 480, (159+160*i)-2400, 640, RightMask[i]); // quarta fileira
		}
	}
  
	readimagefile(".\\assets\\PP\\r3.bmp", 0, 0, 800, 640);
    for(i = 0; i <= MaxSprites; i++) { 
    	getimage(160*i, 0, 159+160*i, 159, Left[i]); // primeira fileira
	    if (i > 4) {
	    	getimage(160*i-800, 160, (159+160*i)-800, 319, Left[i]); // segunda fileira
		}
		if (i > 9) {
	    	getimage(160*i-1600, 320, (159+160*i)-1600, 479, Left[i]); // terceira fileira
		}
		if (i > 14 && i < 20) {
	    	getimage(160*i-2400, 480, (159+160*i)-2400, 640, Left[i]); // quarta fileira
		}
	}
	
	readimagefile(".\\assets\\PP\\m3.bmp", 0, 0, 800, 640);
    for(i = 0; i <= MaxSprites; i++) { 
    	getimage(160*i, 0, 159+160*i, 159, LeftMask[i]); // primeira fileira
	    if (i > 4) {
	    	getimage(160*i-800, 160, (159+160*i)-800, 319, LeftMask[i]); // segunda fileira
		}
		if (i > 9) {
	    	getimage(160*i-1600, 320, (159+160*i)-1600, 479, LeftMask[i]); // terceira fileira
		}
		if (i > 14 && i < 20) {
	    	getimage(160*i-2400, 480, (159+160*i)-2400, 640, LeftMask[i]); // quarta fileira
		}
	}
	
	// carregando spritesheet monstro
	for(i = 0; i < MaxSprites; i++) {
      stkDown[i] = malloc(tam);
    }
    for(i = 0; i < MaxSprites; i++) {
      stkUp[i] = malloc(tam);
    }
    for(i = 0; i < MaxSprites; i++) {
      stkRight[i] = malloc(tam);
    }
    for(i = 0; i < MaxSprites; i++) {
      stkLeft[i] = malloc(tam);
    }
    
    for(i = 0; i < MaxSprites; i++) {
      stkDownMask[i] = malloc(tam);
    }
    for(i = 0; i < MaxSprites; i++) {
      stkUpMask[i] = malloc(tam);
    }
    for(i = 0; i < MaxSprites; i++) {
      stkRightMask[i] = malloc(tam);
    }
    for(i = 0; i < MaxSprites; i++) {
      stkLeftMask[i] = malloc(tam);
    }
    
    readimagefile(".\\assets\\STK\\walk1.bmp", 0, 0, 800, 640);
    for(i = 0; i < MaxSprites; i++) { 
    	getimage(160*i, 0, 159+160*i, 159, stkDown[i]); // primeira fileira
	    if (i > 4) {
	    	getimage(160*i-800, 160, (159+160*i)-800, 319, stkDown[i]); // segunda fileira
		}
		if (i > 9) {
	    	getimage(160*i-1600, 320, (159+160*i)-1600, 479, stkDown[i]); // terceira fileira
		}
		if (i > 14 && i < 19) {
	    	getimage(160*i-2400, 480, (159+160*i)-2400, 640, stkDown[i]); // quarta fileira
		}
	}
	
	readimagefile(".\\assets\\STK\\walk1Mask.bmp", 0, 0, 800, 640);
    for(i = 0; i < MaxSprites; i++) { 
    	getimage(160*i, 0, 159+160*i, 159, stkDownMask[i]); // primeira fileira
	    if (i > 4) {
	    	getimage(160*i-800, 160, (159+160*i)-800, 319, stkDownMask[i]); // segunda fileira
		}
		if (i > 9) {
	    	getimage(160*i-1600, 320, (159+160*i)-1600, 479, stkDownMask[i]); // terceira fileira
		}
		if (i > 14 && i < 19) {
	    	getimage(160*i-2400, 480, (159+160*i)-2400, 640, stkDownMask[i]); // quarta fileira
		}
	}
	
	readimagefile(".\\assets\\STK\\walk3.bmp", 0, 0, 800, 640);
    for(i = 0; i < MaxSprites; i++) { 
    	getimage(160*i, 0, 159+160*i, 159, stkRight[i]); // primeira fileira
	    if (i > 4) {
	    	getimage(160*i-800, 160, (159+160*i)-800, 319, stkRight[i]); // segunda fileira
		}
		if (i > 9) {
	    	getimage(160*i-1600, 320, (159+160*i)-1600, 479, stkRight[i]); // terceira fileira
		}
		if (i > 14 && i < 19) {
	    	getimage(160*i-2400, 480, (159+160*i)-2400, 640, stkRight[i]); // quarta fileira
		}
	}
	
	readimagefile(".\\assets\\STK\\walk3Mask.bmp", 0, 0, 800, 640);
    for(i = 0; i < MaxSprites; i++) { 
    	getimage(160*i, 0, 159+160*i, 159, stkRightMask[i]); // primeira fileira
	    if (i > 4) {
	    	getimage(160*i-800, 160, (159+160*i)-800, 319, stkRightMask[i]); // segunda fileira
		}
		if (i > 9) {
	    	getimage(160*i-1600, 320, (159+160*i)-1600, 479, stkRightMask[i]); // terceira fileira
		}
		if (i > 14 && i < 19) {
	    	getimage(160*i-2400, 480, (159+160*i)-2400, 640, stkRightMask[i]); // quarta fileira
		}
	}
	
	readimagefile(".\\assets\\STK\\walk7.bmp", 0, 0, 800, 640);
    for(i = 0; i < MaxSprites; i++) { 
    	getimage(160*i, 0, 159+160*i, 159, stkLeft[i]); // primeira fileira
	    if (i > 4) {
	    	getimage(160*i-800, 160, (159+160*i)-800, 319, stkLeft[i]); // segunda fileira
		}
		if (i > 9) {
	    	getimage(160*i-1600, 320, (159+160*i)-1600, 479, stkLeft[i]); // terceira fileira
		}
		if (i > 14 && i < 19) {
	    	getimage(160*i-2400, 480, (159+160*i)-2400, 640, stkLeft[i]); // quarta fileira
		}
	}
	
	readimagefile(".\\assets\\STK\\walk7Mask.bmp", 0, 0, 800, 640);
    for(i = 0; i < MaxSprites; i++) { 
    	getimage(160*i, 0, 159+160*i, 159, stkLeftMask[i]); // primeira fileira
	    if (i > 4) {
	    	getimage(160*i-800, 160, (159+160*i)-800, 319, stkLeftMask[i]); // segunda fileira
		}
		if (i > 9) {
	    	getimage(160*i-1600, 320, (159+160*i)-1600, 479, stkLeftMask[i]); // terceira fileira
		}
		if (i > 14 && i < 19) {
	    	getimage(160*i-2400, 480, (159+160*i)-2400, 640, stkLeftMask[i]); // quarta fileira
		}
	}
	
	readimagefile(".\\assets\\STK\\walk5.bmp", 0, 0, 800, 640);
    for(i = 0; i < MaxSprites; i++) { 
    	getimage(160*i, 0, 159+160*i, 159, stkUp[i]); // primeira fileira
	    if (i > 4) {
	    	getimage(160*i-800, 160, (159+160*i)-800, 319, stkUp[i]); // segunda fileira
		}
		if (i > 9) {
	    	getimage(160*i-1600, 320, (159+160*i)-1600, 479, stkUp[i]); // terceira fileira
		}
		if (i > 14 && i < 19) {
	    	getimage(160*i-2400, 480, (159+160*i)-2400, 640, stkUp[i]); // quarta fileira
		}
	}
	
	readimagefile(".\\assets\\STK\\walk5Mask.bmp", 0, 0, 800, 640);
    for(i = 0; i < MaxSprites; i++) { 
    	getimage(160*i, 0, 159+160*i, 159, stkUpMask[i]); // primeira fileira
	    if (i > 4) {
	    	getimage(160*i-800, 160, (159+160*i)-800, 319, stkUpMask[i]); // segunda fileira
		}
		if (i > 9) {
	    	getimage(160*i-1600, 320, (159+160*i)-1600, 479, stkUpMask[i]); // terceira fileira
		}
		if (i > 14 && i < 19) {
	    	getimage(160*i-2400, 480, (159+160*i)-2400, 640, stkUpMask[i]); // quarta fileira
		}
	}
    
	cleardevice();
	
	//loop do jogo
	while (!stop) {
		if (clk1 - clk1a > 1000 / FPS) {
			clk1a = clk1;
			if (pg == 1) pg =2; else pg = 1;
			setactivepage(pg);
			cleardevice();
			
			for(i=0;i<8;i++){
				if(i == 0 or i == 1) iy = 0;
				else if(i == 2 or i == 3) iy = 1;
				else if(i == 4 or i == 5) iy = 2;
				else iy = 3;
				putimage((0 + MC.rpx) + (1260 * (i % 2)),(0 + MC.rpy) + (410 * iy),FBG[i],COPY_PUT);
			}
			
			//Desenho de hitboxes, etc
			if(SHB == true){
				for(i = 0; i < tamBB; i++){
					setcolor(RGB(255,255,0));
					rectangle(walls[i].x1+MC.rpx,walls[i].y1+MC.rpy,walls[i].x2+MC.rpx,walls[i].y2+MC.rpy);
				}
				setcolor(RGB(0, 255, 0));
            	setfillstyle(1, RGB(255,0,0));
            	fillellipse(MC.px, MC.py, MC.r, MC.r);
            
				setfillstyle(1, RGB(0,0,0)); 
            	fillellipse(STK.px + MC.rpx, STK.py + MC.rpy, STK.r, STK.r);
        	}
        	
        	setfillstyle(1, RGB(0,0,255));
            	
            if (FC < 5)fillellipse(fragments[FC].x + MC.rpx, fragments[FC].y + MC.rpy, 10, 10);
            	
        	//HUD
        	setcolor(RGB(200 - MC.stmn * 2, 0 + MC.stmn * 2, 0));
        	for (i=0;i<MC.stmn;i++) line(26 + i, 25, 26 + i, 50);
        	setcolor(RGB(0,90,90));
        	rectangle(25,25, 125, 50);
        	
        	for(i=0;i<5;i++) {
        		setcolor(RGB(0,0,140));
        		rectangle(1200 + i * 30,25,1225 + i * 30,50);
			}
			
			setfillstyle(1, RGB(0,0,180));
			for (i=0;i<FC;i++) fillellipse(1213 + i * 30,38,10,10);
			
			// Desenho personagem
			if (MC.up == true) {
				putimage(MC.px-80, MC.py-80, UpMask[MC.frame], AND_PUT);
				putimage(MC.px-80, MC.py-80, Up[MC.frame], OR_PUT);
				MC.idleDirection = 0;
			} else if (MC.down == true) {
				putimage(MC.px-80, MC.py-80, DownMask[MC.frame], AND_PUT);
				putimage(MC.px-80, MC.py-80, Down[MC.frame], OR_PUT);
				MC.idleDirection = 1;
			} else if (MC.right == true) {
				putimage(MC.px-80, MC.py-80, RightMask[MC.frame], AND_PUT);
        		putimage(MC.px-80, MC.py-80, Right[MC.frame], OR_PUT);
        		MC.idleDirection = 2;
        	} else if (MC.left == true){
        		putimage(MC.px-80, MC.py-80, LeftMask[MC.frame], AND_PUT);
        		putimage(MC.px-80, MC.py-80, Left[MC.frame], OR_PUT);
        		MC.idleDirection = 3;
        	}
        	else {
        		if (MC.idleDirection == 0) {
        			putimage(MC.px-80, MC.py-80, UpMask[19], AND_PUT);
        			putimage(MC.px-80, MC.py-80, Up[19], OR_PUT);
				} else if (MC.idleDirection == 1) {
					putimage(MC.px-80, MC.py-80, DownMask[19], AND_PUT);
					putimage(MC.px-80, MC.py-80, Down[19], OR_PUT);
        		} else if (MC.idleDirection == 2) {
        			putimage(MC.px-80, MC.py-80, RightMask[19], AND_PUT);
				 	putimage(MC.px-80, MC.py-80, Right[19], OR_PUT);
        		} else {
        			putimage(MC.px-80, MC.py-80, LeftMask[19], AND_PUT);
					putimage(MC.px-80, MC.py-80, Left[19], OR_PUT);
				}
			}
			
			// Desenho stalker
			if (STK.direction == 0) { 
				putimage((STK.px+MC.rpx)-80, (STK.py+MC.rpy)-80, stkUpMask[STK.frame], AND_PUT);
				putimage((STK.px+MC.rpx)-80, (STK.py+MC.rpy)-80, stkUp[STK.frame], OR_PUT);
			} else if (STK.direction == 1) {
				putimage((STK.px+MC.rpx)-80, (STK.py+MC.rpy)-80, stkDownMask[STK.frame], AND_PUT);
				putimage((STK.px+MC.rpx)-80, (STK.py+MC.rpy)-80, stkDown[STK.frame], OR_PUT);
			} else if (STK.direction == 2) {
				putimage((STK.px+MC.rpx)-80, (STK.py+MC.rpy)-80, stkRightMask[STK.frame], AND_PUT);
        		putimage((STK.px+MC.rpx)-80, (STK.py+MC.rpy)-80, stkRight[STK.frame], OR_PUT);
        	} else if (STK.direction == 3) {
        		putimage((STK.px+MC.rpx)-80, (STK.py+MC.rpy)-80, stkLeftMask[STK.frame], AND_PUT);
        		putimage((STK.px+MC.rpx)-80, (STK.py+MC.rpy)-80, stkLeft[STK.frame], OR_PUT);
			}

        	//Desenho Tela de Game Over
        	if (killColide == true){
        		putimage(0 + (rand() % 5) * -1 -(rand() % 2),0 + (rand() % 15) * -1 -(rand() % 2),GOS,COPY_PUT);
    		}
    		
    		if (winCondition == true){
    			if (flapsed % 2 == 0 and ie < 10) ie++;
    			putimage(0,0,END[ie],COPY_PUT);
			}
        
			setvisualpage(pg);
			
			//Reset da direção do personagem
			MC.up = false;
			MC.down = false;
			MC.right = false;
			MC.left = false;
			
			//Detecção de teclas
			if (GetKeyState(VK_UP)&0x80) {
				khit = true;
                hitUP = true;
                MC.up = true;
			}
			if (GetKeyState(VK_DOWN)&0x80) {
				khit = true;
                hitDO = true;
                MC.down = true;
	
			}
			if (GetKeyState(VK_RIGHT)&0x80) {
				khit = true;
                hitRI = true;
                MC.right = true;

			}
			if (GetKeyState(VK_LEFT)&0x80) {
				khit = true;
                hitLE = true;
                MC.left = true;
			}
			if(GetKeyState(VK_SHIFT)&0x80) {
				hitSH = true;
			}
			//Variáveis para animações no frame seguinte
			if (khit == true) MC.frame++; if (MC.frame >= MaxSprites) MC.frame = 0;
			if (STK.Chase == true) STK.frame++; if (STK.frame >=MaxSprites) STK.frame = 0;
			
			/*Movimentação Personagem principal*/
			
			//Mecânica de sprint
			if (hitSH == true and khit == true and MC.exausted == false) {
				if (MC.stmn <= 0) MC.exausted = true;
				else {
					MC.sprint = true; 
					MC.stmn -= 1;
				}
			}
			else if (MC.stmn + MC.stmnrgn < MC.maxstmn) MC.stmn += MC.stmnrgn;
			
			if (MC.stmn >= MC.maxstmn * 0.3 and MC.exausted == true) MC.exausted = false;
			
			//Acelereção
			if (hitUP) {
				if (MC.sprint == false) {
					MC.vy -= MC.acl;
					
					if (MC.vy < -MC.vm) {
						MC.vy = -MC.vm;
					}
				}
				if (MC.sprint == true) {
					MC.vy -= MC.acl * MC.sprtam;
					
					if (MC.vy < -MC.vm * MC.sprtvm) {
						MC.vy = -MC.vm * MC.sprtvm;
					}
				}
			}
			else {
				if (MC.vy < 0) {
				    MC.vy += MC.dcl;
				    if (MC.vy > 0) {
					    MC.vy = 0;
					}
				}
			}	
			if (hitDO) {
				if (MC.sprint == false) {
					MC.vy += MC.acl;
					if (MC.vy > MC.vm) {
						MC.vy = MC.vm;
					}
				}
				if (MC.sprint == true) {
					MC.vy += MC.acl * MC.sprtam;
					if (MC.vy > MC.vm * MC.sprtvm) {
						MC.vy = MC.vm * MC.sprtvm;
					}
				}
			}
			else {
				if (MC.vy > 0) {
					MC.vy -= MC.dcl;
				    if (MC.vy < 0) {
						MC.vy = 0;
					}
				}
			}
			if (hitLE) {
				if (MC.sprint == false) {
					MC.vx -= MC.acl;
					if (MC.vx < -MC.vm) {
						MC.vx = -MC.vm;
					}
				}
				if (MC.sprint == true) {
					MC.vx -= MC.acl * MC.sprtam;
					if (MC.vx < -MC.vm * MC.sprtvm) {
						MC.vx = -MC.vm * MC.sprtvm;
					}
				}
			}
			else {
				if (MC.vx < 0) {
				    MC.vx += MC.dcl;
				    if (MC.vx > 0) {
						MC.vx = 0;
					}
				}
			}
			if (hitRI) {
				if (MC.sprint == false) {
			    	MC.vx += MC.acl;
					if (MC.vx > MC.vm) {
						MC.vx = MC.vm;
					}
				}
				if (MC.sprint == true) {
			    	MC.vx += MC.acl * MC.sprtam;
					if (MC.vx > MC.vm * MC.sprtvm) {
						MC.vx = MC.vm * MC.sprtvm;
					}
				}
			}
			else {
				if (MC.vx > 0) {
				    MC.vx -= MC.dcl;
				    if (MC.vx < 0) {
						MC.vx = 0;
					}
				}
			}
			
			//Detecção de colisão com objetos
			for (i=0;i<8;i++) {
				points[i] = Colide(walls, MC.rpx, MC.rpy, MC.px, MC.py, MC.r, MC.vx, MC.vy, i, tamBB);
			}
			
			//Colidiu com objeto
			if (points[0] == true) MC.vx = 0;
			if (points[1] == true) MC.vx = 0;
			if (points[2] == true) MC.vy = 0;
			if (points[3] == true) MC.vy = 0;
			if (points[4] == true) {MC.vy = 0; MC.vx = 0;}
			if (points[5] == true) {MC.vy = 0; MC.vx = 0;}
			if (points[6] == true) {MC.vy = 0; MC.vx = 0;}
			if (points[7] == true) {MC.vy = 0; MC.vx = 0;}
			
			//Detecção de colisão com fragmentos
			fragColide = FragmentColide(MC.px,MC.py,MC.rpx,MC.rpy,MC.r, fragments, FC);
			
			//Colidiu com fragmento
			if (fragColide == true and FC < 5) FC++;
			
			for (i=0;i<8;i++) points[i] = false;
			
			//Aplicando a movimentação
			MC.rpx -= MC.vx;
			MC.rpy -= MC.vy;
			
			if (STK.Chase == true){
				if (STK.px + MC.rpx < MC.px and STK.vx < STK.vm + (FC * FSM)){
					STK.vx += STK.acl + (FC * FAM);
				}
				else if (STK.px + MC.rpx > MC.px and STK.vx > -STK.vm - (FC * FSM)){
					STK.vx -= STK.acl + (FC * FAM);
				}
				if (STK.py + MC.rpy < MC.py and STK.vy < STK.vm + (FC * FSM)){
					STK.vy += STK.acl + (FC * FAM);
				}
				else if (STK.py + MC.rpy > MC.py and STK.vy > -STK.vm - (FC * FSM)){
					STK.vy -= STK.acl + (FC * FAM);
				}
				
				if (STK.vx < -STK.vm - (FC * FSM)) STK.vx = -STK.vm - (FC * FSM);
				else if (STK.vx > STK.vm + (FC * FSM)) STK.vx = STK.vm + (FC * FSM);
				if (STK.vy < -STK.vm - (FC * FSM)) STK.vy = -STK.vm - (FC * FSM);
				else if (STK.vy > STK.vm + (FC * FSM)) STK.vy = STK.vm + (FC * FSM);
				
				STK.px += STK.vx;
				STK.py += STK.vy;
				
				//Variáveis para o próximo frame
				if(STK.Chase == true) STK.direction = STKDirection(STK.px, STK.py, MC.px, MC.py, MC.rpx, MC.rpy);
			}
			
			if(killColide == false and winCondition == false) killColide = KillColide(STK.px, STK.py, STK.r, MC.px, MC.py, MC.rpx, MC.rpy, MC.r);
			
			if (killColide == true) {
					if(flapsed == 0) {
						mciSendString("stop running", NULL, 0, 0);
						mciSendString("stop walking", NULL, 0, 0);
						mciSendString("stop chasing", NULL, 0, 0);
						mciSendString("stop foundyou", NULL, 0, 0);
						waveOutSetVolume(0,0xFFFFFFFF); //volume no maximo
						mciSendString("seek jumpscare to start", NULL, 0, 0);
						mciSendString("play jumpscare", NULL, 0, 0);
					}
				    flapsed++;
					if (flapsed > 90) stop = true;
			}
			
			if (killColide == false and FC == 5) {
				if (winCondition == false) {
					mciSendString("stop running", NULL, 0, 0);
					mciSendString("stop walking", NULL, 0, 0);
					mciSendString("stop chasing", NULL, 0, 0);
					mciSendString("stop foundyou", NULL, 0, 0);
					winCondition = true; 
					STK.Chase = false;
				}
				flapsed++;
				if (flapsed > 160) stop = true;
			}
			
			//Iniciando o Stalker
			if (STK.OnScreen == false and FC == 1 and killColide == false) {
				waveOutSetVolume(0,0xFFFFFFFF); //volume no maximo
				mciSendString("seek foundyou to start", NULL, 0, 0);
				mciSendString("play foundyou", NULL, 0, 0);
				STK.OnScreen = true; 
				STK.Chase = true; 
				STK.px = 1280; 
				STK.py = 180;
			}
			
			//Efeitos sonoros
			if (STK.Chase == true and chaseEffectControl < 120 and killColide == false and winCondition == false) {
				chaseEffectControl++;
				if (chaseEffectControl == 119) {
					waveOutSetVolume(0,0x88888888); //volume no médio
					mciSendString("seek chasing to start", NULL, 0, 0);
					mciSendString("play chasing repeat", NULL, 0, 0);
				}
			}
			
			//Som dos passos
			if (killColide == false and winCondition == false) {

				if (khit == true and hitSH != true) {
			
					if (rlock == true) {
						mciSendString("stop running", NULL, 0, 0);
						rlock = false;
					}
					
					if (wlock == false) {
						wlock = true;
						waveOutSetVolume(0,0x88888888); //volume no médio
						mciSendString("play walking repeat", NULL, 0, 0);
					}
				}
				else if (khit == true and hitSH == true) {
					
					if (rlock == true) {
						mciSendString("stop walking", NULL, 0, 0);
						wlock = false;
					}
					
					if (rlock == false) {
						rlock = true;
						waveOutSetVolume(0,0x88888888); //volume no médio
						mciSendString("play running repeat", NULL, 0, 0);
					}
				}
				else if (khit == false) {
					wlock = false;
					rlock = false;
					mciSendString("stop walking", NULL, 0, 0);
					mciSendString("stop running", NULL, 0, 0);
				}
			}
			//Reset de variáveis
			khit = false;
			hitUP = false;
			hitDO = false;
			hitLE = false;
			hitRI = false;
			hitSH = false;
			MC.sprint = false;
		}
		clk1 = GetTickCount();
		
		fflush(stdin);    
        if (kbhit()) {
            keyp = getch();
            if (keyp == ESC) {
                stop = true;
                mciSendString("stop jumpscare", NULL, 0, 0);
                mciSendString("stop foundyou", NULL, 0, 0);
                mciSendString("stop chasing", NULL, 0, 0);
            }
    	}
	}	
}
