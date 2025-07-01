# PROGETTO

## 1. Compilazione & Esecuzione
- Inserire all'interno della cartella stessa entrambe i file con i dati di input.
- Digitare il comando `make` da terminale: non è necessario fare operazioni di clean sui file eseguiti, in quanto già incluse nel `makefile`.
- Dopo `make` si dovranno inserire i nomi dei file `.txt` con i dati di input seguendo ciò che verrà richiesto da terminale.

## 2. Struttura
Il progetto, oltre il `makefile`, include i seguenti file:

- **main.c**  
  Vengono passati alcuni parametri e ci sono i print di tutte le componenti del circuito.

- **data.c**  
  Con relativo header `data.h`. Nel file si leggono i dati in input e si fa il parsing di ciascuno.

- **result.c**  
  Con relativo header `result.h`. Nel file si prendono i dati e si gestiscono le moltiplicazioni necessarie per ottenere l'output.

## 3. Requisiti
I numeri complessi all'interno di entrambe i file devono essere scritti in modo che la parte reale sia sempre prima di quella immaginaria.  
**es.** `i9+2` non è accettato.
