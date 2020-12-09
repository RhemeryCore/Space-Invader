//-----------------------------------------------------------------------------
// File: tea2.cpp
//
// Desc: In this tutorial, we are rendering some tea2. This introduces the
//       concept of the vertex buffer, a Direct3D object used to store
//       tea2. tea2 can be defined any way we want by defining a
//       custom structure and a custom FVF (flexible vertex format). In this
//       tutorial, we are using tea2 that are transformed (meaning they
//       are already in 2D window coordinates) and lit (meaning we are not
//       using Direct3D lighting, but are supplying our own colors).
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma warning(disable:4995)

#include "leetlib.h"
#include <d3d9.h>
#include <d3dx9.h>
#include <strsafe.h>
#include <math.h>
#include <map>
#include <direct.h>
#include <malloc.h>
//#include "resource.h"

//#include "joypad.h"


#include "fmod/api/inc/fmod.h"
#include <vector>
#include <sstream>
//#include "../main.cpp"
#pragma comment(lib,"lib/fmod/api/lib/fmodvc.lib")
#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")


#pragma warning(disable:4244)

//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------
LPDIRECT3D9             g_pD3D = NULL; // Used to create the D3DDevice
LPDIRECT3DDEVICE9       g_pd3dDevice = NULL; // Our rendering device
LPDIRECT3DVERTEXBUFFER9 g_pVB = NULL; // Buffer to hold tea2
bool fullscreen;
// A structure for our custom vertex type
struct CUSTOMVERTEX
{
	FLOAT x, y, z, rhw; // The transformed position for the vertex
	DWORD color;        // The vertex color
	float u, v;
};

// Our custom FVF, which describes our custom vertex structure
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1)

std::map<int, LPD3DXFONT> fonts;
int lasttextwidth, lasttextheight;
int intextbatch = 0;
LPD3DXSPRITE fontsprite;
#define MA_RELEASE(x) {int c=0;if (x) c=(x)->Release();x=NULL;}
void ReleaseFonts();
typedef unsigned int u32;



LPD3DXFONT FindFont(int size)
{
	std::map<int, LPD3DXFONT>::iterator fit = fonts.find(size);
	if (fit == fonts.end())
	{
		LPD3DXFONT m_pFont = NULL;
		D3DXCreateFont(g_pd3dDevice, size, 0, FW_NORMAL, 0, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Lucida Console"), &m_pFont);
		fit = fonts.insert(std::pair<int, LPD3DXFONT>(size, m_pFont)).first;
		//if (intextbatch) fit->second->Begin();
	}
	if (fit != fonts.end())
	{
		return fit->second;
	}
	else return NULL;
}

void ReleaseFonts()
{
	if (intextbatch) EndTextBatch();
	for (std::map<int, LPD3DXFONT>::iterator it = fonts.begin(); it != fonts.end(); ++it)
	{
		MA_RELEASE(it->second);
	}
	fonts.clear();
	MA_RELEASE(fontsprite);
	intextbatch = 0;
}

void StartTextBatch(int size)
{
	if (intextbatch) EndTextBatch();
	intextbatch = 1;
	if (size) FindFont(size);
	if (!fontsprite)
	{
		D3DXCreateSprite(g_pd3dDevice, &fontsprite);
	}
	if (fontsprite)
	{
		fontsprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_TEXTURE);
	}
	//for (std::map<int, LPD3DXFONT>::iterator fit=fonts.begin();fit!=fonts.end();++fit) fit->second->Begin();


}

void EndTextBatch()
{
	if (intextbatch)
	{
		intextbatch = 0;
		if (fontsprite)
		{
			fontsprite->End();
		}
		//for (std::map<int, LPD3DXFONT>::iterator fit=fonts.begin();fit!=fonts.end();++fit) fit->second->End();		
	}
}



int DrawSomeText(int x, int y, int size, int col, bool centered, const char* pFormat, ...)
{
	char debugText[8192];
	va_list	parameter;
	va_start(parameter, pFormat);
	vsprintf(debugText, pFormat, parameter);

	LPD3DXFONT m_pFont = FindFont(size);
	if (!m_pFont) return 0;
	RECT r = { x,y,x + 100,y + 100 };

	m_pFont->DrawText(intextbatch ? fontsprite : NULL, debugText, -1, &r, DT_CALCRECT, 0);
	int wid = r.right - r.left;
	if (centered) r.left -= wid / 2;
	if (centered) r.right -= wid / 2;
	m_pFont->DrawText(intextbatch ? fontsprite : NULL, debugText, -1, &r, DT_TOP | DT_LEFT, col);
	lasttextheight = r.bottom - r.top;
	lasttextwidth = r.right - r.left;
	return lasttextheight;


}

/*
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::map<int, LPD3DXFONT> fonts;
int lasttextwidth,lasttextheight;
int intextbatch=0;
LPD3DXSPRITE fontsprite;
#define MA_RELEASE(x) {int c=0;if (x) c=(x)->Release();x=NULL;}
void ReleaseFonts();
typedef unsigned int u32;

void StartTextBatch(int size=0);
//int DrawText(int x, int y, int size, u32 col, bool cenetered, const char *pFormat, ...);


void EndTextBatch();

LPD3DXFONT FindFont(int size)
{
	std::map<int, LPD3DXFONT>::iterator fit=fonts.find(size);
	if (fit==fonts.end())
	{
		LPD3DXFONT m_pFont=NULL;
		D3DXCreateFont(g_pd3dDevice,size,0,FW_NORMAL,0,FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Lucida Console"), &m_pFont );
		fit=fonts.insert(std::pair<int,LPD3DXFONT>(size,m_pFont)).first;
		//if (intextbatch) fit->second->Begin();
	}
	if (fit!=fonts.end())
	{
		return fit->second;
	}
	else return NULL;
}

void ReleaseFonts()
{
	if (intextbatch) EndTextBatch();
	for (std::map<int, LPD3DXFONT>::iterator it=fonts.begin();it!=fonts.end();++it)
	{
		MA_RELEASE(it->second);
	}
	fonts.clear();
	MA_RELEASE(fontsprite);
	intextbatch=0;
}

void StartTextBatch(int size)
{
	if (intextbatch) EndTextBatch();
	intextbatch=1;
	if (size) FindFont(size);
	if (!fontsprite)
	{
		D3DXCreateSprite( g_pd3dDevice, &fontsprite);
	}
	if (fontsprite)
	{
		fontsprite->Begin( D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_TEXTURE );
	}
	//for (std::map<int, LPD3DXFONT>::iterator fit=fonts.begin();fit!=fonts.end();++fit) fit->second->Begin();


}

void EndTextBatch()
{
	if (intextbatch)
	{
		intextbatch=0;
		if (fontsprite)
		{
			fontsprite->End();
		}
		//for (std::map<int, LPD3DXFONT>::iterator fit=fonts.begin();fit!=fonts.end();++fit) fit->second->End();
	}
}



int DrawSomeText(int x, int y, int size, int col, bool centered, const char *pFormat, ...)
{
	char debugText[8192];
	va_list	parameter;
	va_start(parameter,pFormat);
	vsprintf(debugText,pFormat,parameter);

	LPD3DXFONT m_pFont=FindFont(size);
	if (!m_pFont) return 0;
	RECT r={x,y,x+100,y+100};

	m_pFont->DrawText(intextbatch?fontsprite:NULL,debugText,-1,&r,DT_CALCRECT,0);
	int wid=r.right-r.left;
	if (centered) r.left-=wid/2;
	if (centered) r.right-=wid/2;
	m_pFont->DrawText(intextbatch?fontsprite:NULL,debugText,-1,&r,DT_TOP|DT_LEFT,col);
	lasttextheight=r.bottom-r.top;
	lasttextwidth=r.right-r.left;
	return lasttextheight;


}

*/
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



//-----------------------------------------------------------------------------
// Name: InitD3D()
// Desc: Initializes Direct3D
//-----------------------------------------------------------------------------
HRESULT InitD3D(HWND hWnd)
{
	// Create the D3D object.
	if (NULL == (g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)))
		return E_FAIL;

	// Set up the structure used to create the D3DDevice
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = !fullscreen;
	d3dpp.SwapEffect = fullscreen ? D3DSWAPEFFECT_FLIP : D3DSWAPEFFECT_DISCARD;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
	d3dpp.BackBufferFormat = fullscreen ? D3DFMT_A8R8G8B8 : D3DFMT_UNKNOWN;
	d3dpp.BackBufferWidth = 800;
	d3dpp.BackBufferHeight = 600;
	d3dpp.FullScreen_RefreshRateInHz = fullscreen ? 60 : 0;

	// Create the D3DDevice
	if (FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp, &g_pd3dDevice)))
	{
		return E_FAIL;
	}

	// Device state would normally be set here

	return S_OK;
}




//-----------------------------------------------------------------------------
// Name: InitVB()
// Desc: Creates a vertex buffer and fills it with our tea2. The vertex
//       buffer is basically just a chuck of memory that holds tea2. After
//       creating it, we must Lock()/Unlock() it to fill it. For indices, D3D
//       also uses index buffers. The special thing about vertex and index
//       buffers is that they can be created in device memory, allowing some
//       cards to process them in hardware, resulting in a dramatic
//       performance gain.
//-----------------------------------------------------------------------------
HRESULT InitVB()
{

	if (FAILED(g_pd3dDevice->CreateVertexBuffer(4 * sizeof(CUSTOMVERTEX),
		D3DUSAGE_DYNAMIC, D3DFVF_CUSTOMVERTEX,
		D3DPOOL_DEFAULT, &g_pVB, NULL)))
	{
		return E_FAIL;
	}



	//D3DXCreateTextureFromFile(g_pd3dDevice,"bg.jpg",&g_bgtex);
	//D3DXCreateTextureFromFile(g_pd3dDevice,"arrow.png",&g_arrow);


	return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Cleanup()
// Desc: Releases all previously initialized objects
//-----------------------------------------------------------------------------
VOID Cleanup()
{
	if (g_pVB != NULL)
		g_pVB->Release();

	if (g_pd3dDevice != NULL)
		g_pd3dDevice->Release();

	if (g_pD3D != NULL)
		g_pD3D->Release();


}




//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Draws the scene
//-----------------------------------------------------------------------------

/*
float RotateU(float u, float v, float a)
{
	u=u-0.5;
	v=v-0.5;
	return u*cos(a)-v*sin(a)+0.5;
}

float RotateV(float u, float v, float a)
{
	u=u-0.5;
	v=v-0.5;
	return u*sin(a)+v*cos(a)+0.5;
}

void DrawQuad(float x1, float y1, float x2, float y2, DWORD col=0xffffffff, float a=0)
{

	CUSTOMVERTEX tea2[] =
	{
		{ x1, y1, 0.5f, 1.0f, col, RotateU(0,0,a), RotateV(0,0,a) }, // x, y, z, rhw, color
		{ x2, y1, 0.5f, 1.0f, col, RotateU(1,0,a), RotateV(1,0,a)},
		{ x1, y2, 0.5f, 1.0f, col, RotateU(0,1,a), RotateV(0,1,a)},
		{ x2, y2, 0.5f, 1.0f, col, RotateU(1,1,a), RotateV(1,1,a)},
	};

	VOID* ptea2;
	if( FAILED( g_pVB->Lock( 0, sizeof(tea2), (void**)&ptea2, D3DLOCK_DISCARD ) ) )
		return ;
	memcpy( ptea2, tea2, sizeof(tea2) );
	g_pVB->Unlock();


	g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
}

void DrawTri(float x1, float y1, float x2, float y2, float x3, float y3, DWORD col=0xffffffff)
{
	//OutputDebugString("triangle!");

	CUSTOMVERTEX tea2[] =
	{
		{ x1, y1, 0.5f, 1.0f, col, 0,0 }, // x, y, z, rhw, color
		{ x2, y2, 0.5f, 1.0f, col, 0,0},
		{ x3, y3, 0.5f, 1.0f, col, 0,0},
		{ x3, y3, 0.5f, 1.0f, col, 0,0},
	};

	VOID* ptea2=NULL;
	if( FAILED( g_pVB->Lock( 0, sizeof(tea2), (void**)&ptea2, D3DLOCK_DISCARD ) ) )
	{
		OutputDebugString("lock failed");
		return ;
	}
	memcpy( ptea2, tea2, sizeof(tea2) );
	g_pVB->Unlock();


	if (FAILED(g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 1 )))
	{
		OutputDebugString("draw failed");
	}
}



/*
VOID Render()
{
	// Clear the backbuffer to a blue color
	g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(128,128,128), 1.0f, 0 );
	g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,true);

	g_pd3dDevice->SetSamplerState(0,D3DSAMP_MAGFILTER,D3DTEXF_LINEAR);
	g_pd3dDevice->SetSamplerState(0,D3DSAMP_MINFILTER,D3DTEXF_LINEAR);
	g_pd3dDevice->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_DIFFUSE);
	g_pd3dDevice->SetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_TEXTURE);
	g_pd3dDevice->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_MODULATE);

	g_pd3dDevice->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_DIFFUSE);
	g_pd3dDevice->SetTextureStageState(0,D3DTSS_ALPHAARG2,D3DTA_TEXTURE);
	g_pd3dDevice->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_MODULATE);


	static float spinpos=0;
	static float spinv=0;
	static bool spinning=false;
	static bool nomorespin=false;
	srand(GetTickCount());
	// Begin the scene
	if( SUCCEEDED( g_pd3dDevice->BeginScene() ) )
	{
		// Draw the triangles in the vertex buffer. This is broken into a few
		// steps. We are passing the tea2 down a "stream", so first we need
		// to specify the source of that stream, which is our vertex buffer. Then
		// we need to let D3D know what vertex shader to use. Full, custom vertex
		// shaders are an advanced topic, but in most cases the vertex shader is
		// just the FVF, so that D3D knows what type of tea2 we are dealing
		// with. Finally, we call DrawPrimitive() which does the actual rendering
		// of our geometry (in this case, just one triangle).
		g_pd3dDevice->SetStreamSource( 0, g_pVB, 0, sizeof(CUSTOMVERTEX) );
		g_pd3dDevice->SetFVF( D3DFVF_CUSTOMVERTEX );

		g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
		g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
		g_pd3dDevice->SetTexture(0,g_bgtex);
		DrawQuad(0,0,1280,1024);



		g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_DESTCOLOR);
		g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_SRCCOLOR);
		g_pd3dDevice->SetTexture(0,NULL);

		float totw=0;
		float temp[10];
		for (int n=0;n<numpeople;n++)
		{
			// ratio weights:
			if (GetAsyncKeyState('R')<0)
			{
				temp[n]= (1.f+teas[n].consumed) / (1.f+teas[n].madecups);
			}
			else
			{
				// dave weights
				temp[n]= powf(1.2f, teas[n].consumed - teas[n].madecups - teas[n].maderounds);
			}


			totw+=temp[n];
		}
		angles[0]=0;
		for (int n=0;n<numpeople;n++)
		{
			temp[n]=temp[n]/totw*PI*2;
			angles[n+1]=angles[n]+temp[n];
		}
		angles[numpeople]=PI*2;

		for (int n=0;n<numpeople;n++)
		{
			int red=sinf(n*PI*2/numpeople)*100+128;
			int green=sinf(n*PI*2/numpeople+PI*2/3)*100+128;
			int blue=sinf(n*PI*2/numpeople+PI*4/3)*100+128;
			DWORD col=(red<<16)+(green<<8)+(blue);
			float t1=0,t2=0;
			for (int i=0;i<16;i++)
			{
				t1=(i/16.f)*temp[n]+angles[n];
				t2=((i+1)/16.f)*temp[n]+angles[n];

				DrawTri(640,512,640+500*sinf(t1),512-500*cosf(t1),640+500*sinf(t2),512-500*cosf(t2),col|0xff000000);

			}
		}

		//exit(1);

		g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
		g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
		g_pd3dDevice->SetTexture(0,g_arrow);

		for (int n=0;n<numpeople;n++)
		{
			float t1=(0.5)*temp[n]+angles[n];
			DrawText(642+300*sinf(t1),514-300*cosf(t1),40,0xcf000000,true,teas[n].name);
			DrawText(640+300*sinf(t1),512-300*cosf(t1),40,0xffffffff,true,teas[n].name);
		}
		for (int c1=0;c1<64;c1++)
		{
			float arrowsize=384;
			DrawQuad(640-arrowsize,512-arrowsize,640+arrowsize,512+arrowsize,0x08ffffff, -spinpos-spinv*c1/64.f);
		}
		//if (GetAsyncKeyState(VK_ESCAPE)&1) PostQuitMessage(0);
		if (GetAsyncKeyState(VK_BACK)<0)
		{
			spinv*=0.8;
		}
		if (GetAsyncKeyState(VK_SPACE)<0)
		{
			if (nomorespin==false) spinv+=0.01+rand()/3276800.f;
			if (spinv>0.2) spinning=true;
		}
		else if (spinning) nomorespin=true;
		spinpos+=spinv;
		spinv*=0.99f;
		if (spinv<0.001f)
		{
			spinv=0;
			if (spinning)
			{
				spinning=0;
				while (spinpos<0) spinpos+=PI*2;
				while (spinpos>=PI*2) spinpos-=PI*2;
				int winner=0;
				for (winner=0;winner<numpeople;winner++) if (spinpos>=angles[winner] && spinpos<angles[winner+1]) break;
				winner=(winner%numpeople);



				sprintf(winnerbuf,"%s is the winner! MAKE SOME TEA BITCH!",teas[winner].name);
				if (DialogBox(GetModuleHandle(NULL),MAKEINTRESOURCE(IDD_DIALOG1),NULL,(DLGPROC)mydialogproc)==IDOK && numcups>0)

				//if (MessageBox(GetForegroundWindow(),buf,"tea!",MB_ICONEXCLAMATION|MB_OKCANCEL)==IDOK)
				{
					teas[winner].maderounds++;
					teas[winner].madecups+=numcups;
					writetea();
				}

				nomorespin=false;
			}
		}
		if (spinpos>PI*2) spinpos-=PI*2;

		if (GetAsyncKeyState(VK_SHIFT)<0)
		{
			for (int c1=0;c1<numpeople;c1++)
			{
				char buf[1024];sprintf(buf,"%9s made %2d rounds, %3d cups, drank %3d cups",teas[c1].name,teas[c1].maderounds,teas[c1].madecups,teas[c1].consumed);
				DrawText(2,2+c1*24,20,0x80000000,false,buf);
				DrawText(0,0+c1*24,20,0xffffffff,false,buf);

			}
		}
		else
		{
			DrawText(0, 0,20,0xffffffff,false,"Press SPACE to spin!");
			DrawText(0,24,20,0xffffffff,false,"Press BACKSPACE to brake");
			DrawText(0,48,20,0xffffffff,false,"Press SHIFT for stats");

		}

		// End the scene
		g_pd3dDevice->EndScene();
	}

	// Present the backbuffer contents to the display
	g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
	Sleep(10);
}

*/


//-----------------------------------------------------------------------------
// Name: MsgProc()
// Desc: The window's message handler
//-----------------------------------------------------------------------------
bool g_keydown[256];
int g_keyhit[256];
int g_mb;
LRESULT WINAPI MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_LBUTTONDOWN:
		SetCapture(hWnd);
		g_mb |= 1;
		g_keydown[VK_LBUTTON] = true;
		g_keyhit[VK_LBUTTON]++;
		break;
	case WM_RBUTTONDOWN:
		SetCapture(hWnd);
		g_keydown[VK_RBUTTON] = true;
		g_keyhit[VK_RBUTTON]++;
		g_mb |= 2;
		break;
	case WM_MBUTTONDOWN:
		SetCapture(hWnd);
		g_mb |= 4;
		g_keydown[VK_MBUTTON] = true;
		g_keyhit[VK_MBUTTON]++;
		break;
	case WM_LBUTTONUP:
		ReleaseCapture();
		g_mb &= ~1;
		g_keydown[VK_LBUTTON] = false;
		break;
	case WM_RBUTTONUP:
		ReleaseCapture();
		g_mb &= ~2;
		g_keydown[VK_RBUTTON] = false;
		break;
	case WM_MBUTTONUP:
		ReleaseCapture();
		g_mb &= ~4;
		g_keydown[VK_MBUTTON] = false;
		break;

	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		g_keydown[wParam & 255] = true;
		g_keyhit[wParam & 255]++;
		return 0;
	case WM_KEYUP:
	case WM_SYSKEYUP:
		g_keydown[wParam & 127] = false;
		break;

	case WM_DESTROY:
		Cleanup();
		PostQuitMessage(0);
		return 0;
	case WM_ACTIVATEAPP:
		if (!wParam)
		{
			memset(g_keydown, 0, sizeof(g_keydown));
		}
		break;

	case WM_ACTIVATE:
		if (WA_INACTIVE != wParam)
		{
			// Make sure the device is acquired, if we are gaining focus.

		}
		break;
	}


	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LARGE_INTEGER starttime;
LARGE_INTEGER freq;
extern HWND hWnd;
HWND hWnd;

//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: The application's entry point
//-----------------------------------------------------------------------------
INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR cmd, INT)
{
	int id = MessageBox(NULL, "Play in fullscreen?", "Startup", MB_YESNOCANCEL);
	if (id == IDCANCEL) return 0;
	fullscreen = (id == IDYES);

	// Register the window class
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L,
					  GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
					  "SpaceInvaders", NULL };
	RegisterClassEx(&wc);

	RECT r = { 0,0,800,600 };
	int style = fullscreen ? WS_POPUP : WS_OVERLAPPEDWINDOW;
	style |= WS_VISIBLE;
	AdjustWindowRect(&r, style, false);

	// Create the application's window

	hWnd = CreateWindow("SpaceInvaders", "Space Invaders",
		style, 0, 0, r.right - r.left, r.bottom - r.top,
		GetDesktopWindow(), NULL, wc.hInstance, NULL);

	FSOUND_Init(44100, 42, 0);

	QueryPerformanceCounter(&starttime);
	QueryPerformanceFrequency(&freq);
	// Initialize Direct3D
	if (SUCCEEDED(InitD3D(hWnd)))
	{
		// Create the vertex buffer
		if (SUCCEEDED(InitVB()))
		{
			//SetWindowPos(hWnd,NULL,0,0,1024,768,SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOMOVE|SWP_ASYNCWINDOWPOS);
			SetCursor(LoadCursor(NULL, IDC_ARROW));

			// Show the window
			ShowWindow(hWnd, SW_SHOWDEFAULT);
			UpdateWindow(hWnd);

			//InitDirectInput( hWnd );			

			Game();
		}
	}

	UnregisterClass("SpaceInvaders", wc.hInstance);
	return 0;
}

//////////////////////////////////



/*WINDOWPLACEMENT g_wpPrev = { sizeof(g_wpPrev) };
void ChangeFullscreen()
{

	DWORD dwStyle = GetWindowLong(hWnd, GWL_STYLE);
	if (dwStyle & WS_OVERLAPPEDWINDOW) {
		MONITORINFO mi = { sizeof(mi) };
		if (GetWindowPlacement(hWnd, &g_wpPrev) &&
			GetMonitorInfo(MonitorFromWindow(hWnd,
				MONITOR_DEFAULTTOPRIMARY), &mi)) {
			SetWindowLong(hWnd, GWL_STYLE,
				dwStyle & ~WS_OVERLAPPEDWINDOW);
			SetWindowPos(hWnd, HWND_TOP,
				mi.rcMonitor.left, mi.rcMonitor.top,
				mi.rcMonitor.right - mi.rcMonitor.left,
				mi.rcMonitor.bottom - mi.rcMonitor.top,
				SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
		}
	}
	else {
		SetWindowLong(hWnd, GWL_STYLE,
			dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(hWnd, &g_wpPrev);
		SetWindowPos(hWnd, NULL, 0, 0, 0, 0,
			SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
			SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
	}
}*/


bool WantQuit(DWORD clearcol)
{
	// Enter the message loop
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));
	while (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		if (msg.message == WM_QUIT) return true;
	}

	g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, clearcol, 1.0f, 0);
	g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, false);

	g_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	g_pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TEXTURE);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);

	g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);

	g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, false);
	//g_pd3dDevice->SetTexture(0,NULL);
	g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	D3DVIEWPORT9 vp = { 0,0, 800, 600, 0, 1 };
	//g_pd3dDevice->SetViewport(&vp);

	g_pd3dDevice->SetFVF(D3DFVF_CUSTOMVERTEX);

	// Begin the scene
	g_pd3dDevice->BeginScene();

	//UpdateDirectInput();

	return false;
}
int	GetTimeInMS() // ...since start of program
{
	LARGE_INTEGER arse;
	QueryPerformanceCounter(&arse);
	return ((arse.QuadPart - starttime.QuadPart) * 1000) / freq.QuadPart;
}

void Flip()
{

	if (!IsKeyDown(VK_LBUTTON))
	{
		ButtonClickPlayed = false;
	}

	static int lastflip = 0;
	if (lastflip == 0) lastflip = GetTimeInMS();
	g_pd3dDevice->EndScene();

	// Present the backbuffer contents to the display

	//while (GetTimeInMS()<lastflip+1000/60) ;// Sleep(0); // clamp to max of 60hz
	lastflip = GetTimeInMS();

	g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
	Sleep(0);
	memset(g_keyhit, 0, sizeof(g_keyhit));
	SetCursor(LoadCursor(NULL, IDC_ARROW));

}


int Object_global_id = 0; //This variable is incremented with every new object, saved as object_id in that object and it's used in instance_destroy as object_id
Sprite Sprites;
Sound Sounds;

std::vector<Object*> Objects; //Main object array
std::vector<Object*> Enemies;
std::vector<Object*> Bullets;
std::vector<Object*> eBullets;
std::vector<Object*> Baricades;
std::vector<Object*> BackgroundParticles;


//Pre-loads all sprites and stores them in variables for later use
void Sound::LoadSounds()
{
	NormalShoot = LoadSnd("sfx/s_rifleShot.ogg");
	s_click = LoadSnd("sfx/s_click.ogg");
	PlayerDestroy = LoadSnd("sfx/s_PlayerDestroyed.ogg");
	PlayerHit = LoadSnd("sfx/s_PlayerHit.ogg");
	EnemyHit = LoadSnd("sfx/s_EnemyHit.ogg");
	EnemyDestroy = LoadSnd("sfx/s_EnemyDestroyed.ogg");
	EnemyShot = LoadSnd("sfx/s_enemyShot.ogg");
	ButtonClick = LoadSnd("sfx/s_buttonClick.ogg");
	ButtonHoverOver = LoadSnd("sfx/s_buttonHover.ogg");
}

//Pre-loads all sprites and stores them in variables for later use
void Sprite::LoadSprites()
{
	/* WARNING
	LoadSprite is not going to be called in stand-alone Release
	D3DXCreateTextureFromFile (in LoadSprite) is not going to be called in stand-alone Debug
	*/
	player.push_back(LoadSprite("gfx/Protector1_0.png"));
	Invader1.push_back(LoadSprite("gfx/Invader1_0.png"));
	Invader1.push_back(LoadSprite("gfx/Invader1_1.png"));
	Invader2.push_back(LoadSprite("gfx/Invader2_0.png"));
	Invader2.push_back(LoadSprite("gfx/Invader2_1.png"));
	Invader3.push_back(LoadSprite("gfx/Invader3_0.png"));
	Invader3.push_back(LoadSprite("gfx/Invader3_1.png"));
	Invader4.push_back(LoadSprite("gfx/Invader4_0.png"));
	bullet.push_back(LoadSprite("gfx/Projectile1_0.png"));
	particle.push_back(LoadSprite("gfx/Particles/glow.png"));
	particle.push_back(LoadSprite("gfx/Particles/fine-smoke-1.png"));
	particle.push_back(LoadSprite("gfx/Particles/fine-smoke-2.png"));
	particle.push_back(LoadSprite("gfx/Particles/Fog1.png"));
	particle.push_back(LoadSprite("gfx/Particles/Smoke.png"));
	baricade.push_back(LoadSprite("gfx/Baricade1_0.png"));

}

/*

Every class that inherits from Object has to have this in it's constructor:
object_index = obj_someObjectIndex; (see defined values in leetlib.h)
sprite = Sprites.someSprite; (see LoadSprites function)

*/

//Object class
Object::Object()
{
	active = false;
	drawSelf = true;
	destroy_event_flag = true;
	object_id = 0;
	object_index = -4;
	image_index = 0;
	image_speed = 0;
	image_speed_timer = 0;
	sprite_width = 1;
	sprite_height = 1;
	image_width = 1;
	image_height = 1;
	image_xscale = 1;
	image_yscale = 1;
	image_angle = 0;
	image_blend = c_white;
	x = 0;
	y = 0;
	xprevious = 0;
	yprevious = 0;

	speed = 0;
	hspeed = 0;
	vspeed = 0;
	direction = 0;

	collider_x1 = 0;
	collider_y1 = 0;
	collider_x2 = 0;
	collider_y2 = 0;
	collider_radius = 0;
	collider_center_x = 0;
	collider_center_y = 0;
}
Object::~Object()
{
	if (destroy_event_flag == true)
	{

	}
}
void Object::Update()
{
	active = true;

	//Animate the object
	image_speed_timer += 1;
	if (image_speed_timer > 60 / image_speed)
	{
		image_speed_timer = 0;
		image_index++;
		if (image_index > sprite.size() - 1)
		{
			image_index = 0;
		}
	}

	//Call update in inherited classes
	if (object_index == obj_player)
	{
		static_cast<Player*>(this)->pUpdate();
	}
	if (object_index == obj_enemy)
	{
		static_cast<Enemy*>(this)->eUpdate();
	}
	if (object_index == obj_particle)
	{
		static_cast<Particle*>(this)->pUpdate();
	}
	if (object_index == obj_baricade)
	{
		static_cast<Baricade*>(this)->bUpdate();
	}

	//Move object by it's speed
	if ((abs(hspeed) > 0) || (abs(vspeed) > 0))
	{
		x += hspeed;
		y += vspeed;
	}
	if (abs(speed) > 0)
	{
		x = x + lengthdir_x(speed, direction);
		y = y + lengthdir_y(speed, direction);
	}

	//Update the collider
	image_width = sprite_width * image_xscale;
	image_height = sprite_height * image_yscale;

	collider_x1 = x - image_width;
	collider_y1 = y - image_height;
	collider_x2 = x + image_width;
	collider_y2 = y + image_height;
	collider_center_x = ((collider_x2 - collider_x1) / 2) + x;
	collider_center_y = ((collider_y2 - collider_y1) / 2) + y;
	if (collider_x2 > collider_y2)
	{
		collider_radius = collider_x2;
	}
	else
	{
		collider_radius = collider_y2;
	}
}

//Draws an object with built-in variables
void Object::DrawSelf()
{
	if ((active == true) && (drawSelf == true))
	{
		DrawSprite(sprite, image_index, x, y, image_width, image_height, image_angle, image_blend);
	}
}

//Player class
Player::Player()
{
	object_index = obj_player;
	sprite = Sprites.player;

	MaxHealth = 100;
	Health = MaxHealth;
	AttackSpeed = 2;
	Damage = 1;
	Cannons = 1;
	hitShock = 0;
}
Player::~Player()
{
	if (destroy_event_flag == true)
	{
		PlaySnd(Sounds.PlayerDestroy, SfxVolume);

		//Create particles
		for (int i = 0; i < 50; i++)
		{
			Object* part = instance_create(new Particle, x, y);
			part->active = true;
			static_cast<Particle*>(part)->part_set_direction(0, 360, 0);
			static_cast<Particle*>(part)->part_set_life(120, 240);
			static_cast<Particle*>(part)->part_set_alpha(1, 0.5, 0);
			static_cast<Particle*>(part)->part_set_size(0.05, 0.5, -0.0001);
			static_cast<Particle*>(part)->part_set_speed(0.5, 5, -0.005);
		}
	}
}
void Player::pUpdate()
{
	if (hitShock > 0)
	{
		hitShock -= 10;
	}
	image_blend = make_color_argb(255, 255, 255 - hitShock, 255 - hitShock);
}

//Bullet class
Bullet::Bullet()
{
	object_index = obj_bullet;
	sprite = Sprites.bullet;

	Damage = 1;
}
Bullet::~Bullet()
{
	if (destroy_event_flag == true)
	{
		//Create particles
		for (int i = 0; i < 10; i++)
		{
			Object* part = instance_create(new Particle, x, y);
			part->active = true;
			static_cast<Particle*>(part)->part_set_direction(0, 360, 0);
			static_cast<Particle*>(part)->part_set_life(20, 30);
			static_cast<Particle*>(part)->part_set_alpha(1, 0.5, 0);
			static_cast<Particle*>(part)->part_set_size(0.01, 0.1, -0.01);
			static_cast<Particle*>(part)->part_set_speed(1, 10, -0.01);
		}
	}
}

//eBullet class
eBullet::eBullet()
{
	object_index = obj_ebullet;
	sprite = Sprites.bullet;
	image_blend = c_red;

	Damage = 1;
}
eBullet::~eBullet()
{
	if (destroy_event_flag == true)
	{
		//Create particles
		for (int i = 0; i < 10; i++)
		{
			Object* part = instance_create(new Particle, x, y);
			part->active = true;
			static_cast<Particle*>(part)->part_set_direction(0, 360, 0);
			static_cast<Particle*>(part)->part_set_life(20, 30);
			static_cast<Particle*>(part)->part_set_alpha(1, 0.5, 0);
			static_cast<Particle*>(part)->part_set_size(0.01, 0.1, -0.01);
			static_cast<Particle*>(part)->part_set_speed(1, 10, -0.01);
		}
	}
}

//Enemy class
Enemy::Enemy()
{
	object_index = obj_enemy;
	sprite = Sprites.Invader1;
	InvaderLevel = 1;

	image_speed = 1;

	MaxHealth = 100;
	Health = MaxHealth;

	Damage = 1;

	hitShock = 0;

	ReadyToShoot = false;
	ChanceToShot = 5;
}
Enemy::~Enemy()
{
	if (destroy_event_flag == true)
	{
		PlaySnd(Sounds.EnemyDestroy, SfxVolume);

		//Create particles
		for (int i = 0; i < 50; i++)
		{
			Object* part = instance_create(new Particle, x, y);
			part->active = true;
			static_cast<Particle*>(part)->part_set_direction(0, 360, 0);
			static_cast<Particle*>(part)->part_set_life(10, 20);
			static_cast<Particle*>(part)->part_set_alpha(1, 0.5, 0);
			static_cast<Particle*>(part)->part_set_size(0.01, 0.1, -0.01);
			static_cast<Particle*>(part)->part_set_speed(5, 20, -0.005);
		}
	}
}
void Enemy::eUpdate()
{
	if (static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (10000))) < ChanceToShot)
	{
		ReadyToShoot = true;
	}

	if (hitShock > 0)
	{
		hitShock -= 10;
	}
	switch (InvaderLevel)
	{
	case 1:
		image_blend = make_color_argb(255, hitShock * 2, 255 - hitShock, 0);
		break;
	case 2:
		image_blend = make_color_argb(255, hitShock * 2, 255 - hitShock, 255 - hitShock);
		break;
	case 3:
		image_blend = make_color_argb(255, 255, 0, 255 - (hitShock * 2));
		break;
	case 4:
		image_blend = make_color_argb(255, 255, hitShock, hitShock);
		break;
	}
}

//Particle class
Particle::Particle()
{
	object_index = obj_particle;
	sprite = Sprites.particle;

	part_color_r = 255;
	part_color_g = 255;
	part_color_b = 255;

	part_life_min = 1;
	part_life_max = 1;
	part_size_min = 1;
	part_size_max = 1;
	part_size_inc = 0;
	part_speed_min = 1;
	part_speed_max = 1;
	part_speed_inc = 0;
	part_color_alpha1 = 255;
	part_color_alpha2 = 255;
	part_color_alpha3 = 255;
	part_angle_min = 0;
	part_angle_max = 0;
	part_angle_inc = 0;
	part_direction_min = 0;
	part_direction_max = 0;
	part_direction_inc = 0;

	part_life_cur = part_life_max;
	part_life_start = part_life_cur;
	part_size_cur = 1;
	part_speed_cur = 0;
	part_color_alpha_cur = part_color_alpha1;
	part_angle_cur = 0;
	part_direction_cur = direction;
}
void Particle::pUpdate()
{
	if (part_life_cur <= 0)
	{
		instance_destroy(this);
	}
	else
	{
		//Update particle properties//

		part_life_cur--;
		part_size_cur += part_size_inc;
		part_speed_cur += part_speed_inc;

		part_size_cur = max(part_size_cur, 0);
		part_speed_cur = max(part_speed_cur, 0);

		image_xscale = part_size_cur;
		image_yscale = part_size_cur;

		part_angle_cur += part_angle_inc;
		part_direction_cur += part_direction_inc;

		//Since part_life_cur is decreasing instead of increasing, those lerp methods has to be like this:
		if ((part_life_cur / part_life_start < 1) && (part_life_cur / part_life_start > 0.5))
		{
			part_color_alpha_cur = lerp(part_color_alpha2, part_color_alpha1, ((part_life_cur / part_life_start) - 0.5) * 2);
		}
		if (part_life_cur / part_life_start <= 0.5)
		{
			part_color_alpha_cur = lerp(part_color_alpha3, part_color_alpha2, ((part_life_cur / part_life_start)) * 2);
		}

		if (part_size_cur <= 0)
		{
			part_color_alpha_cur = 0;
		}
		part_color_alpha_cur = max(part_color_alpha_cur, 0);

		speed = part_speed_cur;
		image_angle = part_angle_cur;
		direction = part_direction_cur;
		image_blend = make_color_argb(part_color_alpha_cur * 255, part_color_r, part_color_g, part_color_b);
	}
}

void Particle::part_set_sprite(std::vector<void*> spr)
{
	sprite = spr;
}
void Particle::part_set_life(float min, float max)
{
	part_life_min = min;
	part_life_max = max;
	part_life_cur = part_life_min + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (part_life_max - part_life_min)));
	part_life_start = part_life_cur;
}
void Particle::part_set_alpha(float alpha1, float alpha2, float alpha3)
{
	part_color_alpha1 = alpha1;
	part_color_alpha2 = alpha2;
	part_color_alpha3 = alpha3;
}
void Particle::part_set_size(float min, float max, float inc)
{
	part_size_min = min;
	part_size_max = max;
	part_size_inc = inc;
	part_size_cur = part_size_min + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (part_size_max - part_size_min)));
}
void Particle::part_set_speed(float min, float max, float inc)
{
	part_speed_min = min;
	part_speed_max = max;
	part_speed_inc = inc;
	part_speed_cur = part_speed_min + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (part_speed_max - part_speed_min)));
}
void Particle::part_set_color(int r, int g, int b)
{
	part_color_r = r;
	part_color_g = g;
	part_color_b = b;
}
void Particle::part_set_angle(float min, float max, float inc)
{
	part_angle_min = min;
	part_angle_max = max;
	part_angle_cur = part_angle_min + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (part_angle_max - part_angle_min)));
}
void Particle::part_set_direction(float min, float max, float inc)
{
	part_direction_min = min;
	part_direction_max = max;
	part_direction_cur = part_direction_min + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (part_direction_max - part_direction_min)));
}

Baricade::Baricade()
{
	object_index = obj_baricade;
	sprite = Sprites.baricade;

	MaxHealth = 200;
	Health = MaxHealth;

	hitShock = 0;
}
Baricade::~Baricade()
{
	if (destroy_event_flag == true)
	{
		PlaySnd(Sounds.PlayerDestroy, SfxVolume);

		//Create particles
		for (int i = 0; i < 100; i++)
		{
			float rand_x = collider_x1 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (collider_x2 - collider_x1)));
			float rand_y = collider_y1 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (collider_y2 - collider_y1)));
			Object* part = instance_create(new Particle, rand_x, rand_y);
			part->active = true;
			static_cast<Particle*>(part)->part_set_direction(0, 360, 0);
			static_cast<Particle*>(part)->part_set_life(120, 240);
			static_cast<Particle*>(part)->part_set_alpha(1, 0.5, 0);
			static_cast<Particle*>(part)->part_set_size(0.05, 0.5, -0.0001);
			static_cast<Particle*>(part)->part_set_speed(2, 10, -0.005);
		}
	}
}
void Baricade::bUpdate()
{
	if (hitShock > 0)
	{
		hitShock -= 10;
	}
	image_blend = make_color_argb(255, 255, 255 - hitShock, 255 - hitShock);
}

//Creates instance of an object and sets default values
Object* instance_create(Object* obj, float x, float y)
{
	//Put this new object into the main vector of objects
	Objects.push_back(obj);

	obj->active = false;

	obj->object_id = Object_global_id;
	Object_global_id++;

	//Get image information
	D3DSURFACE_DESC* text_data = new D3DSURFACE_DESC;
	((IDirect3DTexture9*)(obj->sprite[0]))->GetLevelDesc(0, text_data);

	obj->image_xscale = 1;
	obj->image_yscale = 1;
	obj->sprite_width = text_data->Width;
	obj->sprite_height = text_data->Height;
	obj->image_width = obj->sprite_width * obj->image_xscale;
	obj->image_height = obj->sprite_height * obj->image_yscale;
	obj->image_angle = 0;
	obj->x = x;
	obj->y = y;
	obj->xprevious = x;
	obj->yprevious = y;

	obj->collider_x1 = 0;
	obj->collider_y1 = 0;
	obj->collider_x2 = 0;
	obj->collider_y2 = 0;
	obj->collider_radius = 0;
	obj->collider_center_x = 0;
	obj->collider_center_y = 0;

	delete text_data;

	return obj;
}

//Destroys an object (you have to manually remove an object from other arrays)
Object* instance_destroy(Object* obj, bool run_deconstructor)
{
	for (int i = 0; i < Objects.size(); i++)
	{
		//Find the correct object to remove it from the vector
		if (Objects[i]->object_id == obj->object_id)
		{
			Objects.erase(Objects.begin() + i);
			break;
		}
	}

	obj->destroy_event_flag = run_deconstructor;

	obj->active = false;
	delete obj;
	obj = NULL;
	return obj;
}

void ClearObjects()
{
	//Clear all objects
	for (int i = 0; i < Objects.size(); i++)
	{
		instance_destroy(Objects[i], false);
	}
	Objects.clear();
	Enemies.clear();
	Bullets.clear();
	eBullets.clear();
	Baricades.clear();
	BackgroundParticles.clear();
}

unsigned long make_color_argb(int alpha, int r, int g, int b)
{
	return D3DCOLOR_ARGB(alpha, r, g, b);
}

//Checks collision for specific object
//Returns true if obj's collider is inside of specified rectangle
bool collision_rectangle(float x1, float y1, float x2, float y2, Object* obj)
{
	if (obj != NULL)
	{
		if ((((x1 < obj->collider_x2) && (y1 < obj->collider_y2) && (x1 > obj->collider_x1) && (y1 > obj->collider_y1)) ||
			((x2 > obj->collider_x1) && (y2 > obj->collider_y1) && (x2 < obj->collider_x2) && (y2 < obj->collider_y2)) ||
			((x2 > obj->collider_x1) && (y1 < obj->collider_y2) && (x2 < obj->collider_x2) && (y1 > obj->collider_y1)) ||
			((x1 < obj->collider_x2) && (y2 > obj->collider_y1) && (x1 > obj->collider_x1) && (y2 < obj->collider_y2))) && (obj->active == true))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}
//Collision for array of objects | returns index used to get specific object from array (returns -1 if there is none)
int collision_rectangle(float x1, float y1, float x2, float y2, std::vector<Object*>& obj)
{
	for (int i = 0; i < obj.size(); i++)
	{
		if (obj[i] != NULL)
		{
			if ((((x1 < obj[i]->collider_x2) && (y1 < obj[i]->collider_y2) && (x1 > obj[i]->collider_x1) && (y1 > obj[i]->collider_y1)) ||
				((x2 > obj[i]->collider_x1) && (y2 > obj[i]->collider_y1) && (x2 < obj[i]->collider_x2) && (y2 < obj[i]->collider_y2)) ||
				((x2 > obj[i]->collider_x1) && (y1 < obj[i]->collider_y2) && (x2 < obj[i]->collider_x2) && (y1 > obj[i]->collider_y1)) ||
				((x1 < obj[i]->collider_x2) && (y2 > obj[i]->collider_y1) && (x1 > obj[i]->collider_x1) && (y2 < obj[i]->collider_y2))) && (obj[i]->active == true))
			{
				return i;
			}
		}
		else
		{
			return -1;
		}
	}
	return -1;
}

//0, 0 is top left; 800, 600 is bottom right
void GetMousePos(float& x, float& y)
{
	POINT p;
	GetCursorPos(&p);
	ScreenToClient(hWnd, &p);
	x = p.x;
	y = p.y;
}

//Use windows VK_ codes for special keys, eg VK_LEFT; use capital chars for letter keys eg 'A', '0'
bool IsKeyDown(int key)
{
	return g_keydown[key & 255];
}

//Returns a key that is currently pressed
char GetPressedKey()
{
	int chr = -1;
	bool shift = false;
	for (int i = 0; i < (sizeof(g_keydown) / sizeof(*g_keydown)); i++)
	{
		if (g_keydown[i] == true)
		{
			if (i == 8)
			{
				return 61;
			}
			if (i == 16)
			{
				shift = true;
			}

			if ((i >= 65) && (i <= 90))
			{
				if (shift == true)
				{
					chr = i;
				}
				else
				{
					chr = i + 32;
				}
			}
			if ((i >= 96) && (i <= 105))
			{
				chr = i - 48;
			}
		}
	}//A 65 //Z 90
	return chr;
}

bool IsKeyHitSinceLastFlip(int key)
{
	return g_keyhit[key & 255] > 0;
}


// 'sprite output' 
//Loads a image from a file and returns pointer to it
void* LoadSprite(const char* fname)
{
	IDirect3DTexture9* tex = NULL;
	D3DXCreateTextureFromFile(g_pd3dDevice, fname, &tex);
	return tex;
}

//Set texture for drawing
void SetCurrentTexture(void* tex)
{
	IDirect3DTexture9* t = (IDirect3DTexture9*)tex;
	g_pd3dDevice->SetTexture(0, t);
}

float DrawSlider(float x, float y, float len, float value, int col1, int col2)
{
	float val = (value * len) + x;
	float mx;
	float my;
	GetMousePos(mx, my);

	if ((mx > x - 5) && (mx < x + len + 5) && (my > y - 10) && (my < y + 10) && (IsKeyDown(VK_LBUTTON)))
	{
		val = mx;
	}

	if (val > x + len)
	{
		val = x + len;
	}
	if (val < x)
	{
		val = x;
	}

	value = ((val - x) / (len));

	DrawRectangle(x, y - 5, x + len, y + 5, col1);
	DrawRectangle(val - 5, y - 10, val + 5, y + 10, col2);



	return value;
}

void DrawHealthbar(float x1, float y1, float x2, float y2, float value, int color)
{
	DrawRectangle(x1 - 2, y1 - 2, x1 + x2 + 2, y2 + 2, color);
	DrawRectangle(x1 - 1, y1 - 1, x1 + x2 + 1, y2 + 1, make_color_argb(255, 0, 0, 0));
	DrawRectangle(x1, y1, x1 + value * x2, y2, color);
}

bool DrawButton(float x, float y, float w, float h, int color, int textColor, const char* text, int textSize)
{
	bool pressed = false;
	float mx;
	float my;
	GetMousePos(mx, my);

	DrawRectangle(x - w, y - h, x + w, y + h, color);

	if ((mx > (x - w)) && (mx < (x + w)) && (my > (y - h)) && (my < (y + h)))
	{
		DrawRectangle(x - w, y - h, x + w, y + h, make_color_argb(50, 0, 0, 0));
		if (IsKeyDown(VK_LBUTTON))
		{
			if (ButtonClickPlayed == false)
			{
				PlaySnd(Sounds.ButtonClick, SfxVolume);
				ButtonClickPlayed = true;
			}
			DrawRectangle(x - w, y - h, x + w, y + h, make_color_argb(100, 0, 0, 0));
			pressed = true;
		}
	}

	DrawSomeText(x, y - 8, textSize, textColor, true, text);

	return pressed;
}

void DrawRectangle(float x1, float y1, float x2, float y2, DWORD col)
{
	SetCurrentTexture(LoadSprite("sprite.png"));
	CUSTOMVERTEX tea2[] =
	{
		{ x1, y1, 0.5f, 1.0f, col, 0,0, }, // x, y, z, rhw, color
		{ x2, y1, 0.5f, 1.0f, col, 1,0, },
		{ x1, y2, 0.5f, 1.0f, col, 0,1, },
		{ x2, y2, 0.5f, 1.0f, col, 1,1, },
	};
	g_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, tea2, sizeof(CUSTOMVERTEX));

}

void DrawSprite(std::vector<void*>& sprite, int image_index, float xcentre, float ycentre, float xsize, float ysize, float angle, DWORD col)
{
	if (sprite[image_index] != NULL)
	{
		angle = angle * (PI / 180);
		SetCurrentTexture(sprite[image_index]);
		float c = cosf(angle);
		float s = sinf(angle);
#define ROTATE(xx,yy) xcentre+(xx)*c+(yy)*s,ycentre+(yy)*c-(xx)*s 
		CUSTOMVERTEX tea2[] =
		{

			///{ xcentre+xsize*c+ysize*s,ycentre+ysize*c-xsize*s , 0.5f, 1.0f, col, 0,0, }, // x, y, z, rhw, color

			{ ROTATE(-xsize,-ysize), 0.5f, 1.0f, col, 0,0, }, // x, y, z, rhw, color
			{ ROTATE(xsize,-ysize), 0.5f, 1.0f, col, 1,0, },
			{ ROTATE(-xsize, ysize), 0.5f, 1.0f, col, 0,1, },
			{ ROTATE(xsize, ysize), 0.5f, 1.0f, col, 1,1, },
		};
		g_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, tea2, sizeof(CUSTOMVERTEX));
	}
}


float lengthdir_x(float length, float direction)
{
	return length * cos(direction * PI / -180);
}
float lengthdir_y(float length, float direction)
{
	return length * sin(direction * PI / -180);
}
constexpr float lerp(float a, float b, float t) {
	// Exact, monotonic, bounded, determinate, and (for a=b=0) consistent:
	if (a <= 0 && b >= 0 || a >= 0 && b <= 0) return t * b + (1 - t) * a;

	if (t == 1) return b;                        // exact
	// Exact at t=0, monotonic except near t=1,
	// bounded, determinate, and consistent:
	const float x = a + t * (b - a);
	return t > 1 == b > a ? max(b, x) : min(b, x);  // monotonic near t=1
}
float frand_range(float min, float max)
{
	return (min + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (max - min))));
}
float sign(float number)
{
	if (number > 0)
	{
		return 1;
	}
	else if (number < 0)
	{
		return -1;
	}
	else
	{
		return 0;
	}
}
float smooth_approach(float current, float target, float smooth)
{
	float diff = target - current;
	if (abs(diff) < 0.05f)
	{
		return target;
	}
	else
	{
		return current + sign(diff) * abs(diff) * smooth;
	}
}

/*
// 'flat colour' output
void DrawLine(float x1, float y1, float x2, float y2, DWORD col ) // no texture
{
	IDirect3DBaseTexture9 *oldtex = NULL;
	g_pd3dDevice->GetTexture(0, &oldtex);
	g_pd3dDevice->SetTexture(0, NULL);
	CUSTOMVERTEX tea2[] =
	{
		{ x1, y1, 0.5f, 1.0f, col, 0,0, }, // x, y, z, rhw, color
		{ x2, y2, 0.5f, 1.0f, col, 1,0, }
	};
	g_pd3dDevice->DrawPrimitiveUP(D3DPT_LINELIST, 1, tea2, sizeof(CUSTOMVERTEX));

	g_pd3dDevice->SetTexture(0, oldtex);
}

void DrawTriangle(float x1, float y1, float x2, float y2, float x3, float y3, DWORD col ) // flat colored tri, no texture
{
	IDirect3DBaseTexture9 *oldtex = NULL;
	g_pd3dDevice->GetTexture(0, &oldtex);
	g_pd3dDevice->SetTexture(0, NULL);
	CUSTOMVERTEX tea2[] =
	{
		{ x1, y1, 0.5f, 1.0f, col, 0,0, }, // x, y, z, rhw, color
		{ x2, y2, 0.5f, 1.0f, col, 1,0, },
		{ x3, y3, 0.5f, 1.0f, col, 1,0, }
	};
	g_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 1, tea2, sizeof(CUSTOMVERTEX));

	g_pd3dDevice->SetTexture(0, oldtex);
}


// 'advanced' output - for geeks only - for drawing arbitrarily textured triangles and line lists
void DrawTriangleList(Vertex *verts, int numtris)
{
	CUSTOMVERTEX *vs = (CUSTOMVERTEX *)alloca(sizeof(CUSTOMVERTEX) * numtris * 3);
	CUSTOMVERTEX *d=vs;
	for (int n=0;n<numtris*3;n++)
	{
		d->x=verts->x;
		d->y=verts->y;
		d->z=0.5;
		d->rhw=1;
		d->color=verts->colour;
		d->u=verts->u;
		d->v=verts->v;
		d++;
		verts++;
	}
	g_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, numtris, vs, sizeof(CUSTOMVERTEX));
}

void DrawLineList(Vertex *verts, int numlines)
{
	CUSTOMVERTEX *vs = (CUSTOMVERTEX *)alloca(sizeof(CUSTOMVERTEX) * numlines * 2);
	CUSTOMVERTEX *d=vs;
	for (int n=0;n<numlines * 2;n++)
	{
		d->x=verts->x;
		d->y=verts->y;
		d->z=0.5;
		d->rhw=1;
		d->color=verts->colour;
		d->u=verts->u;
		d->v=verts->v;
		d++;
		verts++;
	}
	g_pd3dDevice->DrawPrimitiveUP(D3DPT_LINELIST, numlines, vs, sizeof(CUSTOMVERTEX));
}
*/

FSOUND_STREAM* music;

int PlayMusic(const char* fname, float volume)
{
	if (music) StopMusic();
	music = FSOUND_Stream_Open(fname, FSOUND_LOOP_NORMAL, 0, 0);
	int chan = FSOUND_Stream_Play(FSOUND_FREE, music);
	if (volume <= 0) volume = 0;
	if (volume > 1) volume = 1;
	FSOUND_SetVolume(chan, (int)(volume * 255));
	return chan;
}

void StopMusic()
{
	if (music)
	{
		FSOUND_Stream_Close(music);
	}
	music = NULL;
}

void* LoadSnd(const char* fname, bool looped)
{
	int flags = 0;
	if (looped) flags |= FSOUND_LOOP_NORMAL;
	return FSOUND_Sample_Load(FSOUND_FREE, fname, flags, 0, 0);
}

int PlaySnd(void* sound, float volume)
{
	if (!sound) return -1;
	if (volume <= 0) volume = 0;
	if (volume > 1) volume = 1;
	int chan = FSOUND_PlaySound(FSOUND_FREE, (FSOUND_SAMPLE*)sound);
	FSOUND_SetVolume(chan, (int)(volume * 255));
	return chan;
}

void StopSnd(int handle)
{
	if (handle <= 0) return;
	FSOUND_StopSound(handle);
}

void ChangeVolume(int handle, float volume)
{
	if (handle <= 0) return;
	if (volume <= 0) volume = 0;
	if (volume > 1) volume = 1;
	FSOUND_SetVolume(handle, (int)(volume * 255));
}
