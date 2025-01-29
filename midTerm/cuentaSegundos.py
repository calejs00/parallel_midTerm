def sumar_segundos(archivo):
    total_segundos = 0

    with open(archivo, 'r') as f:
        for linea in f:
            # Dividir la línea en palabras
            palabras = linea.split()

            # Buscar la posición del número de segundos
            if "segundos." in palabras:
                indice_segundos = palabras.index("segundos.") - 1
                print(palabras[indice_segundos])
                total_segundos += float(palabras[indice_segundos])  # Sumar el número convertido a float

    # Convertir el total de segundos a horas, minutos y segundos
    media = int(total_segundos // 100)
    horas = int(total_segundos // 3600)
    minutos = int((total_segundos % 3600) // 60)
    segundos = total_segundos % 60

    print(f"Tiempo total: {horas} horas, {minutos} minutos, {segundos:.2f} segundos")
    print(f"La media de segundos por contrasenna es: {media}")
    return total_segundos


# Llama a la función con el archivo deseado
archivo = "C:/Users/clara/Documents/3ºUNI/Parallel/parallel_midTerm/midTerm/performanceParallel16hilos_log.txt"  # Cambia esto por el nombre de tu archivo
sumar_segundos(archivo)
