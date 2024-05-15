import numpy as np 
import matplotlib.pyplot as plt 
 
def build_spectrum(file_path): 
    detector_1_energy = [] 
    detector_2_energy = [] 
 
    with open(file_path, 'r') as file: 
        for line in file: 
            parts = line.strip().split() 
            detector_id = int(parts[1]) 
            energy = float(parts[-2])
            if energy >= 15: 
                if detector_id == 1: 
                    detector_1_energy.append(energy) 
                elif detector_id == 2: 
                    detector_2_energy.append(energy) 
 
    hist_1, bin_edges_1 = np.histogram(detector_1_energy, bins=100) 
    hist_2, bin_edges_2 = np.histogram(detector_2_energy, bins=100) 
 
    bin_centers_1 = (bin_edges_1[:-1] + bin_edges_1[1:]) / 2 
    bin_centers_2 = (bin_edges_2[:-1] + bin_edges_2[1:]) / 2 
 
    plt.figure(figsize=(10, 6)) 
    plt.plot(bin_centers_1, hist_1, color='blue', label='Детектор 1') 
    plt.plot(bin_centers_2, hist_2, color='red', label='Детектор 2') 
    plt.yscale('log')
    plt.xlabel('E, кэВ') 
    plt.ylabel('log(Кол-во отсчетов)') 
    plt.title('Спектр') 
    plt.legend() 
    plt.show()
 
file_path = '/home/zas/CERN/SCINT/build/hits.txt'  # Укажите путь к вашему файлу здесь 
build_spectrum(file_path)