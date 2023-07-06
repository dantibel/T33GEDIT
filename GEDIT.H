/* Belov Danil, DB6, 24.12.2021 */

#ifndef __OPS_H_
#define __OPS_H_

#include "windows.h"

/* Debug memory allocation support */
#ifndef _NDEBUG
#  define _CRTDBG_MAP_ALLOC
#  include <crtdbg.h>
#  define SetDbgMemHooks() \
     _CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF | _CRTDBG_CHECK_ALWAYS_DF | \
       _CRTDBG_ALLOC_MEM_DF | _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG))
#else /* _DEBUG */
#  define SetDbgMemHooks() ((void)0)
#endif /* _DEBUG */
#include <stdlib.h>

typedef DOUBLE DBL;
typedef FLOAT FLT;
typedef enum tagLOGOP LOGOP;

extern DWORD *Frame;
extern INT FrameW, FrameH;

LOGOP SetLogOp( LOGOP NewLogOp );
VOID PutPixel( INT X, INT Y, DWORD Color );
VOID PutLine( INT X1, INT Y1, INT X2, INT Y2, DWORD Color);
VOID PutCircle( INT Xc, INT Yc, INT R, DWORD Color );
DWORD GetColor( INT X, INT Y );
VOID MyFloodFill( INT X, INT Y, DWORD FillColor );
VOID PutBarBy2Points( INT X1, INT Y1, INT X2, INT Y2, DWORD Color );
VOID PutBar( INT X, INT Y, INT w, INT h, DWORD Color );

#endif /*__OPS_H_*/
/* END OF 'GEDIT.H' FILE */