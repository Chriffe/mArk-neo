# mArk — Familjedisplay (Svensk version)

> **Den här versionen är en fork av det ursprungliga projektet [mArk av TheSwedishMaker](https://github.com/TheSwedishMaker/mArk-display).**
> Ett stort tack till TheSwedishMaker för grunden som hela det här bygger på.
>
> **Installationsguiden finns hos originalrepository** — följ stegen där för att komma igång:
> 👉 https://github.com/TheSwedishMaker/mArk-display

---

## Vad är mArk?

mArk är en alltid-på-display som omvandlar familjens dagliga kalender till något alla kan se, bocka av och tävla om. Varje familjemedlem får sin egen uppgiftslista hämtad direkt från sin kalender. När uppgifter markeras som klara fylls en LED-remsa på. Klar med allt? Displayen firar. Gör det igen imorgon och din streak växer. Missa en dag och den nollställs — tävlingen är på riktigt.

Byggd på en 10" pekskärm lever mArk på köksbänken eller var familjen samlas. Ingen app att öppna, inga notiser att avfärda. Den finns bara där.

---

## Skillnader mot originalversionen

### Gränssnittet är på svenska

Hela användargränssnittet — uppgiftslistan, knappar, hälsningar, datum och meddelanden — är på svenska. Om du bygger det här för en svensk familj behöver du inte ändra något.

### Förbättrad kalenderhantering

Den här versionen innehåller en kraftigt förbättrad ICS-parser jämfört med originalet:

**Stöd för återkommande händelser (RRULE)**
Originalet kunde bara visa en händelse på dagen den var skapad. Den här versionen hanterar fullt ut:
- `FREQ=DAILY` — händelser som upprepas varje dag (t.ex. rutiner)
- `FREQ=WEEKLY` — händelser som upprepas varje vecka, med stöd för flera veckodagar via `BYDAY`
- `FREQ=MONTHLY` — händelser som upprepas varje månad på samma datum
- `FREQ=YEARLY` — händelser som upprepas varje år (t.ex. födelsedagar)
- `INTERVAL=N` — upprepning var N:te dag/vecka/månad/år (t.ex. varannan vecka)
- `UNTIL=` — upprepning upphör på ett visst datum

Utan det här visas inte rutiner, träningspass, veckostädningen eller andra återkommande händelser alls — oavsett hur de ser ut i din kalender.

**Stöd för borttagna enstaka tillfällen (EXDATE)**
När du tar bort ett enstaka tillfälle av en återkommande händelse i Google Kalender (utan att ta bort hela serien) hanteras det nu korrekt. Originalet visade fortfarande borttagna tillfällen.

**Korrekt tidszonhantering**
Tidsfönstret som används vid kalenterfrågor är nu korrekt konverterat till UTC baserat på svensk tid (CET sommartid / CEST vintertid). Tidigare kunde händelser vid midnatt och tidigt på morgonen falla utanför fönstret.

### Svenska streak-nivåer

Streak-systemet fungerar likadant som i originalet men nivånamnen är på svenska:

| Nivå | Dagar krävs |
|---|---|
| Lärling | 0 |
| Upptäckare | 5 |
| Magiker | 15 |
| Mästare | 30 |
| Stormästare | 50 |
| Tidsväktare | 100 |

---

## Hur man använder displayen

| Åtgärd | Vad den gör |
|---|---|
| Tryck på en uppgift | Markera som klar (tryck igen för att ångra) |
| Vrid på ratten | Bläddra bland uppgifterna |
| Tryck på ratten | Markera aktuell uppgift som klar |
| Tryck på **Uppdatera** | Hämta senaste uppgifterna från kalendern direkt |
| Tryck på **Inställningar** | Hantera användare och kalendrar |
| Tryck på en persons ikon | Byt till den personens uppgiftslista |
| Kort tryck på strömknappen | Slå av/på displayen och LED-remsan |

Uppgifterna uppdateras automatiskt var femte minut. Displayen uppdaterar sig också när den väcks från viloläge.

---

## Kalendertaggar — specialfunktioner via kalendern

Den här versionen stöder speciella taggar som du lägger direkt i händelsenamnet i din kalender. mArk plockar ut dem automatiskt och visar inte taggen i titeln.

### `[T]` — Nedräkningstimer

Lägg till `[T]` i händelsenamnet för att aktivera en inbyggd timer. Timerns längd sätts automatiskt till händelsens duration (sluttid minus starttid).

**Exempel i kalendern:**
```
Träna [T]
```

Händelsen måste ha en starttid och en sluttid. En heldagshändelse fungerar inte med timern.

När du markerar uppgiften på displayen startar en nedräkning. Du kan pausa, återuppta och återstarta timern. När den når noll spelas ett ljud och en notis visas.

---

### `[Namn:N]` — Utmaningsserier

Lägg till en tagg på formen `[Namn:N]` för att koppla uppgiften till en utmaningsserie. `Namn` är seriens namn och `N` är hur många gånger i rad du siktar på att klara den.

**Exempel i kalendern:**
```
Löptur [Löpning:30]
Meditation [Lugn:100]
```

Displayen visar hur många dagar du har klarat i rad för den serien, hur långt kvar det är till målet och om du slog rekordet när du når `N`.

Du kan ha flera olika utmaningar aktiva samtidigt — en per händelse. Serier är kopplade till personen, inte till uppgiften i sig, så varje familjemedlem har sina egna framsteg.

---

## Lägga till familjemedlemmar

mArk stöder upp till 6 personer, var och en med sin egen uppgiftslista, streak och progressbar.

1. Tryck på **Inställningar** på displayen
2. Tryck på **Lägg till användare**
3. Skriv in personens namn
4. Gå till webbgränssnittet (se originalets guide) för att lägga till den personens kalender

Byt mellan personer på displayen genom att trycka på personernas ikoner i vänster sidofält.

---

## Komma igång — följ originalets guide

Installationen av hårdvara, ESP-IDF, drivrutiner och WiFi-inställningar är identisk med originalet. Följ steg-för-steg-guiden hos originalrepository:

👉 **https://github.com/TheSwedishMaker/mArk-display**

Det enda som skiljer sig är att du klonar **det här** repository istället för det ursprungliga, och att gränssnittet sedan är på svenska.

### Tidszonen

Projektet är förinställt på **Central European Time (CET/CEST)** — svensk tid. Bor du i en annan tidszon, öppna `main/calendar_fetch.c` och hitta raden med `"CET-1CEST,M3.5.0/2,M10.5.0/3"` och byt ut den mot din tidszon. En lista med alla koder finns på:
https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv

---

## Hårdvara

### Vad du behöver

| Del | Detaljer |
|---|---|
| Display | Elecrow CrowPanel Advanced 10.1" (ESP32-P4) |
| LED-remsa | WS2812 adresserbara LEDs (32 LEDs rekommenderas) |
| Ratt (rotary encoder) | KY-040 eller liknande med trycknapp |
| Strömknapp | Valfri momentan trycknapp |

### Kopplingsschema

| Del | Signal | Koppla till |
|---|---|---|
| LED-remsa | DIN (data) | IO2 |
| LED-remsa | VCC | 5V |
| LED-remsa | GND | GND |
| Strömknapp | Ena benet | IO3 |
| Strömknapp | Andra benet | GND |
| Ratt | CLK | IO27 |
| Ratt | DT | IO28 |
| Ratt | SW (trycknapp) | IO5 |
| Ratt | VCC | 3.3V |
| Ratt | GND | GND |

---

## Felsökning

**Vit eller blank skärm efter flashning**
→ Displaydrivrutinerna saknas. Följ steg 2 i originalets guide och se till att du kopierar mappen `peripheral/` från just Lesson 09.

**"idf.py not found"-fel**
→ ESP-IDF-miljön är inte aktiv i terminalen. Stäng terminalen, öppna en ny och kör `export.sh` (Mac/Linux) eller `export.bat` (Windows) från ESP-IDF-installationsmappen.

**Displayen ansluter inte till WiFi**
→ Kontrollera nätverksnamn och lösenord i `main/secrets.h`. Nätverket måste vara 2,4 GHz — displayen stöder inte 5 GHz WiFi.

**Inga uppgifter visas**
→ Om du använder ICS-länk: se till att du kopierade hela URL:en inklusive `https://`. Om du använder Google API: kontrollera att kalendern är satt till offentlig och att API-nyckeln har Google Calendar API aktiverat.

**Återkommande händelser visas inte**
→ Kontrollera att du använder den här versionen (forken) och inte originalversionen. Det är den här versionen som lägger till stöd för återkommande händelser.

**Borttagna tillfällen visas fortfarande**
→ Tryck på **Uppdatera**. Om problemet kvarstår: se till att du tagit bort det enstaka tillfället i kalendern (inte hela serien) och att displayen har hämtat ett uppdaterat ICS-flöde.

**Webbgränssnittet laddar inte**
→ Din telefon måste vara på samma WiFi-nätverk som displayen. Prova IP-adressen som visas på displayen istället för `mark.local`.
