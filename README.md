# Connect4 bot
*verzija: 2.1 - Nejc Jezeršek - 1. 3. 2019*

Glavni program je `connect4`. Poženemo ga z ukazom `./connect4`. Izvorna koda tega programa je napisana v C++ (zato da lahko dokaj hitro procesira slike in se odloča o potezi) in se nahaja v mapi `source/`. Program lahko prevedemo z ukazom `g++ source/* -o connect4 -O3`. Oznaka `-O3` pomeni da se program optimizira za največjo hitrost.

Za zajemanje slike se uporablja skripta `captureImage.py`, ker je python-picamera hitrejša od ukaza `raspistill`. Ta skripta naredi sliko in jo shrani v datoteko `capture.png`.

Za dekodiranje in kodiranje PNG slike se uporablja knjižnica **lodepng** (https://github.com/lvandeve/lodepng).

## Instalacija
Če želimo uporabljati orodje za kalibracijo programa za analizo slik, moramo vsebino tega repozitorija dodati v mapo spletneg strežnika, ki ima omogočen PHP in `exec` ukaz. Datoteki `connect4` in `captureImage.py` morata imeti nastavljene pravice tako, da ju lahko kdor koli zažene (777). Datoteki `capture.png` in `config.txt` morata imeti nastavljene pravice tako, da ju lahko kdor koli ureja (666).

Da lahko spletni strežnik dela slike moramo uporabnika `www-data` dodati v skupino `video`. To naredimo z ukazom:
```
sudo usermod -a -G video www-data
sudo service apache2 restart
```
Potem moramo ponovno zagnati spletni strežnik.

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

## Orodje za kalibracijo
Orodje za kalibracijo se nahaja v `index.php`. 

V okvirčku **Analysed image** vidimo obdelano sliko. Na njej lahko določimo robove igralne površine tako, da povelčemo kotne točke. 

Vsaka celica je prekrita s kvadratkom, v katerem so piksli označeni z barvo, če glede na hue komponento HSV barve spadajo v določeno območje. To območje lahko določimo na **Hue histogram**. Če piksel ni dovolj saturiran se pobarva z belo barvo. Prag satureacije lahko nastavimo v okvirčku **Settings**. Če piksel ne ustreza nobenemu od prej naštetih pogojev se pobarva z sivo barvo.

V okvirčku **Settings** lahko nastavimo tudi velikost kvadratkov, ki prekrivajo celice, in delež pobarvanih pikslov v celici, ki je potreben za prepoznavo žetona (če je *PX count threshold* nastavljen na 0.5 to pomeni da mora biti polovica celice pobarvana, da se označi z določeno barvo).

Ko smo zadovoljeni s konfiguracijo, jo lahko shranimo s klikom na gumb *Submit* v okvirčku **Compiled configuration**.

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
    n += 1
    # preštej rumene in zelene piksle
    if pixel.saturation > 0.3 :
        if pixel.hue < 60/255.0 :
            yellow += 1
        else:
            green += 1
#če je pikslov ene barve več kot tretjina, označi celico
if yellow > n/3 :
    label = "O"
if green > n/3 :
    label = "X"
```
Pridobljene podatke o stanju igre vstavi v Minimax algoritem in se odloči za potezo.
