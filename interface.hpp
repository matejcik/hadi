#ifndef INTERFACE__HPP
#define INTERFACE__HPP

#include "SDL.h"
#include <had.hpp>
#include <list>
#include <map>

// typ bonusu:
#define T_GL 128
// XXX pozor, tohle by chtělo udělat jinak

typedef enum { IS_UP, IS_DOWN, IS_LEFT, IS_RIGHT } IfSmer;
// smer zadatelny z klavesnice

class TInterfaceOvl : public TOvladac, public sigslot::has_slots<> {
	public:
		struct ZmenaSmeru {
			IfSmer i;
			Smer s;
			Smer n;
		};
		sigslot::signal2<TInterfaceOvl*,ZmenaSmeru> zmena_smeru;
		
		TInterfaceOvl () : keypressed (false), smer (H_ZMINUS), nahoru (H_YPLUS) { }
		virtual Smer get ();

		virtual void keydown (IfSmer);
		virtual void keyup (IfSmer);
		virtual void set (); // vola se v okamziku, kdy se uz dalsi klavesy nemaji pocitat do tohoto tahu
		
		virtual void set_had (Had h) { had = h; }
	protected:
		Had had;
		Smer smer;
		IfSmer next;
		Smer nahoru; // pro prepocet IfSmeru na Smer
		bool keypressed;
};
typedef TInterfaceOvl* InterfaceOvl;

// Vec, ktera se umi sama kreslit
class TGLVec {
	public:
		virtual void kresli () = 0;
};
typedef TGLVec* GLVec;

class TGLHad : public THad, public TGLVec {
	public:
		typedef TTempSpawner<TGLHad> spawner;
		virtual bool jsi_typu (int typ) { return (typ & (T_HAD | T_GL)); }
		virtual void kresli ();
};
typedef TGLHad* GLHad;

class TGLBonus : public TBonus, public TGLVec {
	public:
		virtual void kresli ();
};
typedef TGLBonus* GLBonus;


// ***************

// interface. tahle vec ridi celou hru
class Interface : public sigslot::has_slots<> {
	public:
		sigslot::signal0<> ovl_set;
		
		struct Keys { // klavesy pro jednoho hrace
			SDLKey left;
			SDLKey right;
			SDLKey up;
			SDLKey down;
		};

		void init ();
		void quit ();
		void tick ();
		// provedeni jednoho cyklu.
		// to se muze delat v samostatnem threadu nebo v nekonecne smycce v main (). proto je lepsi, kdyz nekonecna smycka neni uvnitr tridy

		bool end () { return konec; } // uz je konec hry?

//		void register_had (Had);
		void novy_ovladac (Keys); // vytvoreni noveho klavesnicoveho ovladace
		
		// vytvoreni noveho ovladace jineho typu
		template <class T> T* novy_externi_ovl (const T* hint)
		{
			T* o = new T (hadovnik);
			ovladace.push_back (static_cast<Ovladac>(o));
			return o;
		}

	protected:
		typedef std::list<GLHad> Hadi;
		typedef std::vector<Ovladac> Ovladace;
		struct Keysym {
			InterfaceOvl ovladac;
			IfSmer smer;
		};
		typedef std::map<SDLKey,Keysym> Keysyms;

		Keysyms keysyms; // mapovani klavesa -> smer a ovladac
		Hadi hadi;
		Ovladace ovladace;

		Mapa mapa;
		Hadovnik hadovnik;
		Bonusovnik bonusovnik;

		bool konec;
		void init_sdl ();
		void init_gl ();

		void compile_lists (); // vytvoreni display listu
		
		void quit_gl ();
		void quit_sdl ();
		
		void draw_map ();
		void zmena_smeru (TInterfaceOvl*, TInterfaceOvl::ZmenaSmeru);
		void pohyb_hada (Had h, Coords c) { hlava = c; }
		void do_rotate ();
		Smer rovne,nahoru;
		Coords hlava;
		Coords rot_r,rot_n;
		float frac,half;

#define DL_wirecube 1
#define DL_solidcube 2
#define DL_snakebody 3
#define DL_snakehead 4
#define DL_bonus 5
#define DL_axis3d 6
#define DL_axis2d 7
#define DL_bottom_srf 8
		// id display listu
};

#endif
