#ifndef HAD__HPP
#define HAD__HPP

#include <iostream>

#include <list>
#include <vector>
#include <map>

#define SIGSLOT_PURE_ISO
#include <sigslot.h>

enum Smer {
	H_XPLUS = 1,
	H_XMINUS = -1,
	H_YPLUS = 2,
	H_YMINUS = -2,
	H_ZPLUS = 3,
	H_ZMINUS = -3
};
// mozne smery pohybu v mape
class Sm {
	public:
		static Smer otoc (Smer s) { return static_cast<Smer>(-s); }
		static int sign (Smer s) { return (s>0)?1:-1; }
		static bool x (Smer s) { return (s == H_XMINUS || s == H_XPLUS); }
		static bool x (Smer a, Smer b) { return (x(a) || x(b)); }
		static bool y (Smer s) { return (s == H_YMINUS || s == H_YPLUS); }
		static bool y (Smer a, Smer b) { return (y(a) || y(b)); }
		static bool z (Smer s) { return (s == H_ZMINUS || s == H_ZPLUS); }
		static bool z (Smer a, Smer b) { return (z(a) || z(b)); }
};

typedef long Rychlost;

// Typ Bonusu:
#define T_HAD 1
#define T_BONUS 2
#define TB_JIDLO 4
#define TB_RYCHLOST 8
#define TB_VZHLED 1
#define T_TIMED 32
#define TB_UNITS 64

// generovani nahodnych cisel
class Random {
	public:
		static unsigned long seed;
		static unsigned long get ();
};

// mapove koordinaty
class Coords {
	public:
		Coords (const Coords& c) { x = c.x; y = c.y; z = c.z; }
		Coords (int X=0, int Y=0, int Z=0) { x = X; y = Y; z = Z; }
		int x;
		int y;
		int z;

		Coords operator() (Smer s) // vraci koordinaty posunute do smeru s
		{
			switch (s) {
				case H_XPLUS: return Coords(x+1,y,z);
				case H_XMINUS: return Coords(x-1,y,z);
				case H_YPLUS: return Coords(x,y+1,z);
				case H_YMINUS: return Coords(x,y-1,z);
				case H_ZPLUS: return Coords(x,y,z+1);
				case H_ZMINUS: return Coords(x,y,z-1);
				default: return *this;
			}
		}

		bool operator== (const Coords& c) { return (x==c.x)&&(y==c.y)&&(z==c.z); }
};

typedef Coords MapSize;
// velikost mapy je shodou okolnosti hodne podobna koordinatum

class TOvladac;
typedef TOvladac* Ovladac;

class TVec;
typedef TVec* Vec;

class THad;
typedef THad* Had;

class THadovnik;
typedef THadovnik* Hadovnik;

class TBonus;
typedef TBonus* Bonus;

class TBonusovnik;
typedef TBonusovnik* Bonusovnik;

class TVyhodnocovac;
typedef TVyhodnocovac* Vyhodnocovac;

class TMapa;
typedef TMapa* Mapa;

class TInventar;
typedef TInventar* Inventar;

class TMapa;
typedef TMapa* Mapa;

// obecna Vec, ktera se muze vyskytovat v mape
class TVec {
	public:
		virtual bool jsi_typu (int) = 0; // viz Typy Bonusu
		virtual void interact (Had, Coords) = 0; // interakce pri narazu hada
};

class TOvladac {
	public:
		virtual Smer get () = 0;
		virtual bool ready () { return true; }
};

// bonus. vec, ktera se z mapy da snist
class TBonus : public TVec {
	public:
		sigslot::signal1<Bonus> spotrebovan;
		sigslot::signal1<Bonus> sneden;
		
		TBonus () : eaten (false) { }
		virtual ~TBonus () { }
		
		virtual bool jsi_typu (int typ) { return (typ & T_BONUS); }
		virtual void interact (Had, Coords);
		virtual bool interact (Bonus) { return false; }
		// interact (Bonus) je interakce s dalsimi bonusy v inventari hada
		// false = nepokracovat v prochazeni; true = pokracovat
		virtual bool is_eaten () { return eaten; }
		virtual void eat () {
			sneden (this);
			eaten = true;
		}
		virtual void kill ()
		{
			spotrebovan (this);
			delete this;
		}
		// instantni vycerpani

	protected:
		bool eaten;
};

class TUnitBonus : public TBonus {
	public:
		virtual ~TUnitBonus () { }
		virtual bool jsi_typu (int typ) { return (typ & (T_BONUS | TB_UNITS)); }
		
		TUnitBonus (int i) : units (i) { };
		virtual void feed (int i) { units += i; }
		virtual void consume () {
			units--;
			if (units == 0) spotrebovan (this);
		}
	protected:
		int units;
};

// food bonus. zvetsuje hadovi delku.
class TFoodBonus : public TUnitBonus {
	public:
		TFoodBonus (int i) : TUnitBonus (i) { }
		virtual ~TFoodBonus () { }
		virtual bool interact (Bonus);
		
		virtual bool jsi_typu (int typ) { return (typ & (T_BONUS | TB_JIDLO)); }
};

class TTimedBonus : public TBonus, public sigslot::has_slots<> {
	public:
		TTimedBonus (int Time) : time (Time) { }
		virtual bool jsi_typu (int typ) { return (typ & (T_BONUS | T_TIMED)); }
		virtual void tick () {
			time--;
			if (time == 0) kill ();
		}
		
	protected:
		int time;
};

class TSpeedBonus : public TTimedBonus {
	public:
		TSpeedBonus (int Time, Rychlost Speed) : TTimedBonus (Time), speed (Speed) { }
		virtual ~TSpeedBonus () { }
		virtual bool jsi_typu (int typ) { return (typ & (T_BONUS | TB_RYCHLOST | T_TIMED)); }
		virtual bool interact (Bonus) { return true; }
				
		virtual Rychlost rychlost () { return speed; }
		
	protected:
		Rychlost speed;
};


class TSpawner; // viz nize

class THad : public TVec, public sigslot::has_slots<> {
	public:
		sigslot::signal2<Had,Smer> zmena_smeru;
		sigslot::signal2<Had,Coords> pohyb;
		sigslot::signal1<Had> smrt;
		sigslot::signal1<Had> objeveni;
		sigslot::signal2<Had,Bonus> snedeni;
		
		typedef TSpawner spawner; 
		
		THad ();
		virtual ~THad ();
		virtual void set_map (Mapa m) { mapa = m; }

		virtual void spawn (Coords, Smer); // zjeveni se na nejakem urcitem miste

		virtual void interact (Had, Coords); // srazka s jinym hadem
		virtual bool jsi_typu (int typ) { return (typ & T_HAD); }
		
		virtual void jdi_na (Coords);
		virtual Coords hlava () const;
		virtual Smer kam_jdes () const;
		virtual void ted_jdes (Smer);
#define HAD_SPEED_LIMIT 5
		virtual Rychlost jak_rychle () const;
		virtual void snez (Bonus);
		virtual void umri ();
		virtual bool jsi_mrtev () const;
		
	protected:
		static int lastid;
		int id;
		// pro prirazeni unikatniho id kazdemu novemu hadovi

		typedef std::list<Coords> Telo;
		Telo telo;
		Mapa mapa;
		Inventar inventar;
		Rychlost rychlost;
		Smer kamjdu;
		bool mrtev;
};

class TSpawner { // tahle trida jenom vytvari nove hady
	public:
		virtual Had operator() () { return (new THad()); }
};
typedef TSpawner* Spawner;

template <class T>
class TTempSpawner : public TSpawner { //...a tahle vytvari nove hady urciteho typu
	public:
		virtual Had operator() () { return ((Had) new T()); }
};
// uziti: aby se nemusel templatovat cely hadovnik, dostane tenhle objekt, ktery pak vytvari nove hady jednoho typu za nej


class TInventar : public sigslot::has_slots<> {
	public:
		TInventar ();
		virtual ~TInventar ();

		void interact (Bonus);
		void kill ();
		bool jidlo (); // je v inventari nejake jidlo?
		Rychlost rychlost (); // je tam modifikator rychlosti?
//		void do_free ();

	protected:
		typedef std::list<Bonus> Batoh;
		Batoh inventar;

		void vyrad (Bonus);
};

class TVyhodnocovac {
	public:
		void proved (Smer, Had);
		void set_map (Mapa m) { mapa = m; }
			
	protected:
		Mapa mapa;
};

class THadovnik {
	public:
		THadovnik ();
		THadovnik (Spawner);
		~THadovnik ();

		void tick ();

		Had novy_ovladac (Ovladac); // vytvori noveho hada pro zadany ovladac
		void set_spawner (Spawner);
		Mapa get_map () { return mapa; }
		void set_map (Mapa);
		
	protected:
		struct HadOvl {
			Had had;
			Ovladac ovladac;
			Rychlost remain; // za jak dlouho se ma had pohnout?
		};
		typedef std::vector<HadOvl> HadOvls;
		HadOvls hadi;
		Vyhodnocovac vyhodnocovac;
		Spawner spawn;
		Mapa mapa;
};

class TMapa {
	public:
		TMapa (int x=1, int y=1, int z=1);
		virtual ~TMapa () { };

		virtual Vec& operator() (int, int, int);
		virtual Vec& operator() (const Coords&);
		// XXX tady POZOR:
		// sice to funguje tak, ze lze do mapa(a,b,c) priradit, ale:
		// jakmile je nejaka Vec odkazovana -jenom- z mapy, je to SPATNE

		virtual MapSize get_size () { return rozmery; }
		virtual int get_x () { return rozmery.x; }
		virtual int get_y () { return rozmery.y; }
		virtual int get_z () { return rozmery.z; }

		virtual Coords get_spawnpoint (); // volny spawn point
		virtual Coords get_free (); // libovolne volne pole
		
#define M_OUTSIDE ((Vec)1)
		Vec outside; // pokud jsou pozadovany koordinaty mimo mapu, vraci se reference na tohle

	protected:
		typedef std::vector<Coords> Pole;
		typedef std::vector<Vec> Line;
		typedef std::vector<Line> Surface;
		typedef std::vector<Surface> Space;
		Space mapa;
		MapSize rozmery;
		Pole spawnpoints;
};

class TBonusovnik : public sigslot::has_slots<> {
	public:
		sigslot::signal0<> ticker;
		
		TBonusovnik () : jidlo_v_mape(0) { }
		void set_map (Mapa m) { mapa = m; }
		void tick ();

	private:
		typedef std::map<Bonus,Coords> Bonusy;
		Bonusy bonusy;
		Mapa mapa;
		int jidlo_v_mape;

		void inject (Bonus, Coords);
		void inject (TTimedBonus*, Coords);
		void snez (Bonus);
		void uvolni (Bonus);
};

#endif
