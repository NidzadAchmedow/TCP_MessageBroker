# Prüfungsaufgabe - Message Broker

## Vorbereitung auf Prüfungsaufgabe

Momentan dient dieses Repo zur hypothetischen Vorbereitung 
für die kommende TCP/IP Aufgabe zum Message Broker.

<br>

## Umsetzung

**Subscriber:**
- Kann ein Topic vom Broker anfragen und bekommt eine oder mehrere Antworten
  - Topic kann mehrere Wörter enthalten
  
<br>

- Eingaben um Subscriber erfolgreich zu benutzen :
  - Direkt als Kommando
  ```
  ./sub hostname topic
  ```
  - Start im Client-Modus um mehrere Anfragen zu bearbeiten
  ```
  ./sub hostname
  ```
- Wildcard bietet Möglichkeit jedes Topic abzurufen :
```
./sub hostname "#"
```
oder
```
./sub hostname -> sub -> #
```

**Publisher:**
- Kann dem Broker unter einem bestimmten Topic eine Message übermitteln
  - Topic sowie Message können mehrere Wörter enthalten
  
<br>

- Eingaben um Publisher erfolgreich zu benutzen :
  - Direkt als Kommando
  ```
  ./pub hostname topic "<" message
  ```
  - Start im Client-Modus um mehrere Anfragen zu bearbeiten
  ```
  ./pub hostname
  ```

<br>

**Broker:**
- verwaltet die Topics, Messages und Subscriber in Files
  - damit ist der Broker *statisch*
- Server bekommt Nachrichten von Publisher und Subscriber
  - Publisher: `PUB TOPIC <MSG`
  - Subscriber: `SUB TOPIC` 
- Nachrichten sind durch den Präfix mit `PUB` und `SUB` eindeutig zuzuordnen
  - Inhalt der Nachrichten kann dann in jeweiliges File eingetragen werden

<br>

<u>Struktur der Datei - Topic und Message:</u>
> Filename: Topic.txt

    [Topic] [Message]
    [Topic] [Message]
    [Topic] [Message]
    ...

- strukturell ist es nach einem `[Schlüssel : Wert]` Prinzip angeordnet
- damit ist es möglich gezielt nach einem jeweiligen Topic zu suchen und beim Auffinden die dazugehörige Message zu entnehmen

<br>

<u>Struktur der Datei - Subscriber:</u>
> Filename: Subscriber.txt

    [Topic] [Subscriber]
    [Topic] [Subscriber]
    [Topic] [Subscriber]
    ...

- gleiches Prinzip wie Topic, `[Schlüssel] : [Wert]`
- Subscriber fragt Broker nach jeweiliger Topic

<br>

* * *




### Library-Files

`LibMB.h`: *(Library Message Broker)*
- Header-File, Übersicht zu den enthaltenen Methoden und Konstanten

`LibSrc.c`: *(Library Source)*
- Implementierung der im Header-File enthaltenen Methoden

<br>

* * *

## Ausführungshinweis vom Programm

Terminal-Befehle:

> Object File von Lib-Implementierung:

```
gcc LibSrc.c -c
```

> Kompilierung Broker:

```
gcc -o bro smbBroker.c LibSrc.o -lm -fopenmp
```

> Kompilierung Subscriber:

```
gcc -o sub smbSubscriber.c LibSrc.o -lm -fopenmp
```

> Kompilierung Publisher:

```
gcc -o pub smbPublisher.c LibSrc.o -lm -fopenmp
```
