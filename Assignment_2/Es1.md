# ES 1 - Very Busy Expression

## Tabella 1

|                             | Very Busy Expression                                                     |
| --------------------------- | ------------------------------------------------------------------------ |
| **Domain**                  | Insieme delle espressioni                                                |
| **Direction**               | Backward: <br> in[b] = f<sub>b</sub>(out[b]) <br> out[b] = ⋂ in[succ(b)] |
| **Transfer function**       | f<sub>b</sub>(x) = GEN<sub>b</sub> ∪ ( x − KILL<sub>b</sub> )            |
| **Meet Operation (^)**      | ⋂                                                                        |
| **Boundary Condition**      | out[exit] = ∅                                                            |
| **Initial interior points** | out[b] = { a-b , b-a }                                                   |


## Tabella 2

|           | Iterazione 1  |
|-----------|---------------|
| exit      | in[BB8] = $\varnothing$ <br> out[BB8] = $\varnothing$           |
| BB7       | in[BB7] = (a-b) <br> out[BB7] = $\varnothing$            |
| BB6       | in[BB6] = $\varnothing$ <br> out[BB6] = (a-b)             |
| BB5       | in[BB5] = (b-a) <br> out[BB5] = $\varnothing$             |
| BB4       | in[BB4] = (a-b) <br> out[BB4] = $\varnothing$             |
| BB3       | in[BB3] = (b-a),(a-b) <br> out[BB3] = (a-b)             |
| BB2       | in[BB2] = (b-a) <br> out[BB2] = (b-a)              |
| Entry     | in[BB1] = (b-a) <br> out[BB1] = (b-a)              |