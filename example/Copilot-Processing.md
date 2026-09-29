# Munka követése

## Kérés
A `t1.php` fájlban a `nodeInfo` függvény megvalósítása: egy TreeSitter Node-ról egyetlen sorban írja ki a sort, oszlopot és típust.

## Teendők
- [x] Megnézni a meglévő példát, Node API-t és projektkonvenciókat.
- [x] Megvalósítani a Node információinak egy soros kiírását.
- [x] Ellenőrizni a módosítást diagnosztikával vagy megfelelő teszttel.

## Yekun összefoglaló
`nodeInfo()` a `getStartPoint()` sorát és oszlopát (1-től számozva), valamint a `getType()` értékét írja ki egy sorban. A PHP lint és a szerkesztődiagnosztika hibamentes. A PHP futtatókörnyezet ugyanakkor figyelmeztetett, hogy a betöltendő TreeSitter DLL buildverziója nem egyezik a PHP core verziójával.
