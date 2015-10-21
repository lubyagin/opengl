/*
    Example for OpenGL in Windows

    Автор: Александр Лубягин, lubyagin@yandex.ru
    Опубликовано на сайте GITHUB.COM под двойной лицензией: AGPLv3 or MIT

    Тексты лицензий: http://www.gnu.org/licenses/agpl-3.0.txt
    и http://opensource.org/licenses/mit-license.php

    Графические изображения и фотографии, включенные в проект - под лицензией
    Creative Commons Attribution 4.0 International,
    http://creativecommons.org/licenses/by/4.0/deed.ru

    Примечание: Данный проект не включает сторонний код, и лишь использует
    динамически подключаемые библиотеки OpenGL, GLU, GLUT.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published
    by the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#if defined(__MINGW32__) || defined(__MINGW64__)
#include <windows.h> // CreateThread()
#elif defined(__linux__)
#include <pthread.h> // pthread_create()
#endif

#include <stdio.h>  // printf()
#include <stdlib.h> // exit()
#include <time.h> // clock()

#define __USE_XOPEN_EXTENDED
#include <unistd.h> // usleep()

#include <stdint.h> // uint8_t

#include "gl.h"
#include "glext.h"
#include "glu.h"
#define GLUT_DISABLE_ATEXIT_HACK
#define _STDCALL_SUPPORTED
#ifndef _M_IX86
#define _M_IX86
#endif
#include "glut.h"

long long int FPS = 0;

int W = 800;
int H = 600;

char *filename_i = "example.txt";
char *filename_o = "example.txt";
FILE *f_i, *f_o;

void funcReshape(int w, int h) {
  // coordinate system config
  glViewport(0, 0, W, H);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, W, 0, H);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

int N_X_PALETTE = 10;
int W_PALETTE_BAR = 72;
int H_PALETTE_BAR = 72;

int f_Palette = 1; // TRUE
int f_Grid = 1; // 0 = FALSE

#define N_X_GRID 96
#define N_Y_GRID 64
int W_GRID_CELL = 8;
int H_GRID_CELL = 8;

int x_grid_cell = 7; // save/load session (config\sessions.txt)
int y_grid_cell = 7;

uint8_t a_Canvas[N_Y_GRID][N_X_GRID];

void funcDisplay() {
  glClear(GL_COLOR_BUFFER_BIT);

  // Y-coords at bottom
  if(f_Palette) {
    glColor4f(0.5, 0.5, 0.5, 1.0);
    glBegin(GL_LINES);
    for(int i = 0; i <= N_X_PALETTE; i++) {
      glVertex2f(0.5f + i*W_PALETTE_BAR, 0.5f + H_PALETTE_BAR - 1);
      glVertex2f(0.5f + i*W_PALETTE_BAR, 0.5f + 0);
    }
    glEnd();
    glBegin(GL_LINES);
    glVertex2f(0.5f + 0, 0.5f + H_PALETTE_BAR - 1);
    glVertex2f(0.5f + W_PALETTE_BAR*(N_X_PALETTE), 0.5f + H_PALETTE_BAR - 1);
    glEnd();
    glBegin(GL_LINES);
    glVertex2f(0.5f + 0, 0.5f + 0);
    glVertex2f(0.5f + W_PALETTE_BAR*(N_X_PALETTE), 0.5f + 0); // why 1?
    glEnd();

    for(int i = 0; i <= N_X_PALETTE; i++) {
      glBegin(GL_POLYGON);
      float k = i/(float)N_X_PALETTE;
      glColor4f(k, k, k, 1.0);
      /* #define POINT(...+0.5f) */
      glVertex2f(0.5f + i*W_PALETTE_BAR, 0.5f + H_PALETTE_BAR - 1);
      glVertex2f(0.5f + (i+1)*W_PALETTE_BAR, 0.5f + H_PALETTE_BAR - 1);
      glVertex2f(0.5f + (i+1)*W_PALETTE_BAR, 0.5f + 0);
      glVertex2f(0.5f + i*W_PALETTE_BAR, 0.5f + 0);
      glVertex2f(0.5f + i*W_PALETTE_BAR, 0.5f + H_PALETTE_BAR - 1);
      glEnd();
    }
  }

  if(f_Grid) {
    glEnable(GL_LINE_STIPPLE);
    glLineStipple(1, 0xAAAA);
    glColor4f(0.5, 0.5, 0.5, 1.0);
    glBegin(GL_LINES);
    for(int i = 0; i <= N_X_GRID; i++) {
      glVertex2f(0.5f + i*W_GRID_CELL, 0.5f + H - 1);
      glVertex2f(0.5f + i*W_GRID_CELL, 0.5f + H - 1 - H_GRID_CELL*N_Y_GRID);
    }
    glEnd();
    glBegin(GL_LINES);
    for(int i = 0; i <= N_Y_GRID; i++) {
      glVertex2f(0.5f + 0, 0.5f + H - 1 - i*H_GRID_CELL);
      glVertex2f(0.5f + W_GRID_CELL*N_X_GRID, 0.5f + H - 1 - i*H_GRID_CELL);
    }
    glEnd();
    glDisable(GL_LINE_STIPPLE); // lines shifts in WinXP Professional woth old OpenGL DLL
  }

  glColor4f(1.0, 1.0, 1.0, 1.0);
  glBegin(GL_LINE_LOOP);
  glVertex2f(0.5f + x_grid_cell*W_GRID_CELL, 0.5f + H - 1 - y_grid_cell*H_GRID_CELL);
  glVertex2f(0.5f + (x_grid_cell + 1)*W_GRID_CELL, 0.5f + H - 1 - y_grid_cell*H_GRID_CELL);
  glVertex2f(0.5f + (x_grid_cell + 1)*W_GRID_CELL, 0.5f + H - 1 - (y_grid_cell + 1)*H_GRID_CELL);
  glVertex2f(0.5f + x_grid_cell*W_GRID_CELL, 0.5f + H - 1 - (y_grid_cell + 1)*H_GRID_CELL);
  glVertex2f(0.5f + x_grid_cell*W_GRID_CELL, 0.5f + H - 1 - y_grid_cell*H_GRID_CELL);
  glEnd();

  glDisable(GL_LINE_STIPPLE);

  glColor4f(1.0, 1.0, 1.0, 1.0);
  for(int j = 0; j < N_Y_GRID; j++) {
    for(int i = 0; i < N_X_GRID; i++) {
      /* Black-White points */
      if(a_Canvas[j][i] > 0) {
        glBegin(GL_POLYGON);
        glVertex2f(0.5f + i*W_GRID_CELL + 1, 0.5f + H - 1 - j*H_GRID_CELL - 1);
        glVertex2f(0.5f + (i + 1)*W_GRID_CELL, 0.5f + H - 1 - j*H_GRID_CELL - 1);
        glVertex2f(0.5f + (i + 1)*W_GRID_CELL, 0.5f + H - 1 - (j + 1)*H_GRID_CELL);
        glVertex2f(0.5f + i*W_GRID_CELL + 1, 0.5f + H - 1 - (j + 1)*H_GRID_CELL);
        glVertex2f(0.5f + i*W_GRID_CELL + 1, 0.5f + H - 1 - j*H_GRID_CELL - 1);
        glEnd();
      }
    }
  }

  glutSwapBuffers();
  FPS++;
}

void key_left() {
  x_grid_cell--;
  if(x_grid_cell < 0) x_grid_cell = 0;
  glutPostRedisplay();
}

void key_right() {
  x_grid_cell++;
  if(x_grid_cell > N_X_GRID - 1) x_grid_cell = N_X_GRID - 1;
  glutPostRedisplay();
}

void key_up() {
  y_grid_cell--;
  if(y_grid_cell < 0) y_grid_cell = 0;
  glutPostRedisplay();
}

void key_down() {
  y_grid_cell++;
  if(y_grid_cell > N_Y_GRID - 1) y_grid_cell = N_Y_GRID - 1;
  glutPostRedisplay();
}

void funcSpecial(int k, int x, int y) {
  switch(k) {
    case GLUT_KEY_LEFT: {
      key_left();
      break;
    }
    case GLUT_KEY_RIGHT: {
      key_right();
      break;
    }
    case GLUT_KEY_DOWN: {
      key_down();
      break;
    }
    case GLUT_KEY_UP: {
      key_up();
      break;
    }
  }
}

void funcKeyboard(unsigned char k, int x, int y) {
  switch(k) {
    case 0x1B: {
      exit(0);
    }
    // capital letter (control)
    case 'G': {
      f_Grid = 1 - f_Grid;
      glutPostRedisplay();
      break;
    }
    case 'a': {
      key_left();
      break;
    }
    case 'd': {
      key_right();
      break;
    }
    case 's': {
      key_down();
      break;
    }
    case 'w': {
      key_up();
      break;
    }
    case 'S': {
      for(int j = 0; j < N_Y_GRID; j++) {
        for(int i = 0; i < N_X_GRID; i++) {
          fprintf(f_o, "%c", (a_Canvas[j][i] > 0x00)?'X':' ');
        }
        fprintf(f_o, "\n");
      }
      break;
    }
    // space in ASCII
    case 0x20: {
      // in binary mode
      a_Canvas[y_grid_cell][x_grid_cell] ^= 0x01;
      glutPostRedisplay();
      break;
    }
  }
}

#if defined(__MINGW32__) || defined(__MINGW64__)
DWORD WINAPI funcPrintFPS(LPVOID lpParam) {
#elif defined(__linux__)
void *funcPrintFPS(void *p) {
#endif
  long long int i_was = 0, i_now;
  while(TRUE) {
    i_now = clock(); /* micro- */
    if(i_now >= i_was + CLOCKS_PER_SEC) {
      /* printf("FPS: %lld\n", FPS); */
      FPS = 0;
      i_was = i_now + (i_now - i_was - CLOCKS_PER_SEC);
    }
#if defined(__MINGW32__) || defined(__MINGW64__)
    Sleep(10); /* milliseconds */
#elif defined(__linux__)
    usleep(1000*10); /* micro- */
#endif
  }
}

int main(int argc, char **argv) {

  if(argc > 2) {
    filename_i = argv[1];
    filename_o = argv[2];
  }
  else
    if(argc > 1) {
      filename_i = argv[1];
      filename_o = argv[1];
    }
  f_o = fopen(filename_o, "wt");

  for(int j = 0; j < N_Y_GRID; j++) {
    for(int i = 0; i < N_X_GRID; i++) {
      a_Canvas[j][i] = 0x00; // unsigned char -> uint8_t
    }
  }
  // read filename_i

#if defined(__MINGW32__) || defined(__MINGW64__)
  DWORD dwThreadId, dwThreadParam = 1;
  HANDLE hThread;
  hThread = CreateThread(NULL, 0, funcPrintFPS, &dwThreadParam, 0, &dwThreadId);
  if(hThread == NULL) {
    printf("[Error] CreateThread()\n");
    CloseHandle(hThread);
  }
#elif defined(__linux__)
  pthread_t threadPrintFPS;
  pthread_create(&threadPrintFPS, NULL, funcPrintFPS, NULL);
#endif

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
  glutInitWindowPosition(100, 100);
  glutInitWindowSize(W, H);
  glutCreateWindow("NAME");
  glutDisplayFunc(funcDisplay);
  glutReshapeFunc(funcReshape);
  glutKeyboardFunc(funcKeyboard);
  glutSpecialFunc(funcSpecial);
  glutMainLoop();

  fclose(f_i);
  fclose(f_o);

  return 0;
}
