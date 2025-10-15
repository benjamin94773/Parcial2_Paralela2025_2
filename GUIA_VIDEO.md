# 🎥 GUÍA COMPLETA PARA VIDEO - Parcial 2 Computación Paralela

## 📋 INFORMACIÓN DEL PROYECTO

- **Nombre:** Benjamin Japeto
- **Repositorio:** https://github.com/benjamin94773/Parcial2_Paralela2025_2.git
- **Fecha:** Octubre 2025

---

## 🎬 SCRIPT PARA VIDEO (15-20 minutos)

### INTRODUCCIÓN (1 min)
```
"Hola, soy Benjamin Japeto y les presento mi implementación del Parcial 2 
de Computación Paralela. Implementé algoritmos de búsqueda para resolver 
el 15-puzzle y dos estrategias de paralelización con OpenMP."
```

---

## PARTE 1: CLONAR Y COMPILAR (3 min)

### 1.1 Clonar el repositorio
```bash
# Abrir terminal y navegar a directorio de trabajo
cd ~/workspace  # o la carpeta que prefieras

# Clonar el repositorio
git clone https://github.com/benjamin94773/Parcial2_Paralela2025_2.git

# Entrar al directorio
cd Parcial2_Paralela2025_2

# Ver archivos
ls -la
```

**Explicar mientras se ejecuta:**
```
"Como pueden ver, el repositorio contiene:
- 12 archivos de código fuente en C++
- 5 archivos de puzzles de prueba
- 2 scripts de automatización
- El README con instrucciones completas"
```

### 1.2 Compilar todo
```bash
# Dar permisos de ejecución a los scripts
chmod +x compile_all.sh test_all.sh

# Compilar todo el proyecto
bash compile_all.sh
```

**Explicar:**
```
"El script compile_all.sh compila automáticamente las 12 tareas:
- Tareas 1-3: Utilidades del tablero
- Tareas 4-6: Algoritmos secuenciales (BFS, A*-h1, A*-h2)
- Tarea 7: Soporte para tableros NxN
- Tarea 8: Paralelización por descomposición de datos
- Tarea 9: Paralelización por descomposición de dominio
- Tarea 10: Comparación de algoritmos"
```

---

## PARTE 2: VALIDAR CASOS DEL PDF (4 min)

### 2.1 Tarea 4: BFS (debe dar 8 pasos)
```bash
echo 'ABGD#FHCEIJKLMNO' | ./bsp_solver
```

**Validar:**
- ✅ Solution length: 8
- ✅ Solución: D,L,U,R,D,L,U,L

### 2.2 Tarea 5: A* con h1 (debe dar ~14 pasos)
```bash
echo 'HAGDEFBCIJKLMNO#' | ./h1_solver
```

**Validar:**
- ✅ Encuentra solución óptima
- ✅ Expande menos nodos que BFS

### 2.3 Tarea 6: A* con h2 (debe dar ~30 pasos)
```bash
echo 'HGFEDCBA#ONMLKJI' | ./h2_solver
```

**Validar:**
- ✅ Encuentra solución óptima
- ✅ Expande significativamente menos nodos que BFS

**Explicar:**
```
"Como pueden observar, los tres algoritmos encuentran la solución óptima,
pero A*-h2 con distancia Manhattan es el más eficiente, expandiendo 
hasta 76 veces menos nodos que BFS."
```

---

## PARTE 3: TAREA 10 - COMPARACIÓN DE ALGORITMOS (5 min)

### 3.1 Ejecutar comparación
```bash
# Comparar los tres algoritmos en 20 puzzles medianos
./tarea10_comparacion puzzles_medium.txt
```

**Capturar en pantalla:**
- Tabla comparativa de resultados
- Nodos expandidos promedio
- Tiempo de ejecución promedio
- Longitud de solución

**Explicar resultados esperados:**
```
"Los resultados muestran claramente que:
1. A*-h2 expande aproximadamente 76 veces menos nodos que BFS
2. A*-h2 es hasta 83 veces más rápido que BFS
3. Todos encuentran la solución óptima
4. La heurística de Manhattan (h2) supera a fichas descolocadas (h1)"
```

### 3.2 Probar con puzzle difícil
```bash
# Puzzle extremo para mostrar diferencia dramática
echo 'HGFEDCBA#ONMLKJI' | ./bsp_solver
echo 'HGFEDCBA#ONMLKJI' | ./h2_solver
```

**Explicar:**
```
"En puzzles difíciles, BFS puede tardar varios segundos o incluso minutos,
mientras que A*-h2 lo resuelve en milisegundos. Esta es la importancia
de usar heurísticas informadas."
```

---

## PARTE 4: TAREA 8 - DESCOMPOSICIÓN DE DATOS (5 min)

### 4.1 Ejecutar con diferentes números de hilos
```bash
echo "=== Evaluando Speedup con Descomposición de Datos ==="

# Baseline: 1 hilo (secuencial)
echo "--- 1 hilo (secuencial) ---"
head -10 puzzles_hard.txt | ./parallel_data 1

# 2 hilos
echo "--- 2 hilos ---"
head -10 puzzles_hard.txt | ./parallel_data 2

# 4 hilos
echo "--- 4 hilos ---"
head -10 puzzles_hard.txt | ./parallel_data 4

# 8 hilos
echo "--- 8 hilos ---"
head -10 puzzles_hard.txt | ./parallel_data 8
```

**Capturar métricas:**
- Tiempo total de ejecución
- Speedup alcanzado
- Eficiencia paralela
- Throughput (puzzles/segundo)
- Distribución de carga entre hilos

**Explicar resultados esperados:**
```
"La descomposición de datos funciona muy bien porque:
1. Cada hilo procesa puzzles independientes
2. No hay sincronización entre hilos durante la resolución
3. OpenMP distribuye la carga dinámicamente
4. Alcanzamos un speedup de aproximadamente 2x con 8 hilos

La eficiencia disminuye con más hilos debido a:
- Overhead de creación de hilos
- Carga desigual (algunos puzzles son más difíciles)
- Ley de Amdahl"
```

### 4.2 Mostrar distribución de carga
```bash
# El programa muestra automáticamente qué hilo procesó cada puzzle
# Señalar en pantalla la distribución uniforme
```

---

## PARTE 5: TAREA 9 - DESCOMPOSICIÓN DE DOMINIO (5 min)

### 5.1 Ejecutar con diferentes números de hilos
```bash
echo "=== Evaluando Speedup con Descomposición de Dominio ==="

# Puzzle moderadamente difícil (16 pasos)
PUZZLE="ABGDEFHCIJKLMNO#"

# Baseline: 1 hilo (secuencial)
echo "--- 1 hilo (secuencial) ---"
echo $PUZZLE | ./tarea9_dominio 1

# 2 hilos
echo "--- 2 hilos ---"
echo $PUZZLE | ./tarea9_dominio 2

# 4 hilos
echo "--- 4 hilos ---"
echo $PUZZLE | ./tarea9_dominio 4

# 8 hilos
echo "--- 8 hilos ---"
echo $PUZZLE | ./tarea9_dominio 8
```

**Capturar métricas:**
- Tiempo de ejecución
- Speedup (será < 1.0)
- Nodos expandidos (aumenta con más hilos)
- Nodos duplicados

### 5.2 Análisis detallado
```bash
# Ver métricas por hilo
echo $PUZZLE | ./tarea9_metricas 8
```

**Explicar resultados (NEGATIVOS pero IMPORTANTES):**
```
"Como pueden observar, la descomposición de dominio FALLA en este caso:

1. Speedup < 1.0: Con 8 hilos es 20 veces MÁS LENTO
2. Nodos expandidos aumentan: Trabajo duplicado entre hilos
3. Alto overhead de sincronización: Mutex locks constantes

¿Por qué falla?
- BFS tiene dependencias secuenciales inherentes
- Cada operación de nodo es muy rápida (microsegundos)
- El overhead de sincronización es mayor que el beneficio
- Múltiples hilos exploran estados redundantes

Esta es una lección importante: NO TODO ES PARALELIZABLE.
La descomposición de dominio funciona bien en otros algoritmos,
pero para BFS en puzzles pequeños, el overhead domina."
```

---

## PARTE 6: TAREA 7 - ESCALABILIDAD POR TAMAÑO (2 min)

### 6.1 Probar diferentes tamaños
```bash
echo "=== Escalabilidad por tamaño de tablero ==="

# 3x3 (fácil)
echo "--- Puzzle 3x3 ---"
echo 'ABC#DEFGH' | ./nxn_solver

# 4x4 (medio)
echo "--- Puzzle 4x4 ---"
echo 'ABCDEFGHIJKLMNO#' | ./nxn_solver

# 5x5 (difícil)
echo "--- Puzzle 5x5 ---"
echo 'ABCDEFGHIJKLMNOPQRSTUVWX#' | ./nxn_solver
```

**Explicar:**
```
"El espacio de estados crece factorialmente:
- 3x3: 181,440 estados
- 4x4: 10.4 billones de estados
- 5x5: Cuatrillones de estados

A*-h2 mantiene eficiencia incluso en tableros grandes gracias
a la heurística de Manhattan que guía la búsqueda."
```

---

## PARTE 7: CONCLUSIONES (2 min)

### Resumen de hallazgos
```
"En conclusión, este proyecto demuestra:

1. ALGORITMOS SECUENCIALES:
   - A*-h2 es 76x más eficiente que BFS
   - Las heurísticas admisibles garantizan optimalidad
   - Manhattan > Fichas descolocadas

2. PARALELIZACIÓN POR DATOS (Tarea 8):
   ✅ FUNCIONA: Speedup ~2x con 8 hilos
   ✅ Ideal para procesar múltiples puzzles
   ✅ Escalabilidad aceptable hasta 4-8 hilos

3. PARALELIZACIÓN POR DOMINIO (Tarea 9):
   ❌ FALLA: Speedup 0.05x (20 veces más lento)
   ❌ Overhead de sincronización > beneficio
   ❌ Lección: No todo es paralelizable

4. APLICACIONES PRÁCTICAS:
   - Usar A*-h2 para puzzles individuales
   - Usar descomposición de datos para lotes
   - Evitar paralelización de dominio en BFS

Gracias por su atención."
```

---

## 📊 COMANDOS PARA GENERAR DATOS DEL INFORME

### Ejecutar experimentos completos
```bash
#!/bin/bash
# Script para generar todos los resultados del informe

echo "Generando resultados para el informe LaTeX..."
echo "=========================================="

# Crear archivo de resultados
RESULTS="resultados_informe.txt"
> $RESULTS

# TAREA 10: Comparación de algoritmos
echo "=== TAREA 10: COMPARACION ===" | tee -a $RESULTS
./tarea10_comparacion puzzles_medium.txt | tee -a $RESULTS

# TAREA 8: Descomposición de datos (10 puzzles para rapidez)
echo -e "\n=== TAREA 8: DESCOMPOSICION DE DATOS ===" | tee -a $RESULTS
for threads in 1 2 4 8; do
    echo "--- $threads HILOS ---" | tee -a $RESULTS
    head -10 puzzles_hard.txt | ./parallel_data $threads 2>&1 | grep -E "Puzzles|Tiempo|Speedup|Throughput" | tee -a $RESULTS
done

# TAREA 9: Descomposición de dominio
echo -e "\n=== TAREA 9: DESCOMPOSICION DE DOMINIO ===" | tee -a $RESULTS
PUZZLE="ABGDEFHCIJKLMNO#"
for threads in 1 2 4 8; do
    echo "--- $threads HILOS ---" | tee -a $RESULTS
    echo $PUZZLE | ./tarea9_dominio $threads 2>&1 | grep -E "Time|Nodes|Speedup" | tee -a $RESULTS
done

echo "=========================================="
echo "Resultados guardados en: $RESULTS"
```

### Copiar resultados a Windows
```powershell
# Desde PowerShell en Windows
docker cp bold_ganguly:/home/japeto/app/resultados_informe.txt "C:\Users\benja\OneDrive\Desktop\Parcial 2 Paralela\"
```

---

## ✅ CHECKLIST PARA EL VIDEO

### Preparación
- [ ] Terminal limpia y con fuente legible
- [ ] Repositorio clonado en directorio limpio
- [ ] Docker corriendo (si usas Docker)
- [ ] Micrófono funcionando
- [ ] Pantalla compartida configurada

### Durante la grabación
- [ ] Introducción clara con nombre y tema
- [ ] Mostrar README en GitHub
- [ ] Compilar con compile_all.sh
- [ ] Validar casos del PDF (Tareas 4, 5, 6)
- [ ] Ejecutar Tarea 10 (comparación)
- [ ] Ejecutar Tarea 8 (1, 2, 4, 8 hilos)
- [ ] Ejecutar Tarea 9 (1, 2, 4, 8 hilos)
- [ ] Explicar POR QUÉ Tarea 9 falla
- [ ] Mostrar escalabilidad (3x3, 4x4, 5x5)
- [ ] Conclusiones finales

### Después de grabar
- [ ] Resultados capturados en resultados_informe.txt
- [ ] Actualizar informe LaTeX con datos reales
- [ ] Compilar PDF del informe
- [ ] Subir video a plataforma requerida
- [ ] Entregar informe PDF con link del repo

---

## 🎯 PUNTOS CLAVE PARA ENFATIZAR

1. **A*-h2 es el mejor algoritmo** - 76x más eficiente que BFS
2. **Tarea 8 funciona bien** - Speedup 2x, demuestra paralelización efectiva
3. **Tarea 9 falla (pero está bien)** - Demuestra que no todo es paralelizable
4. **Lección académica importante** - Entender cuándo paralelizar y cuándo no

---

## 📝 NOTAS FINALES

- La Tarea 9 está DISEÑADA para fallar - es una lección académica
- Los resultados negativos son TAN IMPORTANTES como los positivos
- Demostrar que entiendes POR QUÉ algo no funciona es valioso
- El informe debe incluir análisis crítico de ambas estrategias

**¡Buena suerte con el video!** 🎬
