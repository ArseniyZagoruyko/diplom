import matplotlib.pyplot as plt 
import numpy as np
 
file2 = "/home/zas/CERN/SCINT/build/electron_gamma511.txt" 
file1 = "/home/zas/CERN/SCINT/build/electron_gamma>503<511.txt"
# file3 = "/home/zas/CERN/SCINT/build/electron_gamma<503.txt" 
# file3 = "/home/zas/CERN/SCINT/build/output1.txt" 
file3 = "/home/zas/CERN/SCINT/build/output.txt"
 
def read_data(file, lines_to_read): 
    energies = [] 
    with open(file, 'r') as f: 
        for _ in range(lines_to_read): 
            line = next(f, None) 
            if line is None: 
                break 
            parts = line.split(',') 
            energy = float(parts[2].split(':')[1].split()[0].strip())  
            energies.append(energy) 
    return energies 
 
lines_to_read = 100000000
 
energies1 = read_data(file1, lines_to_read) 
energies2 = read_data(file2, lines_to_read)
energies3 = read_data(file3, lines_to_read) 
 

hist1, bins1 = np.histogram(energies1, bins=200) 
hist2, bins2 = np.histogram(energies2, bins=200) 
hist3, bins3 = np.histogram(energies3, bins=200)
 

bin_centers1 = (bins1[:-1] + bins1[1:]) / 2 
bin_centers2 = (bins2[:-1] + bins2[1:]) / 2 
bin_centers3 =bin_centers1
 
plt.figure(figsize=(10, 6)) 
# plt.plot(bin_centers2, hist2, color='red', label='от нерассеянных')
# plt.plot(bin_centers1, hist1, color='blue', label='от рассеянных менее чем на 10 градусов') 
plt.plot(bin_centers3,hist3, color ='green', label = '') 
plt.axvline(x=313.5, color ='purple', linestyle='--',label='Et')
plt.yscale('log')
plt.xlabel('E, кэВ') 
plt.ylabel('Кол-во электронов') 
plt.title('Спектр') 
plt.grid(True) 
plt.legend() 
 
plt.show()