#include "SDL.h"

#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>
#include <had.hpp>
#include "interface.hpp"
#include "hadi/h_stdout.hpp"

void Interface::init ()
{
	konec = false;
	init_sdl ();
	init_gl ();
	compile_lists ();

	mapa = new TMapa (15,5,15);
	hadovnik = new THadovnik (new TStdoutHad::spawner);
	bonusovnik = new TBonusovnik ();
	hadovnik->set_map (mapa);
	bonusovnik->set_map (mapa);

	rovne = H_ZMINUS; nahoru = H_YPLUS;

	int max;
	MapSize m = mapa->get_size();
	max = m.x; if (m.y>max) max = m.y; if (m.z>max) max = m.z;
	frac = 15/max; // 15 = sirka krychle
	half = frac/2;
}

void Interface::quit ()
{
	quit_gl ();
	quit_sdl ();
}

void Interface::init_sdl ()
{
	SDL_Init (SDL_INIT_VIDEO | SDL_INIT_TIMER);

	SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 5 );
	SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 5 );
	SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 5 );
	SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16 );
	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

	SDL_SetVideoMode (640, 480, 24, SDL_OPENGL);

}

void Interface::quit_sdl ()
{
	SDL_Quit ();
}

void Interface::init_gl ()
{
	glShadeModel (GL_SMOOTH);
	glEnable (GL_LINE_SMOOTH);
	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glHint (GL_LINE_SMOOTH_HINT, GL_NICEST);
	
	glCullFace (GL_BACK);
	glEnable (GL_CULL_FACE);

	glClearColor (0.0,0.0,0.0,1.0);
	glClearDepth (1.0);
	glEnable (GL_DEPTH_TEST);
	glDepthFunc (GL_LEQUAL);

	glViewport (0,0,640,480);
	
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	gluPerspective( 45.0, 640.0/480.0, 0.1, 100.0 );
//	glOrtho (-10,10, -7.5,7.5, 0,20);
//	glFrustum (5.5,-5.5, 4.125,-4.125, 4.5,15.5);

	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity ();

	GLfloat LightAmbient[]= { 0.3f, 0.3f, 0.3f, 1.0f };
	GLfloat LightDiffuse[]= { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat LightPosition[]= { 0.0f, 1.0f, 1.0f, 0.0f };
	glLightfv(GL_LIGHT0, GL_POSITION,LightPosition);
	glLightfv(GL_LIGHT0, GL_AMBIENT, LightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, LightDiffuse);
	glEnable (GL_LIGHT0);
	glEnable (GL_LIGHTING);
	glEnable (GL_COLOR_MATERIAL);
	glEnable (GL_NORMALIZE);
}

void Interface::quit_gl () { }

void Interface::tick ()
{
	SDL_Event event;
	Keysyms::iterator i;
	while (SDL_PollEvent(&event)) { // zpracovani sdl eventu - stisk klavesy/zavreni okna
		switch (event.type) {
			case SDL_KEYDOWN:
				if ((i = keysyms.find(event.key.keysym.sym))!=keysyms.end())
					i->second.ovladac->keydown(i->second.smer);
				break;
			case SDL_KEYUP:
				if ((i = keysyms.find(event.key.keysym.sym))!=keysyms.end())
					i->second.ovladac->keyup(i->second.smer);
				else if (event.key.keysym.sym == SDLK_ESCAPE)
					konec = true;
				break;
			case SDL_QUIT:
				konec = true;
				break;
		}
	}
	
	ovl_set ();
//	for (i = keysyms.begin () ; i != keysyms.end () ; i++) i->second.ovladac->set ();
	
	// vykresleni frame
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity ();

	glPushMatrix ();
		glRotatef (20, 1.0,0,0);
		glTranslatef (0,-5,-10);
		do_rotate ();
		glTranslatef (-hlava.x*frac, -hlava.y*frac, -hlava.z*frac);
		draw_map ();
	glPopMatrix ();

	SDL_GL_SwapBuffers ();
	
	// jeden herni tah:
	hadovnik->tick ();
	bonusovnik->tick ();

	// tohle musi jinam!
	SDL_Delay (50);
}

void Interface::draw_map ()
{	
	MapSize m = mapa->get_size();
	int *bd; // base direction
	int *fd; // forward direction
	int *ld; // line direction
	// kresli se v poradi b ( f ( l ( ) ) )
	
	glPushMatrix ();
//	glTranslatef (0,0,-half*m.z);
	for (int z = 0; z < m.z; z++) {
		glPushMatrix ();
//		glTranslatef (0,-half*m.y,0);
		for (int y = 0; y < m.y; y++) {
			glPushMatrix ();
//			glTranslatef (-half*m.x,0,0);
			for (int x = 0; x < m.x; x++) {
				if ((*mapa)(x,y,z)) {
					glColor4f(1,1,1,0.5);
					glCallList(DL_wirecube);
				} /*else  {
					glColor4f (1,1,1,1);
					glCallList (DL_axis3d);
				}*/
				glPushMatrix ();
					if (Sm::x(nahoru) && x == hlava.x) {
						glRotatef (-90*Sm::sign(nahoru),0.0,0.0,1.0);
						if (y == hlava.y || z == hlava.z) glColor4f (0.5,0.5,1.5,0.8);
						else glColor4f (0.3,0.3,1.0,0.5);
						glCallList(DL_bottom_srf);
					} else if (Sm::y(nahoru) && y == hlava.y) {
						if (nahoru == H_YMINUS) glRotatef (180, 1.0,0.0,0.0);
						if (x == hlava.x || z == hlava.z) glColor4f (0.5,0.5,1.5,0.8);
						else glColor4f (0.3,0.3,1.0,0.5);
						glCallList (DL_bottom_srf);
					}
					else if (Sm::z(nahoru) && z == hlava.z) {
						glRotatef (90*Sm::sign(nahoru),1.0,0.0,0.0);
						if (y == hlava.y || x == hlava.x) glColor4f (0.5,0.5,1.5,0.8);
						else glColor4f (0.3,0.3,1.0,0.5);
						glCallList (DL_bottom_srf);
					}
				glPopMatrix ();

				if ((*mapa)(x,y,z) > M_OUTSIDE) { // kresleni hadu/bonusu. to budou obstaravat jejich metody kresli(), ale zatim je to tady
					Vec v = (*mapa)(x,y,z);
					if (v->jsi_typu(T_BONUS)) {
						glColor4f (0,1,0,1);
						glCallList (DL_bonus);
					} else if (v->jsi_typu(T_HAD)) {
						glColor4f (1,1,0,1);
						glCallList (DL_snakehead);
					}
				}
				glTranslatef (frac,0,0);
			}
/*			glRotatef (90, 0,1,0);
			glCallList (DL_axis2d);*/
			glPopMatrix ();
			glTranslatef (0,frac,0);
		}
/*		for (int y = 0; y < m.y; y++) {
			glPushMatrix ();
			glTranslatef (-half*m.x,0,0);
			for (int x = 0; x < m.x; x++) {
				glCallList (DL_axis2d);
				glTranslatef (frac,0,0);
			}
			glPopMatrix ();
			glTranslatef (0,frac,0);
		}*/
		glPopMatrix ();
		glTranslatef (0,0,frac);
	}
/*	for (int z = 0; z < m.z; z++) {
		glPushMatrix ();
		glTranslatef (0,-half*m.y,0);
		for (int y = 0; */
	glPopMatrix();
}

void Interface::novy_ovladac (Keys keys)
{
	Keysym ks;
	InterfaceOvl ovl = new TInterfaceOvl;
	ovl_set.connect (ovl, &TInterfaceOvl::set);
	Had had = hadovnik->novy_ovladac (ovl);
	ovl->set_had (had);

	if (keysyms.empty()) {
		ovl->zmena_smeru.connect (this, &Interface::zmena_smeru);
		had->pohyb.connect (this, &Interface::pohyb_hada);
	}

	ks.ovladac = ovl;
	ks.smer = IS_LEFT;
	keysyms[keys.left] = ks;
	ks.smer = IS_RIGHT;
	keysyms[keys.right] = ks;
	ks.smer = IS_UP;
	keysyms[keys.up] = ks;
	ks.smer = IS_DOWN;
	keysyms[keys.down] = ks;
}

void Interface::zmena_smeru (TInterfaceOvl* o, TInterfaceOvl::ZmenaSmeru z)
{
	rovne = z.s;
	nahoru = z.n;
	std::cout << z.s << ' ' << z.n << std::endl;
}

void stepto (int& what, int where)
{
//	what = where; return;
	if (what != where) {
//		std::cout << "stepping " << what << " to " << where << std::endl;
		int a = 15;
		if ((what-where)>180) what -= 360;
		if ((where-what)>180) where -= 360;
		if (what < where) {
			what += a;
			if (what>where) what = where;
		} else {
			what -= a;
			if (what<where) what = where;
		}
	}
}

void Interface::do_rotate ()
{
	// nastaveni smeru
//	rot_r = Coords (0,0,0); rot_n = Coords (0,0,0);
	if (Sm::z(rovne)) { 
		if (rovne == H_ZPLUS) stepto (rot_r.x,180);
		else stepto (rot_r.x,0);
		stepto (rot_r.y,0);
	} else if (Sm::y(rovne)) {
		stepto (rot_r.x, -90*Sm::sign(rovne));
		stepto (rot_r.y,0); 
	} else {
		stepto (rot_r.y, 90*Sm::sign(rovne));
		stepto (rot_r.x, 0);
	}
	glRotatef (rot_r.x, 1.0,0.0,0.0);
	glRotatef (rot_r.y, 0.0,1.0,0.0);

	// nastaveni nahore
	if (Sm::z(rovne)) {
		if (Sm::x(nahoru)) stepto (rot_n.z, -90*Sm::sign(nahoru)*Sm::sign(rovne));
		else if (Sm::sign(nahoru)*Sm::sign(rovne) > 0) stepto (rot_n.z, 180);
		else stepto (rot_n.z, 0);
		stepto (rot_n.x, 0);
		stepto (rot_n.y, 0);
	} else if (Sm::y(rovne)) {
		if (Sm::x(nahoru)) stepto (rot_n.y, -90*Sm::sign(nahoru)*Sm::sign(rovne));
		else if (Sm::sign(nahoru)*Sm::sign(rovne) < 0) stepto (rot_n.y, 180);
		else stepto (rot_n.y, 0);
		stepto (rot_n.x, 0);
		stepto (rot_n.z, 0);
	} else {
		if (nahoru == H_YMINUS) stepto (rot_n.x, 180);
		else if (Sm::z(nahoru)) stepto (rot_n.x, -90*Sm::sign(nahoru));
		else stepto (rot_n.x, 0);
		stepto (rot_n.z, 0);
		stepto (rot_n.y, 0);
	}
	glRotatef (rot_n.x, 1.0,0.0,0.0);
	glRotatef (rot_n.y, 0.0,1.0,0.0);
	glRotatef (rot_n.z, 0.0,0.0,1.0);
}

void Interface::compile_lists ()
{
	glNewList (DL_wirecube, GL_COMPILE);
		glPushMatrix (); glTranslatef (-0.5, -0.5, -0.5);
		glBegin (GL_LINE_LOOP);
			glNormal3f (-1,-1,-1); glVertex3f (0,0,0);
			glNormal3f (1,-1,-1); glVertex3f (1,0,0);
			glNormal3f (1,-1,1); glVertex3f (1,0,1);
			glNormal3f (-1,-1,1); glVertex3f (0,0,1);
		glEnd ();
		glBegin (GL_LINE_LOOP);
			glNormal3f (-1,1,-1); glVertex3f (0,1,0);
			glNormal3f (1,1,-1); glVertex3f (1,1,0);
			glNormal3f (1,1,1); glVertex3f (1,1,1);
			glNormal3f (-1,1,1); glVertex3f (0,1,1);
		glEnd ();
		glBegin (GL_LINES);
			glNormal3f (-1,-1,-1); glVertex3f (0,0,0);
			glNormal3f (-1,1,-1); glVertex3f (0,1,0);
			glNormal3f (1,-1,-1); glVertex3f (1,0,0);
			glNormal3f (1,1,-1); glVertex3f (1,1,0);
			glNormal3f (1,-1,1); glVertex3f (1,0,1);
			glNormal3f (1,1,1); glVertex3f (1,1,1);
			glNormal3f (-1,-1,1); glVertex3f (0,0,1);
			glNormal3f (-1,1,1); glVertex3f (0,1,1);
		glEnd ();
		glPopMatrix ();
	glEndList ();

	glNewList (DL_axis3d, GL_COMPILE);
		glPushMatrix (); glTranslatef (-0.5,-0.5,-0.5);
		glBegin (GL_LINES);
			glNormal3f (-1,-1,-1); glVertex3f (0,0,0);
			glNormal3f (-1,-1,1); glVertex3f (0,0,1);
			glNormal3f (-1,-1,-1); glVertex3f (0,0,0);
			glNormal3f (-1,1,-1); glVertex3f (0,1,0);
			glNormal3f (-1,-1,-1); glVertex3f (0,0,0);
			glNormal3f (1,-1,-1); glVertex3f (1,0,0);
		glEnd ();
		glPopMatrix ();
	glEndList ();

	glNewList (DL_axis2d, GL_COMPILE);
		glPushMatrix (); glTranslatef (-0.5,-0.5,-0.5);
		glBegin (GL_LINE_STRIP);
			glNormal3f (-1,1,1); glVertex3f (0,1,0);
			glNormal3f (-1,-1,1); glVertex3f (0,0,0);
			glNormal3f (1,-1,1); glVertex3f (1,0,0);
		glEnd ();
	glEndList ();

	glNewList (DL_bonus, GL_COMPILE);
#define B 0.3
		glBegin (GL_TRIANGLE_FAN);
			glNormal3f (0,1,0); glVertex3f (0,B,0);
			glNormal3f (-1,0,0); glVertex3f (-B,0,0);
			glNormal3f (0,0,1); glVertex3f (0,0,B);
			glNormal3f (1,0,0); glVertex3f (B,0,0);
			glNormal3f (0,0,-1); glVertex3f (0,0,-B);
			glNormal3f (-1,0,0); glVertex3f (-B,0,0);
		glEnd (); glBegin (GL_TRIANGLE_FAN);
			glNormal3f (0,-1,0); glVertex3f (0,-B,0);
			glNormal3f (-1,0,0); glVertex3f (-B,0,0);
			glNormal3f (0,0,1); glVertex3f (0,0,B);
			glNormal3f (0,0,-1); glVertex3f (0,0,-B);
			glNormal3f (1,0,0); glVertex3f (B,0,0);
			glNormal3f (-1,0,0); glVertex3f (-B,0,0);
		glEnd ();
#undef B
	glEndList ();

	glNewList (DL_snakehead, GL_COMPILE);
#define B 0.3
		// pas okolo:
		glBegin (GL_TRIANGLE_STRIP);
			glNormal3f (-B,B,0.5); glVertex3f (-B,B,0.5); 
			glNormal3f (-B,-B,0.5); glVertex3f (-B,-B,0.5);
			glNormal3f (B,B,0.5); glVertex3f (B,B,0.5);
			glNormal3f (B,-B,0.5); glVertex3f (B,-B,0.5);
			glNormal3f (0.5,B,B); glVertex3f (0.5,B,B);
			glNormal3f (0.5,-B,B); glVertex3f (0.5,-B,B);
			glNormal3f (0.5,B,-B); glVertex3f (0.5,B,-B);
			glNormal3f (0.5,-B,-B); glVertex3f (0.5,-B,-B);
			glNormal3f (B,B,-0.5); glVertex3f (B,B,-0.5);
			glNormal3f (B,-B,-0.5); glVertex3f (B,-B,-0.5);
			glNormal3f (-B,B,-0.5); glVertex3f (-B,B,-0.5);
			glNormal3f (-B,-B,-0.5); glVertex3f (-B,-B,-0.5);
			glNormal3f (-0.5,B,-B); glVertex3f (-0.5,B,-B);
			glNormal3f (-0.5,-B,-B); glVertex3f (-0.5,-B,-B);
			glNormal3f (-0.5,B,B); glVertex3f (-0.5,B,B);
			glNormal3f (-0.5,-B,B); glVertex3f (-0.5,-B,B);
			glNormal3f (-B,B,0.5); glVertex3f (-B,B,0.5);
			glNormal3f (-B,-B,0.5); glVertex3f (-B,-B,0.5);
		glEnd ();

		// pasy nahore a dole:
		glBegin (GL_TRIANGLE_STRIP);
			glNormal3f (-0.5,B,-B); glVertex3f (-0.5,B,-B);
			glNormal3f (-0.5,B,B); glVertex3f (-0.5,B,B);
			glNormal3f (-B,0.5,-B); glVertex3f (-B,0.5,-B);
			glNormal3f (-B,0.5,B); glVertex3f (-B,0.5,B);
			glNormal3f (B,0.5,-B); glVertex3f (B,0.5,-B);
			glNormal3f (B,0.5,B); glVertex3f (B,0.5,B);
			glNormal3f (0.5,B,-B); glVertex3f (0.5,B,-B);
			glNormal3f (0.5,B,B); glVertex3f (0.5,B,B);
		glEnd ();
		glBegin (GL_TRIANGLE_STRIP);
			glNormal3f (0.5,-B,-B); glVertex3f (0.5,-B,-B);
			glNormal3f (0.5,-B,B); glVertex3f (0.5,-B,B);
			glNormal3f (B,-0.5,-B); glVertex3f (B,-0.5,-B);
			glNormal3f (B,-0.5,B); glVertex3f (B,-0.5,B);
			glNormal3f (-B,-0.5,-B); glVertex3f (-B,-0.5,-B);
			glNormal3f (-B,-0.5,B); glVertex3f (-B,-0.5,B);
			glNormal3f (-0.5,-B,-B); glVertex3f (-0.5,-B,-B);
			glNormal3f (-0.5,-B,B); glVertex3f (-0.5,-B,B);
		glEnd ();

		// doplnky:
		glBegin (GL_TRIANGLE_STRIP);
			glNormal3f (-0.5,B,B); glVertex3f (-0.5,B,B);
			glNormal3f (-B,B,0.5); glVertex3f (-B,B,0.5);
			glNormal3f (-B,0.5,B); glVertex3f (-B,0.5,B);
			glNormal3f (B,B,0.5); glVertex3f (B,B,0.5);
			glNormal3f (B,0.5,B); glVertex3f (B,0.5,B);
			glNormal3f (0.5,B,B); glVertex3f (0.5,B,B);
		glEnd ();
		glBegin (GL_TRIANGLE_STRIP);
			glNormal3f (0.5,B,-B); glVertex3f (0.5,B,-B);
			glNormal3f (B,B,-0.5); glVertex3f (B,B,-0.5);
			glNormal3f (B,0.5,-B); glVertex3f (B,0.5,-B);
			glNormal3f (-B,B,-0.5); glVertex3f (-B,B,-0.5);
			glNormal3f (-B,0.5,-B); glVertex3f (-B,0.5,-B);
			glNormal3f (-0.5,B,-B); glVertex3f (-0.5,B,-B);
		glEnd ();	
#undef B
	glEndList ();

	glNewList (DL_bottom_srf, GL_COMPILE);
		glPushMatrix ();
			glTranslatef (-0.5,-0.5,-0.5);
			glBegin (GL_QUADS);
//				glColor4f (0.3,0.3,1.0,0.5);
				glNormal3f (0.0,1.0,0.0);
				glVertex3f (0,0,0);
				glVertex3f (0,0,1);
				glVertex3f (1,0,1);
				glVertex3f (1,0,0);
			glEnd ();
			glBegin (GL_LINES);
				glColor4f (1.0,0.3,0.3,1.0);
				glVertex3f (0,0.001,0);
				glVertex3f (0,0.001,0.2);
				glVertex3f (0,0.001,0);
				glVertex3f (0.2,0.001,0);
				glVertex3f (1,0.001,1);
				glVertex3f (1,0.001,0.8);
				glVertex3f (1,0.001,1);
				glVertex3f (0.8,0.001,1);
			glEnd ();
		glPopMatrix ();
	glEndList ();
}
	
