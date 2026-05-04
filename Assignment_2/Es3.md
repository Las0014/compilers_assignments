# ES 3 Constant Propagation 

## Identificazione dei Basic Block

Numeriamo i blocchi del CFG nel modo seguente (le variabili coinvolte sono `k, a, x, b, y`):

| Block | Statements |
|-------|------------|
| **BB1** | `k = 2 ; a = k+2 ; x = 5` (con if-test in coda) |
| **BB2** | `a = k*2 ; x = 8` (ramo *then*) |
| **BB3** | `k = a` (ramo *else*) |
| **BB4** | header del `while` (nessuno statement, solo merge + test) |
| **BB5** | `b = 2 ; x = a+k ; y = a*b ; k++` (corpo del loop) |
| **BB6** | `print(a+x)` |

Archi del CFG:
- BB1 → {BB2, BB3}
- {BB2, BB3} → BB4
- BB4 → {BB5, BB6}
- BB5 → BB4 (back edge)

---

## 1) Formalizzazione del framework

|  | **Constant Propagation** |
|---|---|
| **Domain** | Per ogni variabile, valore nel reticolo `{⊤, c ∈ ℤ, ⊥}` (⊤ = non ancora definito; `c` = costante; ⊥ = NAC, *Not A Constant*). Lo stato è un insieme di coppie `<var, val>`. |
| **Direction** | **Forward**: `IN[B] = ⋀ OUT[P]` per ogni `P ∈ pred(B)`; `OUT[B] = f_B(IN[B])` |
| **Transfer function** | `f_B` applica in sequenza ogni statement del blocco. Per `x = expr`: se tutti gli operandi di `expr` sono costanti → `x = eval(expr)`; se almeno uno è ⊥ → `x = ⊥`; gli operatori `+ - * /` su due costanti vengono valutati. |
| **Meet operation (∧)** | Variable-wise: `⊤ ∧ v = v` ; `⊥ ∧ v = ⊥` ; `c₁ ∧ c₂ = c₁` se `c₁ = c₂`, altrimenti `⊥` |
| **Boundary condition** | `IN[ENTRY] = ⊤` (tutte le variabili indefinite) |
| **Initial interior points** | `OUT[B] = ⊤` ∀ `B ≠ ENTRY` (inizializzazione ottimistica) |

---

## 2) Tabella delle iterazioni

| | **Iterazione 1** | | **Iterazione 2** | | **Iterazione 3** | |
|---|---|---|---|---|---|---|
| | **IN[B]** | **OUT[B]** | **IN[B]** | **OUT[B]** | **IN[B]** | **OUT[B]** |
| **BB1** | ∅ | {a=4, k=2, x=5} | ∅ | {a=4, k=2, x=5} | ∅ | {a=4, k=2, x=5} |
| **BB2** | {a=4, k=2, x=5} | {a=4, k=2, x=8} | {a=4, k=2, x=5} | {a=4, k=2, x=8} | {a=4, k=2, x=5} | {a=4, k=2, x=8} |
| **BB3** | {a=4, k=2, x=5} | {a=4, k=4, x=5} | {a=4, k=2, x=5} | {a=4, k=4, x=5} | {a=4, k=2, x=5} | {a=4, k=4, x=5} |
| **BB4** | {a=4, k=⊥, x=⊥} | {a=4, k=⊥, x=⊥} | {a=4, b=2, k=⊥, x=⊥, y=8} | {a=4, b=2, k=⊥, x=⊥, y=8} | {a=4, b=2, k=⊥, x=⊥, y=8} | {a=4, b=2, k=⊥, x=⊥, y=8} |
| **BB5** | {a=4, k=⊥, x=⊥} | {a=4, b=2, k=⊥, x=⊥, y=8} | {a=4, b=2, k=⊥, x=⊥, y=8} | {a=4, b=2, k=⊥, x=⊥, y=8} | {a=4, b=2, k=⊥, x=⊥, y=8} | {a=4, b=2, k=⊥, x=⊥, y=8} |
| **BB6** | {a=4, k=⊥, x=⊥} | {a=4, k=⊥, x=⊥} | {a=4, b=2, k=⊥, x=⊥, y=8} | {a=4, b=2, k=⊥, x=⊥, y=8} | {a=4, b=2, k=⊥, x=⊥, y=8} | {a=4, b=2, k=⊥, x=⊥, y=8} |

L'algoritmo converge alla **terza iterazione** (uguale alla seconda → punto fisso raggiunto).

---

## Dettaglio del calcolo

### BB1 (Iterazione 1)
- `k = 2` → `k` costante
- `a = k + 2 = 2 + 2 = 4` (entrambi gli operandi noti)
- `x = 5` → `x` costante
- **OUT[BB1] = {k=2, a=4, x=5}**

### BB2
Si entra con `IN = {k=2, a=4, x=5}`:
- `a = k * 2 = 2 * 2 = 4`
- `x = 8`
- **OUT[BB2] = {k=2, a=4, x=8}**

### BB3
Si entra con `IN = {k=2, a=4, x=5}`:
- `k = a = 4`
- **OUT[BB3] = {k=4, a=4, x=5}**

### BB4 (Iterazione 1)
Meet dei predecessori `OUT[BB2]` e `OUT[BB3]` (BB5 ancora `⊤`):
- `k`: `2 ∧ 4 = ⊥`
- `a`: `4 ∧ 4 = 4` 
- `x`: `8 ∧ 5 = ⊥`
- **IN[BB4] = OUT[BB4] = {a=4, k=⊥, x=⊥}**

### BB5 (Iterazione 1)
Con `IN = {a=4, k=⊥, x=⊥}`:
- `b = 2` → `b` costante
- `x = a + k = 4 + ⊥ = ⊥`
- `y = a * b = 4 * 2 = 8` 
- `k++` con `k=⊥` resta `⊥`
- **OUT[BB5] = {a=4, b=2, k=⊥, x=⊥, y=8}**

### BB4 (Iterazione 2)
Ora `OUT[BB5]` propaga indietro `b=2, y=8`. Poiché BB2 e BB3 non definiscono `b` né `y` (rimangono a `⊤`), il meet dà:
- `b`: `⊤ ∧ ⊤ ∧ 2 = 2`
- `y`: `⊤ ∧ ⊤ ∧ 8 = 8`
- **IN[BB4] = {a=4, b=2, k=⊥, x=⊥, y=8}**

Le iterazioni successive non producono cambiamenti → punto fisso.

---

## Ottimizzazioni abilitate

L'analisi consente di sostituire con costanti i seguenti statement:

| Statement originale | Ottimizzazione |
|---|---|
| `a = k+2` (BB1) | `a = 4` |
| `a = k*2` (BB2) | `a = 4` |
| `k = a` (BB3) | `k = 4` |
| `y = a*b` (BB5) | `y = 8` |

### Statement NON ottimizzabili

- **`x = a+k`** in BB5 **non** è ottimizzabile perché `k` è NAC (viene incrementato dentro al loop, quindi assume valori diversi a ogni iterazione).
- **`print(a+x)`** in BB6 non può essere ridotta a una costante: `a=4` è noto, ma `x=⊥` perché in ingresso a BB4 le due definizioni di `x` (`x=8` da BB2 e `x=5` da BB3) sono incompatibili, e in BB5 `x` viene ridefinito con un valore non costante.