# Prüfungsaufgabe - Message Broker

## Vorbereitung auf Prüfungsaufgabe

Momentan dient dieses Repo zur hypothetischen Vorbereitung 
für die kommende TCP/IP Aufgabe zum Message Broker.

## Vorstellung / Umsetzung

**Server:**
- verwaltet die Topics, Messages und Subscriber in Files
  - damit ist der Broker *statisch*
- Server bekommt Nachrichten von Publisher und Subscriber
  - Publisher: `[PUB TOPIC MSG]`
  - Subscriber: `[SUB TOPIC ADDR]` 
- Nachrichten sind durch den Head mit `PUB` und `SUB` eindeutig zuzuordnen
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

## Erstellen einer Library

### Nützliche Methoden

- [x] Splitten von Messages
- [x] Senden und Empfangen von Nachrichten (Client-Seitig)
- [ ] Senden und Empfangen von Nachrichten (Server-Seitig) - könnte evtl. zu umständlich sein
- [x] Beschreiben von Dateien (ohne zu löschen, wie Concat)
- [x] Lesen aus Dateien und speichern in Array (für Wildcard-Anforderung #)
- [x] Suchen nach Topic in Datei und Ausgabe von Topic
- [x] Bauen von Nachrichten -> PUB/ SUB
- [ ] Lesen von Verbindungsadresse (Adresse von Subscriber)

<br>

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

> Kompilierung Server:

```
gcc -o server udpServer.c LibSrc.o -lm -fopenmp
```

> Kompilierung Client:

```
gcc -o client udpClient.c LibSrc.o -lm -fopenmp
```
