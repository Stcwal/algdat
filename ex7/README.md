# Øving 7 - Dijkstra og ALT

## Kompilering
```bash
javac oving7.java
```

## Bruk

### 1. Finne korteste vei (Dijkstra)
```bash
java oving7 <noder> <kanter> <interessepkt> <start> <mål>
```

**Eksempel:**
```bash
java oving7 noder.txt kanter.txt interessepkt.txt 100000 200000
```

**Output:**
- Sammenligner Dijkstra vs ALT
- Viser reisetid (timer:minutter:sekunder)
- Antall noder undersøkt
- Rutens lengde
- Rutens noder (hvis ≤ 50 noder)
- Speedup og node-reduksjon

### 2. Finne 5 nærmeste interessepunkter
```bash
java oving7 <noder> <kanter> <interessepkt> <startnode> POI:<type>
```

**POI-typer:**
- `2` = Bensinstasjon
- `4` = Ladestasjon
- `8` = Spisested
- `16` = Drikkested
- `24` = Både spise og drikke (8+16)

**Eksempler:**
```bash
# Finn nærmeste bensinstasjoner
java oving7 noder.txt kanter.txt interessepkt.txt 100000 POI:2

# Finn nærmeste spisesteder
java oving7 noder.txt kanter.txt interessepkt.txt 100000 POI:8

# Finn nærmeste ladestasjoner
java oving7 noder.txt kanter.txt interessepkt.txt 100000 POI:4

# Finn nærmeste steder med både mat og drikke
java oving7 noder.txt kanter.txt interessepkt.txt 100000 POI:24
```

**Output:**
- Liste over 5 nærmeste POI-er
- Navn, node-ID, reisetid, avstand, rutelengde
- Rute (hvis ≤ 20 noder)

### 3. Endre antall landemerker (valgfritt)
```bash
java oving7 <noder> <kanter> <interessepkt> <start> <mål> <antall_landemerker>
```

**Eksempel med 16 landemerker:**
```bash
java oving7 noder.txt kanter.txt interessepkt.txt 100000 200000 16
```

Standard er 8 landemerker.

## Algoritmer

### Dijkstra
- Klassisk Dijkstra med heap-basert prioritetskø
- Stopper når målnoden plukkes ut
- Brukes også til preprosessering av landemerker

### ALT (A* med Landmarks og Triangle inequality)
1. **Preprosessering:** Velger tilfeldige landemerker og kjører Dijkstra fra/til hvert landemerke
2. **Søk:** Bruker A* med heuristikk basert på trekantulikheten
3. **Heuristikk:** `h(n) = max{|d(n,Li) - d(goal,Li)|, |d(Li,n) - d(Li,goal)|}` for alle landemerker Li

### POI-søk
- Modifisert Dijkstra som stopper når 5 POI-er av riktig type er funnet
- Filtrerer på POI-type med bitwise operasjoner
- Rekonstruerer full rute til hvert POI

## Ytelse (7.9M noder, 17.8M kanter)

**Dijkstra vs ALT:**
- Dijkstra: ~0.3-0.5 ms
- ALT: ~0.04-0.2 ms (6-16x raskere)
- Preprosessering: ~3-45 sekunder (avhengig av antall landemerker og grafstørrelse)

**POI-søk:**
- ~27-53 ms for 5 POI-er
- Stopper tidlig når 5 er funnet

## Dataformat

### noder.txt
```
<antall noder>
<id> <breddegrad> <lengdegrad>
...
```

### kanter.txt
```
<antall kanter>
<fra> <til> <reisetid> <lengde> <fartsgrense>
...
```

### interessepkt.txt
```
<antall POI-er>
<node-id>	<type>	"<navn>"
...
```

## Tips
- Bruk små datasett for testing (head -10001 noder.txt > test.txt)
- Preprosessering tar tid på store grafer - vurder færre landemerker for testing
- Node-IDer må eksistere i datasettet
- Ikke alle noder er koblet sammen (kan få "Infinity" som resultat)
