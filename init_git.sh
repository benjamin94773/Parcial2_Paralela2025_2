#!/bin/bash
# Script para inicializar Git y preparar el primer commit
# Ejecutar este script DESPUÉS de crear el repositorio en GitHub

echo "=========================================="
echo "  Inicializando Repositorio Git"
echo "=========================================="
echo ""

# Paso 1: Inicializar Git
echo "📁 Paso 1: Inicializando Git..."
git init
echo "✓ Git inicializado"
echo ""

# Paso 2: Configurar usuario (opcional, si no está configurado globalmente)
read -p "¿Configurar nombre de usuario? (s/n): " config_user
if [ "$config_user" = "s" ]; then
    read -p "Nombre: " git_name
    read -p "Email: " git_email
    git config user.name "$git_name"
    git config user.email "$git_email"
    echo "✓ Usuario configurado"
fi
echo ""

# Paso 3: Agregar archivos
echo "📝 Paso 2: Agregando archivos al staging area..."
git add *.cpp
git add *.txt
git add *.sh
git add *.md
git add .gitignore
git add *.tex
git add *.pdf
echo "✓ Archivos agregados"
echo ""

# Paso 4: Verificar status
echo "🔍 Paso 3: Verificando archivos agregados..."
git status --short
echo ""

# Paso 5: Contar archivos
staged_count=$(git status --short | wc -l)
echo "📊 Total de archivos en staging: $staged_count"
echo ""

# Paso 6: Commit
echo "💾 Paso 4: Creando commit inicial..."
git commit -m "Initial commit: 15-Puzzle Solver - Parcial 2 Computación Paralela

Implementación completa de 12 tareas:
- Tareas 1-3: Utilidades de tablero
- Tareas 4-6: Algoritmos secuenciales (BFS, A*-h1, A*-h2)
- Tarea 7: Soporte NxN (3x3 a 6x6)
- Tarea 8: Descomposición de datos (speedup 1.98x)
- Tarea 9: Descomposición de dominio (análisis académico)
- Tarea 10: Comparación de algoritmos

Incluye:
- 12 archivos de código fuente C++
- 5 conjuntos de puzzles (easy, medium, hard, extreme)
- Scripts de compilación y pruebas
- Informe LaTeX completo
- Documentación detallada en README"

echo "✓ Commit creado"
echo ""

# Paso 7: Configurar remote
echo "🌐 Paso 5: Configurando repositorio remoto..."
read -p "URL del repositorio GitHub: " repo_url
git remote add origin "$repo_url"
echo "✓ Remote configurado"
echo ""

# Paso 8: Push
echo "🚀 Paso 6: Subiendo archivos a GitHub..."
git branch -M main
git push -u origin main
echo ""

echo "=========================================="
echo "✅ ¡Repositorio creado exitosamente!"
echo "=========================================="
echo ""
echo "Verifica tu repositorio en:"
echo "$repo_url"
echo ""
