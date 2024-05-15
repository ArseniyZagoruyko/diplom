import matplotlib.pyplot as plt 
import numpy as np 
from scipy.optimize import curve_fit 
 
def process_data(file_path): 
    times_detector1 = [] 
    times_detector2 = [] 
    with open(file_path, 'r') as file: 
        for line in file: 
            if line.startswith('DetectorID:'): 
                detector_id = int(line.split()[1]) 
                time = float(line.split()[3]) 
                if detector_id == 1: 
                    times_detector1.append(time) 
                elif detector_id == 2: 
                    times_detector2.append(time) 
     
    time_diffs = [] 
    for time1, time2 in zip(times_detector1, times_detector2): 
        time_diffs.append(time1 - time2) 
     
    return time_diffs 
 

file_path = '/home/zas/CERN/SCINT/build/hits.txt' 
data = process_data(file_path) 
 

def cauchy_distribution(x, x0, gamma): 
    return 1 / (np.pi * gamma * (1 + ((x - x0) / gamma) ** 2)) 
 

x0_guess = np.median(data) 
gamma_guess = np.std(data) 
 
# # Определение количества бинов по методу Optimal bins
# n = len(data)
# h = 2* np.percentile(data,75)* (n**(-1/3))
# bins = int((max(data)-min(data))/h)
bins = 100
 
 

hist, bin_edges = np.histogram(data, bins=bins, density=True) 
 

bin_centers = (bin_edges[1:] + bin_edges[:-1]) / 2 
 

popt, pcov = curve_fit(cauchy_distribution, bin_centers, hist, p0=[x0_guess, gamma_guess]) 

x0_fit = popt[0]
gamma_fit = popt[1]


 

plt.hist(data, bins=bin_edges, density=True, alpha=0.5, label='Гистограмма') 
 

x_values = np.linspace(min(data), max(data), 100) 
label_text = 'Фитирование: x0=%5.3e, gamma=%5.3e' % (popt[0], popt[1]) 
plt.plot(x_values, cauchy_distribution(x_values, *popt), 'r-', label=label_text)
plt.xlabel('\u0394t, с')
plt.ylabel('Кол-во совпадений') 
plt.legend(loc='upper left', fontsize=7) 
plt.show()


max_height = max(cauchy_distribution(x_values, *popt)) 
half_max_height = max_height / 2 
indices = np.where(cauchy_distribution(x_values, *popt) > half_max_height) 
half_max_width_x = x_values[indices] 

 
fwhm = half_max_width_x[-1] - half_max_width_x[0] 
 
print("FWHM:", fwhm)
