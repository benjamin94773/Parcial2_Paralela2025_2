# 🧩 Parcial 2 - Computación Paralela y Distribuida
## Resolución del Puzzle 15 con Algoritmos de Búsqueda y Paralelización

[![C++17](https://img.shields.io/badge/C++-17-blue.svg)](https://isocpp.org/)
[![OpenMP](https://img.shields.io/badge/OpenMP-4.5-green.svg)](https://www.openmp.org/)
[![Docker](https://img.shields.io/badge/Docker-Ready-2496ED.svg)](https://www.docker.com/)
[![License](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

---

## 📋 Tabla de Contenidos

- [Descripción](#-descripción)
- [Requisitos](#-requisitos)
- [Estructura del Proyecto](#-estructura-del-proyecto)
- [Compilación y Ejecución](#-compilación-y-ejecución)
  - [Tareas 1-3: Utilidades](#tareas-1-3-utilidades)
  - [Tareas 4-6: Algoritmos de Búsqueda](#tareas-4-6-algoritmos-de-búsqueda)
  - [Tarea 7: Solver NxN](#tarea-7-solver-nxn)
  - [Tarea 8: Descomposición de Datos](#tarea-8-descomposición-de-datos)
  - [Tarea 9: Descomposición de Dominio](#tarea-9-descomposición-de-dominio)
  - [Tarea 10: Comparación de Algoritmos](#tarea-10-comparación-de-algoritmos)
- [Resultados](#-resultados)
- [Análisis de Desempeño](#-análisis-de-desempeño)
- [Documentación](#-documentación)
- [Autor](#-autor)

---

## 🎯 Descripción

Este proyecto implementa **10 tareas** para resolver el **15-Puzzle** usando diferentes algoritmos de búsqueda (BFS, A*) y técnicas de paralelización con **OpenMP**. Incluye:

- ✅ Implementaciones secuenciales de BFS, A*-h1, A*-h2
- ✅ Solver generalizado para puzzles NxN (3x3, 4x4, 5x5, 8x8)
- ✅ Paralelización con descomposición de datos y dominio
- ✅ Análisis comparativo exhaustivo con 42 puzzles
- ✅ Visualización de tableros y movimientos

**Estado objetivo:** `ABCDEFGHIJKLMNO#` (espacio # al final)

---

## 🔧 Requisitos

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

## 📁 Estructura del Proyecto

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

## 🚀 Compilación y Ejecución

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

**Entrada opcional:** Puzzle por stdin (default: `ABGDEFHCIJKLMNO#`)

**Estrategia:** Múltiples hilos colaboran en **UN solo puzzle** con cola compartida thread-safe.

**Resultados típicos:**
| Hilos | Speedup | Eficiencia |
|-------|---------|------------|
| 1     | 1.00x   | 100%       |
| 2     | 0.73x   | 36.6%      |
| 4     | 1.05x   | 26.3%      |
| 8     | 0.19x   | 2.4%       |

**Observación:** Alto overhead de sincronización limita la escalabilidad.

---

### Tarea 10: Comparación de Algoritmos

```bash
g++ -std=c++17 tarea10_comparacion_algoritmos.cpp -o tarea10_comparacion
cat puzzles.txt | ./tarea10_comparacion
```

**Genera:** `comparacion_algoritmos.csv` con 127 filas (42 puzzles × 3 algoritmos)

---

## 📊 Resultados

### Comparación de Algoritmos (42 Puzzles)

| Algoritmo | Resueltos | Nodos Expandidos (promedio) | Longitud Solución (promedio) | Tiempo (ms) promedio |
|-----------|-----------|----------------------------|------------------------------|----------------------|
| **A*-h2** | **42/42 (100%)** | **29.6** | **7.8** | **0.059** ⚡ |
| **A*-h1** | **42/42 (100%)** | **203.2** | **7.8** | **8.606** |
| **BFS** | **40/42 (95%)** | **36546.6** | **7.1** | **85.324** |

### Observaciones Clave

1. **A*-h2 (Manhattan Distance)** es el claro ganador:
   - ✅ Resuelve **100%** de los puzzles
   - ✅ Expande **1235x menos nodos** que BFS
   - ✅ Es **1444x más rápido** que BFS

2. **A*-h1 (Misplaced Tiles)**:
   - ✅ Resuelve todos los puzzles
   - ⚠️ 7x más nodos que A*-h2
   - ⚠️ 145x más lento que A*-h2

3. **BFS**:
   - ❌ Falla en 2/42 puzzles (alcanza límite de 500K nodos)
   - ❌ Muy ineficiente en puzzles de 16+ pasos

### Casos Extremos

**Puzzle más difícil resuelto:** `AFCGIEBD#NKHJMOL` (22 pasos)
- BFS: ❌ No resuelto (500K nodos)
- A*-h1: 6099 nodos, 13.3 ms
- A*-h2: 560 nodos, 1.08 ms ✅

**Puzzle más fácil:** `ABCDEFGHIJKLMN#O` (1 paso)
- BFS: 4 nodos, 0.005 ms
- A*-h1: 2 nodos, 0.003 ms
- A*-h2: 2 nodos, 0.002 ms ✅

---

## 🔬 Análisis de Desempeño

### Solvabilidad del 15-Puzzle

**Regla para tablero 4x4:**
```
(inversiones + fila_del_espacio) % 2 == 1  →  Solvable
```

Solo **50% de las permutaciones** son solubles.

### Complejidad Computacional

| Algoritmo | Complejidad Temporal | Complejidad Espacial | Optimalidad |
|-----------|----------------------|----------------------|-------------|
| BFS       | O(b^d)               | O(b^d)               | ✅ Garantizada |
| A*-h1     | O(b^d)               | O(b^d)               | ✅ Si h admisible |
| A*-h2     | O(b^d)               | O(b^d)               | ✅ Si h admisible |

- **b** = factor de ramificación (~2-4 para 15-puzzle)
- **d** = profundidad de la solución

### Eficiencia de Heurísticas

**h2 (Manhattan) domina h1 (Misplaced):**
- h2(n) ≥ h1(n) para todo estado n
- h2 es más informativa → expande menos nodos

---

## 📚 Documentación

### Archivos de Documentación

- **README.md** - Este archivo (guía completa)
- **INFORME_PARCIAL2.tex** - Informe académico en LaTeX
- **GUIA_VIDEO.md** - Script para video de demostración
- **RESULTADOS_EJECUCION.md** - Outputs de todas las ejecuciones
- **INSTRUCCIONES_FINALES.md** - Pasos de entrega
- **RESUMEN_EJECUTIVO.md** - Resumen del proyecto
- **CHECKLIST_ENTREGA.md** - Checklist de entrega

### Scripts de Utilidad

```bash
# Compilar todo
./compile_all.sh

# Probar todas las tareas
./test_all.sh
```

---

## 🎓 Conceptos Aprendidos

### 1. Algoritmos de Búsqueda
- **BFS:** Garantiza solución óptima pero es ineficiente
- **A*:** Óptimo y eficiente con heurística admisible
- **Heurísticas:** h2 (Manhattan) superior a h1 (Misplaced)

### 2. Paralelización
- **Descomposición de Datos:** Dividir el espacio de búsqueda por niveles
- **Descomposición de Dominio:** Hilos colaboran en cola compartida
- **Trade-off:** Overhead de sincronización vs ganancia paralela

### 3. Optimización
- **Detección temprana:** Verificar solvabilidad antes de buscar
- **Estructuras eficientes:** `unordered_set` para visitados
- **Límites:** Evitar explosión en espacio de búsqueda

---

## 👨‍💻 Autor

**Benjamin Rodriguez**
- 📧 Email: [tu-email@ejemplo.com]
- 🔗 GitHub: [@benjamin94773](https://github.com/benjamin94773)
- 📂 Repositorio: [Parcial2_Paralela2025_2](https://github.com/benjamin94773/Parcial2_Paralela2025_2)

---

## 📝 Licencia

Este proyecto es parte del curso de Computación Paralela y Distribuida.

---

## 🙏 Agradecimientos

- Profesor del curso por las especificaciones claras
- Comunidad de OpenMP por la documentación
- Docker por facilitar el entorno reproducible

---

**Fecha de entrega:** Octubre 2025  
**Curso:** Computación Paralela y Distribuida  
**Institución:** [Tu Universidad]

---

## 📌 Notas Adicionales

### Puzzles de Prueba

- **puzzles.txt**: 42 puzzles variados (algunos no solubles)
- **puzzles_final.txt**: 3 puzzles verificados solubles (1, 3, 30 pasos)

### Limitaciones Conocidas

- **nxn_solver**: Limitado a puzzles simples/medios (timeout en complejos)
- **Tarea 8**: Overhead alto en puzzles pequeños
- **Tarea 9**: Escalabilidad limitada por contención en estructuras compartidas

### Recomendaciones

- Usar A*-h2 para puzzles complejos
- BFS solo para puzzles muy simples
- Paralelización efectiva solo en puzzles con gran factor de ramificación
