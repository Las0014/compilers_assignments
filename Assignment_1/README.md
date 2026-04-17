# Assignment 1 – LLVM Optimization Passes

| Pass | Nome pipeline | Trasformazione |
|---|---|---|
| `AlgebraicIdentityPass` | `algebraic-identity` | `x+0→x`, `0+x→x`, `x*1→x`, `1*x→x` |
| `StrengthReductionPass` | `strength-reduction` | `x*(2ᵏ−1)→(x<<k)−x`, `udiv x/2ᵏ→x>>k` |
| `MultiInstructionOptPass` | `multi-inst-opt` | `a=x+C; b=a-C → b=x` |

---

## Struttura del progetto

```
Assignment_1/
├── Passes.h                  # Dichiarazioni dei tre pass
├── AlgebraicIdentity.cpp     # Pass 1
├── StrengthReduction.cpp     # Pass 2
├── MultiInstructionOtp.cpp   # Pass 3
├── PassPlugin.cpp            # Registrazione plugin LLVM
├── makefile
├── test/
│   └── test.c                # Sorgente C di test
└── build/                    # Generato automaticamente
    ├── Asgn1.so              # Plugin compilato
    ├── test.ll               # IR grezzo
    ├── test_clean.ll         # IR dopo mem2reg
    └── testOpt.ll            # IR dopo i pass custom
```

---

## Compilazione e build

Un singolo comando esegue l'intera pipeline:

```bash
make
```

Questo target (`all`) esegue in sequenza i quattro step seguenti.

### Step 1 — Compilazione del plugin

```bash
clang++ -fPIC -std=c++17 $(llvm-config --cxxflags) \
        -shared $(llvm-config --ldflags --libs core passes support) \
        PassPlugin.cpp AlgebraicIdentity.cpp \
        StrengthReduction.cpp MultiInstructionOtp.cpp \
        -o build/Asgn1.so
```

Produce `build/Asgn1.so`

### Step 2 — C → LLVM IR grezzo

```bash
clang -S -emit-llvm -O0 -Xclang -disable-O0-optnone \
      test/test.c -o build/test.ll
```

`-disable-O0-optnone` impedisce a LLVM di marcare le funzioni con `optnone`,
che bloccherebbe i pass successivi.

### Step 3 — IR grezzo → IR pulito (mem2reg)

```bash
opt -passes=mem2reg -S build/test.ll -o build/test_clean.ll
```

`mem2reg` promuove le variabili alloca a registri SSA, rendendo l'IR
leggibile e ottimizzabile dai pass custom.

### Step 4 — Applicazione dei pass custom

```bash
opt -load-pass-plugin build/Asgn1.so \
    -passes="algebraic-identity,strength-reduction,multi-inst-opt" \
    -S build/test_clean.ll -o build/testOpt.ll
```

I pass vengono eseguiti nell'ordine indicato nella stringa `--passes`.


---

### Risultato per ogni funzione di test

**`test_algebraic`** — tutte le operazioni neutre eliminate:
```llvm
; prima (4 istruzioni)          ; dopo
%2 = add nsw i32 %0, 0         ret i32 %0
%3 = add nsw i32 0, %2
%4 = mul nsw i32 %3, 1
%5 = mul nsw i32 1, %4
ret i32 %5
```

**`test_strength`** — moltiplicazione e divisione abbassate a shift:
```llvm
; prima                         ; dopo
%2 = mul i32 %0, 15            %shl_sr  = shl  i32 %0, 4
%3 = udiv i32 %2, 8            %mul_sr  = sub  i32 %shl_sr, %0
ret i32 %3                     %udiv_sr = lshr i32 %mul_sr, 3
                                ret i32 %udiv_sr
```

**`test_multi`** — coppia add/sub annullata:
```llvm
; prima                         ; dopo
%2 = add nsw i32 %0, 1         %2 = add nsw i32 %0, 1  ← dead (rimossa da DCE)
%3 = sub nsw i32 %2, 1         ret i32 %0
ret i32 %3
```

> **Nota:** l'istruzione `add` residua in `test_multi` è **dead code**.

---

## Eseguire un solo pass

È possibile applicare i pass singolarmente o in combinazione arbitraria:

```bash
# Solo algebraic identity
opt -load-pass-plugin build/Asgn1.so \
    -passes="algebraic-identity" \
    -S build/test_clean.ll -o /dev/stdout

# Strength reduction + multi-inst
opt -load-pass-plugin build/Asgn1.so \
    -passes="strength-reduction,multi-inst-opt" \
    -S build/test_clean.ll -o /dev/stdout
```
