#ifndef heading
#include <stdbool.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_audio.h>

#define heading
/**
*Szeroko�� bufora i wy�wietlacza
*/
#define DISP_W 1280
/**
*Wysoko�� bufora i wy�wietlacza
*/
#define DISP_H 960
/**
*Naci�ni�ty klawisz
*/
#define KEY_USED   1
/**
*Odpuszczony klawisz
*/
#define KEY_RELEASED 2
/**
*Liczba klatek na sekund�
*/
#define FRAMERATE 40
/**
*Ilo�� przeciwnik�w
*/
#define ENEMYCOUNT 72
/**
*Ilo�� przeciwnik�w na rz�d
*/
#define ENEMYPERROW 12
/**
*Wysoko�� tekstury gracza
*/
#define PLAYERHEIGHT 64
/**
*Szeroko�� tekstury gracza
*/
#define PLAYERWIDTH 64
/**
*Wysoko�� tekstury �ciany
*/
#define WALLHEIGHT 16
/**
*Szeroko�� tekstury �ciany
*/
#define WALLWIDTH 32
/**
*Wysoko�� tekstury przeciwnika
*/
#define ENEMYHEIGHT 32
/**
*Szeroko�� tekstury przeciwnika
*/
#define ENEMYWIDTH 32
/**
*Wysoko�� tekstury pocisku
*/
#define BULLETHEIGHT 32
/**
*Szeroko�� tekstury pocisku
*/
#define BULLETWIDTH 16
/**
*Bia�y kolor
*/
#define WHITE al_map_rgb(255, 255, 255)
/**
*��ty kolor
*/
#define YELLOW al_map_rgb(255, 215, 160)
/**
*Maksymalne przesuni�cie wertykalne czcionki
*/
#define FONTOFFSETY al_get_font_line_height(font) + DISP_H/96
/**
*Grubo�� rysowanych linii/przycisk�w
*/
#define BOXTHICKNESS 5
/**
*Podstawowa wertykalna pozycja przycisku
*/
#define BASEBOXY DISP_H / 6
/**
*Podstawowa horyzontalna pozycja przycisku
*/
#define BASEBOXX DISP_W * 3 / 8
/**
*Podstawowa szeroko�� przycisku
*/
#define BOXOFFSETX DISP_W / 4
/**
*Podstawowa wysoko�� przycisku
*/
#define BOXOFFSETY DISP_H * 9 / 48
/**
*Typ wyliczeniowy reprezentuj�cy obecn� scen� programu
*/
typedef enum EnScreenFlag {
    /// Scena g��wnego menu
    start = 0, 
    /// Scena gry
    play, 
    /// Scena odliczania
    countdown, 
    /// Scena inicjalizacji zmiennych
    initiation, 
    /// Scena przegrania gry
    gameover, 
    /// Scena sklepu
    shop, 
    /// Scena opcji
    other, 
    /// Scena wpisywania wynik�w
    scores 
}EnScreenFlag;
/**
*Struktura obszar�w kolizyjnych
*/
typedef struct StrRect {
    int 
        /// Lewa granica
        x1, 
        /// Prawa granica
        x2, 
        /// G�rna granica
        y1, 
        /// Dolna granica
        y2;
}StrRect;
/**
*Struktura reprezentuj�ca w�ze� listy przechowuj�cy koordynaty x i y
*/
typedef struct StrList {
    int 
        /// Pozycja horyzontalna
        x, 
        /// Pozycja wertykalna
        y;
        /// Wska�nik na kolejny element listy
        struct StrList* next;
}StrList;
/**
*Struktura przechowuj�cy statystyki obiekt�w
*/
typedef struct StrEntStats {
    char
        /// Szybko�� poruszania si� przeciwnik�w 
        enMovSpd, 
        /// Szybko�� poruszania si� pocisk�w przeciwnik�w 
        enBullSpd, 
        /// Minimalna pauza czasowa pomi�dzy kolejnymi pociskami przeciwnik�w
        enFireWaitMax, 
        /// Czas do zako�czenie minimalnej pauzy czasowej mi�dzy kolejnymi pociskami przeciwnik�w
        enFireWaitNow, 
        /// Szybko�� poruszania si� gracza 
        plMovSpd, 
        /// Szybko�� poruszania si� pocisk�w gracza
        plBullSpd, 
        /// Minimalna pauza czasowa pomi�dzy kolejnymi pociskami gracza
        plFireWaitMax, 
        /// Czas do zako�czenie minimalnej pauzy czasowej mi�dzy kolejnymi pociskami gracza
        plFireWaitNow, 
        /// Ilo�� �cian 
        wallAmnt;
    int 
        /// Horyzontalna pozycja gracza
        plX, 
        /// Wertykalna pozycja gracza
        plY;
}StrEntStats;
/**
*Struktura przechowuj�cy obecny stan programu
*/
typedef struct StrGaState {
    bool 
        /// Stan wyj�cia z programu
        quitProg, 
        /// Stan reprezentuj�cy czy kolejna klatka programu powinna by� narysowana
        reDraw, 
        /// Stan reprezentuj�cy czy menu pauzy powinno by� narysowane
        pauseMenuFlag;
    char 
        /// Obecnie wybrany przycisk w menu 
        menuCount, 
        /// Stoper
        timer, 
        /// Obecna runda
        roundNum;
        /// Obecna wynik
    int currentScore;
        /// Obecny scena programu
    EnScreenFlag screenFlag;
}StrGaState;
/**
*Struktura posiadaj�ca zmienne zapisywane w pliku data.txt
*/
typedef struct StrFileVars {
    int 
        /// Poziom ulepszenia pr�dko�ci poruszania si�
        movSpdUpg, 
        /// Poziom ulepszenia pr�dko�ci poruszania si� pocisk�w
        bullSpdUpg, 
        /// Poziom ulepszenia pr�dko�ci strzelania
        attSpdUpg, 
        /// Wybrany schemat przycisk�w
        keyBindFlag;
        /// Ca�kowity wynik do u�ycia jako waluta w sklepie
    long totalScore;
        /// Tablica przechowuj�ca wyniki w tablicy wynik�w
    int scoreTable[8];
        /// Tablica przechowuj�ca nazwy w tablicy wynik�w
    char nameTable[65];
}StrFileVars;

/**
*Funkcja pomocnicza sprawdzaj�ca czy inicjalizacja si� uda�a
*(void) <- (bool) - konwertowana zmienna na bool + (char*) - nazwa zmiennej
*/
void _initTest(bool, char*);
/**
*Funkcja inicjuj�ca warto�ci zwi�zane z bibliotek� ALLEGRO5
*(void) <- (void)
*/
void _initAllegroVars();
/**
*Funkcja inicjuj�ca struktur� przechowuj�c� stany programu
*(StrGaState) - zwracana struktura stan�w gry <- (void)
*/
StrGaState _initGameState();
/**
*Funkcja inicjuj�ca tablic� obiekt�w �cian
*(StrRect*) - zwracana tablica struktur obiekt�w �cian <- (int) - ilo�� �cian
*/
StrRect* _initWallArr(int);
/**
*Funkcja inicjuj�ca list� obiekt�w przeciwnik�w
*(StrList*) - zwracany wska�nik na pocz�tek listy <- (void)
*/
StrList* _initEnemyList();
/**
*Funkcja inicjuj�ca struktur� przechowuj�c� statystyki obiekt�w
*(StrEntStats) - zwracana struktura statystyki obiekt�w <- (int) - numer kolejnej rundy + (StrFileVars) - struktura zmiennych plikowych
*/
StrEntStats _initEntStats(int, StrFileVars);
/**
*Funkcja kontroluj�ca zapisywanie/odczytywanie warto�ci z pliku
*(void) <- (StrFileVars*) - wska�nik na struktur� zmiennych plikowych + (int) - flaga decyduj�ca o zapisywaniu/odczytywaniu
*/
void _initFileVars(StrFileVars*, int);
/**
*Funkcja pomocnicza zapisuj�ca warto�ci do pliku
*(void) <- (StrFileVars) - struktura zmiennych plikowych
*/
void _initFileVarsWrite(StrFileVars);
/**
*Funkcja pomocnicza zapisuj�ca warto�ci do pliku je�li nie zosta� on stworzony wcze�niej
*(void) <- (void) 
*/
void _initFileVarsWriteFirst();
/**
*Funkcja pomocnicza odczytuj�ca warto�ci plikowe
*(void) <- (StrFileVars*) - wska�nik na struktur� zmiennych plikowych
*/
void _initFileVarsRead(StrFileVars*);
/**
*Funkcja inicjuj�ca struktury potrzebne podczas rozpocz�cia gry
*(EnScreenFlag) - zwracana flaga rozpocz�cia gry <- (StrEntStats*) - wska�nik na struktur� statystyk obiekt�w + (StrList**) - wska�nik na wska�nik przechowuj�cy pocz�tek listy przeciwnik�w 
* + (StrList**) - wska�nik na wska�nik przechowuj�cy pocz�tek listy pocisk�w gracza + (StrList**) - wska�nik na wska�nik przechowuj�cy poczatek listy pocisk�w przeciwnik�w 
* + (StrRect**) - wska�nik na tablic� przechowuj�c� obiekty �ciany + (StrRect*) - wska�nik na obiekt obszaru kolizyjnego przeciwnik�w + (StrFileVars) - struktura zmiennych plikowych 
*/
EnScreenFlag _firstRound(StrEntStats*, StrList**, StrList**, StrList**, StrRect**, StrRect*, StrFileVars);
/**
*Funkcja przygotowuj�ca gr� do kolejnej rundy
*(EnScreenFlag) - zwracana flaga rozpocz�cia gry <- (StrGaState*) - wska�nik na struktur� stan�w gry + (StrFileVars*) - wska�nik na struktur� zmiennych plikowych
*/
EnScreenFlag _nextRound(StrGaState*, StrFileVars*);
/**
*Funkcja deinicjalizuj�ca list�
*(StrList*) - zwracana warto�� NULL <- (StrList*) - wska�nik na list�
*/
StrList* _freeList(StrList*);
/**
*Funkcja deinicjalizuj�ca tablic� obiekt�w �cian
*(StrRect*) - zwracana warto�� NULL <- (StrRect*) - wska�nik na tablic� obiekt�w �cian
*/
StrRect* _freeWallArr(StrRect*);
/**
*Funkcja usuwaj�ca konkretny element listy
*(void) <- (StrList**) - wska�nik na wska�nik poprzedniego element listy + (StrList**) - wska�nik na wska�nik elementu listy do usuni�cia
*/
void _listDelete(StrList**, StrList**);
/**
*Funkcja sprawdzaj�ca czy funkcja malloc si� powiod�a
*(void) <- (void*) - sprawdzany wska�nik, konwertowany na typ (void*)
*/
void _ptrCheckNull(void*);
/**
*Funkcja kontroluj�c� rysowanie kiedy program jest w scenie gry
*(void) <- (StrEntStats) - struktura statystyk obiekt�w + (StrList*) - wska�nik listy przeciwnik�w + (StrList*) - wska�nik listy pocisk�w gracza + (StrList*) - wska�nik listy pocisk�w przeciwnik�w 
*+ (StrRect*) - tablica obiekt�w �ciany + (StrRect) - struktura obiektu obszaru kolizji przeciwnik�w + (int) - obecny wynik + (int) - obecny numer rundy
*/
void _logicPlayDraw(StrEntStats, StrList*, StrList*, StrList*, StrRect*, StrRect, int, int);
/**
*Funkcja kontroluj�c� logik� kiedy program jest w scenie gry
*(void) <- (StrGaState*) - wska�nik na struktur� stan�w gry +  (StrEntStats*) - wska�nik na struktur� statystyk obiekt�w + (StrList**) - wska�nik na wska�nik listy przeciwnik�w
*+ (StrList**) - wska�nik na wska�nik listy pocisk�w gracza + (StrList**) - wska�nik na wska�nik listy pocisk�w przeciwnik�w + (StrRect*) - tablica obiekt�w �ciany 
*+ (StrRect*) - wska�nik na struktur� obiektu obszaru kolizji przeciwnik�w + (StrFileVars) - struktura zmiennych plikowych
*/
void _logicPlayMec(StrGaState*, StrEntStats*, StrList**, StrList**, StrList**, StrRect*, StrRect*, StrFileVars*);
/**
*Funkcja zajmuj�ca si� zachowaniem przeciwnik�w
*(void) <- (StrList*) - wska�nik na list� przeciwnik�w + (StrList**) - wska�nik na wska�nik listy pocisk�w przeciwnik�w + (StrRect*) - wska�nik na obiekt obszaru kolizji przeciwnik�w
* + (StrEntStats*) - wska�nik na struktur� statystyk obiekt�w
*/
void _enemyBehaviour(StrList*, StrList**, StrRect*, StrEntStats*);
/**
*Funkcja zajmuj�ca si� sterowaniem statku gracza
*(void) <- (StrEntStats*) - wska�nik na struktur� statystyk obiekt�w + (StrList**) - wska�nik na wska�nik listy pocisk�w gracza + (int) - flaga wybranego schematu sterowania
*/
void _playerBehaviour(StrEntStats*, StrList**, int);
/**
*Funkcja zajmuj�ca si� stworzeniem pocisku
*(void) <- (StrList**) - wska�nik na wska�nik listy pocisk�w + (int) - pozycja horyzontalna jak� nada� pociskowi + (int) - pozycja wertykalna jak� nada� pociskowi 
*/
void _bulletCreate(StrList**, int, int);
/**
*Funkcja zajmuj�ca si� poruszaniem pocisk�w
*(void) <- (StrList*) - wska�nik na w list� pocisk�w + (int) - pr�dko�� poruszania si� pocisk�w
*/
void _bulletMovement(StrList*, int);
/**
*Funkcja sprawdzaj�ca kolizj� wszystkich pocisku gracza
*(void) <- (StrList**) - wska�nik na wska�nik listy przeciwnik�w + (StrList**) - wska�nik na wska�nik listy pocisk�w gracza + (StrRect*) - tablica obiekt�w �ciany +
*+ (StrRect) - struktura obiektu obszaru kolizji przeciwnik�w + (int) - ilo�� obiekt�w �ciany + (int*) - wska�nik na zmienn� przechowuj�c� obecny wynik
*/
void _playerBulletsCollision(StrList**, StrList**, StrRect*, StrRect, int, int*);
/**
*Funkcja pomocnicza sprawdzaj�ca kolizj� danego pocisku gracza
*(StrList*) - zwracany wska�nik traktowany jako test czy je�li nasz�o do kolizji, by�a ona z pierwszym przeciwnikiem listy <- (StrList**) - wska�nik na wska�nik obecnie analizowanego pocisku gracza 
*+ (StrList**) - wska�nik na wska�nik poprzedniego pocisku gracza  + (StrRect*) - tablica obiekt�w �ciany + (StrList**) - wska�nik na wska�nik listy przeciwnik�w 
*+ (StrRect) - struktura obiektu obszaru kolizji przeciwnik�w + (int) - ilo�� obiekt�w �ciany + (int*) - wska�nik na zmienn� przechowuj�c� obecny wynik
*/
StrList* _playerBulletsCollisionCheck(StrList**, StrList**, StrRect*, StrList**, StrRect, int, int*);
/**
*Funkcja sprawdzaj�ca kolizj� wszystkich pocisk�w przeciwnik�w
*(bool) - zmienna sygnalizuj�ca czy nast�pi�a kolizja z graczem <- (StrEntStats) - struktura statystyk obiekt�w + (StrList**) - wska�nik na wska�nik listy pocisk�w przeciwnik�w 
*+ (StrRect*) - tablica obiekt�w �ciany
*/
bool _enemyBulletsCollision(StrEntStats, StrList**, StrRect*);
/**
*Funkcja pomocznicza sprawdzaj�ca kolizj� danego pocisku przeciwnika
*(bool) - zmienna sygnalizuj�ca czy nast�pi�a kolizja z graczem <- (StrEntStats) - struktura statystyk obiekt�w + (StrList**) - wska�nik na wska�nik obecnie analizowanego pocisku 
*+ (StrList**) - wska�nik na wska�nik poprzedniego pocisku + (StrRect*) - tablica obiekt�w �ciany
*/
bool _enemyBulletsCollisionCheck(StrEntStats, StrList**, StrList**, StrRect*);
/**
*Funkcja sprawdzaj�ca kolizj� mi�dzy dwoma obiektami
*(bool) - zmienna sygnalizuj�ca czy nast�pi�a kolizja <- (int) - lewa granica pierwszego obiektu + (int) - g�rna granica pierwszego obietku + (int) - prawa granica pierwszego obiektu
* + (int) - dolna granica pierwszego obiektu + (int) - lewa granica drugiego obiektu + (int) - g�rna granica drugiego obiektu + (int) - prawa granica drugiego obiektu 
* + (int) - dolna granica drugiego obiektu 
*/
bool _checkCollision(int, int, int, int, int, int, int, int);
/**
*Funkcja zmieniaj�ca po�o�enie i rozmiary obszaru kolizji przeciwnik�w 
*(void) <- (StrRect*) - wska�nik na obiekt obszaru kolizji przeciwnik�w + (StrList*) - wska�nik listy przeciwnik�w
*/
void _collReposition(StrRect*, StrList*);
/**
*Funkcja kontroluj�ca logik� i rysowanie obrazu kiedy program jest w scenie odliczania
*(void) <- (StrGaState*) - wska�nik na obiekt stan�w gry
*/
void _logicCountdown(StrGaState*);
/**
*Funkcja kontroluj�ca logik� i rysowanie obrazu kiedy program jest w scenie przegranej gry
*(void) <- (StrGaState*) - wska�nik na obiekt stan�w gry 
*/
void _logicGameover(StrGaState*);
/**
*Funkcja kontroluj�ca logik� i rysowanie obrazu kiedy program jest w scenie menu pauzy
*(void) <- (StrGaState*) - wska�nik na obiekt stan�w gry + (StrList**)  - wska�nik na wska�nik listy przeciwnik�w
*/
void _logicPause(StrGaState*, StrList**);
/**
*Funkcja kontroluj�ca logik� i rysowanie obrazu kiedy program jest w scenie g��wnego menu
*(void) <- (StrGaState*) - wska�nik na obiekt stan�w gry
*/
void _logicStart(StrGaState*);
/**
*Funkcja kontroluj�ca logik� i rysowanie obrazu kiedy program jest w scenie sklepu
*(void) <- (StrFileVars*) - wska�nik na obiekt zmiennych plikowych + (StrGaState*) - wska�nik na obiekt stan�w gry
*/
void _logicShop(StrFileVars*, StrGaState*);
/**
*Funkcja kontroluj�ca logik� i rysowanie obrazu kiedy program jest w scenie opcji
*(void) <- (StrFileVars*) - wska�nik na obiekt zmiennych plikowych + (StrGaState*) - wska�nik na obiekt stan�w gry
*/
void _logicOther(StrFileVars*, StrGaState*);
/**
*Funkcja kontroluj�ca logik� i rysowanie obrazu kiedy program jest w scenie wypisywania wynik�w
*(void) <- (StrFileVars*) - wska�nik na obiekt zmiennych plikowych + (StrGaState*) - wska�nik na obiekt stan�w gry
*/
void _logicScores(StrFileVars*, StrGaState*);
/**
*Funkcja kontroluj�ca przewijanie po przyciskach menu
*(void) <- (StrGaState*) - wska�nik na obiekt stan�w gry + (int) - ilo�� przycisk�w menu
*/
void _menuScroll(StrGaState*, int);
/**
*Funkcja rysuj�ca przyciski w menu
*(void) <- (int) - numer przycisk�w do narysowania + (int) - obecnie zaznaczony przycisk + (int) - dodatkowe zwi�kszenie szeroko�ci przycisku od podstawowej szeroko�ci
*/
void _drawButtons(int, int, int);
/**
*G��wna funkcja zajmuj�ca si� wydarzeniami 
*(void) <- (bool*) - wska�nik na zmienn� stanu rysowania kolejnej klatki + (bool*) - wska�nik na zmienn� stanu wyj�cia z programu 
*/
void _eventHandler(bool*, bool*);
/**
*G��wna funkcja kontroluj�ca program
*(void) <- (StrGaState*) - wska�nik na struktur� stan�w gry + (StrEntStats*) - wska�nik na struktur� statystyk obiekt�w + (StrList**) - wska�nik na wska�nik listy przeciwnik�w
* + (StrList**) - wska�nik na wska�nik listy pocisk�w gracza + (StrList**) - wska�nik na wska�nik listy pocisk�w przeciwnik�w + (StrRect**) - wska�nik na tablic� obiekt�w �ciany
* + (StrRect*) - wska�nik na obiekt obszaru kolizji przeciwnik�w + (StrFileVars*) - wska�nik na obiekt zmiennych plikowych
*/
void _logic(StrGaState*, StrEntStats*, StrList**, StrList**, StrList**, StrRect**, StrRect*, StrFileVars*);
/**
*Zmienna bibliotekowa wy�wietlacza
*/
ALLEGRO_DISPLAY* allDisplay;
/**
*Zmienna bibliotekowa buffora
*/
ALLEGRO_BITMAP* allBuffer;
/**
*Zmienna bibliotekowa czcionki
*/
ALLEGRO_FONT* font;
/**
*Zmienna bibliotekowa mniejszej wersji czcionki
*/
ALLEGRO_FONT* smallFont;
/**
*Zmienna bibliotekowa stopera
*/
ALLEGRO_TIMER* allTimer;
/**
*Zmienna bibliotekowa kolejki wydarze�
*/
ALLEGRO_EVENT_QUEUE* allQueue;
/**
*Zmienna bibliotekowa obrazu tekstur u�ywanych w grze
*/
ALLEGRO_BITMAP* bmapSpriteMap;
/**
*Zmienna bibliotekowa obrazu t�a u�ywanego w grze
*/
ALLEGRO_BITMAP* bmapBackground;
/**
*Zmienna bibliotekowa wydarzenia
*/
ALLEGRO_EVENT allEvent;
/**
*Zmienna bibliotekowa d�wi�ku strza�u przez gracza
*/
ALLEGRO_SAMPLE* sndLaser;
/**
*Zmienna bibliotekowa d�wi�ku strza�u przez przeciwnika
*/
ALLEGRO_SAMPLE* sndEnemyLaser;
/**
*Zmienna bibliotekowa d�wi�ku kolizji pocisku ze �cian� lub przeciwnikiem
*/
ALLEGRO_SAMPLE* sndHitHurt;
/**
*Zmienna bibliotekowa d�wi�ku naci�ni�cia przycisk�w w menu
*/
ALLEGRO_SAMPLE* sndMenuSelect;
/**
*Zmienna bibliotekowa d�wi�ku zmiany przycisku w menu
*/
ALLEGRO_SAMPLE* sndMenuChange;
/**
*Tablica przechowuj�ca stany naci�ni�cia poszczeg�lnych klawiszy
*/
unsigned char keyPressed[ALLEGRO_KEY_MAX];

#endif