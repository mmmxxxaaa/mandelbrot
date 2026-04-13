import matplotlib.pyplot as plt

def read_monitoring_data(filename):
    frequencies = []
    temperatures = []
    file = open(filename, 'r', encoding='UTF-16')
    for line in file:           # Пропускаем строки до заголовка
        if 'MHz;%;°C' in line:
            break

    for line in file:           # Читаем остальные строки
        line = line.strip()
        if line == '':
            continue
        parts = line.split(';')
        if len(parts) < 3:
            continue
        freq = int(parts[0])    # Преобразуем частоту (первый столбец) и температуру (третий столбец)
        temp = int(parts[2])
        frequencies.append(freq)
        temperatures.append(temp)
    file.close()
    return frequencies, temperatures

file_name = 'Quad_after_two_fixes/monitoring.csv'
freqs, temps = read_monitoring_data(file_name)
print('Загружено', len(freqs), 'измерений.')

if len(freqs) > 0:
    count_less = 0      # Считаем долю измерений с частотой ниже 1995 МГц
    for f in freqs:
        if f < 1995:
            count_less = count_less + 1
    total = len(freqs)
    ratio = count_less / total
    print('Доля измерений c частотой < 1995 МГц: {:.2%}'.format(ratio))

    time_axis = list(range(len(freqs)))                                 # Строим графики
    fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(12, 8), sharex=True)

    ax1.plot(time_axis, freqs, color='blue', linewidth=0.6)
    ax1.set_ylabel('Частота (МГц)')
    ax1.set_title('Частота ядра CPU 4')
    ax1.grid(True, linestyle='--', alpha=0.7)
    ax1.axhline(y=1995, color='green', linestyle=':', linewidth=1, alpha=0.7)
    ax1.text(0.98, 0.95, 'Частота < 1995 МГц: {:.2%}'.format(ratio),
             transform=ax1.transAxes, ha='right', va='top',
             bbox=dict(boxstyle='round', facecolor='white', alpha=0.8))

    ax2.plot(time_axis, temps, color='red', linewidth=0.6)
    ax2.set_xlabel('Номер измерения (≈ время)')
    ax2.set_ylabel('Температура (°C)')
    ax2.set_title('Температура ядра')
    ax2.grid(True, linestyle='--', alpha=0.7)

    plt.tight_layout()
    plt.show()
else:
    print('Данные в файле не найдены.')
