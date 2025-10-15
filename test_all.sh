#!/bin/bash

# Script de pruebas rápidas para validar todas las implementaciones
# Autor: Benjamin Japeto

echo "=========================================="
echo "  Validación 15-Puzzle Solver"
echo "=========================================="
echo ""

# Puzzle de prueba del PDF (BFS debe dar 8 pasos)
PUZZLE_BFS="ABGD#FHCEIJKLMNO"
# Puzzle de prueba para A*-h1
PUZZLE_H1="HAGDEFBCIJKLMNO#"
# Puzzle de prueba para A*-h2
PUZZLE_H2="HGFEDCBA#ONMLKJI"
# Puzzle moderado
PUZZLE_MEDIUM="ABGDEFHCIJKLMNO#"

echo "=== Test 1: Validación con casos del PDF ==="
echo ""

echo "Prueba BFS (esperado: 8 pasos):"
echo $PUZZLE_BFS | ./bsp_solver | grep -E "Solution length|Nodes expanded" | head -2
echo ""

echo "Prueba A*-h1 (esperado: ~14 pasos):"
echo $PUZZLE_H1 | ./h1_solver | grep -E "Solution length|Nodes expanded" | head -2
echo ""

echo "Prueba A*-h2 (esperado: ~30 pasos):"
echo $PUZZLE_H2 | ./h2_solver | grep -E "Solution length|Nodes expanded" | head -2
echo ""

echo "=== Test 2: Comparación de Algoritmos ==="
echo ""
echo "Ejecutando comparación en puzzles_medium.txt..."
./tarea10_comparacion puzzles_medium.txt 2>/dev/null | tail -10
echo ""

echo "=== Test 3: Descomposición de Datos (Tarea 8) ==="
echo ""
echo "Probando con 1, 2, 4 y 8 hilos en puzzles_hard.txt..."
echo ""

for threads in 1 2 4 8; do
    echo -n "$threads hilos: "
    result=$(./parallel_data puzzles_hard.txt $threads 2>/dev/null | grep -E "Total time|Speedup" | head -2)
    time=$(echo "$result" | grep "Total time" | awk '{print $3}')
    speedup=$(echo "$result" | grep "Speedup" | awk '{print $2}')
    echo "Tiempo=${time}s, Speedup=${speedup}x"
done
echo ""

echo "=== Test 4: Descomposición de Dominio (Tarea 9) ==="
echo ""
echo "Probando con 1, 2, 4 y 8 hilos en un puzzle..."
echo ""

for threads in 1 2 4 8; do
    echo -n "$threads hilos: "
    result=$(echo $PUZZLE_MEDIUM | ./tarea9_dominio $threads 2>/dev/null | grep -E "Time|Speedup" | head -2)
    time=$(echo "$result" | grep "Time" | awk '{print $2}')
    speedup=$(echo "$result" | grep "Speedup" | awk '{print $2}')
    echo "Tiempo=${time}s, Speedup=${speedup}x"
done
echo ""

echo "=== Test 5: Soporte NxN ==="
echo ""

echo "Puzzle 3x3:"
echo 'ABC#DEFGH' | ./nxn_solver | grep -E "Solution length|Board size" | head -2
echo ""

echo "Puzzle 4x4:"
echo 'ABCDEFGHIJKLMNO#' | ./nxn_solver | grep -E "Solution length|Board size" | head -2
echo ""

echo "=========================================="
echo "✓ Validación completa"
echo "=========================================="
