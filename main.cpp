#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>


// ── Bresenham Line Drawing Algorithm ──────────
void drawLineBres(int x1,int y1,int x2,int y2){
    int dx=abs(x2-x1),dy=abs(y2-y1);
    int sx=(x1<x2)?1:-1,sy=(y1<y2)?1:-1,err=dx-dy;
    glBegin(GL_POINTS);
    while(1){
        glVertex2i(x1,y1);
        if(x1==x2&&y1==y2) break;
        int e2=2*err;
        if(e2>-dy){err-=dy;x1+=sx;}
        if(e2< dx){err+=dx;y1+=sy;}
    }
    glEnd();
}

// ── Midpoint Circle Algorithm ─────────────────
void drawCircleMidpoint(int cx,int cy,int r){
    int x=0,y=r;
    float d=1.25f-r;
    glBegin(GL_POINTS);
    while(x<=y){
        glVertex2i(cx+x,cy+y); glVertex2i(cx-x,cy+y);
        glVertex2i(cx+x,cy-y); glVertex2i(cx-x,cy-y);
        glVertex2i(cx+y,cy+x); glVertex2i(cx-y,cy+x);
        glVertex2i(cx+y,cy-x); glVertex2i(cx-y,cy-x);
        if(d<0) d+=2*x+3;
        else   {d+=2*(x-y)+5;y--;}
        x++;
    }
    glEnd();
}


// ── OpenGL init ───────────────────────────────
void openGLInit(){
    glClearColor(0,0,0,1);
    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    gluOrtho2D(0,WIN_W,0,WIN_H);
    glMatrixMode(GL_MODELVIEW);  glLoadIdentity();
}

int main(int argc,char**argv){
    srand((unsigned)time(NULL));
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);
    glutInitWindowSize(WIN_W,WIN_H);
    glutInitWindowPosition(100,50);
    glutCreateWindow("DX Ball - CSE 426");
    openGLInit();
     glutMainLoop();
    return 0;
}
