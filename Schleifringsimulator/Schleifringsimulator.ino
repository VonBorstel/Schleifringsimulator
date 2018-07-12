/*
 Name:		Schleifringsimulator.ino
 Created:	18.10.2016 11:02:38
 Author:	matthis.vonborstel
 Firma:		kumkeo GmbH
*/

// Schleifringsimulator Software V2.0

#include <LiquidCrystal_SR3W.h>
#include <LiquidCrystal_SR2W.h>
#include <LiquidCrystal_SR.h>
#include <LiquidCrystal.h>
#include <I2CIO.h>
#include <FastIO.h>
#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7);	// Namespace lcd mit Adresse f�r I2C:0x27


#define S1  2										//Zuweisung der Pins zu den Schalternamen
#define S2  3
#define S3  4
#define S4  5
#define S5  6
#define Kodier1 53
#define Kodier2 52
#define Kodier3 51
#define Kodier4 50

//Widerstandsarraylängenbestimmung rMatrixWidth. Da die Zeilen eines jeden Blocks in einem 3D Array gleich lang sind muss über das Array rMatrixWidth die gwünschte länge jedes Blocks angegeben werden.    
    
int rMatrixWidth[10] =  {8,8,2,80,11,11,11,11,11,11}; 
    
//Array für die Schaltzeit der Widerstände rSwitchTime. Zeitarray für die Schaltzeit der Widerstände eines Modus in ms. Für einen schnellstmöglichen durchlauf "0" für den gewünschten Modus eingeben. !!!ACHTUNG!!! Die Arduino Zeit schwankt um 1 bis 2ms!!!    
    
int rSwitchTime[10] =   {0,50,10,0,100,200,300,400,500,1000};  
    
//Widerstandsarray rMatrix    
/* 3D Array f�r die acht Widerst�nde die geschaltet werden und die 10 �ber den BCD-Schalter schaltbaren Modi. Da der Speicher des Arduino begrenzt ist ist das Array auf 8*10*80 begrenzt. Laut Kompiler sind 84% des Stack belegt. Jeder Block stellt einen Modus dar, jede Zeile ist ein Widerstand und jede Spalte ist ein Durchlauf des Schaltalgorithmus (der loop()), also die Zeitliche Komponente. [Zeile][Block][Spalte] */    
    
bool rMatrix [8][10][80] = {	{{1,0,0,0,0,0,0,0},{1,0,0,0,0,0,0,0},{1,0},{0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,1,0,0,0,0,0,0,0,0,0},{0,1,0,0,0,0,0,0,0,0,0},{0,1,0,0,0,0,0,0,0,0,0},{0,1,0,0,0,0,0,0,0,0,0},{0,1,0,0,0,0,0,0,0,0,0},{0,1,0,0,0,0,0,0,0,0,0}},  
								{{0,1,0,0,0,0,0,0},{0,1,0,0,0,0,0,0},{1,0},{0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,0,1,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,1},{0,0,0,0,0,0,0,0,0,0,1},{0,0,0,0,0,0,0,0,0,0,1},{0,0,0,0,0,0,0,0,0,0,1},{0,0,0,0,0,0,0,0,0,0,1},{0,0,0,0,0,0,0,0,0,0,1}},  
								{{0,0,1,0,0,0,0,0},{0,0,1,0,0,0,0,0},{1,0},{0,0,1,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,1,0,1,0,0,1,1,1,1,1,1,1,1,1,1,0,0,1,1,0,1,1,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{1,0,1,1,1,0,0,0,0,0,1},{1,0,1,1,1,0,0,0,0,0,1},{1,0,1,1,1,0,0,0,0,0,1},{1,0,1,1,1,0,0,0,0,0,1},{1,0,1,1,1,0,0,0,0,0,1},{1,0,1,1,1,0,0,0,0,0,1}},  
								{{0,0,0,1,0,0,0,0},{0,0,0,1,0,0,0,0},{1,0},{0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,1,1,0,1,0,1,0,0,1,1,1,0,1,1,1,0,0,0,1,1,0,0,1,0,0,1,1,1,1,1,1,0,0,0,0,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1},{0,0,1,0,0,0,0,0,0,1,0},{0,0,1,0,0,0,0,0,0,1,0},{0,0,1,0,0,0,0,0,0,1,0},{0,0,1,0,0,0,0,0,0,1,0},{0,0,1,0,0,0,0,0,0,1,0},{0,0,1,0,0,0,0,0,0,1,0}},  
								{{0,0,0,0,1,0,0,0},{0,0,0,0,1,0,0,0},{0,1},{0,0,1,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,1,1,1,1,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,1,0,0,0,0,0},{0,0,0,0,0,1,0,0,0,0,0},{0,0,0,0,0,1,0,0,0,0,0},{0,0,0,0,0,1,0,0,0,0,0},{0,0,0,0,0,1,0,0,0,0,0},{0,0,0,0,0,1,0,0,0,0,0}},  
					            {{0,0,0,0,0,1,0,0},{0,0,0,0,0,1,0,0},{0,1},{0,0,0,0,0,0,0,0,0,1,0,1,1,0,0,0,0,1,1,0,1,0,0,1,0,0,1,0,0,0,1,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,1,0,0},{0,0,0,0,0,0,0,0,1,0,0},{0,0,0,0,0,0,0,0,1,0,0},{0,0,0,0,0,0,0,0,1,0,0},{0,0,0,0,0,0,0,0,1,0,0},{0,0,0,0,0,0,0,0,1,0,0}},  
								{{0,0,0,0,0,0,1,0},{0,0,0,0,0,0,1,0},{0,1},{0,0,0,0,0,0,0,0,0,1,0,0,1,1,0,1,0,1,0,1,1,0,0,0,1,1,0,0,0,0,1,0,0,1,1,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,1,0,0,1,1,0,0,0},{0,0,0,1,0,0,1,1,0,0,0},{0,0,0,1,0,0,1,1,0,0,0},{0,0,0,1,0,0,1,1,0,0,0},{0,0,0,1,0,0,1,1,0,0,0},{0,0,0,1,0,0,1,1,0,0,0}},  
								{{0,0,0,0,0,0,0,1},{0,0,0,0,0,0,0,1},{0,1},{1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0}}}; 


//Struct zum speichern aller notwendigen Daten f�r die IBS

struct IBSVaribles {
	int IBS[8];										//IBS konfigurations Array
	int columnIBS = 0;								//Z�hler f�r die Widerstandsschaltung
}IBSData[5];										//Array des Structs. F�r jede Spur ein Array


//Startverzögerung, Modusvariablen initialisieren und modusOld f�r den ersten Start auf einen f�r modus nicht erreichbaren Wert setzen um erst Start zu erzwingen.

int startDelay, modus, modusOld = 10;

//Zeitstempel f�r den Start eines neuen Modus

unsigned long start;

//Timer f�r die Verz�gerungsschleife

unsigned long delayTimer;

//Interrupt Flag

bool newModus = false;

void setup() {

	//LCD initialisieren
	lcd.begin(16, 2);					//Einstellen des Displays
	lcd.setBacklightPin(3, POSITIVE);
	lcd.setBacklight(HIGH);
	lcd.clear();
	lcd.home();
	lcd.print("   Willkommen");
	lcd.setCursor(0, 1);
	lcd.print("Schlringsimu v2.0");


	// Initialisieren der Ausgaenge f�r die Widerstaende/LEDs
	for (int i = 0; i <= 7; i++){
		if (i <= 6)IBSData[0].IBS[i] = 14 - i;      //Pin f�r IBS1-1 bis IBS1-7
		if (i == 7)IBSData[0].IBS[i] = 69;          //Pin f�r IBS1-8
		if (i <= 2)IBSData[1].IBS[i] = 24 - i;      //Pin f�r IBS2-1 bis IBS2-3
		if (i >  2)IBSData[1].IBS[i] = 22 - i;      //Pin f�r IBS2-4 bis IBS2-8
		IBSData[2].IBS[i] = 32 - i;                 //Pin f�r IBS3-1 bis IBS3-8
		IBSData[3].IBS[i] = 40 - i;                 //Pin f�r IBS4-1 bis IBS4-8
		IBSData[4].IBS[i] = 48 - i;                 //Pin f�r IBS5-1 bis IBS5-8
	}

	for (int i = 0; i <= 8; i++){								//Pins werden als Output deklariert.
		pinMode(IBSData[0].IBS[i], OUTPUT);
		digitalWrite(IBSData[0].IBS[i], LOW);                   //Ausgaenge werden "Aus" gesetzt
		pinMode(IBSData[1].IBS[i], OUTPUT);
		digitalWrite(IBSData[1].IBS[i], LOW);                   //Ausgaenge werden "Aus" gesetzt
		pinMode(IBSData[2].IBS[i], OUTPUT);
		digitalWrite(IBSData[2].IBS[i], LOW);                   //Ausgaenge werden "Aus" gesetzt
		pinMode(IBSData[3].IBS[i], OUTPUT);
		digitalWrite(IBSData[3].IBS[i], LOW);                   //Ausgaenge werden "Aus" gesetzt
		pinMode(IBSData[4].IBS[i], OUTPUT);
		digitalWrite(IBSData[4].IBS[i], LOW);                   //Ausgaenge werden "Aus" gesetzt
	}

	//Initialisieren der Eingaenge
	pinMode(S1, INPUT);                            //Schalter S1
	pinMode(S2, INPUT);                            //Schalter S2
	pinMode(S3, INPUT);                            //Schalter S3
	pinMode(S4, INPUT);                            //Schalter S4
	pinMode(S5, INPUT);                            //Schalter S5
	pinMode(Kodier1, INPUT);                       //BCD-Schalter
	pinMode(Kodier2, INPUT);
	pinMode(Kodier3, INPUT);
	pinMode(Kodier4, INPUT);
	

	//Interrupt einstellen
	cli();
	PCICR = 0x01;									//PCINT0 atkivieren
	PCMSK0 = 0x15;									//Eingänge für Interrupt freigeben
	sei();

	delay(2000);									//Delay f�r Anzeige
	lcd.clear();									//Anzeige zur�cksetzen
	lcd.home();
}

ISR(PCINT0_vect){												//Interruptserviceroutine zum einlesen des Modus

	newModus = true;											//Interrupt Flag setzen
}
	
void allLedOff(int IBS){											//Funktion zum ausschalten der Widerstaende einer Spur. Es ist nur der IBSX-8 aktiv in dem kein Widerstand geschaltet ist.

	for (int i = 7; i >= 0; i--){
		if (i<7)													//Alle Widerstände aus
		{
			digitalWrite(IBSData[IBS].IBS[i], LOW);
		}
		else														//Nur der Ausgang ohne Widerstand wird geschaltet.
		{
			digitalWrite(IBSData[IBS].IBS[i], HIGH);
		}
		
	}
	return;
}

void ledOn(int IBS, int column){									//Funktion um den Status der Widerstaende/LEDs pro Spur zu aendern

	for (int i = 0; i <= 7; i++){
		if (rMatrix[i][modus][column] == true){                      //Es wird gepr�ft ob der jeweilige Widerstand geschaltet werden soll
			digitalWrite(IBSData[IBS].IBS[i], HIGH);
		}
		else{
			digitalWrite(IBSData[IBS].IBS[i], LOW);
		}
	}
	return;
}

void switchON(int IBS){															//Funktion f�r eingeschaltete IBS
	if (IBSData[IBS].columnIBS < rMatrixWidth[modus]){                          //Pr�fen ob Arrayende erreicht
		ledOn(IBS, IBSData[IBS].columnIBS);
		IBSData[IBS].columnIBS++;
	}
	if (IBSData[IBS].columnIBS == rMatrixWidth[modus])
	{
		IBSData[IBS].columnIBS = 0;
	}
	return;
}

void switchOFF(int IBS){														//Funktion f�r ausgeschaltete IBS
	allLedOff(IBS);																//Wenn der Schalter nicht gesetzt ist zu Funktion ledOff springen
	if (IBSData[IBS].columnIBS < rMatrixWidth[modus]){                          //Pr�fen ob Arrayende erreicht
		IBSData[IBS].columnIBS++;												//Z�hler wird weiter gez�hlt, damit er aktuell ist wenn die Spur wieder zugeschaltet wird
	}
	if (IBSData[IBS].columnIBS == rMatrixWidth[modus])
	{
		IBSData[IBS].columnIBS = 0;
	}
	return;
}

void initNewModus(){										//Funktion f�r Moduswechsel
	modusOld = modus;										//Alten Modus �berschreiben
	for (int i = 0; i < 5; i++)								//Beim wechsel des Modus muss der Z�hler f�r die Durchl�ufe der Spalten zur�ckgesetzt werden
	{
		IBSData[i].columnIBS = 0;
		switchOFF(i);
	}
	lcd.clear();
	lcd.setCursor(0, 0);                                       //Modus in Display schreiben
	lcd.print("Modus ");
	lcd.print(modus);
	lcd.setCursor(0, 1);                                       //Zeitverz�gerung ins Display schreiben
	lcd.print("Zeit ");
	if (rSwitchTime[modus]<2)
	{
		lcd.print("450us");
	}
	else
	{
		lcd.print(rSwitchTime[modus]);
		lcd.print("ms");
	}
	if (rSwitchTime[modus] == 0)
	{
		startDelay = 1;
	}
	else
	{
		startDelay = rSwitchTime[modus];
	}
	start = millis();
	return;
}

void loop() {                                       // main code

	if (newModus == true)											//Prüfen ob Modus gewechselt wurde
	{
		newModus = false;											//Interrupt Flag zurücksetzen
		do{
			modus = 0;
			if (digitalRead(Kodier1) == LOW){						//BCD-Schalter auslesen und die Wertigkeit festlegen
				modus += 1;
			}
			if (digitalRead(Kodier2) == LOW){
				modus += 2;
			}
			if (digitalRead(Kodier3) == LOW){
				modus += 4;
			}
			if (digitalRead(Kodier4) == LOW){
				modus += 8;
			}
		} while (modus>9);                                          //Pr�fen ob der Modus g�ltig ist

		if (modusOld != modus){										//Pr�fen ob neuer Modus geschaltet wurde                                
			initNewModus();
		}
	}

	//Referrenz f�r die Verz�gerungsschleife setzen
	delayTimer = millis();

	if (digitalRead(S1) == HIGH){                                                   //Schalter S1 auslesen und in Funktion switchOn springen
		switchON(0);																//Funktion mit Logik zum Schalten der Widerst�nde und setzten der Timer und Z�hler
	}
	else{																			//Wenn der Schalter nicht gesetzt ist zu Funktion switchOff springen
		switchOFF(0);																//Funktion mit Logik zum Ausschalten der Widerst�nde und setzen der Timer und Z�hler
	}

	if (digitalRead(S2) == HIGH){                                                   //Schalter S2 auslesen und in Funktion switchOn springen
		if (start + startDelay < millis()){											//Pr�fen ob Startzeit f�r IBS erreicht 
			switchON(1);
		}
	}
	else{																			//Wenn der Schalter nicht gesetzt ist zu Funktion switchOff springen
		if (start + startDelay < millis()){											//Pr�fen ob Startzeit f�r IBS erreicht 
			switchOFF(1);
		}
	}
	if (digitalRead(S3) == HIGH){                                                   //Schalter S3 auslesen und in Funktion switchOn springen
		if (start + 2 * startDelay < millis()){										//Pr�fen ob Startzeit f�r IBS erreicht 
			switchON(2);
		}
	}
	else{																			//Wenn der Schalter nicht gesetzt ist zu Funktion switchOff springen
		if (start + 2 * startDelay < millis()){										//Pr�fen ob Startzeit f�r IBS erreicht 
			switchOFF(2);
		}
	}

	if (digitalRead(S4) == HIGH){                                                   //Schalter S4 auslesen und in Funktion switchOn springen
		if (start + 3 * startDelay < millis()){										//Pr�fen ob Startzeit f�r IBS erreicht 
			switchON(3);
		}
	}
	else{																			//Wenn der Schalter nicht gesetzt ist zu Funktion switchOff springen
		if (start + 3 * startDelay < millis()){										//Pr�fen ob Startzeit f�r IBS erreicht 
			switchOFF(3);
		}
	}
	if (digitalRead(S5) == HIGH){                                                   //Schalter S5 auslesen und in Funktion switchOn springen
		if (start + 4 * startDelay < millis()){										//Pr�fen ob Startzeit f�r IBS erreicht 
			switchON(4);
		}
	}
	else{																			//Wenn der Schalter nicht gesetzt ist zu Funktion switchOff springen
		if (start + 4 * startDelay < millis()){										//Pr�fen ob Startzeit f�r IBS erreicht 
			switchOFF(4);
		}
	}

	//Verz�gerungsschleife um die Schaltzeit der Widerst�nde zu erh�hen

	for (unsigned long i = millis(); i < delayTimer + rSwitchTime[modus]; i++)		//Durch das Vergleichen der Bedingung kommt es zu einer Verz�gerung, daher das delayMicroseconds mit 991�s damit ann�hernd 1ms pro Schleifendurchlauf verz�gert wird
	{
		delayMicroseconds(991);
	}
}
