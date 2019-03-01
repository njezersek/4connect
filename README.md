# Connect4 bot
*verzija: 2.1 - Nejc Jezeršek - 1. 3. 2019*

Glavni program je `connect4`. Poženemo ga z ukazom `./connect4`. Izvorna koda tega programa je napisana v C++ (zato da lahko dokaj hitro procesira slike in se odloča o potezi) in se nahaja v mapi `source/`. Program lahko prevedemo z ukazom `g++ source/* -o connect4 -O3`. Oznaka `-O3` pomeni da se program optimizira za največjo hitrost.

Za zajemanje slike se uporablja skripta `captureImage.py`, ker je python-picamera hitrejša od ukaza `raspistill`. Ta skripta naredi sliko in jo shrani v datoteko `capture.png`.

Za dekodiranje in kodiranje PNG slike se uporablja knjižnica **lodepng** (https://github.com/lvandeve/lodepng).

## Konfiguracija
Konfiguracija se nahaja v datoteki `config.txt`.

konfiguracijska datoteka mora biti v formatu:
```
X0 Y0
X1 Y1
X2 Y2
X3 Y3
HXmin HXmax
HOmin HOmax
ST
CS
TH
```
> Vse vrednosti razen TH morajo biti celo stevilo!

X in Y od 0 do 3 so stiri tocke, ki prdstavljajo vogale igralne mreze
-	**X0 Y0**	- zgornji desni kot
-	**X1 Y1**	- spodnji desni kot
-	**X2 Y2**	- spodnji levi kot
-	**X3 Y3**	- zgornji levi kot

**HXmin HXmax** (hue igralca X) predstavljata razpon odtenkov žetona igralca X (če je min večji od max gre razpon od 0 do max in od min do 255)

**HOmin HOmax** (hue igralca O)

**ST** (saturation threshold) predstavlja mejo saturacije, ki morabiti zadoscena, da se prepozna zeton

**CS** (cell size) predstavlja velikost kvadrata v katerem se zaznava zeton

**TH** (threshold) koliksen del (TH = 0.5 pomeni ena polovica) celice mora biti dolocene berve, da se celica oznaci s to barvo

> Konfiguracijski podatki morajo biti na vrhu datoteke!

## Delovanje programa

Program pocaka, da uporabnik naredi potezo in pritisne gumb. Nato zajame sliko tako da izvede python skripto `captureImage.py`.

Zajeto sliko nato obdela, da ugotovi katera polja so zasedena s katerim žetonom.
Iz konfiguracijske datoteke `config.txt` prebere koordinate **štirih vogalov igralne plošče**. Med njimi z interpolacijo določi ostalih 7x6 celic. Čez vsako celico naredi kvadrat in na njem izvede nasleden postopek, da ugotovi s katerim žetonom je zaseden:
```python
label = "."
n = 0
yellow = 0
green = 0
# pojdi čez vse piksle v kvadratu, ki prekriva skoraj celo celico
for pixel in kvadrat:
    n++
    # preštej rumene in zelene piksle
    if pixel.saturation > 0.3 :
        if pixel.hue < 60/255.0 :
            yellow++
        else:
            green++
#če je pikslov ene barve več kot tretjina, označi celico
if yellow > n/3 :
    label = "O"
if green > n/3 :
    label = "X"
```
Pridobljene podatke o stanju igre vstavi v Minimax algoritem in se odloči za potezo.
