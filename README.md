# 15-Puzzle Solver - Parcial 2 Computación Paralela

**Autor:** Benjamin Japeto  
**Repositorio:** https://github.com/benjamin94773/Parcial2_Paralela2025_2.git  
**Curso:** Computación Paralela - 2025

---

## 📋 Descripción

Este proyecto implementa y compara algoritmos de búsqueda para resolver el 15-puzzle, incluyendo estrategias de paralelización con OpenMP.

**Algoritmos implementados:**
- BFS (Breadth-First Search)
- A* con heurística h1 (fichas descolocadas)
- A* con heurística h2 (distancia Manhattan)

**Estrategias de paralelización:**
- Descomposición de datos (múltiples puzzles en paralelo)
- Descomposición de dominio (múltiples hilos en un puzzle)

---

## 🗂️ Estructura del Proyecto

```
.
├── README.md                              # Este archivo
│
├── Código Fuente (12 archivos)
│   ├── board_printer.cpp                 # Tarea 1: Imprime tablero
│   ├── board_moves.cpp                   # Tarea 2: Genera movimientos
│   ├── board_available.cpp               # Tarea 3: Verifica movimientos
│   ├── bsp_solver.cpp                    # Tarea 4: BFS
│   ├── h1_solver.cpp                     # Tarea 5: A* con h1
│   ├── h2_solver.cpp                     # Tarea 6: A* con h2
│   ├── nxn_puzzle_solver.cpp             # Tarea 7: Soporte 3x3 a 6x6
│   ├── parallel_data_decomposition.cpp   # Tarea 8: Paralelización por datos
│   ├── tarea9_descomposicion_dominio.cpp # Tarea 9: Paralelización por dominio
│   ├── tarea9_analisis_metricas.cpp      # Tarea 9: Análisis detallado
│   ├── tarea10_comparacion_algoritmos.cpp# Tarea 10: Comparación
│   └── parallel_bfs_solver.cpp           # Adicional
│
├── Datos de Prueba (5 archivos)
│   ├── puzzles.txt                       # Puzzles generales
│   ├── puzzles_easy.txt                  # 20 puzzles fáciles (5-10 movs)
│   ├── puzzles_medium.txt                # 20 puzzles medianos (10-15 movs)
│   ├── puzzles_hard.txt                  # 50 puzzles difíciles (15-30 movs)
│   └── puzzles_extreme.txt               # 10 puzzles extremos (30-50 movs)
│
└── Scripts de Automatización
    ├── compile_all.sh                    # Compila todos los archivos
    └── test_all.sh                       # Ejecuta pruebas de validación
```

---

## 🔧 Requisitos

- **Compilador:** GCC con soporte C++17
- **Paralelización:** OpenMP 4.5+
- **Sistema:** Linux/Unix (o Docker)
- **Memoria:** Mínimo 2GB RAM

---

## 📦 Compilación

### Compilar todo automáticamente:
```bash
bash compile_all.sh
```

### Compilar manualmente:

#### Tareas 1-3: Utilidades
```bash
g++ -std=c++17 -O2 board_printer.cpp -o board_printer
g++ -std=c++17 -O2 board_moves.cpp -o board_moves
g++ -std=c++17 -O2 board_available.cpp -o board_available
```

#### Tareas 4-6: Algoritmos Secuenciales
```bash
g++ -std=c++17 -O2 bsp_solver.cpp -o bsp_solver
g++ -std=c++17 -O2 h1_solver.cpp -o h1_solver
g++ -std=c++17 -O2 h2_solver.cpp -o h2_solver
```

#### Tarea 7: Soporte NxN
```bash
g++ -std=c++17 -O2 nxn_puzzle_solver.cpp -o nxn_solver
```

#### Tarea 8: Paralelización por Datos (OpenMP)
```bash
g++ -std=c++17 -O2 -fopenmp parallel_data_decomposition.cpp -o parallel_data
```

#### Tarea 9: Paralelización por Dominio (OpenMP)
```bash
g++ -std=c++17 -O2 -fopenmp tarea9_descomposicion_dominio.cpp -o tarea9_dominio
g++ -std=c++17 -O2 -fopenmp tarea9_analisis_metricas.cpp -o tarea9_metricas
```

#### Tarea 10: Comparación de Algoritmos
```bash
g++ -std=c++17 -O2 tarea10_comparacion_algoritmos.cpp -o tarea10_comparacion
```

---

## 🚀 Ejecución

### Formato de Entrada
Los puzzles se representan como cadenas:
- **4x4:** 16 caracteres (A-O + #)
- **3x3:** 9 caracteres (A-H + #)
- Ejemplo ordenado 4x4: `ABCDEFGHIJKLMNO#`

### Ejemplos de Uso

#### 1. Algoritmos Secuenciales

```bash
# BFS - Garantiza solución óptima
echo 'ABGD#FHCEIJKLMNO' | ./bsp_solver

# A* con heurística h1 (fichas descolocadas)
echo 'ABGD#FHCEIJKLMNO' | ./h1_solver

# A* con heurística h2 (distancia Manhattan) - MÁS EFICIENTE
echo 'ABGD#FHCEIJKLMNO' | ./h2_solver
```

#### 2. Soporte NxN

```bash
# Puzzle 3x3
echo 'ABC#DEFGH' | ./nxn_solver

# Puzzle 4x4
echo 'ABCDEFGHIJKLMNO#' | ./nxn_solver

# Puzzle 5x5
echo 'ABCDEFGHIJKLMNOPQRSTUVWX#' | ./nxn_solver
```

#### 3. Tarea 8: Descomposición de Datos

Procesa múltiples puzzles en paralelo:

```bash
# Secuencial (baseline)
./parallel_data puzzles_hard.txt 1

# Con 2 hilos
./parallel_data puzzles_hard.txt 2

# Con 4 hilos
./parallel_data puzzles_hard.txt 4

# Con 8 hilos (mejor speedup)
./parallel_data puzzles_hard.txt 8
```

#### 4. Tarea 9: Descomposición de Dominio

Un puzzle resuelto por múltiples hilos:

```bash
# Secuencial
echo 'ABGDEFHCIJKLMNO#' | ./tarea9_dominio 1

# Con 2 hilos
echo 'ABGDEFHCIJKLMNO#' | ./tarea9_dominio 2

# Con 4 hilos
echo 'ABGDEFHCIJKLMNO#' | ./tarea9_dominio 4

# Con 8 hilos
echo 'ABGDEFHCIJKLMNO#' | ./tarea9_dominio 8

# Análisis detallado con métricas
echo 'ABGDEFHCIJKLMNO#' | ./tarea9_metricas 8
```

#### 5. Tarea 10: Comparación de Algoritmos

```bash
# Comparar BFS vs A*-h1 vs A*-h2 en 20 puzzles
./tarea10_comparacion puzzles_medium.txt
```

---

## 🧪 Pruebas de Validación

### Ejecutar todas las pruebas:
```bash
bash test_all.sh
```

### Casos de Prueba del PDF

```bash
# Caso BFS (debe dar 8 pasos)
echo 'ABGD#FHCEIJKLMNO' | ./bsp_solver

# Caso A*-h1
echo 'HAGDEFBCIJKLMNO#' | ./h1_solver

# Caso A*-h2
echo 'HGFEDCBA#ONMLKJI' | ./h2_solver
```

---

## 📊 Experimentos Sugeridos

### Experimento 1: Comparar eficiencia de algoritmos
```bash
./tarea10_comparacion puzzles_medium.txt
```

### Experimento 2: Evaluar speedup con paralelización de datos
```bash
for threads in 1 2 4 8; do
    echo "=== $threads hilos ==="
    ./parallel_data puzzles_hard.txt $threads
done
```

### Experimento 3: Analizar paralelización de dominio
```bash
for threads in 1 2 4 8; do
    echo "=== $threads hilos ==="
    echo 'ABGDEFHCIJKLMNO#' | ./tarea9_dominio $threads
done
```

### Experimento 4: Probar escalabilidad por tamaño
```bash
# 3x3
echo 'ABC#DEFGH' | ./nxn_solver

# 4x4
echo 'ABCDEFGHIJKLMNO#' | ./nxn_solver

# 5x5
echo 'ABCDEFGHIJKLMNOPQRSTUVWX#' | ./nxn_solver
```

---

## 📚 Archivos de Datos

- **puzzles_easy.txt:** Puzzles rápidos para pruebas (5-10 movimientos)
- **puzzles_medium.txt:** Incluye casos del PDF (10-15 movimientos)
- **puzzles_hard.txt:** Para evaluar Tarea 8 (15-30 movimientos)
- **puzzles_extreme.txt:** Puzzles muy difíciles (30-50 movimientos)

---

## 👤 Autor

**Benjamin Japeto**  
Computación Paralela - 2025  
Repositorio: https://github.com/benjamin94773/Parcial2_Paralela2025_2.git

---

## 📝 Notas

- Todos los algoritmos garantizan encontrar la solución óptima
- La paralelización por datos funciona bien (speedup ~2x con 8 hilos)
- La paralelización por dominio tiene limitaciones debido al overhead de sincronización
- A*-h2 es significativamente más eficiente que BFS en todos los casos
