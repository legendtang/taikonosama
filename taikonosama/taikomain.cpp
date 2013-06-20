/* About Engine:
** Haaf's Game Engine 1.8
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** Taiko no Sama v0.1.0 pre-alpha
** Description: A simple game modelled after Taiko no Tatsuji（太鼓の達人）
** Environment: Microsoft Visual Studio Express 2012 for Windows Desktop
**						  with Haaf's Game Engine 1.81 libraries included
** Program author: 唐闻生<sp3478@gmail.com> 
**				   WHUCS 2012 Class 3 2012302580292
** Last change on June 18th.
**
** Under CC BY-NC 3.0 and CC BY-NC-SA 3.0 licences
** You may not use this work for commercial purposes.
*/

#include "hge.h"
#include "hgesprite.h"
#include "hgefont.h"
#include <math.h>
// #include "hgeparticle.h"
#include <stdio.h>

// Define the initial horizontal position & the maximum amount of objects
#define INITX (810.0f)
#define MAX_OBJECTS 1500
#define BPM (138.0f)
#define BGM_OFFSET (-0.5f)
#define OBJPERIOD (60.0f/BPM) // seconds 
#define SPEED_FACTOR (2.2f) // num 
#define TRUE_OBJSPEED (64.0f / OBJPERIOD * SPEED_FACTOR) // px/seconds
#define PERIOD_TIME (670.0f / TRUE_OBJSPEED ) // seconds

#define DISAPPEAR (140.0f)
#define SMALL_CIRCLE (167.0f)
#define LARGE_CIRCLE (149.5f)
#define RANGE_FIX (1.2f)
//#define SMALL_CIRCLE_FIX (SMALL_CIRCLE * RANGE_FIX)
//#define LARGE_CIRCLE_FIX (LARGE_CIRCLE * RANGE_FIX)



// Pointer to the HGE interface.
// Helper classes require this to work.
HGE *hge=0;

// Pointers to the HGE objects we will use
hgeSprite*			spr1;			//the Taiko "Dong" object
hgeSprite*			spr2;			//the Taiko "Ka" object
hgeSprite*			bgspr;			//Background image object
//hgeSprite*		spt;			//Will be activated in next version
hgeSprite*			stspr1;			//Status "Perfect" object
hgeSprite*			stspr2;         //Status "Good" object
hgeSprite*			stspr0;         //Status "Lost" object, more status is coming soon!
hgeFont*			fnt;			//Font object
//hgeParticleSystem*	par;        //Particle system object, disabled in this version

// Handles for HGE resourcces
HTEXTURE			tex1, tex2;				//Texture for "Dong" & "Ka" object
HTEXTURE			sts1, sts2, sts0;		//Texture for status "Perfect", "Good" & "Lost" object
HTEXTURE			bgtex;					//Texture for background image object
HEFFECT			snd1, snd2, bgm;		//Resources for sound effect and background music

// Each taiko Objects will use this
class tkObject
{
public:
	float x, y;		//Object positio
	int tktype;			//Object for diferent Taiko type
	int ststype;			//Object for the status call back
	float stx;
};

// Some "gameplay" variables
int obj[MAX_OBJECTS];
//int subobj[100];			  // [Veto] Debug for asynchronous loading
int bmtimes = 0;
int ObjNum=0;
HCHANNEL BgmChannel;
int StartTime;
int NowTime;
int StartNum, TailNum ;
float TimeDelta;
bool PlayToggle = false;


void InitBeatmap()
{
	//const int n=1;			//Debug counter
	FILE *beatmap;
	if((beatmap=fopen("s01.txt","r"))==NULL) 
	{
		fnt->printf(400, 300, HGETEXT_LEFT, "Loading failure!", hge->Timer_GetDelta(), hge->Timer_GetFPS());
		return;
	}
    while(!feof(beatmap))
    {
        fscanf(beatmap,"%d",&obj[ObjNum]);
        ObjNum++;
    }
	fclose(beatmap);
}

tkObject* pObjects;

// Load Frame function
bool FrameFunc()
{
	float dt=hge->Timer_GetDelta();
	NowTime = hge->Timer_GetTime();

	if ( !PlayToggle) {
		if ((NowTime - StartTime) > (PERIOD_TIME + BGM_OFFSET)) {
			hge->Channel_Resume(BgmChannel);
			PlayToggle = true;
		}
	}

	// Process keys status & Play sound effect
	if (hge->Input_GetKeyState(HGEK_ESCAPE)) return true;
	if (hge->Input_KeyDown(HGEK_Z) || hge->Input_KeyDown(HGEK_C))
	{
		hge->Effect_Play(snd1);
	}
	
	if (hge->Input_KeyDown(HGEK_X) || hge->Input_KeyDown(HGEK_V))
	{
		hge->Effect_Play(snd2);
	}
	StartNum = (int) ((NowTime - StartTime - PERIOD_TIME) / OBJPERIOD) ;
	StartNum < 0 ? 0 : StartNum ;
	TimeDelta = fmodf(NowTime - StartTime - PERIOD_TIME, OBJPERIOD) ;
	TailNum = (int) ((NowTime - StartTime) / OBJPERIOD);
	TailNum > ObjNum ? ObjNum : TailNum;

	for (int i = StartNum; i <= TailNum; ++i)
	{
		if (pObjects[i].tktype==0) continue;

		//if (i>=1 && pObjects[i-1].ststype==0)
		//{
		//	pObjects[i-1].stx=INITX;
		//	pObjects[i-1].ststype=3;
		//}
		pObjects[i].x -= (( TailNum - i ) * OBJPERIOD + TimeDelta) * TRUE_OBJSPEED;
		//pObjects[i].y+=pObjects[i].dy;
	}`

		if (hge->Input_KeyDown(HGEK_Z) || hge->Input_KeyDown(HGEK_C))
		{
			pObjects[i].stx=134;
			if(pObjects[i].tktype==1)
			{
				//if (pObjects[i].x<=180 && pObjects[i].x>=155) 
				if (fabsf(pObjects[i].x - SMALL_CIRCLE) < 39.0f) 
				{
					pObjects[i].ststype=1;
					pObjects[i].x=INITX;
					pObjects[i].dx=0.0f;
				}
				else if (fabsf(pObjects[i].x - LARGE_CIRCLE) < 48.0f) 
				{
					pObjects[i].ststype=2;
					pObjects[i].x=INITX;
					pObjects[i].dx=0.0f;
				}
				else pObjects[i].ststype=0;
			}
			else pObjects[i].ststype=0;
		}
		if (hge->Input_KeyDown(HGEK_X) || hge->Input_KeyDown(HGEK_V))
		{
			pObjects[i].stx=134;
			if(pObjects[i].tktype==2)
			{
				if (fabsf(pObjects[i].x - SMALL_CIRCLE) < 39.0f) 
				//if (pObjects[i].x<=180 && pObjects[i].x>=155) 
				{
					pObjects[i].ststype=1;
					pObjects[i].x=INITX;
				}
				else if (fabsf(pObjects[i].x - LARGE_CIRCLE) < 48.0f) 
				//else if (pObjects[i].x<=200 && pObjects[i].x>=150) 
				{
					pObjects[i].ststype=2;
					pObjects[i].x=INITX;
				}
				else pObjects[i].ststype=0;
			}
			else pObjects[i].ststype=0;
		}
		//if(pObjects[i].x<=140) 
		//{
		//	pObjects[i].dx=0.0f;
		//	pObjects[i].x=INITX;
		//	if(!pObjects[i-1].ststype) pObjects[i-1].stx=INITX;
		//	//int time_broke = 500; 
		//	//if (hge->Timer_GetTime() >= time_broke) pObjects[i].stx=INITX;
		//}
		//delay(250);
	}

	// Update particle system (Plan in next version!)
	//par->info.nEmission=(int)(dx*dx+dy*dy)*2;
	//par->MoveTo(x,y);
	//par->Update(dt);
	//hge->Gfx_EndScene();
	return false;
}

bool RenderFunc()
{
	// Render graphics
	hge->Gfx_BeginScene();
	bgspr->Render(0,0);
	hge->Gfx_Clear(0);
		
	//par->Render();
	for (int i = 0; i < ObjNum; ++i)
	{
		// if (!hge->Channel_IsPlaying(BgmChannel)) break;
		if (pObjects[i].tktype==1)
		{
			spr1->RenderEx(pObjects[i].x, pObjects[i].y, 0, 0.8f, 0.8f);
			//stspr1->RenderEx(134,186,0,0.5f,0.5f);
		}
		if (pObjects[i].tktype==2)
		{
			spr2->RenderEx(pObjects[i].x, pObjects[i].y, 0, 0.8f, 0.8f);
			//Boom(i);
		}

		switch(pObjects[i].ststype)
		{
			case 1: stspr1->RenderEx(pObjects[i].stx,186,0,0.5f,0.5f);break;
			case 2: stspr2->RenderEx(pObjects[i].stx,186,0,0.5f,0.5f);break;
			case 0: stspr0->RenderEx(pObjects[i].stx,186,0,0.5f,0.5f);
		}
	}

	//Debug info: Get time between this and last RenderFunc function and FPS
	fnt->printf(5, 5, HGETEXT_LEFT, "dt:%.3f\nFPS:%d (constant)", hge->Timer_GetDelta(), hge->Timer_GetFPS());

	//End scene
	hge->Gfx_EndScene();

	return false;
}


int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	hge = hgeCreate(HGE_VERSION);
	//hge->System_SetState(HGE_SHOWSPLASH, false);				//LOGO cancelled for debug
	hge->System_SetState(HGE_LOGFILE, "taikonosama.log");
	hge->System_SetState(HGE_FRAMEFUNC, FrameFunc);
	hge->System_SetState(HGE_RENDERFUNC, RenderFunc);
	hge->System_SetState(HGE_TITLE, "Touhou Tama On Taiko  - th unofficial project");
	hge->System_SetState(HGE_FPS, 60);
	hge->System_SetState(HGE_WINDOWED, true);
	hge->System_SetState(HGE_SCREENWIDTH, 800);
	hge->System_SetState(HGE_SCREENHEIGHT, 600);
	hge->System_SetState(HGE_SCREENBPP, 32);

	if(hge->System_Initiate()) {

		// Load sound and texture
		snd1=hge->Effect_Load("dong.wav");
		snd2=hge->Effect_Load("ka.wav");
		tex1=hge->Texture_Load("dong.png");
		tex2=hge->Texture_Load("ka.png");
		bgtex=hge->Texture_Load("bg.jpg");
		sts1=hge->Texture_Load("perfect.png");
		sts2=hge->Texture_Load("good.png");
		sts0=hge->Texture_Load("bad.png");
		bgm=hge->Stream_Load("BadApple.wav");
		if(!snd1 || !snd2 || !tex1 || !tex2 || !sts1 || !sts2 || !sts0)
		{
			// If one of the datwa files is not found, display
			// an error message and shutdown.
			MessageBox(NULL, "Can't load one of the following files:\nMENU.WAV, PARTICLES.PNG, FONT1.FNT, FONT1.PNG, TRAIL.PSI", "Error", MB_OK | MB_ICONERROR | MB_APPLMODAL);
			hge->System_Shutdown();
			hge->Release();
			return 0;
		}
		//else MessageBox(NULL, "Everything works well!", "Debug info", MB_OK | MB_ICONERROR | MB_APPLMODAL); return 0;

		// Create and set up a sprite
		spr1=new hgeSprite(tex1, 0, 0, 80, 80);
		//spr1->SetHotSpot(40,40);
		spr2=new hgeSprite(tex2, 0, 0, 80, 80);
		//spr2->SetHotSpot(40,40);

		// Create and set up a sprite
		bgspr=new hgeSprite(bgtex, 0, 0, 800, 600);

		stspr1=new hgeSprite(sts1, 0, 0, 256, 256);
		//stspr1->SetHotSpot(40,40);
		stspr2=new hgeSprite(sts2, 0, 0, 256, 256);
		//stspr2->SetHotSpot(40,40);
		stspr0=new hgeSprite(sts0, 0, 0, 256, 256);
		//stspr0->SetHotSpot(40,40);

		// Load a font
		fnt=new hgeFont("font1.fnt");

		// Create and set up a particle system (Plan in the next version!)
		//spt=new hgeSprite(tex, 32, 32, 32, 32);
		//spt->SetBlendMode(BLEND_COLORMUL | BLEND_ALPHAADD | BLEND_NOZWRITE);
		//spt->SetHotSpot(16,16);
		//par=new hgeParticleSystem("trail.psi",spt);
		//par->Fire();

		// Initialize Taiko objects

		//pObjects=new tkObject[100];							// [Veto] Debug for asynchronous loading
		pObjects=new tkObject[MAX_OBJECTS];

		// Load beatmap
		InitBeatmap();

		// Load Taiko objects
		for(int i=0;i < MAX_OBJECTS;i++)
		{
			pObjects[i].x=INITX;
			pObjects[i].y=215.0f;
			pObjects[i].tktype=obj[i];
			pObjects[i].ststype=3;
			pObjects[i].stx = INITX;
		}

		// Let's rock now!
		BgmChannel = hge->Stream_Play(bgm,false,100);
		hge->Channel_Pause(BgmChannel);
		StartTime = hge->Timer_GetTime();
		hge->System_Start();

		hge->Channel_Stop(BgmChannel);

		// Delete created objects and free loaded resources
		//delete par;
		delete fnt;
		//delete spt;/* About Engine:
** Haaf's Game Engine 1.8
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** Taiko no Sama v0.1.0 pre-alpha
** Description: A simple game modelled after Taiko no Tatsuji（太鼓の達人）
** Environment: Microsoft Visual Studio Express 2012 for Windows Desktop
**						  with Haaf's Game Engine 1.81 libraries included
** Program author: 唐闻生<sp3478@gmail.com> 
**				   WHUCS 2012 Class 3 2012302580292
** Last change on June 18th.
**
** Under CC BY-NC 3.0 and CC BY-NC-SA 3.0 licences
** You may not use this work for commercial purposes.
*/

#include "hge.h"
#include "hgesprite.h"
#include "hgefont.h"
#include <math.h>
// #include "hgeparticle.h"
#include <stdio.h>

// Define the initial horizontal position & the maximum amount of objects
#define INITX (810.0f)
#define MAX_OBJECTS 1500
#define BPM (138.0f)
#define BGM_OFFSET (-0.5f)
#define OBJPERIOD (60.0f/BPM) // seconds 
#define SPEED_FACTOR (2.2f) // num 
#define TRUE_OBJSPEED (64.0f / OBJPERIOD * SPEED_FACTOR) // px/seconds
#define PERIOD_TIME (670.0f / TRUE_OBJSPEED ) // seconds

#define DISAPPEAR (140.0f)
#define SMALL_CIRCLE (167.0f)
#define LARGE_CIRCLE (149.5f)
#define RANGE_FIX (1.2f)
//#define SMALL_CIRCLE_FIX (SMALL_CIRCLE * RANGE_FIX)
//#define LARGE_CIRCLE_FIX (LARGE_CIRCLE * RANGE_FIX)



// Pointer to the HGE interface.
// Helper classes require this to work.
HGE *hge=0;

// Pointers to the HGE objects we will use
hgeSprite*			spr1;			//the Taiko "Dong" object
hgeSprite*			spr2;			//the Taiko "Ka" object
hgeSprite*			bgspr;			//Background image object
//hgeSprite*		spt;			//Will be activated in next version
hgeSprite*			stspr1;			//Status "Perfect" object
hgeSprite*			stspr2;         //Status "Good" object
hgeSprite*			stspr0;         //Status "Lost" object, more status is coming soon!
hgeFont*			fnt;			//Font object
//hgeParticleSystem*	par;        //Particle system object, disabled in this version

// Handles for HGE resourcces
HTEXTURE			tex1, tex2;				//Texture for "Dong" & "Ka" object
HTEXTURE			sts1, sts2, sts0;		//Texture for status "Perfect", "Good" & "Lost" object
HTEXTURE			bgtex;					//Texture for background image object
HEFFECT			snd1, snd2, bgm;		//Resources for sound effect and background music

// Each taiko Objects will use this
class tkObject
{
public:
	float x, y;		//Object positio
	int tktype;			//Object for diferent Taiko type
	int ststype;			//Object for the status call back
	float stx;
};

// Some "gameplay" variables
int obj[MAX_OBJECTS];
//int subobj[100];			  // [Veto] Debug for asynchronous loading
int bmtimes = 0;
int ObjNum=0;
HCHANNEL BgmChannel;
int StartTime;
int NowTime;
int StartNum, TailNum ;
float TimeDelta;
bool PlayToggle = false;


void InitBeatmap()
{
	//const int n=1;			//Debug counter
	FILE *beatmap;
	if((beatmap=fopen("s01.txt","r"))==NULL) 
	{
		fnt->printf(400, 300, HGETEXT_LEFT, "Loading failure!", hge->Timer_GetDelta(), hge->Timer_GetFPS());
		return;
	}
    while(!feof(beatmap))
    {
        fscanf(beatmap,"%d",&obj[ObjNum]);
        ObjNum++;
    }
	fclose(beatmap);
}

tkObject* pObjects;

// Load Frame function
bool FrameFunc()
{
	float dt=hge->Timer_GetDelta();
	NowTime = hge->Timer_GetTime();

	if ( !PlayToggle) {
		if ((NowTime - StartTime) > (PERIOD_TIME + BGM_OFFSET)) {
			hge->Channel_Resume(BgmChannel);
			PlayToggle = true;
		}
	}

	// Process keys status & Play sound effect
	if (hge->Input_GetKeyState(HGEK_ESCAPE)) return true;
	if (hge->Input_KeyDown(HGEK_Z) || hge->Input_KeyDown(HGEK_C))
	{
		hge->Effect_Play(snd1);
	}
	
	if (hge->Input_KeyDown(HGEK_X) || hge->Input_KeyDown(HGEK_V))
	{
		hge->Effect_Play(snd2);
	}
	StartNum = (int) ((NowTime - StartTime - PERIOD_TIME) / OBJPERIOD) ;
	StartNum < 0 ? 0 : StartNum ;
	TimeDelta = fmodf(NowTime - StartTime - PERIOD_TIME, OBJPERIOD) ;
	TailNum = (int) ((NowTime - StartTime) / OBJPERIOD);
	TailNum > ObjNum ? ObjNum : TailNum;

	for (int i = StartNum; i <= TailNum; ++i)
	{
		if (pObjects[i].tktype==0) continue;

		//if (i>=1 && pObjects[i-1].ststype==0)
		//{
		//	pObjects[i-1].stx=INITX;
		//	pObjects[i-1].ststype=3;
		//}
		pObjects[i].x -= (( TailNum - i ) * OBJPERIOD + TimeDelta) * TRUE_OBJSPEED;
		//pObjects[i].y+=pObjects[i].dy;
	}`

		if (hge->Input_KeyDown(HGEK_Z) || hge->Input_KeyDown(HGEK_C))
		{
			pObjects[i].stx=134;
			if(pObjects[i].tktype==1)
			{
				//if (pObjects[i].x<=180 && pObjects[i].x>=155) 
				if (fabsf(pObjects[i].x - SMALL_CIRCLE) < 39.0f) 
				{
					pObjects[i].ststype=1;
					pObjects[i].x=INITX;
					pObjects[i].dx=0.0f;
				}
				else if (fabsf(pObjects[i].x - LARGE_CIRCLE) < 48.0f) 
				{
					pObjects[i].ststype=2;
					pObjects[i].x=INITX;
					pObjects[i].dx=0.0f;
				}
				else pObjects[i].ststype=0;
			}
			else pObjects[i].ststype=0;
		}
		if (hge->Input_KeyDown(HGEK_X) || hge->Input_KeyDown(HGEK_V))
		{
			pObjects[i].stx=134;
			if(pObjects[i].tktype==2)
			{
				if (fabsf(pObjects[i].x - SMALL_CIRCLE) < 39.0f) 
				//if (pObjects[i].x<=180 && pObjects[i].x>=155) 
				{
					pObjects[i].ststype=1;
					pObjects[i].x=INITX;
				}
				else if (fabsf(pObjects[i].x - LARGE_CIRCLE) < 48.0f) 
				//else if (pObjects[i].x<=200 && pObjects[i].x>=150) 
				{
					pObjects[i].ststype=2;
					pObjects[i].x=INITX;
				}
				else pObjects[i].ststype=0;
			}
			else pObjects[i].ststype=0;
		}
		//if(pObjects[i].x<=140) 
		//{
		//	pObjects[i].dx=0.0f;
		//	pObjects[i].x=INITX;
		//	if(!pObjects[i-1].ststype) pObjects[i-1].stx=INITX;
		//	//int time_broke = 500; 
		//	//if (hge->Timer_GetTime() >= time_broke) pObjects[i].stx=INITX;
		//}
		//delay(250);
	}

	// Update particle system (Plan in next version!)
	//par->info.nEmission=(int)(dx*dx+dy*dy)*2;
	//par->MoveTo(x,y);
	//par->Update(dt);
	//hge->Gfx_EndScene();
	return false;
}

bool RenderFunc()
{
	// Render graphics
	hge->Gfx_BeginScene();
	bgspr->Render(0,0);
	hge->Gfx_Clear(0);
		
	//par->Render();
	for (int i = 0; i < ObjNum; ++i)
	{
		// if (!hge->Channel_IsPlaying(BgmChannel)) break;
		if (pObjects[i].tktype==1)
		{
			spr1->RenderEx(pObjects[i].x, pObjects[i].y, 0, 0.8f, 0.8f);
			//stspr1->RenderEx(134,186,0,0.5f,0.5f);
		}
		if (pObjects[i].tktype==2)
		{
			spr2->RenderEx(pObjects[i].x, pObjects[i].y, 0, 0.8f, 0.8f);
			//Boom(i);
		}

		switch(pObjects[i].ststype)
		{
			case 1: stspr1->RenderEx(pObjects[i].stx,186,0,0.5f,0.5f);break;
			case 2: stspr2->RenderEx(pObjects[i].stx,186,0,0.5f,0.5f);break;
			case 0: stspr0->RenderEx(pObjects[i].stx,186,0,0.5f,0.5f);
		}
	}

	//Debug info: Get time between this and last RenderFunc function and FPS
	fnt->printf(5, 5, HGETEXT_LEFT, "dt:%.3f\nFPS:%d (constant)", hge->Timer_GetDelta(), hge->Timer_GetFPS());

	//End scene
	hge->Gfx_EndScene();

	return false;
}


int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	hge = hgeCreate(HGE_VERSION);
	//hge->System_SetState(HGE_SHOWSPLASH, false);				//LOGO cancelled for debug
	hge->System_SetState(HGE_LOGFILE, "taikonosama.log");
	hge->System_SetState(HGE_FRAMEFUNC, FrameFunc);
	hge->System_SetState(HGE_RENDERFUNC, RenderFunc);
	hge->System_SetState(HGE_TITLE, "Touhou Tama On Taiko  - th unofficial project");
	hge->System_SetState(HGE_FPS, 60);
	hge->System_SetState(HGE_WINDOWED, true);
	hge->System_SetState(HGE_SCREENWIDTH, 800);
	hge->System_SetState(HGE_SCREENHEIGHT, 600);
	hge->System_SetState(HGE_SCREENBPP, 32);

	if(hge->System_Initiate()) {

		// Load sound and texture
		snd1=hge->Effect_Load("dong.wav");
		snd2=hge->Effect_Load("ka.wav");
		tex1=hge->Texture_Load("dong.png");
		tex2=hge->Texture_Load("ka.png");
		bgtex=hge->Texture_Load("bg.jpg");
		sts1=hge->Texture_Load("perfect.png");
		sts2=hge->Texture_Load("good.png");
		sts0=hge->Texture_Load("bad.png");
		bgm=hge->Stream_Load("BadApple.wav");
		if(!snd1 || !snd2 || !tex1 || !tex2 || !sts1 || !sts2 || !sts0)
		{
			// If one of the datwa files is not found, display
			// an error message and shutdown.
			MessageBox(NULL, "Can't load one of the following files:\nMENU.WAV, PARTICLES.PNG, FONT1.FNT, FONT1.PNG, TRAIL.PSI", "Error", MB_OK | MB_ICONERROR | MB_APPLMODAL);
			hge->System_Shutdown();
			hge->Release();
			return 0;
		}
		//else MessageBox(NULL, "Everything works well!", "Debug info", MB_OK | MB_ICONERROR | MB_APPLMODAL); return 0;

		// Create and set up a sprite
		spr1=new hgeSprite(tex1, 0, 0, 80, 80);
		//spr1->SetHotSpot(40,40);
		spr2=new hgeSprite(tex2, 0, 0, 80, 80);
		//spr2->SetHotSpot(40,40);

		// Create and set up a sprite
		bgspr=new hgeSprite(bgtex, 0, 0, 800, 600);

		stspr1=new hgeSprite(sts1, 0, 0, 256, 256);
		//stspr1->SetHotSpot(40,40);
		stspr2=new hgeSprite(sts2, 0, 0, 256, 256);
		//stspr2->SetHotSpot(40,40);
		stspr0=new hgeSprite(sts0, 0, 0, 256, 256);
		//stspr0->SetHotSpot(40,40);

		// Load a font
		fnt=new hgeFont("font1.fnt");

		// Create and set up a particle system (Plan in the next version!)
		//spt=new hgeSprite(tex, 32, 32, 32, 32);
		//spt->SetBlendMode(BLEND_COLORMUL | BLEND_ALPHAADD | BLEND_NOZWRITE);
		//spt->SetHotSpot(16,16);
		//par=new hgeParticleSystem("trail.psi",spt);
		//par->Fire();

		// Initialize Taiko objects

		//pObjects=new tkObject[100];							// [Veto] Debug for asynchronous loading
		pObjects=new tkObject[MAX_OBJECTS];

		// Load beatmap
		InitBeatmap();

		// Load Taiko objects
		for(int i=0;i < MAX_OBJECTS;i++)
		{
			pObjects[i].x=INITX;
			pObjects[i].y=215.0f;
			pObjects[i].tktype=obj[i];
			pObjects[i].ststype=3;
			pObjects[i].stx = INITX;
		}

		// Let's rock now!
		BgmChannel = hge->Stream_Play(bgm,false,100);
		hge->Channel_Pause(BgmChannel);
		StartTime = hge->Timer_GetTime();
		hge->System_Start();

		hge->Channel_Stop(BgmChannel);

		// Delete created objects and free loaded resources
		//delete par;
		delete fnt;
		//delete spt;
		delete spr1;
		delete spr2;
		delete bgspr;
		delete stspr1;
		delete stspr2;
		delete stspr0;
		hge->Texture_Free(tex1);
		hge->Texture_Free(tex2);
		hge->Texture_Free(bgtex);
		hge->Texture_Free(sts1);
		hge->Texture_Free(sts2);
		hge->Texture_Free(sts0);
		hge->Effect_Free(snd1);
		hge->Effect_Free(snd2);
		hge->Stream_Free(bgm);
	}

	// Clean up and shutdown
	hge->System_Shutdown();
	hge->Release();
	return 0;
}
		delete spr1;
		delete spr2;
		delete bgspr;
		delete stspr1;
		delete stspr2;
		delete stspr0;
		hge->Texture_Free(tex1);
		hge->Texture_Free(tex2);
		hge->Texture_Free(bgtex);
		hge->Texture_Free(sts1);
		hge->Texture_Free(sts2);
		hge->Texture_Free(sts0);
		hge->Effect_Free(snd1);
		hge->Effect_Free(snd2);
		hge->Stream_Free(bgm);
	}

	// Clean up and shutdown
	hge->System_Shutdown();
	hge->Release();
	return 0;
}