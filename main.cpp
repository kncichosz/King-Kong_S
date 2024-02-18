#define _USE_MATH_DEFINES
#include<math.h>
#include<stdio.h>
#include<string.h>

extern "C" {
#include"./SDL2-2.0.10/include/SDL.h"
#include"./SDL2-2.0.10/include/SDL_main.h"
}

#define SCREEN_WIDTH 578
#define SCREEN_HEIGHT 600
#define PREDKOSC_POSTAC 4
#define POSTAC_WIDTH 18
#define POSTAC_HEIGHT 34
#define DRABINA_WIDTH 24
#define DRABINA_HEIGHT 80
#define PLATFORMA_WIDTH 48
#define PLATFORMA_HEIGHT 16
#define BECZKA_WIDTH 12
#define BECZKA_HEIGHT 12
#define CIASTKO_WIDTH 42
#define CIASTKO_HEIGHT 42
#define PIERWSZA_PLATFORMA_Y 500
#define ILOSC_DRABIN 5

const int drabinka_lewaX = 500;//piksele
const int drabinka_prawaX = 80;//piksele


struct danePostac {
	int postacX;
	int postacY;
	int ktoraPlat;
	int ostatni_ruch;
	int czas_od_ruchu;
	int postacAnimacja;
};

struct daneCiastko {
	int ciastkoX;
	int ciastkoY;
	int ciastkoAnimacja;
};

struct daneBeczka {
	int beczkaX;
	int beczkaY;
	int beczkaAnimacja;
};

// narysowanie napisu txt na powierzchni screen, zaczynajπc od punktu (x, y)
// charset to bitmapa 128x128 zawierajπca znaki
// draw a text txt on surface screen, starting from the point (x, y)
// charset is a 128x128 bitmap containing character images
void DrawString(SDL_Surface* screen, int x, int y, const char* text,
	SDL_Surface* charset) {
	int px, py, c;
	SDL_Rect s, d;
	s.w = 8;
	s.h = 8;
	d.w = 8;
	d.h = 8;
	while (*text) {
		c = *text & 255;
		px = (c % 16) * 8;
		py = (c / 16) * 8;
		s.x = px;
		s.y = py;
		d.x = x;
		d.y = y;
		SDL_BlitSurface(charset, &s, screen, &d);
		x += 8;
		text++;
	};
};


// narysowanie na ekranie screen powierzchni sprite w punkcie (x, y)
// (x, y) to punkt úrodka obrazka sprite na ekranie
// draw a surface sprite on a surface screen in point (x, y)
// (x, y) is the center of sprite on screen
void DrawSurface(SDL_Surface* screen, SDL_Surface* sprite, int x, int y) {
	SDL_Rect dest;
	dest.x = x - sprite->w / 2;
	dest.y = y - sprite->h / 2;
	dest.w = sprite->w;
	dest.h = sprite->h;
	SDL_BlitSurface(sprite, NULL, screen, &dest);
};


// rysowanie pojedynczego pixela
// draw a single pixel
void DrawPixel(SDL_Surface* surface, int x, int y, Uint32 color) {
	int bpp = surface->format->BytesPerPixel;
	Uint8* p = (Uint8*)surface->pixels + y * surface->pitch + x * bpp;
	*(Uint32*)p = color;
};


// rysowanie linii o d≥ugoúci l w pionie (gdy dx = 0, dy = 1) 
// bπdü poziomie (gdy dx = 1, dy = 0)
// draw a vertical (when dx = 0, dy = 1) or horizontal (when dx = 1, dy = 0) line
void DrawLine(SDL_Surface* screen, int x, int y, int l, int dx, int dy, Uint32 color) {
	for (int i = 0; i < l; i++) {
		DrawPixel(screen, x, y, color);
		x += dx;
		y += dy;
	};
};


// rysowanie prostokπta o d≥ugoúci bokÛw l i k
// draw a rectangle of size l by k
void DrawRectangle(SDL_Surface* screen, int x, int y, int l, int k,
	Uint32 outlineColor, Uint32 fillColor) {
	int i;
	DrawLine(screen, x, y, k, 0, 1, outlineColor);
	DrawLine(screen, x + l - 1, y, k, 0, 1, outlineColor);
	DrawLine(screen, x, y, l, 1, 0, outlineColor);
	DrawLine(screen, x, y + k - 1, l, 1, 0, outlineColor);
	for (i = y + 1; i < y + k - 1; i++)
		DrawLine(screen, x + 1, i, l - 2, 1, 0, fillColor);
};

void resetPostac(danePostac *danePos) {
	(*danePos).postacX = 30;
	(*danePos).ktoraPlat = 0;
	(*danePos).ostatni_ruch = 0;
	(*danePos).postacY = PIERWSZA_PLATFORMA_Y - (POSTAC_HEIGHT / 2);
	(*danePos).czas_od_ruchu = 0;
	(*danePos).postacAnimacja = 1;
}

void nowaGra(double *worldTime, int *czy_skok)
{
	*worldTime = 0;
	*czy_skok = 2;
}

void resetCiastko(daneCiastko* daneCias, const int currentMap)
{
	if (currentMap == 2) {
		(*daneCias).ciastkoX = 200;
	}
	else
	{
		(*daneCias).ciastkoX = 400;
	}
	(*daneCias).ciastkoY = PIERWSZA_PLATFORMA_Y - (DRABINA_HEIGHT * 5) - (CIASTKO_HEIGHT / 2);
	(*daneCias).ciastkoAnimacja = 1;
}

void resetBeczka(daneBeczka *daneBecz, const int currentMap)
{
	if (currentMap == 2) {
		(*daneBecz).beczkaX = 250;
	}
	else 
	{
		(*daneBecz).beczkaX = 350;
	}
	(*daneBecz).beczkaY = PIERWSZA_PLATFORMA_Y - (DRABINA_HEIGHT * 5) - (BECZKA_HEIGHT / 2);
	(*daneBecz).beczkaAnimacja = 1;
}

void rysujPierwszaPlarforma(SDL_Surface* screen, SDL_Surface* platforma, int platformaTab[][3])
{

	platformaTab[0][0] = PIERWSZA_PLATFORMA_Y;//[][0]srodekY
	platformaTab[0][1] = 1;//[][1]lewa strona platfomy
	platformaTab[0][2] = SCREEN_WIDTH - 1;//[][2]prawo strona platfomy

	for (int j = 0; j < 12; j++) {

		DrawSurface(screen, platforma, (PLATFORMA_WIDTH / 2 + 1) + (j * PLATFORMA_WIDTH), PIERWSZA_PLATFORMA_Y + (PLATFORMA_HEIGHT / 2));

	}

}

void rysujPlatfomaTop1(SDL_Surface* screen, SDL_Surface* platforma, int platformaTab[][3], const int map)
{
	platformaTab[5][0] = PIERWSZA_PLATFORMA_Y - (DRABINA_HEIGHT * 5);
	if (!map)
	{
		platformaTab[5][1] = (6 * PLATFORMA_WIDTH) + 1;
		platformaTab[5][2] = SCREEN_WIDTH - 1;

	}
	else
	{
		platformaTab[5][1] = 1;
		platformaTab[5][2] = (6 * PLATFORMA_WIDTH) + 1;
	}

	for (int j = 0; j < 6; j++) {

		if (!map)
		{
			DrawSurface(screen, platforma, (PLATFORMA_WIDTH / 2 + 1) + ((j + 6) * PLATFORMA_WIDTH), PIERWSZA_PLATFORMA_Y + (PLATFORMA_HEIGHT / 2) - (DRABINA_HEIGHT * 5));
		}
		else
		{
			DrawSurface(screen, platforma, (PLATFORMA_WIDTH / 2 + 1) + ((j)*PLATFORMA_WIDTH), PIERWSZA_PLATFORMA_Y + (PLATFORMA_HEIGHT / 2) - (DRABINA_HEIGHT * 5));
		}

	}

}

void rysujPlatformyMapa1(SDL_Surface* screen, SDL_Surface* platforma, int platformaTab[][3], int dziuraTab[][3], const int map) {

	int strona = 0;
	static int k = 0;
	if (k == 2) {
		k = 0;
	}
	
	if (map == 2) 
	{
		strona = 1;
	}
	else 
	{
		strona = 0;
	}

	for (int i = 1; i <= 4; i++) {

		platformaTab[i][0] = PIERWSZA_PLATFORMA_Y - (DRABINA_HEIGHT * i);
		if (i % 2 == strona)
		{
			platformaTab[i][1] = PLATFORMA_WIDTH + 1;
			platformaTab[i][2] = SCREEN_WIDTH - 1;
		}
		else
		{
			platformaTab[i][1] = 1;
			platformaTab[i][2] = SCREEN_WIDTH - PLATFORMA_WIDTH - 1;
		}


		for (int j = 0; j < 11; j++) {

			if (i % 2 == strona)
			{
				DrawSurface(screen, platforma, ((PLATFORMA_WIDTH / 2) + 1) + ((j + 1) * PLATFORMA_WIDTH), PIERWSZA_PLATFORMA_Y + (PLATFORMA_HEIGHT / 2) - (DRABINA_HEIGHT * (i)));
			}
			else
			{
				if (map == 3 && j == 5) {
					dziuraTab[k][0] = PIERWSZA_PLATFORMA_Y - (DRABINA_HEIGHT * i);
					dziuraTab[k][1] = 1 + (j * PLATFORMA_WIDTH);
					dziuraTab[k][2] = 1 + PLATFORMA_WIDTH + (j * PLATFORMA_WIDTH);
					k ++;
					continue;
				}
				DrawSurface(screen, platforma, (PLATFORMA_WIDTH / 2 + 1) + (j * PLATFORMA_WIDTH), PIERWSZA_PLATFORMA_Y + (PLATFORMA_HEIGHT / 2) - (DRABINA_HEIGHT * i));
			}



		}

	}

}

void rysujDrabinki1(SDL_Surface* screen, SDL_Surface* drabina, int drabinaTab[][4], const int map) {

	for (int i = 0; i < ILOSC_DRABIN; i++) {

		drabinaTab[i][0] = (PIERWSZA_PLATFORMA_Y)-(DRABINA_HEIGHT * (i + 1));//gora drabinki
		drabinaTab[i][1] = PIERWSZA_PLATFORMA_Y - (DRABINA_HEIGHT * i);//dol drabinki

		if (i % 2 == map) {
			DrawSurface(screen, drabina, drabinka_lewaX, PIERWSZA_PLATFORMA_Y - (DRABINA_HEIGHT / 2) - (DRABINA_HEIGHT * i));
			drabinaTab[i][2] = drabinka_lewaX + (DRABINA_WIDTH / 2);//prawa stona drabinki
			drabinaTab[i][3] = drabinka_lewaX - (DRABINA_WIDTH / 2);//lewa strona drabinki

		}
		else
		{
			DrawSurface(screen, drabina, drabinka_prawaX, PIERWSZA_PLATFORMA_Y - (DRABINA_HEIGHT / 2) - (DRABINA_HEIGHT * i));
			drabinaTab[i][2] = drabinka_prawaX + (DRABINA_WIDTH / 2);//prawa stona drabinki
			drabinaTab[i][3] = drabinka_prawaX - (DRABINA_WIDTH / 2);//lewa strona drabinki
		}

	}

}

void ruchGoraPostac(danePostac* danePos, int drabinaTab[][4]) {

	for (int i = 0; i < ILOSC_DRABIN; i++) {

		if (((*danePos).postacY + (POSTAC_HEIGHT / 2)) > (drabinaTab[i][0]) && ((*danePos).postacY + (POSTAC_HEIGHT / 2)) <= (drabinaTab[i][1])) {

			if (((*danePos).postacX) <= drabinaTab[i][2] && ((*danePos).postacX) >= drabinaTab[i][3]) {

				(*danePos).postacY = (*danePos).postacY - 2;
				(*danePos).postacAnimacja = 4;
				break;
			}
		}

	}
}

void ruchDolPostac(danePostac* danePos, int drabinaTab[][4]) {

	for (int i = 0; i < ILOSC_DRABIN; i++) {

		if (((*danePos).postacY + (POSTAC_HEIGHT / 2)) >= (drabinaTab[i][0]) && ((*danePos).postacY + (POSTAC_HEIGHT / 2)) < (drabinaTab[i][1])) {


			if (((*danePos).postacX) <= drabinaTab[i][2] && ((*danePos).postacX) >= drabinaTab[i][3]) {

				(*danePos).postacY = (*danePos).postacY + 2;
				(*danePos).postacAnimacja = 4;
				break;
			}
		}

	}
}

void ruchLewoPostac(danePostac* danePos, const int platformaTab[][3]) {

	for (int i = 0; i < PREDKOSC_POSTAC; i++) {

		if ((*danePos).postacX - (POSTAC_WIDTH / 2) != 1) {

			for (int j = 0; j < 6; j++) {

				if (platformaTab[j][0] == (*danePos).postacY + (POSTAC_HEIGHT / 2)) {
					(*danePos).ostatni_ruch = 1;
					(*danePos).postacX = (*danePos).postacX - 1;
					(*danePos).postacAnimacja = 2;
					break;

				}

			}

		}

	}
}

void ruchPrawoPostac(danePostac* danePos, const int platformaTab[][3]) {

	for (int i = 0; i < PREDKOSC_POSTAC; i++) {

		if ((*danePos).postacX + (POSTAC_WIDTH / 2) != SCREEN_WIDTH - 1) {

			for (int j = 0; j < 6; j++) {


				if (platformaTab[j][0] == (*danePos).postacY + (POSTAC_HEIGHT / 2)) {
					(*danePos).ostatni_ruch = 2;
					(*danePos).postacX += 1;
					(*danePos).postacAnimacja = 1;
					break;

				}

			}

		}

	}
}

void skokPostac(danePostac *danePos, const int platformaTab[][3], int* czy_skok) {

	static int skok_flaga = 60;

	//printf("skok_flaga = %d\n", skok_flaga);

	if (((*danePos).postacX + (POSTAC_WIDTH / 2) != SCREEN_WIDTH - 1) && ((*danePos).postacX - (POSTAC_WIDTH / 2) != 1))
	{

		if (*czy_skok == 2) {
			skok_flaga = 60;
			*czy_skok = 0;
		}
		else if (skok_flaga == 0) {
			skok_flaga = 60;
			*czy_skok = 0;
		}
		else if (skok_flaga > 30)
		{
			(*danePos).postacY -= 1;
			(*danePos).postacAnimacja = 3;
			skok_flaga--;
		}
		else
		{
			(*danePos).postacY += 1;
			skok_flaga--;
		}
		if ((*danePos).ostatni_ruch == 1)
		{
			(*danePos).postacX -= 1;
		}
		else if ((*danePos).ostatni_ruch == 2)
		{
			(*danePos).postacX += 1;
		}
		if (skok_flaga > 55) {
			(*danePos).postacAnimacja = 1;
		}

	}
	else
	{
		skok_flaga = 60;
		(*danePos).postacAnimacja = 1;
		*czy_skok = 0;
	}

}

void ruchBeczkaSrodkowePlatformy(daneBeczka* daneBecz, const int beczkaP, const int beczkaL, const int mapa)
{
	for (int i = 3; i > 0; i--) {

		if (i % 2 == mapa)
		{
			if ((*daneBecz).beczkaY == PIERWSZA_PLATFORMA_Y - (DRABINA_HEIGHT * i) - (BECZKA_HEIGHT / 2) && beczkaL <= (11 * PLATFORMA_WIDTH) + 1)
			{
				(*daneBecz).beczkaX += 1;
				break;
			}
			else if ((*daneBecz).beczkaY < PIERWSZA_PLATFORMA_Y - (DRABINA_HEIGHT * (i - 1)) - (BECZKA_HEIGHT / 2) && beczkaL >(11 * PLATFORMA_WIDTH) + 1)
			{
				(*daneBecz).beczkaY += 2;
				break;
			}
		}
		else
		{
			if ((*daneBecz).beczkaY == PIERWSZA_PLATFORMA_Y - (DRABINA_HEIGHT * i) - (BECZKA_HEIGHT / 2) && beczkaP >= PLATFORMA_WIDTH + 1)
			{
				(*daneBecz).beczkaX -= 1;
				break;
			}
			else if ((*daneBecz).beczkaY < PIERWSZA_PLATFORMA_Y - (DRABINA_HEIGHT * (i - 1)) - (BECZKA_HEIGHT / 2) && beczkaP < PLATFORMA_WIDTH + 1)
			{
				(*daneBecz).beczkaY += 2;
				break;
			}
		}

	}

}

void ruchBeczka(daneBeczka* daneBecz)
{
	int beczkaP = (*daneBecz).beczkaX + (BECZKA_WIDTH / 2);
	int beczkaL = (*daneBecz).beczkaX - (BECZKA_WIDTH / 2);

	if ((*daneBecz).beczkaY == PIERWSZA_PLATFORMA_Y - (DRABINA_HEIGHT * 5) - (BECZKA_HEIGHT / 2) && beczkaP >= (6 * PLATFORMA_WIDTH) + 1) {
		(*daneBecz).beczkaX -= 1;
	}
	else if ((*daneBecz).beczkaY < PIERWSZA_PLATFORMA_Y - (DRABINA_HEIGHT * 4) - (BECZKA_HEIGHT / 2) && beczkaP < (6 * PLATFORMA_WIDTH) + 1) {
		(*daneBecz).beczkaY += 2;
	}
	else if ((*daneBecz).beczkaY == PIERWSZA_PLATFORMA_Y - (DRABINA_HEIGHT * 4) - (BECZKA_HEIGHT / 2) && beczkaP >= PLATFORMA_WIDTH + 1) {
		(*daneBecz).beczkaX -= 1;
	}
	else if ((*daneBecz).beczkaY < PIERWSZA_PLATFORMA_Y - (DRABINA_HEIGHT * 3) - (BECZKA_HEIGHT / 2) && beczkaP < PLATFORMA_WIDTH + 1) {
		(*daneBecz).beczkaY += 2;
	}
	else if ((*daneBecz).beczkaY == PIERWSZA_PLATFORMA_Y - (BECZKA_HEIGHT / 2))
	{

		if (beczkaL <= 1) {
			(*daneBecz).beczkaX = 450;
			(*daneBecz).beczkaY = PIERWSZA_PLATFORMA_Y - (DRABINA_HEIGHT * 5) - (BECZKA_HEIGHT / 2);
		}
		else
		{
			(*daneBecz).beczkaX -= 1;
		}
	}
	else
	{

		ruchBeczkaSrodkowePlatformy(daneBecz, beczkaP, beczkaL, 1);

	}

}

void ruchBeczka2(daneBeczka* daneBecz)
{
	int beczkaP = (*daneBecz).beczkaX + (BECZKA_WIDTH / 2);
	int beczkaL = (*daneBecz).beczkaX - (BECZKA_WIDTH / 2);

	if ((*daneBecz).beczkaY == PIERWSZA_PLATFORMA_Y - (DRABINA_HEIGHT * 5) - (BECZKA_HEIGHT / 2) && beczkaL <= (6 * PLATFORMA_WIDTH) + 1) {
		(*daneBecz).beczkaX += 1;
	}
	else if ((*daneBecz).beczkaY < PIERWSZA_PLATFORMA_Y - (DRABINA_HEIGHT * 4) - (BECZKA_HEIGHT / 2) && beczkaL >(6 * PLATFORMA_WIDTH) + 1) {
		(*daneBecz).beczkaY += 2;
	}
	else if ((*daneBecz).beczkaY == PIERWSZA_PLATFORMA_Y - (DRABINA_HEIGHT * 4) - (BECZKA_HEIGHT / 2) && beczkaL <= (11 * PLATFORMA_WIDTH) + 1) {
		(*daneBecz).beczkaX += 1;
	}
	else if ((*daneBecz).beczkaY < PIERWSZA_PLATFORMA_Y - (DRABINA_HEIGHT * 3) - (BECZKA_HEIGHT / 2) && beczkaL > PLATFORMA_WIDTH + 1) {
		(*daneBecz).beczkaY += 2;
	}
	else if ((*daneBecz).beczkaY == PIERWSZA_PLATFORMA_Y - (BECZKA_HEIGHT / 2))
	{

		if (beczkaP >= SCREEN_WIDTH - 1) {
			(*daneBecz).beczkaX = 50;
			(*daneBecz).beczkaY = PIERWSZA_PLATFORMA_Y - (DRABINA_HEIGHT * 5) - (BECZKA_HEIGHT / 2);
		}
		else
		{
			(*daneBecz).beczkaX += 1;
		}
	}
	else
	{

		ruchBeczkaSrodkowePlatformy(daneBecz, beczkaP, beczkaL, 0);

	}

}

void czyKolizja(const danePostac danePos, const daneBeczka daneBecz, int *kolizja)
{
	int postacL = ((danePos).postacX) - (POSTAC_WIDTH/2);
	int postacP = ((danePos).postacX) + (POSTAC_WIDTH / 2);
	int postacT = ((danePos).postacY) - (POSTAC_HEIGHT / 2);
	int postacD = ((danePos).postacY) + (POSTAC_HEIGHT / 2);
	int beczkaL = ((daneBecz).beczkaX) - (BECZKA_WIDTH / 2);
	int beczkaP = ((daneBecz).beczkaX) + (BECZKA_WIDTH / 2);
	int beczkaT = ((daneBecz).beczkaY) - (BECZKA_HEIGHT / 2);
	int beczkaD = ((daneBecz).beczkaY) + (BECZKA_HEIGHT / 2);

	if ((postacT >= beczkaT)&& (postacT < beczkaD) || (postacD <= beczkaD) && (postacD > beczkaT) || (postacT < beczkaT && postacD > beczkaD))
	{
	
		if (postacL == beczkaP || postacP == beczkaL) {
			*kolizja =  1;
		}
	
	}
}

int ktoraPlatforma(const int postacY, const int platformaTab[][3], const int ktoraPlat) {

	for (int i = 0; i < 6; i++) {

		if (platformaTab[i][0] == postacY + (POSTAC_HEIGHT / 2)) {

			//printf("ktoraPlat = %d\n", ktoraPlat);
			return i;

		}

	}

	return ktoraPlat;

}

void resetDziur(int dziuraTab[][3])
{
	for (int i = 0; i < 2; i++) 
	{
		dziuraTab[i][0] = 0;
		dziuraTab[i][1] = 0;
		dziuraTab[i][2] = 0;
	}
}

void grawitacja(danePostac *danePos, const int platformaTab[][3], const int drabinaTab[][4], int dziuraTab[][3]) {

	int postacL = (*danePos).postacX - (POSTAC_WIDTH / 2);
	int postacP = (*danePos).postacX + (POSTAC_WIDTH / 2);
	int dziura = -1;

	//printf("platforma: % d\n", *ktoraPlat);


	if (postacL > platformaTab[(*danePos).ktoraPlat][2] || postacP < platformaTab[(*danePos).ktoraPlat][1]) {

		(*danePos).postacY += 2;
		(*danePos).ktoraPlat = ktoraPlatforma((*danePos).postacY, platformaTab, (*danePos).ktoraPlat);

	}

	for (int i = 0; i < 2; i++) 
	{
		if (platformaTab[(*danePos).ktoraPlat][0] == dziuraTab[i][0])
		{
			if (postacL > dziuraTab[i][1] && postacP < dziuraTab[i][2]) {
				(*danePos).postacY += 2;
				(*danePos).ktoraPlat = ktoraPlatforma((*danePos).postacY, platformaTab, (*danePos).ktoraPlat);
			}
			
		}
	}

	//printf("platformaTab[*ktoraPlat][0] = %d\n", platformaTab[*ktoraPlat][0]);
	//printf("dziuraTab[0][0] = %d\n", dziuraTab[0][0]);
	//printf("dziuraTab[0][1] = %d\n", dziuraTab[1][0]);
	

	if (platformaTab[(*danePos).ktoraPlat][0] != (*danePos).postacY + (POSTAC_HEIGHT / 2)) {

		

		for (int i = 0; i < ILOSC_DRABIN; i++) {

			if (((*danePos).postacY + (POSTAC_HEIGHT / 2)) >= (drabinaTab[i][0]) && ((*danePos).postacY + (POSTAC_HEIGHT / 2)) <= (drabinaTab[i][1])) {

				if ((*danePos).postacX <= drabinaTab[i][2] && ((*danePos).postacX) >= drabinaTab[i][3]) {

					break;

				}
				else
				{
					(*danePos).postacY += 1;
					(*danePos).ktoraPlat = ktoraPlatforma((*danePos).postacY, platformaTab, (*danePos).ktoraPlat);
				}
			}

		}

	}


}




// main
#ifdef __cplusplus
extern "C"
#endif
int main(int argc, char** argv) {
	int t1, t2, quit, frames, rc;
	int czy_skok = 0, czasGry = 0, kolizja = 0;
	int  currentMap = 1, drabinaTab[ILOSC_DRABIN][4], platformaTab[6][3], dziuraTab[2][3] = { 0 }, kolizjaCzas = 0, beczkaCzas = 0;
	double delta, worldTime;
	SDL_Event event;
	SDL_Surface* screen, * charset, * postac, * platforma, * platforma1, * platforma2, * platforma3, * drabina, *beczka, * beczka2, *postac1, *postac2, * postac3, * postac4, *beczkaAL, *beczkaAD, *beczkaAG, *beczkaAP, *ciastko, *ciastko1, *ciastko2;
	SDL_Texture* scrtex;
	SDL_Window* window;
	SDL_Renderer* renderer;

	daneBeczka daneBecz;
	resetBeczka(&daneBecz, currentMap);

	daneBeczka daneBecz2;
	resetBeczka(&daneBecz2, currentMap);

	danePostac danePos;
	resetPostac(&danePos);


	daneCiastko daneCias;
	resetCiastko(&daneCias, currentMap);

	// okno konsoli nie jest widoczne, jeøeli chcemy zobaczyÊ
	// komunikaty wypisywane printf-em trzeba w opcjach:
	// project -> szablon2 properties -> Linker -> System -> Subsystem
	// zmieniÊ na "Console"
	// console window is not visible, to see the printf output
	// the option:
	// project -> szablon2 properties -> Linker -> System -> Subsystem
	// must be changed to "Console"
	printf("wyjscie printfa trafia do tego okienka\n");
	printf("printf output goes here\n");

	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		printf("SDL_Init error: %s\n", SDL_GetError());
		return 1;
	}

	// tryb pe≥noekranowy / fullscreen mode
	rc = SDL_CreateWindowAndRenderer(0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP, &window, &renderer);
	//rc = SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &window, &renderer);
	if (rc != 0) {
		SDL_Quit();
		printf("SDL_CreateWindowAndRenderer error: %s\n", SDL_GetError());
		return 1;
	};

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
	SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

	SDL_SetWindowTitle(window, "Szablon do zdania drugiego 2017");


	screen = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32,
		0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);

	scrtex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING,
		SCREEN_WIDTH, SCREEN_HEIGHT);


	// wy≥πczenie widocznoúci kursora myszy
	SDL_ShowCursor(SDL_DISABLE);

	// wczytanie obrazka cs8x8.bmp
	charset = SDL_LoadBMP("./cs8x8.bmp");
	if (charset == NULL) {
		printf("SDL_LoadBMP(cs8x8.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};
	SDL_SetColorKey(charset, true, 0x000000);

	postac2 = SDL_LoadBMP("./postacL.bmp");
	if (postac2 == NULL) {
		printf("SDL_LoadBMP(postac.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};

	postac1 = SDL_LoadBMP("./postacP.bmp");
	if (postac1 == NULL) {
		printf("SDL_LoadBMP(postacP.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};

	postac3 = SDL_LoadBMP("./skok.bmp");
	if (postac3 == NULL) {
		printf("SDL_LoadBMP(postac3.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};

	postac4 = SDL_LoadBMP("./postacDrabina.bmp");
	if (postac4 == NULL) {
		printf("SDL_LoadBMP(postac4.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};

	drabina = SDL_LoadBMP("./drabina.bmp");
	if (drabina == NULL) {
		printf("SDL_LoadBMP(drabina.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};

	platforma1 = SDL_LoadBMP("./platforma.bmp");
	if (platforma1 == NULL) {
		printf("SDL_LoadBMP(platforma.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};
	platforma2 = SDL_LoadBMP("./platforma2.bmp");
	if (platforma2 == NULL) {
		printf("SDL_LoadBMP(platforma2.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};
	platforma3 = SDL_LoadBMP("./platforma3.bmp");
	if (platforma3 == NULL) {
		printf("SDL_LoadBMP(platforma3.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};

	beczkaAP = SDL_LoadBMP("./beczkaP.bmp");
	if (beczkaAP == NULL) {
		printf("SDL_LoadBMP(beczkaP.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};

	beczkaAD = SDL_LoadBMP("./beczkaD.bmp");
	if (beczkaAD == NULL) {
		printf("SDL_LoadBMP(beczkaD.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};

	beczkaAL = SDL_LoadBMP("./beczkaL.bmp");
	if (beczkaAL == NULL) {
		printf("SDL_LoadBMP(beczkaL.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};

	beczkaAG = SDL_LoadBMP("./beczkaG.bmp");
	if (beczkaAG == NULL) {
		printf("SDL_LoadBMP(beczkaG.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};

	ciastko1 = SDL_LoadBMP("./ciastko1.bmp");
	if (beczkaAL == NULL) {
		printf("SDL_LoadBMP(beczkaL.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};

	ciastko2 = SDL_LoadBMP("./ciastko2.bmp");
	if (beczkaAL == NULL) {
		printf("SDL_LoadBMP(beczkaL.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};

	platforma = platforma1;

	char text[128];
	int czarny = SDL_MapRGB(screen->format, 0x00, 0x00, 0x00);
	int zielony = SDL_MapRGB(screen->format, 0x00, 0xFF, 0x00);
	int czerwony = SDL_MapRGB(screen->format, 0xFF, 0x00, 0x00);
	int niebieski = SDL_MapRGB(screen->format, 0x11, 0x11, 0xCC);

	t1 = SDL_GetTicks();

	frames = 0;
	quit = 0;
	worldTime = 0;

	while (!quit) {

		if (danePos.postacAnimacja == 1) {
			postac = postac1;
		}
		else if (danePos.postacAnimacja == 2) {
			postac = postac2;
		}
		else if (danePos.postacAnimacja == 3) {
			postac = postac3;
		}
		else if (danePos.postacAnimacja == 4) {
			postac = postac4;
		}

		if (daneBecz.beczkaAnimacja == 1) {
			beczka = beczkaAG;
			beczka2 = beczkaAG;
		}
		else if (daneBecz.beczkaAnimacja == 2) {
			beczka = beczkaAP;
			beczka2 = beczkaAP;
		}
		else if (daneBecz.beczkaAnimacja == 3) {
			beczka = beczkaAD;
			beczka2 = beczkaAD;
		}
		else if (daneBecz.beczkaAnimacja == 4) {
			beczka = beczkaAD;
			beczka2 = beczkaAD;
		}

		if (czasGry % 20 == 0) 
		{
			if (daneBecz.beczkaAnimacja == 1) {
				daneBecz.beczkaAnimacja = 2;
			}
			else if(daneBecz.beczkaAnimacja == 2){
				daneBecz.beczkaAnimacja = 3;
			}
			else if (daneBecz.beczkaAnimacja == 3) {
				daneBecz.beczkaAnimacja = 4;
			}
			else if (daneBecz.beczkaAnimacja == 4) {
				daneBecz.beczkaAnimacja = 1;
			}
				
		}

		if (czasGry % 1400 == 0 || czasGry % 2700 == 0)
		{

			daneCias.ciastkoAnimacja = 1;

		}
		else if(czasGry % 1300 == 0 || czasGry % 2600 == 0)
		{

			daneCias.ciastkoAnimacja = 2;

		}



		SDL_FillRect(screen, NULL, czarny);

		//Obrysowanie planszy
		DrawRectangle(screen, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, zielony, czarny);

		rysujPierwszaPlarforma(screen, platforma, platformaTab);

		if (currentMap == 1) {

			rysujPlatformyMapa1(screen, platforma, platformaTab, dziuraTab, 1);
			rysujPlatfomaTop1(screen, platforma, platformaTab, 0);
			rysujDrabinki1(screen, drabina, drabinaTab, 0);

		}
		else if (currentMap == 2)
		{
			rysujPlatformyMapa1(screen, platforma, platformaTab, dziuraTab, 2);
			rysujPlatfomaTop1(screen, platforma, platformaTab, 1);
			rysujDrabinki1(screen, drabina, drabinaTab, 1);
		}
		else if (currentMap == 3)
		{
			rysujPlatformyMapa1(screen, platforma, platformaTab, dziuraTab, 3);
			rysujPlatfomaTop1(screen, platforma, platformaTab, 0);
			rysujDrabinki1(screen, drabina, drabinaTab, 0);
		}



		if (daneCias.ciastkoAnimacja == 1) 
		{
			ciastko = ciastko1;
		}
		else if (daneCias.ciastkoAnimacja == 2)
		{ 
			ciastko = ciastko2;
		}


		DrawSurface(screen, postac, danePos.postacX, danePos.postacY);//rysoanie postaci
		DrawSurface(screen, beczka, daneBecz.beczkaX, daneBecz.beczkaY);//rysoanie beczki
		DrawSurface(screen, ciastko, daneCias.ciastkoX, daneCias.ciastkoY);//rysoanie ciastka

		if (czasGry >= beczkaCzas + 1340) {
			DrawSurface(screen, beczka2, daneBecz2.beczkaX, daneBecz2.beczkaY);
		}

		// tekst informacyjny / info text
		DrawRectangle(screen, 4, 5, SCREEN_WIDTH - 8, 36, czerwony, niebieski);
		DrawRectangle(screen, 4, 550, SCREEN_WIDTH - 8, 36, czerwony, niebieski);
		sprintf(text, "Esc - wyjscie, n - nowa gra");
		DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, 555, text, charset);
		sprintf(text, "(\030)gora (\31)dol (\32)lewo (\33)prawo (space)skok");
		DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, 570, text, charset);

		if (platformaTab[5][0] < danePos.postacY + (POSTAC_HEIGHT / 2)) {

			t2 = SDL_GetTicks();

			// w tym momencie t2-t1 to czas w milisekundach,
			// jaki uplyna≥ od ostatniego narysowania ekranu
			// delta to ten sam czas w sekundach
			// here t2-t1 is the time in milliseconds since
			// the last screen was drawn
			// delta is the same time in seconds
			delta = (t2 - t1) * 0.001;
			t1 = t2;

			worldTime += delta;

		}

		czasGry = (int)(worldTime * 100);

		//            "template for the second project, elapsed time = %.1lf s  %.0lf frames / s"
		if (kolizja == 1) {
			sprintf(text, "Donkey king, czas trwania = %.1lf s KOLIZJA", worldTime);
			if (kolizjaCzas == 0) 
			{
				kolizjaCzas = czasGry;	
			}
		}
		else {
			sprintf(text, "Donkey king, czas trwania = %.1lf s ", worldTime);
		}
		
		DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, 10, text, charset);

		sprintf(text, "Wykonane podpunkty: 1, 2, 3, 4, A, B, C, E");
		DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, 26, text, charset);
		//	      "Esc - exit, \030 - faster, \031 - slower"

		SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
		//SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, scrtex, NULL, NULL);
		SDL_RenderPresent(renderer);

		
		
		/*if (ruchBeczkiCzas + 1 < czasGry)
		{
			printf("GIT");
			ruchBeczka(&beczkaX, &beczkaY);
			ruchBeczkiCzas = (int)(worldTime * 10);
		}*/

		if (kolizjaCzas + 500 < czasGry) 
		{
			kolizja = 0;
			kolizjaCzas = 0;
		}

		if (czasGry % 1 == 0) 
		{
			if (currentMap == 1)
			{
				ruchBeczka(&daneBecz);
				if (czasGry >= beczkaCzas + 1340) {
					ruchBeczka(&daneBecz2);
				}
			}
			else if (currentMap == 2)
			{
				ruchBeczka2(&daneBecz);
				if (czasGry >= beczkaCzas + 1340) {
					ruchBeczka2(&daneBecz2);
				}
			}
			else if (currentMap == 3)
			{
				ruchBeczka(&daneBecz);
				if (czasGry >= beczkaCzas + 1340) {
					ruchBeczka(&daneBecz2);
				}

			}
		}

		czyKolizja(danePos, daneBecz, &kolizja);
		czyKolizja(danePos, daneBecz2, &kolizja);
		//printf("kolizja = %d\n", kolizja);

		if (czy_skok) {
			skokPostac(&danePos, platformaTab, &czy_skok);
		}
		else
		{
			if (danePos.czas_od_ruchu + 20 < czasGry) {
				danePos.ostatni_ruch = 0;
			}
			grawitacja(&danePos, platformaTab, drabinaTab, dziuraTab);
		}

		// obs≥uga zdarzeÒ (o ile jakieú zasz≥y) / handling of events (if there were any)
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_KEYDOWN:
				if (event.key.keysym.sym == SDLK_ESCAPE) quit = 1;
				else if (event.key.keysym.sym == SDLK_n) {
					nowaGra(&worldTime, &czy_skok);
					resetPostac(&danePos);
					resetDziur(dziuraTab);
					resetBeczka(&daneBecz, currentMap);
					resetBeczka(&daneBecz2, currentMap);
					resetCiastko(&daneCias, currentMap);
					beczkaCzas = 0;
				}
				else if (event.key.keysym.sym == SDLK_1)
				{
					currentMap = 1;
					resetPostac(&danePos);
					resetDziur(dziuraTab);
					resetBeczka(&daneBecz, currentMap);
					resetBeczka(&daneBecz2, currentMap);
					resetCiastko(&daneCias, currentMap);
					platforma = platforma1;
					beczkaCzas = czasGry;
				}
				else if (event.key.keysym.sym == SDLK_2)
				{
					currentMap = 2;
					resetPostac(&danePos);
					resetDziur(dziuraTab);
					resetBeczka(&daneBecz, currentMap);
					resetBeczka(&daneBecz2, currentMap);
					resetCiastko(&daneCias, currentMap);
					platforma = platforma2;
					beczkaCzas = czasGry;
				}
				else if (event.key.keysym.sym == SDLK_3)
				{
					currentMap = 3;
					resetPostac(&danePos);
					resetDziur(dziuraTab);
					resetBeczka(&daneBecz, currentMap);
					resetBeczka(&daneBecz2, currentMap);
					resetCiastko(&daneCias, currentMap);
					platforma = platforma3;
					beczkaCzas = czasGry;
				}
				else if (event.key.keysym.sym == SDLK_LEFT) {
					ruchLewoPostac(&danePos, platformaTab);
					danePos.czas_od_ruchu = czasGry;
				}
				else if (event.key.keysym.sym == SDLK_RIGHT) {
					ruchPrawoPostac(&danePos, platformaTab);
					danePos.czas_od_ruchu = czasGry;
				}

				if (event.key.keysym.sym == SDLK_UP) {
					ruchGoraPostac(&danePos, drabinaTab);
					danePos.ktoraPlat = ktoraPlatforma(danePos.postacY, platformaTab, danePos.ktoraPlat);
				}
				else if (event.key.keysym.sym == SDLK_DOWN) {
					ruchDolPostac(&danePos, drabinaTab);
					danePos.ktoraPlat = ktoraPlatforma(danePos.postacY, platformaTab, danePos.ktoraPlat);
				}
				break;
			case SDL_KEYUP:
				if (event.key.keysym.sym == SDLK_SPACE) {
					if (czy_skok == 0) {
						czy_skok = 1;
					}

				}
				break;
			case SDL_QUIT:
				quit = 1;
				break;
			};
		};
		frames++;
	};

	// zwolnienie powierzchni / freeing all surfaces
	SDL_FreeSurface(charset);
	SDL_FreeSurface(screen);
	SDL_DestroyTexture(scrtex);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	SDL_Quit();
	return 0;
};