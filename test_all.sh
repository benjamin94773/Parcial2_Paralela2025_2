#!/bin/bash

# Script de pruebas rápidas para validar todas las implementaciones
# Autor: Benjamin Japeto

echo "=========================================="
echo "  Validación 15-Puzzle Solver"
echo "=========================================="
echo ""

# Puzzles de prueba verificados
PUZZLE_EASY="ABCDEFGHIJKLMN#O"    # 1 paso
PUZZLE_MEDIUM="ABCDEFGHIJKL#MNO"  # 3 pasos
PUZZLE_HARD="ABCDEFG#HIJKLMNO"    # 30 pasos

echo "=== Test 1: Validación con puzzles verificados ==="
echo ""

echo "Prueba BFS (esperado: 1 paso):"
echo $PUZZLE_EASY | ./bsp_solver
echo ""

echo "Prueba A*-h1 (esperado: 3 pasos):"
echo $PUZZLE_MEDIUM | ./h1_solver
echo ""

echo "Prueba A*-h2 (esperado: 30 pasos):"
echo $PUZZLE_HARD | ./h2_solver
echo ""

echo "=== Test 2: Comparación de Algoritmos ==="
echo ""
echo "Ejecutando comparación en puzzles_final.txt..."
cat puzzles_final.txt | ./tarea10_comparacion 2>/dev/null | tail -10
echo ""

echo "=== Test 3: Descomposición de Datos (Tarea 8) ==="
echo ""
echo "Probando con 1, 2, 4 y 8 hilos en puzzles_final.txt..."
echo ""

for threads in 1 2 4 8; do
    echo -n "$threads hilos: "
    result=$(cat puzzles_final.txt | ./parallel_data $threads 2>/dev/null | grep -E "Total time|Speedup" | head -2)
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
