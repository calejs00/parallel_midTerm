import cv2
import numpy as np
import os

# Función para realizar la composición alfa
def alpha_composition(background, overlay, alpha=0.5, position=(0, 0)):
    # Redimensionar la imagen de superposición al tamaño del fondo si es necesario
    overlay_resized = cv2.resize(overlay, (background.shape[1], background.shape[0]))
    
    # Crear una copia del fondo para no modificarlo directamente
    composed_image = background.copy()
    
    # Aplicar la composición alfa en la posición deseada
    h, w = overlay_resized.shape[:2]
    composed_image[position[1]:position[1]+h, position[0]:position[0]+w] = cv2.addWeighted(
        composed_image[position[1]:position[1]+h, position[0]:position[0]+w], alpha,
        overlay_resized, 1 - alpha, 0)
    
    return composed_image

# Función para procesar el dataset secuencialmente
def process_dataset_sequential(background_folder, overlay_image, alpha=0.5, positions=None):
    if positions is None:
        positions = [(0, 0), (100, 100), (200, 200), (300, 300)]  # Posições predefinidas
    
    background_files = os.listdir(background_folder)
    results = []

    for file in background_files:
        if file.endswith('.jpg') or file.endswith('.png'):
            background = cv2.imread(os.path.join(background_folder, file))  # Cargar fondo
            for pos in positions:
                # Componer la imagen
                composed_image = alpha_composition(background, overlay_image, alpha, pos)
                results.append(composed_image)

    return results

# Ruta del dataset de fondos y de la imagen de superposición
background_folder = 'dataset/backgrounds'
overlay_image = cv2.imread('overlay.png')  # Cargar imagen de superposición

# Procesar el dataset secuencialmente
composed_images = process_dataset_sequential(background_folder, overlay_image)

# Mostrar las imágenes resultantes
for idx, img in enumerate(composed_images):
    cv2.imshow(f'Composed Image {idx + 1}', img)

cv2.waitKey(0)
cv2.destroyAllWindows()
