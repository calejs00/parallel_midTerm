import matplotlib.pyplot as plt
import numpy as np

# Datos de ejemplo
k_values = [3, 5, 10, 20, 50, 100]
time_1 = [602, 390, 2257, 5567, 9669, 23878]  # Tiempo de ejecución con 1 hilo (sec. - secuencial)
time_2 = [233, 70, 829, 2480, 2219, 10042]  # Tiempo de ejecución con 2 hilos
time_4 = [79, 45, 1198, 433, 159, 9754]  # Tiempo de ejecución con 4 hilos
time_8 = [13, 17, 367, 109, 449, 2715]  # Tiempo de ejecución con 8 hilos
time_16 = [14, 17, 519, 848, 111, 114]

speedup_2 = [2.58, 5.57, 2.72, 2.24, 4.36, 2.38]  # Speedup para 2 hilos
speedup_4 = [7.63, 8.67, 1.88, 12.87, 60.86, 2.45]  # Speedup para 4 hilos
speedup_8 = [46.31, 22.92, 6.14, 51.13, 21.56, 8.79]  # Speedup para 8 hilos
speedup_16 = [43.5, 22.92, 4.35, 6.57, 87.03, 8.79]  # Speedup para 8 hilos

efficiency_2 = [1.29, 2.79, 1.36, 1.12, 2.38, 1.19]  # Eficiencia para 2 hilos
efficiency_4 = [1.91, 2.17, 0.47, 5.25, 2.45, 0.61]  # Eficiencia para 4 hilos
efficiency_8 = [5.79, 2.87, 0.77, 6.39, 8.79, 1.10]  # Eficiencia para 8 hilos
efficiency_16 = [2.72, 1.43, 0.27, 0.41, 209.06, 13.06]  # Eficiencia para 8 hilos

# Ajustar el ancho de las barras
bar_width = 0.2
index = np.arange(len(k_values))

# --- Gráfico 1: Speedup ---
fig, ax = plt.subplots(figsize=(10, 6))
ax.bar(index - 1.5 * bar_width, speedup_2, bar_width, label='Speedup (2 Threads)', color='teal')
ax.bar(index - 0.5 * bar_width, speedup_4, bar_width, label='Speedup (4 Threads)', color='mediumseagreen')
ax.bar(index + 0.5 * bar_width, speedup_8, bar_width, label='Speedup (8 Threads)', color='lightcoral')
ax.bar(index + 1.5 * bar_width, speedup_16, bar_width, label='Speedup (16 Threads)', color='plum')

# Añadir etiquetas y título
ax.set_xlabel('Number of Clusters (k)')
ax.set_ylabel('Speedup')
ax.set_title('K-means Speedup vs. Number of Clusters (k)')
ax.set_xticks(index)
ax.set_xticklabels(k_values)
ax.legend()

# Mostrar el gráfico
plt.tight_layout()
plt.show()

# --- Gráfico 2: Efficiency ---
fig, ax = plt.subplots(figsize=(10, 6))
ax.bar(index - 1.5 * bar_width, efficiency_2, bar_width, label='Efficiency (2 Threads)', color='teal')
ax.bar(index - 0.5 * bar_width, efficiency_4, bar_width, label='Efficiency (4 Threads)', color='mediumseagreen')
ax.bar(index + 0.5 * bar_width, efficiency_8, bar_width, label='Efficiency (8 Threads)', color='lightcoral')
ax.bar(index + 1.5 * bar_width, efficiency_16, bar_width, label='Efficiency (16 Threads)', color='plum')  # Añadido para 16 hilos

# Añadir etiquetas y título
ax.set_xlabel('Number of Clusters (k)')
ax.set_ylabel('Efficiency')
ax.set_title('K-means Efficiency vs. Number of Clusters (k)')
ax.set_xticks(index)
ax.set_xticklabels(k_values)
ax.legend()
ax.set_ylim(0, 25)
# Mostrar el gráfico
plt.tight_layout()
plt.show()

index = np.arange(len(time_1))  # Índices para cada valor de k
bar_width = 0.15  # Ancho de las barras

# Crear la figura y los ejes
fig, ax = plt.subplots(figsize=(12, 6))

# Añadir las barras para cada número de hilos
ax.bar(index - 2.0 * bar_width, time_1, bar_width, label='Execution Time (Sequential)', color='khaki')  # Tiempo secuencial
ax.bar(index - 1.0 * bar_width, time_2, bar_width, label='Execution Time (2 Threads)', color='teal')
ax.bar(index, time_4, bar_width, label='Execution Time (4 Threads)', color='mediumseagreen')
ax.bar(index + 1.0 * bar_width, time_8, bar_width, label='Execution Time (8 Threads)', color='lightcoral')
ax.bar(index + 2.0 * bar_width, time_16, bar_width, label='Execution Time (16 Threads)', color='plum')

# Personalizar el gráfico
ax.set_xlabel('k Values')  # Etiqueta del eje X
ax.set_ylabel('Execution Time (ms)')  # Etiqueta del eje Y
ax.set_title('Execution Time for Different Number of Threads')  # Título
ax.set_xticks(index)  # Marcas en el eje X
ax.set_xticklabels(['3', '5', '10', '20', '50', '100'])  # Reemplazar por los valores de k
ax.legend()  # Añadir la leyenda

# Ajustar el rango del eje Y para la visualización
ax.set_ylim(0, max(time_16) + 2000)  # Ajusta este valor según los datos, para dejar espacio si los valores son altos

# Mostrar el gráfico
plt.tight_layout()
plt.show()