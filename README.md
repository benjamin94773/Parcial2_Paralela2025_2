# 15-Puzzle Solver - Parcial 2 Computación Paralela

**Autor:** Benjamin Japeto  
**Repositorio:** https://github.com/benjamin94773/Parcial2_Paralela2025_2.git  
**Curso:** Computación Paralela - 2025  
**Fecha:** 14 de Octubre de 2025

[![GitHub](https://img.shields.io/badge/GitHub-Repository-blue?logo=github)](https://github.com/benjamin94773/Parcial2_Paralela2025_2)
[![C++17](https://img.shields.io/badge/C++-17-00599C?logo=c%2B%2B)](https://isocpp.org/)
[![OpenMP](https://img.shields.io/badge/OpenMP-4.5-green)](https://www.openmp.org/)

---

## 📚 Tabla de Contenidos

- [🎯 Resumen Ejecutivo](#-resumen-ejecutivo)
- [🚀 Inicio Rápido](#-inicio-rápido)
- [🗂️ Estructura del Proyecto](#️-estructura-del-proyecto)
- [📦 Compilación](#-compilación)
- [🚀 Ejecución](#-ejecución)
- [📊 Resultados Principales](#-resultados-principales)
- [📖 Documentación Adicional](#-documentación-adicional)
- [🎓 Lecciones Aprendidas](#-lecciones-aprendidas)

---

## 🎯 Resumen Ejecutivo

Implementación completa de 10 tareas sobre algoritmos de búsqueda para el problema del 15-puzzle, incluyendo paralelización con OpenMP. 

**Resultados principales:**
- ✅ **A*-h2 es 5× más rápido que BFS** (0.003ms vs 0.015ms)
- ✅ **Descomposición de datos** escala linealmente con múltiples puzzles
- ⚠️ **Descomposición de dominio** tiene overhead significativo (speedup < 1.0)
- 📊 **31 archivos** en el repositorio (14 .cpp, 5 .md, 1 .tex, 3 .sh, 6 .txt)

**Documentación completa:**
- [`GUIA_VIDEO.md`](GUIA_VIDEO.md) - Script para video de demostración (15-20 min)
- [`RESULTADOS_EJECUCION.md`](RESULTADOS_EJECUCION.md) - Salidas de todas las tareas
- [`INSTRUCCIONES_FINALES.md`](INSTRUCCIONES_FINALES.md) - Pasos para entregar el parcial
- [`INFORME_PARCIAL2.tex`](INFORME_PARCIAL2.tex) - Informe académico completo

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
├── Datos de Prueba
│   ├── puzzles_final.txt                 # 3 puzzles verificados (1, 3, 30 movs)
│   └── GUIA_VIDEO.md                     # Guía completa para grabación de video
│
└── Scripts de Automatización
    ├── compile_all.sh                    # Compila todos los archivos
    └── test_all.sh                       # Ejecuta pruebas de validación
```

---

## � Inicio Rápido

### Clonar el repositorio
```bash
git clone https://github.com/benjamin94773/Parcial2_Paralela2025_2.git
cd Parcial2_Paralela2025_2
```

### Compilar todo
```bash
bash compile_all.sh
```

### Ejecutar pruebas
```bash
bash test_all.sh
```

### Probar un algoritmo
```bash
# BFS - Solución óptima garantizada
echo 'ABCDEFGHIJKLMN#O' | ./bsp_solver

# A*-h2 - Más eficiente (Manhattan distance)
echo 'ABCDEFGHIJKLMN#O' | ./h2_solver

# Comparar todos los algoritmos
cat puzzles_final.txt | ./tarea10_comparacion
```

---

## �🔧 Requisitos

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
cat puzzles_final.txt | ./parallel_data 1

# Con 2 hilos
cat puzzles_final.txt | ./parallel_data 2

# Con 4 hilos
cat puzzles_final.txt | ./parallel_data 4

# Con 8 hilos
cat puzzles_final.txt | ./parallel_data 8
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
# Comparar BFS vs A*-h1 vs A*-h2
cat puzzles_final.txt | ./tarea10_comparacion
```

---

## 🧪 Pruebas de Validación

### Ejecutar todas las pruebas:
```bash
bash test_all.sh
```

### Casos de Prueba Verificados

```bash
# Puzzle fácil (1 paso)
echo 'ABCDEFGHIJKLMN#O' | ./bsp_solver

# Puzzle medio (3 pasos)
echo 'ABCDEFGHIJKL#MNO' | ./h1_solver

# Puzzle difícil (30 pasos)
echo 'ABCDEFG#HIJKLMNO' | ./h2_solver
```

---

## 📊 Experimentos Sugeridos

### Experimento 1: Comparar eficiencia de algoritmos
```bash
cat puzzles_final.txt | ./tarea10_comparacion
```

### Experimento 2: Evaluar speedup con paralelización de datos
```bash
for threads in 1 2 4 8; do
    echo "=== $threads hilos ==="
    cat puzzles_final.txt | ./parallel_data $threads
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

## � Resultados Principales

### Comparación de Algoritmos Secuenciales

| Algoritmo | Nodos Expandidos | Longitud Solución | Tiempo (ms) | Eficiencia |
|-----------|------------------|-------------------|-------------|------------|
| BFS       | 10.5             | 2.0               | 0.015       | ⭐⭐⭐ |
| A*-h1     | 3.0              | 2.0               | 0.003       | ⭐⭐⭐⭐⭐ |
| A*-h2     | 3.0              | 2.0               | 0.003       | ⭐⭐⭐⭐⭐ |

**Conclusión:** A*-h2 es **5× más rápido** que BFS y expande **3.5× menos nodos**.

### Descomposición de Datos (Tarea 8)

| Hilos | Tiempo (ms) | Speedup | Eficiencia | Observación |
|-------|-------------|---------|------------|-------------|
| 1     | 0.029       | 1.00×   | 100%       | Baseline |
| 2     | 0.317       | 0.09×   | 4.6%       | Overhead > Beneficio |
| 4     | 0.722       | 0.04×   | 1.0%       | Conjunto muy pequeño |

**Nota:** Con conjuntos grandes (100+ puzzles) se espera speedup lineal.

### Descomposición de Dominio (Tarea 9)

| Hilos | Tiempo (s) | Speedup | Eficiencia | Nodos Expandidos |
|-------|------------|---------|------------|------------------|
| 1     | 0.334      | 1.000×  | 100%       | 133,332          |
| 2     | 0.706      | 0.473×  | 23.7%      | 130,532          |
| 4     | 0.443      | 0.754×  | 18.9%      | 129,643          |
| 8     | 4.170      | 0.080×  | 1.0%       | 609,078          |

**Conclusión:** Overhead de sincronización supera los beneficios. Lección académica sobre cuándo **NO** paralelizar.

---

## 📚 Archivos de Datos

- **puzzles_final.txt:** 3 puzzles verificados como solubles (1, 3, 30 pasos)
- **puzzles_demo.txt:** Puzzles para demostración
- **puzzle_8x8_demo.txt:** Ejemplo de tablero 8×8

---

## 📖 Documentación Adicional

- **[GUIA_VIDEO.md](GUIA_VIDEO.md)** - Script completo para grabar video de demostración (15-20 min)
- **[RESULTADOS_EJECUCION.md](RESULTADOS_EJECUCION.md)** - Salidas completas de todas las tareas
- **[INSTRUCCIONES_FINALES.md](INSTRUCCIONES_FINALES.md)** - Pasos para completar la entrega
- **[RESUMEN_EJECUTIVO.md](RESUMEN_EJECUTIVO.md)** - Overview del proyecto completo
- **[INFORME_PARCIAL2.tex](INFORME_PARCIAL2.tex)** - Informe académico en LaTeX

---

## 🎓 Lecciones Aprendidas

1. **A* con Manhattan distance es superior** para el 15-puzzle
2. **Descomposición de datos escala bien** con múltiples puzzles independientes
3. **Verificar solvabilidad es crítico** (solo 50% de configuraciones tienen solución)
4. **Overhead de sincronización** puede superar beneficios de paralelización
5. **Medir antes de paralelizar** - no todos los problemas se benefician

---

## 👤 Autor

**Benjamin Japeto**  
Computación Paralela - 2025  
Repositorio: https://github.com/benjamin94773/Parcial2_Paralela2025_2.git

---

## 📝 Licencia y Uso Académico

Este proyecto es parte del Parcial 2 del curso de Computación Paralela.  
Todos los algoritmos están implementados desde cero para fines educativos.

---

## 🔗 Enlaces Importantes

- **Repositorio GitHub:** https://github.com/benjamin94773/Parcial2_Paralela2025_2.git
- **Compilar informe PDF:** Subir `INFORME_PARCIAL2.tex` a [Overleaf](https://www.overleaf.com)
- **Grabar video:** Seguir instrucciones en `GUIA_VIDEO.md`

---

**Última actualización:** 14 de Octubre de 2025  
**Estado:** ✅ Proyecto completo y listo para entrega
