/* Belov Danil, BD6, 25.12.2021 */

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <time.h>
#include <windows.h>

#include "glut.h"

#include "gedit.h"

/* Time variables */
static BOOL IsPause = 0;
static DOUBLE SyncTime, DeltaTime, FPS;

/* Measure delta time function */
static VOID Timer( VOID )
{
  LONG t;
  static LONG StartTime = -1, FPSTime, FrameCount, PauseTime, OldTime;
 
  t = clock();
  if (StartTime == -1)
    StartTime = FPSTime = OldTime = t;
 
  if (!IsPause)
  {
    DeltaTime = (t - OldTime) / (DBL)CLOCKS_PER_SEC;
    SyncTime = (t - StartTime - PauseTime) / (DBL)CLOCKS_PER_SEC;
  }
  else
  {
    DeltaTime = 0;
    PauseTime += t - OldTime;
  } 
  FrameCount++;
  if (t - FPSTime > 3 * CLOCKS_PER_SEC)
  {
    FPS = FrameCount / ((t - FPSTime) / (DBL)CLOCKS_PER_SEC);
    FPSTime = t;
    FrameCount = 0;
  }
  OldTime = t;
} /* End of 'Timer' function */


/* Frame variables */
DWORD *Frame;
INT  FrameW = 800, FrameH = 800, FrameZoom = 1;
static INT WinW, WinH;
static HWND hWnd;
static INT OldX, OldY;
static DWORD DrawColor = 0xFFFFFFFF;
extern enum 
{
  COPY, AND, OR, XOR
} LogOp;

enum 
{
  DROPPER, DRAW_PIXEL, DRAW_CIRCLE, FILL, DRAW_RECT
} mode = DRAW_PIXEL;

/* Select color in menu function */
VOID SelectColor( VOID )
{
  CHOOSECOLOR cc = {0};
  static DWORD CustomColors[16];

  cc.lStructSize = sizeof(CHOOSECOLOR);
  cc.rgbResult = RGB((DrawColor >> 16) & 0xFF, (DrawColor >> 8) & 0xFF, DrawColor & 0xFF);
  cc.hwndOwner = hWnd;
  cc.lpCustColors = CustomColors;
  cc.Flags = CC_ANYCOLOR | CC_FULLOPEN | CC_PREVENTFULLOPEN | CC_RGBINIT; 

  if (ChooseColor(&cc))
  {
    BYTE 
      r = GetRValue(cc.rgbResult),
      g = GetGValue(cc.rgbResult),
      b = GetBValue(cc.rgbResult);

    DrawColor = 0xFF000000 |+ (r << 16) | (g << 8) | b;
  }
} /* End of 'SelectColor' function */   

/* Glut Display function */
VOID Display( VOID )
{
  CHAR Buff[100];
  
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);   
  Timer();

  sprintf(Buff, "FPS: %.3lf", FPS);
  SetWindowText(hWnd, Buff);
  glPixelZoom(FrameZoom, -FrameZoom);
  glRasterPos2d(-1, 1);
  glDrawPixels(FrameW, FrameH, GL_BGRA_EXT, GL_UNSIGNED_BYTE, Frame);

  glFinish();
  glutSwapBuffers();
  glutPostRedisplay();
} /* End of Display function */


/* Set new size of frame function */
VOID FrameSetSize( INT NewW, INT NewH )
{
  if (Frame != NULL)
    free(Frame);

  FrameW = NewW, FrameH = NewH;
  Frame = malloc(FrameH * FrameW * sizeof(DWORD));
  if (Frame == NULL)
    exit(30);
} /* End of 'frameSetSize' function */

/* GLUT Reshape function */
VOID Reshape( INT NewW, INT NewH )
{
  WinW = NewW;
  WinH = NewH;
  FrameSetSize(WinW / FrameZoom, WinH / FrameZoom);
  memset(Frame, 0, FrameW * FrameH * 4);
  glViewport(0, 0, NewW, NewH);
} /* End of 'Reshape' function */

static INT maxX, maxY;
/* GLUT Motion function */
VOID MouseMove( INT X, INT Y )
{
  switch (mode)
  {
  case DRAW_PIXEL:
    SetLogOp(COPY);
    PutLine(OldX, OldY, X / FrameZoom, Y / FrameZoom, DrawColor);
    OldX = X / FrameZoom, OldY = Y / FrameZoom;
    break;
  /*
  case DRAW_RECT:
    SetLogOp(XOR);
    if (X >= OldX && X < maxX || Y >= OldY && Y < maxY || X < OldX && X > maxX || Y < OldY && Y > maxY) 
      PutBarBy2Points(OldX, OldY, maxX, maxY, 0x55000000);
    else
      maxX = X, maxY = Y;
    PutBarBy2Points(OldX, OldY, X / FrameZoom, Y / FrameZoom, 0x55555555);
    break;
    */
  }
} /* End of 'MouseMove' function */

/* GLUT Mouse function */
VOID Mouse( INT Button, INT State, INT X, INT Y )
{
  if (Button == GLUT_LEFT_BUTTON)
    if (State == GLUT_DOWN)
      switch (mode)
      {
      case DRAW_PIXEL:
        //SetLogOp(COPY);
        PutPixel(OldX = X / FrameZoom, OldY = Y / FrameZoom, DrawColor);
        break;
      case DRAW_CIRCLE:
        OldX = X / FrameZoom, OldY = Y / FrameZoom;
        break;
      case FILL:
        //SetLogOp(COPY);
        MyFloodFill(X / FrameZoom, Y / FrameZoom, DrawColor);
        break;
      case DROPPER:
        DrawColor = GetColor(X, Y);
        break;
      case DRAW_RECT:
        //SetLogOp(COPY);
        PutPixel(OldX = X / FrameZoom, OldY = Y / FrameZoom, DrawColor);
        break;
    }
    else if (State == GLUT_UP)
      switch (mode)
      {
      case DRAW_CIRCLE:
        //SetLogOp(COPY);
        PutCircle(X / FrameZoom, Y / FrameZoom, 20, DrawColor);
        break;
      case DRAW_RECT:
        //SetLogOp(COPY);
        PutBarBy2Points(OldX, OldY, X / FrameZoom, Y / FrameZoom, DrawColor);
        OldX = X / FrameZoom, OldY = Y / FrameZoom;
        maxX = OldX, maxY = OldY;
        break;
      }
} /* End of 'Mouse' function */

/* Glut Keyboard func */
VOID Keyboard( UCHAR Key, INT X, INT Y )
{
  switch (Key)
  {
  case 27:
    exit(30);
  case 32:
    IsPause = !IsPause;
    break;
  case 8:
    memset(Frame, 0, FrameW * FrameH * 4);
    break;
  case '+':
    if (FrameZoom < 13)
      FrameZoom++;
    break;
  case '-':
    if (FrameZoom > 1)
      FrameZoom--;
    break;
  case 's':
  case 'S':
    SelectColor();
    break;
  case 'p':
  case 'P':
    mode = DRAW_PIXEL;
    break;
  case 'c':
  case 'C':
    mode = DRAW_CIRCLE;
    break;
  case 'f':
  case 'F':
    mode = FILL;
    break;
  case 'b':
  case 'B':
    mode = DRAW_RECT;
    break;
  case 'd':
  case 'D':
    mode = DROPPER;	
    break;
  case '1':
    SetLogOp(COPY);
    break;
  case '2':
    SetLogOp(AND);
    break;
  case '3':
    SetLogOp(OR);
    break;
  case '4':
    SetLogOp(XOR);
    break;
  }
}

/* GLUT Special function */
VOID Special( INT Key, INT X, INT Y )
{
  INT SaveW = FrameH, SaveH = FrameW;
  static BOOL IsFullScreen = FALSE;

  if (Key == GLUT_KEY_F2)
  {
    if (IsFullScreen)
    {
      Reshape(SaveW, SaveH);
      glutReshapeWindow(SaveW, SaveH);
    }
    else
    {
      SaveW = FrameW;
      SaveH = FrameH;
      glutFullScreen();
    }
    IsFullScreen = !IsFullScreen;
	}
} /* End of 'Special' function */

/* Load image function */
VOID ImageLoad( VOID )
{
  OPENFILENAME ofn = {0};
  static CHAR FileName[300];

  ofn.lStructSize = sizeof(OPENFILENAME);
  ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST;
  ofn.hwndOwner = hWnd;
  ofn.lpstrDefExt = "*.g24";
  ofn.lpstrFile = FileName;
  ofn.nMaxFile = sizeof(FileName);
  ofn.lpstrTitle = "Load Image";
  ofn.lpstrFilter = "G24 images (*.g24)\0*.g24\0All files (*.*)\0*.*\0";
  ofn.nFilterIndex = 1;
  if (GetOpenFileName(&ofn))
  {
    FILE *F;

    if ((F = fopen(FileName, "rb")) != NULL)
    {
      INT w = 0, h = 0;

      fread(&w, 2, 1, F);
      fread(&h, 2, 1, F);
      fseek(F, 0, SEEK_END);
      if (ftell(F) == 4 + 3 * w * h)
      {
        INT x, y;

        fseek(F, 4, SEEK_SET);
        for (y = 0; y < h; y++)
          for (x = 0; x < w; x++)
          {
            BYTE rgb[3];

            fread(rgb, 3, 1, F);
            PutPixel(OldX + x, OldY + y,
              0xFF000000 | (rgb[2] << 16) | (rgb[1] << 8) | rgb[0]);
          }
      }
      fclose(F);
    }
  }
} /* End of 'ImageLoad' function */


/* Menu statments function */
VOID MenuHandle( INT Value )
{
  switch (Value)
  {
  case 0:
    exit(0);
  case 1:
    ImageLoad();
    break;
  case 2:
    break;
  case 3:
    SelectColor();
    break;
  case 4:
    mode = DROPPER;
    break;
  case 5:
    mode = DRAW_PIXEL;
    break;
  case 6:
    mode = DRAW_CIRCLE;
    break;
  case 7:
    mode = FILL;
    break;
  case 8:
    mode = DRAW_RECT;
    break;
  }
} /* End of 'MenuHandle' function */

/* Program main function */
VOID main( INT argc, CHAR *argv[] )
{
  INT menu, file_menu, tools_menu;

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

  FreeConsole();
  glutInitWindowPosition(0, 0);
  glutInitWindowSize(FrameW, FrameH);
  glutCreateWindow("T33GEDIT");
  FrameSetSize(FrameW, FrameH);
  hWnd = FindWindow(NULL, "T33GEDIT");

  glutDisplayFunc(Display);
  glutKeyboardFunc(Keyboard);
  glutSpecialFunc(Special);
  glutReshapeFunc(Reshape);
  glutMouseFunc(Mouse);
  glutMotionFunc(MouseMove);

  file_menu = glutCreateMenu(MenuHandle);
  glutSetMenu(file_menu);
  glutAddMenuEntry("Load image", 1);
  glutAddMenuEntry("Save image", 2);

  tools_menu = glutCreateMenu(MenuHandle);
  glutSetMenu(tools_menu);
  glutAddMenuEntry("Dropper [D]", 4);
  glutAddMenuEntry("Draw pixels [P]", 5);
  // glutAddMenuEntry("Draw circles [C]", 6);
  glutAddMenuEntry("Fill [F]", 7);
  glutAddMenuEntry("Draw rectangles [R]", 8);

  menu = glutCreateMenu(MenuHandle);
  glutSetMenu(menu);
  glutAddMenuEntry("Exit [ESC]", 0);
  glutAddSubMenu("File", file_menu);
  glutAddMenuEntry("Select Color [S]", 3);
  glutAddSubMenu("Tools", tools_menu);
  glutAttachMenu(GLUT_RIGHT_BUTTON);

  glClearColor(0.1, 0.1, 0.1, 1);
  glutMainLoop();
} /* End of 'main' function */



/* END OF 'T33GEDIT' FILE */