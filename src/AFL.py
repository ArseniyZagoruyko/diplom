import matplotlib.pyplot as plt

def read_hits_file(filename):
    data = {}
    with open(filename, 'r') as file:
        for line in file:
            parts = line.split()
            detector_id = int(parts[3])
            energy = float(parts[8])
            if detector_id not in data:
                data[detector_id] = {'Particles': [], 'Energies': []}
            data[detector_id]['Particles'].append(len(data[detector_id]['Particles']) + 1)
            data[detector_id]['Energies'].append(energy)
    return data

def plot_energy_particles(data):
    for detector_id, detector_data in data.items():
        plt.hist(detector_data['Energies'], bins=100, label=f'Detector {detector_id}')
        plt.ylabel('Number of Particles')
        plt.xlabel('Energy (keV)')
        plt.title(f'Спектр {detector_id}')
        plt.legend()
        plt.show()

hits_data = read_hits_file('/home/zas/CERN/SCINT/build/hits.txt')

plot_energy_particles(hits_data)