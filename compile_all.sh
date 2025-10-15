#!/bin/bash

# Script de compilación para el proyecto 15-Puzzle Solver
# Autor: Benjamin Japeto
# Fecha: Octubre 2025

echo "=========================================="
echo "  Compilando 15-Puzzle Solver"
echo "=========================================="
echo ""

# Flags de compilación
CXX="g++"
CXXFLAGS="-std=c++17 -O2 -Wall"
OMPFLAGS="-fopenmp"

# Contador de errores
ERRORS=0

# Función para compilar
compile() {
    local source=$1
    local output=$2
    local flags=$3
    
    echo -n "Compilando $source... "
    if $CXX $CXXFLAGS $flags $source -o $output 2>/dev/null; then
        echo "✓ OK"
    else
        echo "✗ ERROR"
        ((ERRORS++))
    fi
}

echo "--- Tareas 1-3: Utilidades de Tablero ---"
compile "board_printer.cpp" "board_printer" ""
compile "board_moves.cpp" "board_moves" ""
compile "board_available.cpp" "board_available" ""
echo ""

echo "--- Tareas 4-6: Algoritmos Secuenciales ---"
compile "bsp_solver.cpp" "bsp_solver" ""
compile "h1_solver.cpp" "h1_solver" ""
compile "h2_solver.cpp" "h2_solver" ""
echo ""

echo "--- Tarea 7: Soporte NxN ---"
compile "nxn_puzzle_solver.cpp" "nxn_solver" ""
echo ""

echo "--- Tarea 8: Descomposición de Datos ---"
compile "parallel_data_decomposition.cpp" "parallel_data" "$OMPFLAGS"
echo ""

echo "--- Tarea 9: Descomposición de Dominio ---"
compile "tarea9_descomposicion_dominio.cpp" "tarea9_dominio" "$OMPFLAGS"
compile "tarea9_analisis_metricas.cpp" "tarea9_metricas" "$OMPFLAGS"
echo ""

echo "--- Tarea 10: Comparación de Algoritmos ---"
compile "tarea10_comparacion_algoritmos.cpp" "tarea10_comparacion" ""
echo ""

echo "=========================================="
if [ $ERRORS -eq 0 ]; then
    echo "✓ Compilación exitosa - 0 errores"
    echo ""
    echo "Ejecutables generados:"
    ls -1 board_printer board_moves board_available \
       bsp_solver h1_solver h2_solver \
       nxn_solver parallel_data \
       tarea9_dominio tarea9_metricas \
       tarea10_comparacion 2>/dev/null | sed 's/^/  - /'
else
    echo "✗ Compilación con $ERRORS error(es)"
fi
echo "=========================================="
