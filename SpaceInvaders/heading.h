#ifndef heading
#include <stdbool.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_audio.h>

#define heading
/**
*Szerokoœæ bufora i wyœwietlacza
*/
#define DISP_W 1280
/**
*Wysokoœæ bufora i wyœwietlacza
*/
#define DISP_H 960
/**
*Naciœniêty klawisz
*/
#define KEY_USED   1
/**
*Odpuszczony klawisz
*/
#define KEY_RELEASED 2
/**
*Liczba klatek na sekundê
*/
#define FRAMERATE 40
/**
*Iloœæ przeciwników
*/
#define ENEMYCOUNT 72
/**
*Iloœæ przeciwników na rz¹d
*/
#define ENEMYPERROW 12
/**
*Wysokoœæ tekstury gracza
*/
#define PLAYERHEIGHT 64
/**
*Szerokoœæ tekstury gracza
*/
#define PLAYERWIDTH 64
/**
*Wysokoœæ tekstury œciany
*/
#define WALLHEIGHT 16
/**
*Szerokoœæ tekstury œciany
*/
#define WALLWIDTH 32
/**
*Wysokoœæ tekstury przeciwnika
*/
#define ENEMYHEIGHT 32
/**
*Szerokoœæ tekstury przeciwnika
*/
#define ENEMYWIDTH 32
/**
*Wysokoœæ tekstury pocisku
*/
#define BULLETHEIGHT 32
/**
*Szerokoœæ tekstury pocisku
*/
#define BULLETWIDTH 16
/**
*Bia³y kolor
*/
#define WHITE al_map_rgb(255, 255, 255)
/**
*¯ó³ty kolor
*/
#define YELLOW al_map_rgb(255, 215, 160)
/**
*Maksymalne przesuniêcie wertykalne czcionki
*/
#define FONTOFFSETY al_get_font_line_height(font) + DISP_H/96
/**
*Gruboœæ rysowanych linii/przycisków
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
*Podstawowa szerokoœæ przycisku
*/
#define BOXOFFSETX DISP_W / 4
/**
*Podstawowa wysokoœæ przycisku
*/
#define BOXOFFSETY DISP_H * 9 / 48
/**
*Typ wyliczeniowy reprezentuj¹cy obecn¹ scenê programu
*/
typedef enum EnScreenFlag {
    /// Scena g³ównego menu
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
    /// Scena wpisywania wyników
    scores 
}EnScreenFlag;
/**
*Struktura obszarów kolizyjnych
*/
typedef struct StrRect {
    int 
        /// Lewa granica
        x1, 
        /// Prawa granica
        x2, 
        /// Górna granica
        y1, 
        /// Dolna granica
        y2;
}StrRect;
/**
*Struktura reprezentuj¹ca wêze³ listy przechowuj¹cy koordynaty x i y
*/
typedef struct StrList {
    int 
        /// Pozycja horyzontalna
        x, 
        /// Pozycja wertykalna
        y;
        /// WskaŸnik na kolejny element listy
        struct StrList* next;
}StrList;
/**
*Struktura przechowuj¹cy statystyki obiektów
*/
typedef struct StrEntStats {
    char
        /// Szybkoœæ poruszania siê przeciwników 
        enMovSpd, 
        /// Szybkoœæ poruszania siê pocisków przeciwników 
        enBullSpd, 
        /// Minimalna pauza czasowa pomiêdzy kolejnymi pociskami przeciwników
        enFireWaitMax, 
        /// Czas do zakoñczenie minimalnej pauzy czasowej miêdzy kolejnymi pociskami przeciwników
        enFireWaitNow, 
        /// Szybkoœæ poruszania siê gracza 
        plMovSpd, 
        /// Szybkoœæ poruszania siê pocisków gracza
        plBullSpd, 
        /// Minimalna pauza czasowa pomiêdzy kolejnymi pociskami gracza
        plFireWaitMax, 
        /// Czas do zakoñczenie minimalnej pauzy czasowej miêdzy kolejnymi pociskami gracza
        plFireWaitNow, 
        /// Iloœæ œcian 
        wallAmnt;
    int 
        /// Horyzontalna pozycja gracza
        plX, 
        /// Wertykalna pozycja gracza
        plY;
}StrEntStats;
/**
*Struktura przechowuj¹cy obecny stan programu
*/
typedef struct StrGaState {
    bool 
        /// Stan wyjœcia z programu
        quitProg, 
        /// Stan reprezentuj¹cy czy kolejna klatka programu powinna byæ narysowana
        reDraw, 
        /// Stan reprezentuj¹cy czy menu pauzy powinno byæ narysowane
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
*Struktura posiadaj¹ca zmienne zapisywane w pliku data.txt
*/
typedef struct StrFileVars {
    int 
        /// Poziom ulepszenia prêdkoœci poruszania siê
        movSpdUpg, 
        /// Poziom ulepszenia prêdkoœci poruszania siê pocisków
        bullSpdUpg, 
        /// Poziom ulepszenia prêdkoœci strzelania
        attSpdUpg, 
        /// Wybrany schemat przycisków
        keyBindFlag;
        /// Ca³kowity wynik do u¿ycia jako waluta w sklepie
    long totalScore;
        /// Tablica przechowuj¹ca wyniki w tablicy wyników
    int scoreTable[8];
        /// Tablica przechowuj¹ca nazwy w tablicy wyników
    char nameTable[65];
}StrFileVars;

/**
*Funkcja pomocnicza sprawdzaj¹ca czy inicjalizacja siê uda³a
*(void) <- (bool) - konwertowana zmienna na bool + (char*) - nazwa zmiennej
*/
void _initTest(bool, char*);
/**
*Funkcja inicjuj¹ca wartoœci zwi¹zane z bibliotek¹ ALLEGRO5
*(void) <- (void)
*/
void _initAllegroVars();
/**
*Funkcja inicjuj¹ca strukturê przechowuj¹c¹ stany programu
*(StrGaState) - zwracana struktura stanów gry <- (void)
*/
StrGaState _initGameState();
/**
*Funkcja inicjuj¹ca tablicê obiektów œcian
*(StrRect*) - zwracana tablica struktur obiektów œcian <- (int) - iloœæ œcian
*/
StrRect* _initWallArr(int);
/**
*Funkcja inicjuj¹ca listê obiektów przeciwników
*(StrList*) - zwracany wskaŸnik na pocz¹tek listy <- (void)
*/
StrList* _initEnemyList();
/**
*Funkcja inicjuj¹ca strukturê przechowuj¹c¹ statystyki obiektów
*(StrEntStats) - zwracana struktura statystyki obiektów <- (int) - numer kolejnej rundy + (StrFileVars) - struktura zmiennych plikowych
*/
StrEntStats _initEntStats(int, StrFileVars);
/**
*Funkcja kontroluj¹ca zapisywanie/odczytywanie wartoœci z pliku
*(void) <- (StrFileVars*) - wskaŸnik na strukturê zmiennych plikowych + (int) - flaga decyduj¹ca o zapisywaniu/odczytywaniu
*/
void _initFileVars(StrFileVars*, int);
/**
*Funkcja pomocnicza zapisuj¹ca wartoœci do pliku
*(void) <- (StrFileVars) - struktura zmiennych plikowych
*/
void _initFileVarsWrite(StrFileVars);
/**
*Funkcja pomocnicza zapisuj¹ca wartoœci do pliku jeœli nie zosta³ on stworzony wczeœniej
*(void) <- (void) 
*/
void _initFileVarsWriteFirst();
/**
*Funkcja pomocnicza odczytuj¹ca wartoœci plikowe
*(void) <- (StrFileVars*) - wskaŸnik na strukturê zmiennych plikowych
*/
void _initFileVarsRead(StrFileVars*);
/**
*Funkcja inicjuj¹ca struktury potrzebne podczas rozpoczêcia gry
*(EnScreenFlag) - zwracana flaga rozpoczêcia gry <- (StrEntStats*) - wskaŸnik na strukturê statystyk obiektów + (StrList**) - wskaŸnik na wskaŸnik przechowuj¹cy pocz¹tek listy przeciwników 
* + (StrList**) - wskaŸnik na wskaŸnik przechowuj¹cy pocz¹tek listy pocisków gracza + (StrList**) - wskaŸnik na wskaŸnik przechowuj¹cy poczatek listy pocisków przeciwników 
* + (StrRect**) - wskaŸnik na tablicê przechowuj¹c¹ obiekty œciany + (StrRect*) - wskaŸnik na obiekt obszaru kolizyjnego przeciwników + (StrFileVars) - struktura zmiennych plikowych 
*/
EnScreenFlag _firstRound(StrEntStats*, StrList**, StrList**, StrList**, StrRect**, StrRect*, StrFileVars);
/**
*Funkcja przygotowuj¹ca grê do kolejnej rundy
*(EnScreenFlag) - zwracana flaga rozpoczêcia gry <- (StrGaState*) - wskaŸnik na strukturê stanów gry + (StrFileVars*) - wskaŸnik na strukturê zmiennych plikowych
*/
EnScreenFlag _nextRound(StrGaState*, StrFileVars*);
/**
*Funkcja deinicjalizuj¹ca listê
*(StrList*) - zwracana wartoœæ NULL <- (StrList*) - wskaŸnik na listê
*/
StrList* _freeList(StrList*);
/**
*Funkcja deinicjalizuj¹ca tablicê obiektów œcian
*(StrRect*) - zwracana wartoœæ NULL <- (StrRect*) - wskaŸnik na tablicê obiektów œcian
*/
StrRect* _freeWallArr(StrRect*);
/**
*Funkcja usuwaj¹ca konkretny element listy
*(void) <- (StrList**) - wskaŸnik na wskaŸnik poprzedniego element listy + (StrList**) - wskaŸnik na wskaŸnik elementu listy do usuniêcia
*/
void _listDelete(StrList**, StrList**);
/**
*Funkcja sprawdzaj¹ca czy funkcja malloc siê powiod³a
*(void) <- (void*) - sprawdzany wskaŸnik, konwertowany na typ (void*)
*/
void _ptrCheckNull(void*);
/**
*Funkcja kontroluj¹c¹ rysowanie kiedy program jest w scenie gry
*(void) <- (StrEntStats) - struktura statystyk obiektów + (StrList*) - wskaŸnik listy przeciwników + (StrList*) - wskaŸnik listy pocisków gracza + (StrList*) - wskaŸnik listy pocisków przeciwników 
*+ (StrRect*) - tablica obiektów œciany + (StrRect) - struktura obiektu obszaru kolizji przeciwników + (int) - obecny wynik + (int) - obecny numer rundy
*/
void _logicPlayDraw(StrEntStats, StrList*, StrList*, StrList*, StrRect*, StrRect, int, int);
/**
*Funkcja kontroluj¹c¹ logikê kiedy program jest w scenie gry
*(void) <- (StrGaState*) - wskaŸnik na strukturê stanów gry +  (StrEntStats*) - wskaŸnik na strukturê statystyk obiektów + (StrList**) - wskaŸnik na wskaŸnik listy przeciwników
*+ (StrList**) - wskaŸnik na wskaŸnik listy pocisków gracza + (StrList**) - wskaŸnik na wskaŸnik listy pocisków przeciwników + (StrRect*) - tablica obiektów œciany 
*+ (StrRect*) - wskaŸnik na strukturê obiektu obszaru kolizji przeciwników + (StrFileVars) - struktura zmiennych plikowych
*/
void _logicPlayMec(StrGaState*, StrEntStats*, StrList**, StrList**, StrList**, StrRect*, StrRect*, StrFileVars*);
/**
*Funkcja zajmuj¹ca siê zachowaniem przeciwników
*(void) <- (StrList*) - wskaŸnik na listê przeciwników + (StrList**) - wskaŸnik na wskaŸnik listy pocisków przeciwników + (StrRect*) - wskaŸnik na obiekt obszaru kolizji przeciwników
* + (StrEntStats*) - wskaŸnik na strukturê statystyk obiektów
*/
void _enemyBehaviour(StrList*, StrList**, StrRect*, StrEntStats*);
/**
*Funkcja zajmuj¹ca siê sterowaniem statku gracza
*(void) <- (StrEntStats*) - wskaŸnik na strukturê statystyk obiektów + (StrList**) - wskaŸnik na wskaŸnik listy pocisków gracza + (int) - flaga wybranego schematu sterowania
*/
void _playerBehaviour(StrEntStats*, StrList**, int);
/**
*Funkcja zajmuj¹ca siê stworzeniem pocisku
*(void) <- (StrList**) - wskaŸnik na wskaŸnik listy pocisków + (int) - pozycja horyzontalna jak¹ nadaæ pociskowi + (int) - pozycja wertykalna jak¹ nadaæ pociskowi 
*/
void _bulletCreate(StrList**, int, int);
/**
*Funkcja zajmuj¹ca siê poruszaniem pocisków
*(void) <- (StrList*) - wskaŸnik na w listê pocisków + (int) - prêdkoœæ poruszania siê pocisków
*/
void _bulletMovement(StrList*, int);
/**
*Funkcja sprawdzaj¹ca kolizjê wszystkich pocisku gracza
*(void) <- (StrList**) - wskaŸnik na wskaŸnik listy przeciwników + (StrList**) - wskaŸnik na wskaŸnik listy pocisków gracza + (StrRect*) - tablica obiektów œciany +
*+ (StrRect) - struktura obiektu obszaru kolizji przeciwników + (int) - iloœæ obiektów œciany + (int*) - wskaŸnik na zmienn¹ przechowuj¹c¹ obecny wynik
*/
void _playerBulletsCollision(StrList**, StrList**, StrRect*, StrRect, int, int*);
/**
*Funkcja pomocnicza sprawdzaj¹ca kolizjê danego pocisku gracza
*(StrList*) - zwracany wskaŸnik traktowany jako test czy jeœli nasz³o do kolizji, by³a ona z pierwszym przeciwnikiem listy <- (StrList**) - wskaŸnik na wskaŸnik obecnie analizowanego pocisku gracza 
*+ (StrList**) - wskaŸnik na wskaŸnik poprzedniego pocisku gracza  + (StrRect*) - tablica obiektów œciany + (StrList**) - wskaŸnik na wskaŸnik listy przeciwników 
*+ (StrRect) - struktura obiektu obszaru kolizji przeciwników + (int) - iloœæ obiektów œciany + (int*) - wskaŸnik na zmienn¹ przechowuj¹c¹ obecny wynik
*/
StrList* _playerBulletsCollisionCheck(StrList**, StrList**, StrRect*, StrList**, StrRect, int, int*);
/**
*Funkcja sprawdzaj¹ca kolizjê wszystkich pocisków przeciwników
*(bool) - zmienna sygnalizuj¹ca czy nast¹pi³a kolizja z graczem <- (StrEntStats) - struktura statystyk obiektów + (StrList**) - wskaŸnik na wskaŸnik listy pocisków przeciwników 
*+ (StrRect*) - tablica obiektów œciany
*/
bool _enemyBulletsCollision(StrEntStats, StrList**, StrRect*);
/**
*Funkcja pomocznicza sprawdzaj¹ca kolizjê danego pocisku przeciwnika
*(bool) - zmienna sygnalizuj¹ca czy nast¹pi³a kolizja z graczem <- (StrEntStats) - struktura statystyk obiektów + (StrList**) - wskaŸnik na wskaŸnik obecnie analizowanego pocisku 
*+ (StrList**) - wskaŸnik na wskaŸnik poprzedniego pocisku + (StrRect*) - tablica obiektów œciany
*/
bool _enemyBulletsCollisionCheck(StrEntStats, StrList**, StrList**, StrRect*);
/**
*Funkcja sprawdzaj¹ca kolizjê miêdzy dwoma obiektami
*(bool) - zmienna sygnalizuj¹ca czy nast¹pi³a kolizja <- (int) - lewa granica pierwszego obiektu + (int) - górna granica pierwszego obietku + (int) - prawa granica pierwszego obiektu
* + (int) - dolna granica pierwszego obiektu + (int) - lewa granica drugiego obiektu + (int) - górna granica drugiego obiektu + (int) - prawa granica drugiego obiektu 
* + (int) - dolna granica drugiego obiektu 
*/
bool _checkCollision(int, int, int, int, int, int, int, int);
/**
*Funkcja zmieniaj¹ca po³o¿enie i rozmiary obszaru kolizji przeciwników 
*(void) <- (StrRect*) - wskaŸnik na obiekt obszaru kolizji przeciwników + (StrList*) - wskaŸnik listy przeciwników
*/
void _collReposition(StrRect*, StrList*);
/**
*Funkcja kontroluj¹ca logikê i rysowanie obrazu kiedy program jest w scenie odliczania
*(void) <- (StrGaState*) - wskaŸnik na obiekt stanów gry
*/
void _logicCountdown(StrGaState*);
/**
*Funkcja kontroluj¹ca logikê i rysowanie obrazu kiedy program jest w scenie przegranej gry
*(void) <- (StrGaState*) - wskaŸnik na obiekt stanów gry 
*/
void _logicGameover(StrGaState*);
/**
*Funkcja kontroluj¹ca logikê i rysowanie obrazu kiedy program jest w scenie menu pauzy
*(void) <- (StrGaState*) - wskaŸnik na obiekt stanów gry + (StrList**)  - wskaŸnik na wskaŸnik listy przeciwników
*/
void _logicPause(StrGaState*, StrList**);
/**
*Funkcja kontroluj¹ca logikê i rysowanie obrazu kiedy program jest w scenie g³ównego menu
*(void) <- (StrGaState*) - wskaŸnik na obiekt stanów gry
*/
void _logicStart(StrGaState*);
/**
*Funkcja kontroluj¹ca logikê i rysowanie obrazu kiedy program jest w scenie sklepu
*(void) <- (StrFileVars*) - wskaŸnik na obiekt zmiennych plikowych + (StrGaState*) - wskaŸnik na obiekt stanów gry
*/
void _logicShop(StrFileVars*, StrGaState*);
/**
*Funkcja kontroluj¹ca logikê i rysowanie obrazu kiedy program jest w scenie opcji
*(void) <- (StrFileVars*) - wskaŸnik na obiekt zmiennych plikowych + (StrGaState*) - wskaŸnik na obiekt stanów gry
*/
void _logicOther(StrFileVars*, StrGaState*);
/**
*Funkcja kontroluj¹ca logikê i rysowanie obrazu kiedy program jest w scenie wypisywania wyników
*(void) <- (StrFileVars*) - wskaŸnik na obiekt zmiennych plikowych + (StrGaState*) - wskaŸnik na obiekt stanów gry
*/
void _logicScores(StrFileVars*, StrGaState*);
/**
*Funkcja kontroluj¹ca przewijanie po przyciskach menu
*(void) <- (StrGaState*) - wskaŸnik na obiekt stanów gry + (int) - iloœæ przycisków menu
*/
void _menuScroll(StrGaState*, int);
/**
*Funkcja rysuj¹ca przyciski w menu
*(void) <- (int) - numer przycisków do narysowania + (int) - obecnie zaznaczony przycisk + (int) - dodatkowe zwiêkszenie szerokoœci przycisku od podstawowej szerokoœci
*/
void _drawButtons(int, int, int);
/**
*G³ówna funkcja zajmuj¹ca siê wydarzeniami 
*(void) <- (bool*) - wskaŸnik na zmienn¹ stanu rysowania kolejnej klatki + (bool*) - wskaŸnik na zmienn¹ stanu wyjœcia z programu 
*/
void _eventHandler(bool*, bool*);
/**
*G³ówna funkcja kontroluj¹ca program
*(void) <- (StrGaState*) - wskaŸnik na strukturê stanów gry + (StrEntStats*) - wskaŸnik na strukturê statystyk obiektów + (StrList**) - wskaŸnik na wskaŸnik listy przeciwników
* + (StrList**) - wskaŸnik na wskaŸnik listy pocisków gracza + (StrList**) - wskaŸnik na wskaŸnik listy pocisków przeciwników + (StrRect**) - wskaŸnik na tablicê obiektów œciany
* + (StrRect*) - wskaŸnik na obiekt obszaru kolizji przeciwników + (StrFileVars*) - wskaŸnik na obiekt zmiennych plikowych
*/
void _logic(StrGaState*, StrEntStats*, StrList**, StrList**, StrList**, StrRect**, StrRect*, StrFileVars*);
/**
*Zmienna bibliotekowa wyœwietlacza
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
*Zmienna bibliotekowa kolejki wydarzeñ
*/
ALLEGRO_EVENT_QUEUE* allQueue;
/**
*Zmienna bibliotekowa obrazu tekstur u¿ywanych w grze
*/
ALLEGRO_BITMAP* bmapSpriteMap;
/**
*Zmienna bibliotekowa obrazu t³a u¿ywanego w grze
*/
ALLEGRO_BITMAP* bmapBackground;
/**
*Zmienna bibliotekowa wydarzenia
*/
ALLEGRO_EVENT allEvent;
/**
*Zmienna bibliotekowa dŸwiêku strza³u przez gracza
*/
ALLEGRO_SAMPLE* sndLaser;
/**
*Zmienna bibliotekowa dŸwiêku strza³u przez przeciwnika
*/
ALLEGRO_SAMPLE* sndEnemyLaser;
/**
*Zmienna bibliotekowa dŸwiêku kolizji pocisku ze œcian¹ lub przeciwnikiem
*/
ALLEGRO_SAMPLE* sndHitHurt;
/**
*Zmienna bibliotekowa dŸwiêku naciœniêcia przycisków w menu
*/
ALLEGRO_SAMPLE* sndMenuSelect;
/**
*Zmienna bibliotekowa dŸwiêku zmiany przycisku w menu
*/
ALLEGRO_SAMPLE* sndMenuChange;
/**
*Tablica przechowuj¹ca stany naciœniêcia poszczególnych klawiszy
*/
unsigned char keyPressed[ALLEGRO_KEY_MAX];

#endif