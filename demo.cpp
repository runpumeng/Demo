#include <stdio.h>
#include <string.h>
#include <chrono>
#include "fssimplewindow.h"
#include "yssimplesound.h"
#include "ysglfontdata.h"
#include <math.h>
#include <time.h>
#include <stdlib.h>


using namespace std;

const int window_height = 800;
const int window_width = 800;
const int sample_bank_length = 8192;
const double YS_PI = 3.1415926;


void ToScreenCoords(int &sx, int &sy, int x, int y) {
    sx = x + window_width/2;
    sy = -(y + window_height/2) + window_height;
}

void DrawWave(int x[], int y[]) {
    srand(time(NULL));
    int a = rand()%256;
    int b = rand()%256;
    int c = rand()%256;
    glColor3ub(a, b, c);
    glBegin(GL_POINTS);
    for(int i = 0; i<sample_bank_length; ++i){
        int sx, sy;
        ToScreenCoords(sx, sy, x[i], y[i]);
        glVertex2i(sx, sy);
    }
    glEnd();
}

void DrawBackground(void) {
    glColor3ub(0,0,0);
    glBegin(GL_QUADS);
    glVertex2i(0,0);
    glVertex2i(window_width, 0);
    glVertex2i(window_width, window_height);
    glVertex2i(0, window_height);
    glEnd();
}

void DisplayText(char text1[], char text2[], char text3[], int ms){
    
    srand(time(NULL));
    int l[3] = {0, 0, 0};
    int nl = 0;
    
    for(int i = 0; text1[i] != 0; ++i){
        l[0] = i;
    }
    for(int i = 0; text2[i] != 0; ++i){
        l[1] = i;
    }
    for(int i = 0; text3[i] != 0; ++i){
        l[2] = i;
    }
    for(int i = 0; i < 3; ++i){
        if(l[i] > 0){
            ++nl;
        }
    }

    int elapsed = 0;
    auto begin = chrono::high_resolution_clock::now();
    while(elapsed < ms){
        FsPollDevice();
        int key = FsInkey();
        if( key == FSKEY_ESC ) {
            break;
        }
        auto cur = chrono::high_resolution_clock::now();
        auto dur = cur - begin;
        elapsed = (int)chrono::duration_cast<chrono::milliseconds>(dur).count();
        glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
        DrawBackground();
        
        int x = rand()%256;
        int y = rand()%256;
        int z = rand()%256;
        
        glColor3ub(x, y, z);
        glRasterPos2i(window_width/2 - l[0]/2*16, window_height/2 - nl*20/2);
        YsGlDrawFontBitmap16x20(text1);
        if(l[1]){
            glRasterPos2i(window_width/2 - l[1]/2*16, window_height/2 - nl*20/2 + 25);
            YsGlDrawFontBitmap16x20(text2);
        }
        if(l[2]){
            glRasterPos2i(window_width/2 - l[2]/2*16, window_height/2 - nl*20/2 + 50);
            YsGlDrawFontBitmap16x20(text3);
        }
        FsSwapBuffers();
        FsSleep(25);
    }
}

int PlaySound(char fName[], int ms){
    YsSoundPlayer player;
    YsSoundPlayer::SoundData wav;
    player.MakeCurrent();
    player.Start();
    
    int curNumSample = 0;
    int lastNumSample = 0;
    int sampleBank_x[sample_bank_length];
    int sampleBank_y[sample_bank_length];
    if( YSOK != wav.LoadWav(fName) ) {
        printf("Couldn't load file");
        return 1;
    }
    int num_channels = wav.GetNumChannel();
    int play_back_rate = wav.PlayBackRate();

    player.PlayOneShot(wav);

    int elapsed = 0;
    auto begin = chrono::high_resolution_clock::now();

    while(YSTRUE==player.IsPlaying(wav) && elapsed < ms){
        FsPollDevice();
        int key = FsInkey();
        if( key == FSKEY_ESC ) {
            break;
        }
        
        auto cur = chrono::high_resolution_clock::now();
        auto dur = cur - begin;
        elapsed = (int)chrono::duration_cast<chrono::milliseconds>(dur).count();
        double s = (double)std::chrono::duration_cast<std::chrono::milliseconds>(dur).count()/1000.0;
        if(s<0){ s = 0; }

        long long int curNumSample = wav.SecToNumSample(s, play_back_rate);
        long long int pastNumSample = (curNumSample - sample_bank_length) < 0 ? 0 : (curNumSample - sample_bank_length) ;
        
        
        for( int i = pastNumSample; i < curNumSample; ++i) {
            int idx = i - pastNumSample;
            sampleBank_x[idx] = wav.GetSignedValueRaw(0, i)/100;
            sampleBank_y[idx] = wav.GetSignedValueRaw(1, i)/100;
            printf("%d,%d\n",sampleBank_x[idx],sampleBank_y[idx]); /////
        }
        
        printf("\n\n\n\n");
        glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
        DrawBackground();
        DrawWave(sampleBank_x, sampleBank_y);
        FsSwapBuffers();
        player.KeepPlaying();
        FsSleep(1);
    }

    player.End();

    return 0;
}



void Drawlogo(void)
{
    glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
    DrawBackground();
    glColor3f(1.0,0.0,0.0);
    glBegin(GL_LINE_STRIP);
    glVertex2i(400,250);
    glVertex2i(200,500);
    glVertex2i(250,500);
    glVertex2i(400,300);
    glVertex2i(600,600);
    glVertex2i(650,600);
    glVertex2i(400,250);
    glEnd();
    
    glBegin(GL_LINE_STRIP);
    glVertex2i(400,350);
    glVertex2i(350,430);
    glVertex2i(250,200);
    glVertex2i(200,200);
    glVertex2i(350,500);
    glVertex2i(400,400);
    glVertex2i(450,500);
    glVertex2i(650,275);
    glVertex2i(600,275);
    glVertex2i(450,430);
    glVertex2i(400,350);
    glEnd();
    FsSwapBuffers();
    FsSleep(3000);
}










void DrawGalaxy(void)
{
    glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
    DrawBackground();
    
    // Galaxy1
    glColor3f(255, 0, 0);
    glBegin(GL_LINE_STRIP);
    glVertex2i(400,350);
    glVertex2i(350,355);
    glVertex2i(300,365);
    glVertex2i(250,380);
    glVertex2i(200,400);
    glVertex2i(250,420);
    glVertex2i(300,435);
    glVertex2i(350,445);
    glVertex2i(400,450);
    glVertex2i(450,445);
    glVertex2i(500,435);
    glVertex2i(550,420);
    glVertex2i(600,400);
    glVertex2i(550,380);
    glVertex2i(500,365);
    glVertex2i(450,355);
    glVertex2i(400,350);
    glEnd();
    glBegin(GL_LINE_LOOP);
    int i;
    for(i=0; i<64; i++)
    {
        double angle=(double)i*YS_PI/32.0;
        double x=400+cos(angle)*70;
        double y=400+sin(angle)*70;
        glVertex2d(x,y);
    }
    glEnd();
    
    // Galaxy2
    glColor3f(0, 255, 0);
    glBegin(GL_LINE_STRIP);
    glVertex2i(600,150);
    glVertex2i(550,155);
    glVertex2i(500,165);
    glVertex2i(450,180);
    glVertex2i(400,200);
    glVertex2i(450,220);
    glVertex2i(500,235);
    glVertex2i(550,245);
    glVertex2i(600,250);
    glVertex2i(650,245);
    glVertex2i(700,235);
    glVertex2i(750,220);
    glVertex2i(800,200);
    glVertex2i(750,180);
    glVertex2i(700,165);
    glVertex2i(650,155);
    glVertex2i(600,150);
    glEnd();
    glBegin(GL_LINE_LOOP);
    for(i=0; i<64; i++)
    {
        double angle=(double)i*YS_PI/32.0;
        double x=600+cos(angle)*70;
        double y=200+sin(angle)*70;
        glVertex2d(x,y);
    }
    glEnd();
    
    // Galaxy 3
    glColor3f(0, 0, 255);
    glBegin(GL_LINE_STRIP);
    glVertex2i(200,250);
    glVertex2i(150,255);
    glVertex2i(100,265);
    glVertex2i(50,280);
    glVertex2i(0,300);
    glVertex2i(50,320);
    glVertex2i(100,335);
    glVertex2i(150,345);
    glVertex2i(200,350);
    glVertex2i(250,345);
    glVertex2i(300,335);
    glVertex2i(350,320);
    glVertex2i(400,300);
    glVertex2i(350,280);
    glVertex2i(300,265);
    glVertex2i(250,255);
    glVertex2i(200,250);
    glEnd();
    glBegin(GL_LINE_LOOP);
    for(i=0; i<64; i++)
    {
        double angle=(double)i*YS_PI/32.0;
        double x=200+cos(angle)*70;
        double y=300+sin(angle)*70;
        glVertex2d(x,y);
    }
    glEnd();
    
    // Galaxy 4
    glColor3f(255, 255, 0);
    glBegin(GL_LINE_STRIP);
    glVertex2i(300,550);
    glVertex2i(250,555);
    glVertex2i(200,565);
    glVertex2i(150,580);
    glVertex2i(100,600);
    glVertex2i(150,620);
    glVertex2i(200,635);
    glVertex2i(250,645);
    glVertex2i(300,650);
    glVertex2i(350,645);
    glVertex2i(400,635);
    glVertex2i(450,620);
    glVertex2i(500,600);
    glVertex2i(450,580);
    glVertex2i(400,565);
    glVertex2i(350,555);
    glVertex2i(300,550);
    glEnd();
    glBegin(GL_LINE_LOOP);
    for(i=0; i<64; i++)
    {
        double angle=(double)i*YS_PI/32.0;
        double x=300+cos(angle)*70;
        double y=600+sin(angle)*70;
        glVertex2d(x,y);
    }
    glEnd();
    
    //star 5
    glColor3f(255, 0, 255);
    glBegin(GL_LINE_STRIP);
    glVertex2i(100,100);
    glVertex2i(200,100);
    glVertex2i(115,200);
    glVertex2i(150,50);
    glVertex2i(185,200);
    glVertex2i(100,100);
    glEnd();
    
    //star 6
    glColor3f(0, 255, 255);
    glBegin(GL_LINE_STRIP);
    glVertex2i(600,600);
    glVertex2i(700,600);
    glVertex2i(615,700);
    glVertex2i(650,550);
    glVertex2i(685,700);
    glVertex2i(600,600);
    glEnd();
    
    
    FsSwapBuffers();
    FsSleep(3000);
}

void DrawAlien(void)
{
    glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
    DrawBackground();
    
    // Alien1
    glColor3f(1, 1, 1);
    glBegin(GL_LINE_LOOP);
    int i;
    for(i=0; i<64; i++)
    {
        double angle=(double)i*YS_PI/32.0;
        double x=400+cos(angle)*100;
        double y=400+sin(angle)*100;
        glVertex2d(x,y);
    }
    glEnd();
    
    glBegin(GL_LINE_LOOP);
    int j;
    for(j=0; j<64; j++)
    {
        double angle=(double)j*YS_PI/32.0;
        double x=360+cos(angle)*20;
        double y=380+sin(angle)*20;
        glVertex2d(x,y);
    }
    glEnd();
    
    glBegin(GL_LINE_LOOP);
    int k;
    for(k=0; k<64; k++)
    {
        double angle=(double)k*YS_PI/32.0;
        double x=440+cos(angle)*20;
        double y=380+sin(angle)*20;
        glVertex2d(x,y);
    }
    glEnd();
    
    glColor3ub(255,255,255);
    glBegin(GL_POLYGON);
    int l;
    for(l=0; l<64; l++)
    {
        double angle=(double)l*YS_PI/32.0;
        double x=355+cos(angle)*5;
        double y=375+sin(angle)*5;
        glVertex2d(x,y);
    }
    glEnd();
    
    glBegin(GL_POLYGON);
    int m;
    for(m=0; m<64; m++)
    {
        double angle=(double)m*YS_PI/32.0;
        double x=435+cos(angle)*5;
        double y=375+sin(angle)*5;
        glVertex2d(x,y);
    }
    glEnd();
    
    glColor3f(1, 1, 1);
    glBegin(GL_LINE_STRIP);
    glVertex2i(380,420);
    glVertex2i(420,420);
    glEnd();
    
    glColor3f(1, 1, 1);
    glBegin(GL_LINE_STRIP);
    glVertex2i(360,310);
    glVertex2i(360,250);
    glEnd();
    
    glColor3f(1, 1, 1);
    glBegin(GL_LINE_STRIP);
    glVertex2i(440,310);
    glVertex2i(440,250);
    glEnd();
    
    glColor3ub(255,255,255);
    glBegin(GL_POLYGON);
    int n;
    for(n=0; n<64; n++)
    {
        double angle=(double)n*YS_PI/32.0;
        double x=360+cos(angle)*10;
        double y=240+sin(angle)*10;
        glVertex2d(x,y);
    }
    glEnd();
    
    glColor3ub(255,255,255);
    glBegin(GL_POLYGON);
    int t;
    for(t=0; t<64; t++)
    {
        double angle=(double)t*YS_PI/32.0;
        double x=440+cos(angle)*10;
        double y=240+sin(angle)*10;
        glVertex2d(x,y);
    }
    glEnd();
    
    
    // Alien2
    glColor3f(1, 0, 0);
    glBegin(GL_LINE_LOOP);
    for(i=0; i<64; i++)
    {
        double angle=(double)i*YS_PI/32.0;
        double x=600+cos(angle)*100;
        double y=600+sin(angle)*100;
        glVertex2d(x,y);
    }
    glEnd();
    
    glBegin(GL_LINE_LOOP);
    for(j=0; j<64; j++)
    {
        double angle=(double)j*YS_PI/32.0;
        double x=560+cos(angle)*20;
        double y=580+sin(angle)*20;
        glVertex2d(x,y);
    }
    glEnd();
    
    glBegin(GL_LINE_LOOP);
    for(k=0; k<64; k++)
    {
        double angle=(double)k*YS_PI/32.0;
        double x=640+cos(angle)*20;
        double y=580+sin(angle)*20;
        glVertex2d(x,y);
    }
    glEnd();
    
    glColor3ub(255,0,0);
    glBegin(GL_POLYGON);
    for(l=0; l<64; l++)
    {
        double angle=(double)l*YS_PI/32.0;
        double x=555+cos(angle)*5;
        double y=575+sin(angle)*5;
        glVertex2d(x,y);
    }
    glEnd();
    
    glBegin(GL_POLYGON);
    for(m=0; m<64; m++)
    {
        double angle=(double)m*YS_PI/32.0;
        double x=635+cos(angle)*5;
        double y=575+sin(angle)*5;
        glVertex2d(x,y);
    }
    glEnd();
    
    glColor3f(1, 0, 0);
    glBegin(GL_LINE_STRIP);
    glVertex2i(580,620);
    glVertex2i(620,620);
    glEnd();
    
    glColor3f(1, 0, 0);
    glBegin(GL_LINE_STRIP);
    glVertex2i(560,510);
    glVertex2i(560,450);
    glEnd();
    
    glColor3f(1, 0, 0);
    glBegin(GL_LINE_STRIP);
    glVertex2i(640,510);
    glVertex2i(640,450);
    glEnd();
    
    glColor3ub(255,0,0);
    glBegin(GL_POLYGON);
    for(n=0; n<64; n++)
    {
        double angle=(double)n*YS_PI/32.0;
        double x=560+cos(angle)*10;
        double y=440+sin(angle)*10;
        glVertex2d(x,y);
    }
    glEnd();
    
    glColor3ub(255,0,0);
    glBegin(GL_POLYGON);
    for(t=0; t<64; t++)
    {
        double angle=(double)t*YS_PI/32.0;
        double x=640+cos(angle)*10;
        double y=440+sin(angle)*10;
        glVertex2d(x,y);
    }
    glEnd();
    
    
    // Alien3
    glColor3f(0, 1, 0);
    glBegin(GL_LINE_LOOP);
    for(i=0; i<64; i++)
    {
        double angle=(double)i*YS_PI/32.0;
        double x=200+cos(angle)*100;
        double y=200+sin(angle)*100;
        glVertex2d(x,y);
    }
    glEnd();
    
    glBegin(GL_LINE_LOOP);
    for(j=0; j<64; j++)
    {
        double angle=(double)j*YS_PI/32.0;
        double x=160+cos(angle)*20;
        double y=180+sin(angle)*20;
        glVertex2d(x,y);
    }
    glEnd();
    
    glBegin(GL_LINE_LOOP);
    for(k=0; k<64; k++)
    {
        double angle=(double)k*YS_PI/32.0;
        double x=240+cos(angle)*20;
        double y=180+sin(angle)*20;
        glVertex2d(x,y);
    }
    glEnd();
    
    glColor3ub(0,255,0);
    glBegin(GL_POLYGON);
    for(l=0; l<64; l++)
    {
        double angle=(double)l*YS_PI/32.0;
        double x=155+cos(angle)*5;
        double y=175+sin(angle)*5;
        glVertex2d(x,y);
    }
    glEnd();
    
    glBegin(GL_POLYGON);
    for(m=0; m<64; m++)
    {
        double angle=(double)m*YS_PI/32.0;
        double x=235+cos(angle)*5;
        double y=175+sin(angle)*5;
        glVertex2d(x,y);
    }
    glEnd();
    
    glColor3f(0, 1, 0);
    glBegin(GL_LINE_STRIP);
    glVertex2i(180,220);
    glVertex2i(220,220);
    glEnd();
    
    glColor3f(0, 1, 0);
    glBegin(GL_LINE_STRIP);
    glVertex2i(160,110);
    glVertex2i(160,50);
    glEnd();
    
    glColor3f(0, 1, 0);
    glBegin(GL_LINE_STRIP);
    glVertex2i(240,110);
    glVertex2i(240,50);
    glEnd();
    
    glColor3ub(0,255,0);
    glBegin(GL_POLYGON);
    for(n=0; n<64; n++)
    {
        double angle=(double)n*YS_PI/32.0;
        double x=160+cos(angle)*10;
        double y=40+sin(angle)*10;
        glVertex2d(x,y);
    }
    glEnd();
    
    glColor3ub(0,255,0);
    glBegin(GL_POLYGON);
    for(t=0; t<64; t++)
    {
        double angle=(double)t*YS_PI/32.0;
        double x=240+cos(angle)*10;
        double y=40+sin(angle)*10;
        glVertex2d(x,y);
    }
    glEnd();
    
    
    // Alien4
    glColor3f(0, 0, 1);
    glBegin(GL_LINE_LOOP);
    for(i=0; i<64; i++)
    {
        double angle=(double)i*YS_PI/32.0;
        double x=200+cos(angle)*100;
        double y=600+sin(angle)*100;
        glVertex2d(x,y);
    }
    glEnd();
    
    glBegin(GL_LINE_LOOP);
    for(j=0; j<64; j++)
    {
        double angle=(double)j*YS_PI/32.0;
        double x=160+cos(angle)*20;
        double y=580+sin(angle)*20;
        glVertex2d(x,y);
    }
    glEnd();
    
    glBegin(GL_LINE_LOOP);
    for(k=0; k<64; k++)
    {
        double angle=(double)k*YS_PI/32.0;
        double x=240+cos(angle)*20;
        double y=580+sin(angle)*20;
        glVertex2d(x,y);
    }
    glEnd();
    
    glColor3ub(0,0,255);
    glBegin(GL_POLYGON);
    for(l=0; l<64; l++)
    {
        double angle=(double)l*YS_PI/32.0;
        double x=155+cos(angle)*5;
        double y=575+sin(angle)*5;
        glVertex2d(x,y);
    }
    glEnd();
    
    glBegin(GL_POLYGON);
    for(m=0; m<64; m++)
    {
        double angle=(double)m*YS_PI/32.0;
        double x=235+cos(angle)*5;
        double y=575+sin(angle)*5;
        glVertex2d(x,y);
    }
    glEnd();
    
    glColor3f(0, 0, 1);
    glBegin(GL_LINE_STRIP);
    glVertex2i(180,620);
    glVertex2i(220,620);
    glEnd();
    
    glColor3f(0, 0, 1);
    glBegin(GL_LINE_STRIP);
    glVertex2i(160,510);
    glVertex2i(160,450);
    glEnd();
    
    glColor3f(0, 0, 1);
    glBegin(GL_LINE_STRIP);
    glVertex2i(240,510);
    glVertex2i(240,450);
    glEnd();
    
    glColor3ub(0,0,255);
    glBegin(GL_POLYGON);
    for(n=0; n<64; n++)
    {
        double angle=(double)n*YS_PI/32.0;
        double x=160+cos(angle)*10;
        double y=440+sin(angle)*10;
        glVertex2d(x,y);
    }
    glEnd();
    
    glColor3ub(0,0,255);
    glBegin(GL_POLYGON);
    for(t=0; t<64; t++)
    {
        double angle=(double)t*YS_PI/32.0;
        double x=240+cos(angle)*10;
        double y=440+sin(angle)*10;
        glVertex2d(x,y);
    }
    glEnd();
    
    // Alien5
    glColor3f(1, 1, 0);
    glBegin(GL_LINE_LOOP);
    for(i=0; i<64; i++)
    {
        double angle=(double)i*YS_PI/32.0;
        double x=600+cos(angle)*100;
        double y=200+sin(angle)*100;
        glVertex2d(x,y);
    }
    glEnd();
    
    glBegin(GL_LINE_LOOP);
    for(j=0; j<64; j++)
    {
        double angle=(double)j*YS_PI/32.0;
        double x=560+cos(angle)*20;
        double y=180+sin(angle)*20;
        glVertex2d(x,y);
    }
    glEnd();
    
    glBegin(GL_LINE_LOOP);
    for(k=0; k<64; k++)
    {
        double angle=(double)k*YS_PI/32.0;
        double x=640+cos(angle)*20;
        double y=180+sin(angle)*20;
        glVertex2d(x,y);
    }
    glEnd();
    
    glColor3ub(255,255,0);
    glBegin(GL_POLYGON);
    for(l=0; l<64; l++)
    {
        double angle=(double)l*YS_PI/32.0;
        double x=555+cos(angle)*5;
        double y=175+sin(angle)*5;
        glVertex2d(x,y);
    }
    glEnd();
    
    glBegin(GL_POLYGON);
    for(m=0; m<64; m++)
    {
        double angle=(double)m*YS_PI/32.0;
        double x=635+cos(angle)*5;
        double y=175+sin(angle)*5;
        glVertex2d(x,y);
    }
    glEnd();
    
    glColor3f(1, 1, 0);
    glBegin(GL_LINE_STRIP);
    glVertex2i(580,220);
    glVertex2i(620,220);
    glEnd();
    
    glColor3f(1, 1, 0);
    glBegin(GL_LINE_STRIP);
    glVertex2i(560,110);
    glVertex2i(560,50);
    glEnd();
    
    glColor3f(1, 1, 0);
    glBegin(GL_LINE_STRIP);
    glVertex2i(640,110);
    glVertex2i(640,50);
    glEnd();
    
    glColor3ub(255,255,0);
    glBegin(GL_POLYGON);
    for(n=0; n<64; n++)
    {
        double angle=(double)n*YS_PI/32.0;
        double x=560+cos(angle)*10;
        double y=40+sin(angle)*10;
        glVertex2d(x,y);
    }
    glEnd();
    
    glColor3ub(255,255,0);
    glBegin(GL_POLYGON);
    for(t=0; t<64; t++)
    {
        double angle=(double)t*YS_PI/32.0;
        double x=640+cos(angle)*10;
        double y=40+sin(angle)*10;
        glVertex2d(x,y);
    }
    glEnd();
    
    FsSwapBuffers();
    FsSleep(3000);
}




int main(void)
{
    FsChangeToProgramDir();
    FsOpenWindow(0, 0, window_width, window_height, 1);

    DisplayText("Hi Bro!","","", 3000);
    DisplayText("What do you think Universe it is?","The Galaxy?","Illusion?", 4000);
    DrawGalaxy();
    DisplayText("Or?","","Alien!!!", 3000);
    DrawAlien();
    DisplayText("Everyone has their own opinion,","","Their own universe",3000);
    DisplayText("Anyway,","Do you know a famous DJ master","AW?", 3000);
    DisplayText("Let me show you his logo ","","Can you recognize it? ",3000);
    Drawlogo();
    DisplayText("That's right, Alan Walker!", "His most famous song is...?", "Fade!", 3000);
    DisplayText("So, if...", "The Galaxy meet Alan Walker?", "That must be a magic and wonderful show!", 4000);
    DisplayText("I called this <<Alan's Universe>>","","Please just enjoy it!",3000);
    PlaySound("./Fade.wav",130000);
    DisplayText("The Starsky will fade in the end", "But our dream will never be!", "Thank you for your watching - runpum", 4000);
    

    return 0;
}
