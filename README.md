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
- [x] Senden und Empfangen von Nachrichten
- [ ] Beschreiben von Dateien (ohne zu löschen, wie Concat)
- [ ] Lesen aus Dateien
- [ ] Suchen in Dateien
- [ ] Bauen von Nachrichten -> PUB/ SUB

<br>

### Library-Files

`LibMB.h`: *(Library Message Broker)*
- Header-File, Übersicht zu den enthaltenen Methoden und Konstanten

`LibSrc.c`: *(Library Source)*
- Implementierung der im Header-File enthaltenen Methoden