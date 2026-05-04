# ES 2 - Dominator Analysis

## Framework

|                         | Dominator Analysis                                |
|-------------------------|-----------------------------------------------------|
| **Domain** | Insieme dei nodi appartenenti al CFG |
| **Direction** | Forward: in[b]  = ⋂ out[pred(b)]; out[b] = fb(in[b]) * |
| **Transfer function** | f_b(x) = {b} ∪ x |
| **Meet Operation (^)** | ⋂ (Intersezione) |
| **Boundary Condition** | in[entry] = {entry}  |
| **Initial interior points** | in[b] = AllNodes (per b ≠ entry);    |

## Calcolo dei Dominatori – Iterazioni

### Inizializzazione
- in[A] = {A}
- in[B] = in[C] = in[D] = in[E] = in[F] = in[G] = {A,B,C,D,E,F,G}
- out[A] = out[B] = out[C] = out[D] = out[E] = out[F] = out[G] = ∅

## Iterazione 1

| Nodo | in[n] | Calcolo out[n] | out[n] |
|------|-------|----------------|--------|
| A    | {A}   | {A} ∪ {A} = {A} | {A}   |
| B    | ∩out[pred(B)] = out[A] = {A} | {B} ∪ {A} = {A,B} | {A,B} |
| C    | ∩out[pred(C )] = out[A] = {A} | {C} ∪ {A} = {A,C} | {A,C}   |
| D    | ∩out[pred(D)] = out[C] = {A,C} | {D} ∪ {A,C} = {A,C,D} | {A,C,D} |
| E    | ∩out[pred(E)] = out[C] = {A,C} | {E} ∪ {A,C} = {A,C,E} | {A,C,E} |
| F    | ∩out[pred(F)] = out[D] ∩ out[E] = {A,C,D} ∩ {A,C,E} = {A,C} | {F} ∪ {A,C} = {A,C,F} | {A,C,F}   |
| G    | ∩out[pred(G)] = out[B] ∩ out[F] = {A,B} ∩ {A,C,F} = {A} | {G} ∪ {A} = {A,G} | {A,G} |

## Iterazione 2

| Nodo | in[n] | Calcolo out[n] | out[n] |
|------|-------|----------------|--------|
| A    | {A}   | {A} ∪ {A} = {A} | {A}   |
| B    | ∩out[pred(B)] = out[A] = {A} | {B} ∪ {A} = {A,B} | {A,B}   |
| C    | ∩out[pred(C )] = out[A] = {A} | {C} ∪ {A} = {A,C} | {A,C} |
| D    | ∩out[pred(D)] = out[C] = {A,C} | {D} ∪ {A,C} = {A,C,D} | {A,C,D} |
| E    | ∩out[pred(E)] = out[C] = {A,C} | {E} ∪ {A,C} = {A,C,E} | {A,C,E} |
| F    | ∩out[pred(F)] = out[D] ∩ out[E] = {A,C,D} ∩ {A,C,E} = {A,C} | {F} ∪ {A,C} = {A,C,F} | {A,C,F} |
| G    | ∩out[pred(G)] = out[B] ∩ out[F] = {A,B} ∩ {A,C,F} = {A} | {G} ∪ {A} = {A,G} | {A,G}   |

Tutti i valori di `in[n]` e `out[n]` rimangono invariati tra l'iterazione 1 e l'iterazione 2, punto fisso raggiunto quindi l'algoritmo termina.

## Risultato Finale (Dominatori)
- Dom(A) = {A}
- Dom(B) = {A,B}
- Dom(C ) = {A,C}
- Dom(D) = {A,C,D}
- Dom(E) = {A,C,E}
- Dom(F) = {A,C,F}
- Dom(G) = {A,G}