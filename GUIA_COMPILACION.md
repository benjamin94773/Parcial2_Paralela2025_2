# Guía Rápida de Compilación y Ejecución

## Compilación Automática

```bash
chmod +x compile_all.sh
./compile_all.sh
```

---

## Compilación Manual por Tarea

### Tareas 1-3: Utilidades

```bash
# Tarea 1: Visualización
g++ -std=c++17 -O2 board_printer.cpp -o board_printer

# Tarea 2: Movimientos
g++ -std=c++17 -O2 board_moves.cpp -o board_moves

# Tarea 3: Movimientos Disponibles
g++ -std=c++17 -O2 board_available.cpp -o board_available
```

### Tareas 4-6: Algoritmos de Búsqueda

```bash
# Tarea 4: BFS
g++ -std=c++17 -O2 bsp_solver.cpp -o bsp_solver

# Tarea 5: A*-h1
g++ -std=c++17 -O2 h1_solver.cpp -o h1_solver

# Tarea 6: A*-h2
g++ -std=c++17 -O2 h2_solver.cpp -o h2_solver
```

### Tarea 7: NxN Solver

```bash
g++ -std=c++17 -O2 nxn_puzzle_solver.cpp -o nxn_solver
```

### Tarea 8: Descomposición de Datos

```bash
g++ -std=c++17 -O2 -fopenmp parallel_data_decomposition.cpp -o parallel_data
```

### Tarea 9: Descomposición de Dominio

```bash
g++ -std=c++17 -O2 -fopenmp -pthread tarea9_descomposicion_dominio.cpp -o tarea9_dominio
g++ -std=c++17 -O2 -fopenmp -pthread tarea9_analisis_metricas.cpp -o tarea9_metricas
```

### Tarea 10: Comparación

```bash
g++ -std=c++17 -O2 tarea10_comparacion_algoritmos.cpp -o tarea10_comparacion
```

---

## Ejecución por Tarea

### Tarea 1: Visualización

```bash
echo 'ABCDEFGHIJKLMNO#' | ./board_printer
```

**Salida esperada:**
```
A B C D
E F G H
I J K L
M N O #
```

---

### Tarea 2: Ejecutar Movimiento

```bash
(echo 'ABCDEFGHIJKLMN#O'; echo 'LEFT') | ./board_moves
```

**Salida esperada:**
```
ABCDEFGHIJKLMNO#
```

---

### Tarea 3: Listar Movimientos Disponibles

```bash
echo 'ABCDEFGHIJKLMNO#' | ./board_available
```

**Salida esperada:**
```
UP
LEFT
```

---

### Tarea 4: BFS

```bash
echo 'ABCDEFGHIJKLMN#O' | ./bsp_solver   # → 1
echo '#AGCEBFDIJKHMNOL' | ./bsp_solver   # → 8
echo 'AGCEB#FDIJKHMNOL' | ./bsp_solver   # → 7
```

---

### Tarea 5: A*-h1

```bash
echo 'ABCDEFGHIJKLMN#O' | ./h1_solver    # → 1
echo 'ABCDEFGHIJKL#MNO' | ./h1_solver    # → 3
echo 'EC#DBAJHIGFLMNKO' | ./h1_solver    # → 14
```

---

### Tarea 6: A*-h2

```bash
echo 'ABCDEFGHIJKLMN#O' | ./h2_solver    # → 1
echo 'EABCM#GDKFILNOJH' | ./h2_solver    # → ?
```

---

### Tarea 7: NxN Solver

```bash
# 3x3
(echo '3'; echo 'ABCDEFG#H') | ./nxn_solver

# 4x4
(echo '4'; echo 'ABCDEFGHIJKLMN#O') | ./nxn_solver

# 5x5
(echo '5'; echo 'ABCDEFGHIJKLMNOPQRSTUVW#X') | ./nxn_solver
```

---

### Tarea 8: Descomposición de Datos

```bash
# Con 2 hilos
(echo '4'; echo 'ABCDEFGHIJKL#MNO') | ./parallel_data 2

# Con 4 hilos - puzzle más complejo
(echo '4'; echo 'AGCEB#FDIJKHMNOL') | ./parallel_data 4
(echo '4'; echo 'EC#DBAJHIGFLMNKO') | ./parallel_data 4

# Puzzle 5x5 con 4 hilos
(echo '5'; echo 'ABCDEFGHIJKLMNOPQRSTUV#WX') | ./parallel_data 4
```

---

### Tarea 9: Descomposición de Dominio

```bash
# Programa principal con puzzle default
./tarea9_dominio

# Con puzzle específico
./tarea9_dominio EC#DBAJHIGFLMNKO

# Métricas
echo 'ABCDEFGHIJKLMN#O' | ./tarea9_metricas
```

---

### Tarea 10: Comparación de Algoritmos

```bash
cat puzzles.txt | ./tarea10_comparacion
```

**Genera:** `comparacion_algoritmos.csv`

---

## Pruebas Rápidas

### Probar todas las tareas secuencialmente

```bash
#!/bin/bash
echo "=== Tarea 1 ==="
echo 'ABCDEFGHIJKLMNO#' | ./board_printer

echo -e "\n=== Tarea 2 ==="
(echo 'ABCDEFGHIJKLMN#O'; echo 'LEFT') | ./board_moves

echo -e "\n=== Tarea 3 ==="
echo 'ABCDEFGHIJKLMNO#' | ./board_available

echo -e "\n=== Tarea 4 (BFS) ==="
echo 'ABCDEFGHIJKLMN#O' | ./bsp_solver

echo -e "\n=== Tarea 5 (A*-h1) ==="
echo 'ABCDEFGHIJKL#MNO' | ./h1_solver

echo -e "\n=== Tarea 6 (A*-h2) ==="
echo 'ABCDEFGHIJKLMN#O' | ./h2_solver

echo -e "\n=== Tarea 7 (NxN 3x3) ==="
(echo '3'; echo 'ABCDEFG#H') | ./nxn_solver

echo -e "\n=== Tarea 8 (Parallel) ==="
(echo '4'; echo 'ABCDEFGHIJKL#MNO') | ./parallel_data 2

echo -e "\n=== Tarea 9 (Dominio) ==="
./tarea9_dominio

echo -e "\n=== Tarea 10 (Comparación) ==="
cat puzzles.txt | ./tarea10_comparacion | head -50
```
