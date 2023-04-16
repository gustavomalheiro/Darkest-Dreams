#include <graphics.h>
#include <conio.h>
#include<stdio.h>
#include<stdlib.h>

int main() {
    int pg = 1;
    int Fim;
    int x, y, raio;
    int passox, passoy;
    int FPS;
    int teste;
    char tecla = 0;
    unsigned long long gt1, gt2;
    int tam;
	void *R; // vetor dinâmico que contem o sprite
	
	initwindow(1280, 800, "TOPDOWN TERROR");
    gt1 = GetTickCount();
    gt2 = gt1 + 1200;
    Fim = false;
	
	tam = imagesize(0, 0, 123, 130);
	R = malloc(tam);
	
	readimagefile("assets/tocha21.bmp", 0, 0, 123, 130);
	getimage(0, 0, 123, 130, R);
	
    x = 200;
    y = 250;
    raio = 18;
    passox = 10;
    passoy = 10;
    FPS = 60;
    
    // testando a branch
    teste = 1000;
	
	//loop principal do jogo
    while(!Fim) {
        if (gt2 - gt1 > 1000/FPS) {
            gt1 = gt2;

            if (pg == 1) pg = 2; else pg = 1;
            setactivepage(pg);
			setbkcolor(RGB(255, 255, 255));
            cleardevice();
			
			/*	
		    setfillstyle(1, YELLOW);
		    setcolor (YELLOW);      
		    fillellipse(x, y, raio, raio);
		    setvisualpage(pg);
			*/
			putimage(x, y, R, OR_PUT);
			setvisualpage(pg);
			
			
			if(GetKeyState(VK_UP)&0x80)
				y = y - passoy;
      		if(GetKeyState(VK_DOWN)&0x80)
      			y = y + passoy;
      		if(GetKeyState(VK_RIGHT)&0x80)
      			x = x + passox;
      		if(GetKeyState(VK_LEFT)&0x80)
				x = x - passox;	
			
			
			delay(50);
			if (kbhit())
      			tecla = getch();		
        }

        gt2 = GetTickCount();
    }
	closegraph();
	return 0;	
}


