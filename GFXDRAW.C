/* Belov Danil, DB6, 27.12.2021 */

#include "gedit.h"

#define MAX_STORE 1000

typedef enum tagLOGOP 
{
  COPY, AND, OR, XOR
} LOGOP;

LOGOP LogOp = COPY;

/* Set logical operation for drawing function */
LOGOP SetLogOp( LOGOP NewLogOp )
{
  LOGOP old = NewLogOp;

  LogOp = NewLogOp;
  return old;
} /* End of 'SetlogOp' function */;

/* Put pixel in Frame in x, y coordinates func */
static VOID PutPixelCopy( INT X, INT Y, DWORD Color )
{
  Frame[FrameW * Y + X] = Color;  
} /* End of 'PutPixelCopy' function */

/* Put pixel in Frame in x, y coordinates func */
static VOID PutPixelAnd( INT X, INT Y, DWORD Color )
{
  Frame[FrameW * Y + X] &= Color;  
} /* End of 'PutPixelAnd' function */

/* Put pixel in Frame in x, y coordinates func */
static VOID PutPixelOr( INT X, INT Y, DWORD Color )
{
  Frame[FrameW * Y + X] |= Color;  
} /* End of 'PutPixelOr' function */

/* Put pixel in Frame in x, y coordinates func */
static VOID PutPixelXor( INT X, INT Y, DWORD Color )
{
  Frame[FrameW * Y + X] ^= Color;  
} /* End of 'PutPixelXor' function */

static VOID (*PutPixelFuncs[])( INT X, INT Y, DWORD Color ) = 
{
  PutPixelCopy, PutPixelAnd, PutPixelOr, PutPixelXor
};

VOID PutPixel( INT X, INT Y, DWORD Color )
{
  if (Frame == NULL || X < 0 || Y < 0 || X >= FrameW || Y >= FrameH)
    return;

  PutPixelFuncs[LogOp](X, Y, Color);
} /* End of 'PutPixel' function */

/*
VOID PutSpot( INT X, INT Y, INT R, DWORD Color)
{
  INT i = Y;

  while (R-- > 0)
  {
    while (i++ < Y)
      PutPixel(X + R, Y, Color);
  }
}
*/

/* Put line in frame by coordinates */
VOID PutLine( INT X1, INT Y1, INT X2, INT Y2, DWORD Color)
{
  INT dX, dY, tmp, F, IncrE, IncrNE, sX, X, Y, count;

  if (Y1 > Y2)
    tmp = Y1, Y1 = Y2, Y2 = tmp, tmp = X1, X1 = X2, X2 = tmp;
  dY = Y2 - Y1;

  dX = X2 - X1;

  if (dX < 0)
    sX = -1, dX = -dX;
  else 
    sX = 1;

  X = X1; 
  Y = Y1;
  PutPixelCopy(X, Y, Color);
  if (dX >= dY)
  {
    F = 2 * dY - dX;
    IncrE = 2 * dY;
    IncrNE = 2 * dY - 2 * dX;
    count = dX;
    while (count-- > 0)
    {
      if (F > 0)
        Y++, F += IncrNE;
      else 
        F += IncrE;
      X += sX;
      PutPixelCopy(X, Y, Color);
    }
  }
  else 
  {
    F = 2 * dX - dY;
    IncrE = 2 * dX;
    IncrNE = 2 * dX - 2 * dY;
    count = dY;
    while (count-- > 0)
    {
      if (F > 0)
        X += sX, F += IncrNE;
      else 
        F += IncrE;
      Y++;
      PutPixelCopy(X, Y, Color);
    }
  }
} /* End of 'PutLine' function */

/* Put 4 pixels in frame by coordinates function */
static VOID Put4Points( INT Xc, INT Yc, INT R, DWORD Color )
{
  PutPixelCopy(Xc, Yc + R, Color);
  PutPixelCopy(Xc, Yc - R, Color);
  PutPixelCopy(Xc + R, Yc, Color);
  PutPixelCopy(Xc - R, Yc, Color);
} /* End of 'Put4Ponts' function */

/* Put 8 pixels in frame b coordinates function */
static VOID Put8Points( INT Xc, INT Yc, INT X, INT Y, DWORD Color )
{
  /*
  PutPixelApr(Xc + X, Yc + Y, Color);
  PutPixelApr(Xc - X, Yc + Y, Color);
  PutPixelApr(Xc + X, Yc - Y, Color);
  PutPixelApr(Xc - X, Yc - Y, Color);
  PutPixelApr(Xc + Y, Yc + X, Color);
  PutPixelApr(Xc - Y, Yc + X, Color);
  PutPixelApr(Xc + Y, Yc - X, Color);
  PutPixelApr(Xc - Y, Yc - X, Color);
  */
} /* End of 'Put8Points' function */

/* Put circle in frame by center coordinates and radius function */
VOID PutCircle( INT Xc, INT Yc, INT R, DWORD Color )
{
  INT F, X, Y, IncrE = 5 - 2 * R, IncrSE = 3;

  F = 1 - R;
  X = 0, Y = R;
  
  Put4Points(Xc, Yc, R, Color);
  while (X <= Y)
  {
    if (F >= 0)
      Y--, F += 2 * X - 2 * Y + 5;
    else
      F += 2 * X + 3;
    X++;
    Put8Points(Xc, Yc, X, Y, Color);
  }
} /* End of 'PutCircle' function */

/* Get pixel's color function */
DWORD GetColor( INT X, INT Y )
{
  if (Frame == NULL || X < 0 || Y < 0 || X >= FrameW || Y >= FrameH)
    return 0;

  return Frame[FrameW * Y + X];
} /* End of 'GetColor' function */

typedef struct tagPLIST PLIST;
struct tagPLIST
{
  INT X[MAX_STORE], Y[MAX_STORE];
  PLIST *Next;
};

static PLIST *PixelList;
static StoreSize;

static VOID Store( INT X, INT Y )
{
  if (PixelList == NULL || StoreSize >= MAX_STORE)
  {
    PLIST *NewElement = malloc(sizeof(PLIST));
    
    if (NewElement == NULL)
      return;
    NewElement->Next = PixelList;
    PixelList = NewElement;
    StoreSize = 0;
  }
  PixelList->X[StoreSize] = X;
  PixelList->Y[StoreSize] = Y;
  StoreSize++;
}

static BOOL ReStore( INT *X, INT *Y )
{
  if (PixelList == NULL || StoreSize == 0)
    return FALSE;
  
  StoreSize--;
  *X = PixelList->X[StoreSize];
  *Y = PixelList->Y[StoreSize];

  if (StoreSize == 0)
  {
    PLIST *OldElement = PixelList;

    PixelList = PixelList->Next;
    free(OldElement);
    if (PixelList != NULL)
      StoreSize = MAX_STORE;
  }
  return TRUE;
}

/* Fill one color area function */
VOID MyFloodFill( INT X, INT Y, DWORD FillColor )
{
  DWORD BackColor = GetColor(X, Y);

  if (BackColor == FillColor)
    return;
  
  Store(X, Y);
  while (ReStore(&X, &Y))
  {
    if (X >= 0 && Y >= 0 && X < FrameW && Y < FrameH && GetColor(X, Y) == BackColor)
    {
      PutPixel(X, Y, FillColor);
      Store(X - 1, Y);
      Store(X + 1, Y);
      Store(X, Y - 1);
      Store(X, Y + 1);
    }
  }
} /* End of 'MyFloodFill' function */

void swap( int *a, int *b)
{
  int tmp = *a;
  *a = *b;
  *b = tmp;
}

/* Put bar in frame by coordinates function */
VOID PutBarBy2Points( INT X1, INT Y1, INT X2, INT Y2, DWORD Color )
{
  INT w, delta, count;
  DWORD *addr;

  if (Y1 > Y2)
    swap(&Y1, &Y2);
  if (Y2 < 0 || Y1 >= FrameH)
    return;
  if (X1 > X2)
    swap(&X1, &X2);
  if (X2 < 0 || X1 >= FrameW)
    return;

  if (Y1 < 0)
    Y1 = 0;
  if (Y2 >= FrameH)
    Y2 = FrameH - 1;
  if (X1 < 0)
    X1 = 0;
  if (X2 >= FrameW)
    X2 = FrameW - 1;

  addr = &Frame[0 + Y1 * FrameW + X1];
  count = Y2 - Y1 + 1;
  w = X2 - X1 + 1;
  delta = FrameW - w;

  while (count-- > 0)
  {
    INT countx = w;

    while (countx-- > 0)
      *addr++ = Color;
    addr += delta;
  }
} /* End of 'PutBarBy2points' function */

/* Put bar in frame by width and height function */
VOID PutBar( INT X, INT Y, INT w, INT h, DWORD Color )
{
  if (w <= 0 || h <= 0)
    return;
  PutBarBy2Points(X, Y, X + (INT)w - 1, Y + (INT)h - 1, Color);
} /* End of 'PutBar' function */
/* END OF 'GFXDRAW.C '*/