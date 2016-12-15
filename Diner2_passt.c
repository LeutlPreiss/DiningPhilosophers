/* Autor: Stefan Preiß & Mark Leutl
 * Date: 10.12.2016
 * Place of Origin: HTL Hollabrunn
 * Projekt-Name: Dining Philosophers
 * Lösung: Linkshänder
*/
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
 
#define N 5							//Anzahl der Philosophen
const char *names[N] = { "Aristoteles", "Kant", "Platon", "Marx", "Aquin" };	//Namen der Philosophen
pthread_mutex_t forks[N];			//Thread anlegen
 
#define M 5 /* Denkblasen */		//Anzahl der Denkgänge
const char *topic[M] = { "Spaghetti!", "Life", "Universe", "Everything", "Bathroom" };	//Denkgänge anlegen
 
#define lock pthread_mutex_lock				//Lock-Variable erstellen
#define unlock pthread_mutex_unlock			//Unlock-Variable erstellen
#define xy(x, y) printf("\033[%d;%dH", x, y)	//Ausgabe definieren
#define clear_eol(x) print(x, 12, "\033[K")		//Ausgabe clearen
void print(int y, int x, const char *fmt, ...)	//Unterprogramm für die Ausgabe
{
	static pthread_mutex_t screen = PTHREAD_MUTEX_INITIALIZER;	//
	va_list ap;						//Returnwert anlegen
	va_start(ap, fmt);				//Arugmentliste initialisieren
 
	lock(&screen);					//Während Mutex sperren
	xy(y + 1, x), vprintf(fmt, ap); //Argumentenliste ausgeben
	xy(N + 1, 1), fflush(stdout);	//Ausgabe-Puffer leeren
	unlock(&screen);				//Nach Mutex entsperren
}
 
void eat(int id)					//Problemlösungsvorgang Essen
{
	int f[2], ration, i; 			//Variablen für Forks anlegen
	f[0] = f[1] = id;
 
	/* Lösungsweg: Ein paar Philosophen sind Linkshänder aber nicht alle!
	   Code-Alternative: f[!id] = (id + 1) %N; */
	f[id & 1] = (id + 1) % N;		//Linkshänder anlegen
 
	clear_eol(id);					//=ClearEndOfLine
	print(id, 12, "..oO (forks, need forks)");	//Ausgabe Diner
 
	for (i = 0; i < 2; i++) {		
		lock(forks + f[i]);			//Während Mutex sperren
		if (!i) clear_eol(id);
 
		print(id, 12 + (f[i] != id) * 6, "fork%d", f[i]);	//Ausgabe
		/* Verzögerung von 1 Sekunde um Fork-Erschaffung zu verbildlichen */
		sleep(1);					//Verzögerung 1s
	}
 
	for (i = 0, ration = 3 + rand() % 8; i < ration; i++)
		print(id, 24 + i * 4, "nom"), sleep(1);	//Ausgabe Essgeräusche
 
	
	for (i = 0; i < 2; i++) unlock(forks + f[i]);	//Nach Mutex entsperren
}
 
void think(int id)					//Unterprogramm Denken
{
	int i, t;
	char buf[64] = {0};
 
	do {
		clear_eol(id);				//=ClearEndOfLine
		sprintf(buf, "..oO (%s)", topic[t = rand() % M]);	//Denkaufgabe in String speichern
 
		for (i = 0; buf[i]; i++) {
			print(id, i+12, "%c", buf[i]);	//Denkblase ausgeben
			if (i < 5) usleep(200000);
		}
		usleep(500000 + rand() % 1000000);	//Vorgang verzögern
	} while (t);
}
 
void* philosophize(void *a)			//Unterprogramm Philosophieren
{
	int id = *(int*)a;
	print(id, 1, "%10s", names[id]);//Ausgabe Philosophennamen
	while(1) think(id), eat(id);	//Essen & Denken ausführen
}
 
int main()
{
	int i, id[N];
	pthread_t tid[N];
 
	for (i = 0; i < N; i++)
		pthread_mutex_init(forks + (id[i] = i), 0);	//Mutex initialisieren
 
	for (i = 0; i < N; i++)
		pthread_create(tid + i, 0, philosophize, id + i);	//neuen Thread erstellen
 
	/* wait forever: the threads don't actually stop */
	return pthread_join(tid[0], 0);	//Warten bis Thread beendet wird
}

