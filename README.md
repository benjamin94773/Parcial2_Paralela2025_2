# Parcial 2 - Computación Paralela y Distribuida
## Resolución del Puzzle 15 con Algoritmos de Búsqueda y Paralelización

## Descripción

Este proyecto implementa **10 tareas** para resolver el **15-Puzzle** usando diferentes algoritmos de búsqueda (BFS, A*) y técnicas de paralelización con **OpenMP**. Incluye:

- Implementaciones secuenciales de BFS, A*-h1, A*-h2
- Solver generalizado para puzzles NxN (3x3, 4x4, 5x5, 8x8)
- Paralelización con descomposición de datos y dominio
- Análisis comparativo exhaustivo con 42 puzzles
- Visualización de tableros y movimientos

**Estado objetivo:** `ABCDEFGHIJKLMNO#` (espacio # al final)

---

## Requisitos

### Software Necesario

```bash
- Docker con imagen japeto/parallel-tools:v64
- g++ con soporte C++17
- OpenMP 4.5+
- pthread
```

### Contenedor Docker

```bash
docker run -it -v "${PWD}:/home/japeto/app:rw" -w /home/japeto/app japeto/parallel-tools:v64 bash
```

---

## Estructura del Proyecto

```
.
├── board_printer.cpp              # Tarea 1: Visualización
├── board_moves.cpp                # Tarea 2: Ejecutar movimientos
├── board_available.cpp            # Tarea 3: Listar movimientos válidos
├── bsp_solver.cpp                 # Tarea 4: BFS
├── h1_solver.cpp                  # Tarea 5: A* con heurística h1
├── h2_solver.cpp                  # Tarea 6: A* con heurística h2
├── nxn_puzzle_solver.cpp          # Tarea 7: Solver NxN genérico
├── parallel_data_decomposition.cpp # Tarea 8: Paralelización por datos
├── tarea9_descomposicion_dominio.cpp # Tarea 9: Paralelización por dominio
├── tarea10_comparacion_algoritmos.cpp # Tarea 10: Comparación exhaustiva
├── puzzles.txt                    # 42 puzzles de prueba
├── puzzles_final.txt              # Puzzles verificados solubles
├── comparacion_algoritmos.csv     # Resultados de Tarea 10
└── README.md                      # Este archivo
```

---

## Compilación y Ejecución

### Tareas 1-3: Utilidades

#### **Tarea 1: Visualización del Tablero**

```bash
g++ board_printer.cpp -o board_printer
echo 'ABCDEFGHIJKLMNO#' | ./board_printer
```

**Salida esperada:**
```
A B C D
E F G H
I J K L
M N O #
```

#### **Tarea 2: Ejecutar Movimientos**

```bash
g++ board_moves.cpp -o board_moves
(echo 'ABCDEFGHIJKLMN#O'; echo 'LEFT') | ./board_moves
```

**Salida esperada:**
```
ABCDEFGHIJKLMNO#
```

#### **Tarea 3: Movimientos Disponibles**

```bash
g++ board_available.cpp -o board_available
echo 'ABCDEFGHIJKLMNO#' | ./board_available
```

**Salida esperada:**
```
UP
LEFT
```

---

### Tareas 4-6: Algoritmos de Búsqueda

#### **Tarea 4: BFS (Breadth-First Search)**

```bash
g++ bsp_solver.cpp -o bsp_solver
echo 'ABCDEFGHIJKLMN#O' | ./bsp_solver  # Retorna: 1
echo '#AGCEBFDIJKHMNOL' | ./bsp_solver  # Retorna: 8
echo 'AGCEB#FDIJKHMNOL' | ./bsp_solver  # Retorna: 7
```

#### **Tarea 5: A* con Misplaced Tiles (h1)**

```bash
g++ h1_solver.cpp -o h1_solver
echo 'ABCDEFGHIJKLMN#O' | ./h1_solver   # Retorna: 1
echo 'ABCDEFGHIJKL#MNO' | ./h1_solver   # Retorna: 3
echo 'EC#DBAJHIGFLMNKO' | ./h1_solver   # Retorna: 14
```

#### **Tarea 6: A* con Manhattan Distance (h2)**

```bash
g++ h2_solver.cpp -o h2_solver
echo 'ABCDEFGHIJKLMN#O' | ./h2_solver   # Retorna: 1
echo 'EABCM#GDKFILNOJH' | ./h2_solver   # Retorna: ?
```

---

### Tarea 7: Solver NxN

```bash
g++ nxn_puzzle_solver.cpp -o nxn_solver

# Puzzle 3x3
(echo '3'; echo 'ABCDEFG#H') | ./nxn_solver  # Retorna: 1

# Puzzle 4x4
(echo '4'; echo 'ABCDEFGHIJKLMN#O') | ./nxn_solver  # Retorna: 1

# Puzzle 5x5
(echo '5'; echo 'ABCDEFGHIJKLMNOPQRSTUVW#X') | ./nxn_solver  # Retorna: 1
```

**Característica clave:** Resuelve puzzles de **cualquier tamaño** NxN.

---

### Tarea 8: Descomposición de Datos

```bash
g++ -fopenmp parallel_data_decomposition.cpp -o parallel_data

# Con 2 hilos
(echo '4'; echo 'ABCDEFGHIJKL#MNO') | ./parallel_data 2

# Con 4 hilos
(echo '4'; echo 'AGCEB#FDIJKHMNOL') | ./parallel_data 4
(echo '4'; echo 'EC#DBAJHIGFLMNKO') | ./parallel_data 4

# Puzzle 5x5 con 4 hilos
(echo '5'; echo 'ABCDEFGHIJKLMNOPQRSTUV#WX') | ./parallel_data 4
```

**Estrategia:** Divide el **espacio de búsqueda** en cada nivel del BFS entre hilos.

**Resultados típicos:**
- Puzzle simple (3 pasos): Speedup ~0.73x (overhead > beneficio)
- Puzzle complejo: Mejor aprovechamiento del paralelismo

---

### Tarea 9: Descomposición de Dominio

```bash
g++ -std=c++17 -fopenmp -pthread tarea9_descomposicion_dominio.cpp -o tarea9_dominio
./tarea9_dominio
```

### Tarea 10: Comparación de Algoritmos

```bash
g++ -std=c++17 tarea10_comparacion_algoritmos.cpp -o tarea10_comparacion
cat puzzles.txt | ./tarea10_comparacion
```

**Genera:** `comparacion_algoritmos.csv` con 127 filas (42 puzzles × 3 algoritmos)

---
