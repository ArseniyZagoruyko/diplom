import scipy.constants as const 
 
n = 1.63  
c = const.c 
 

v = c / n 
 
m_e = const.electron_mass 
 
E =  m_e * v*c  
E_eV = E / const.electron_volt 
E_keV = E_eV/1000
 
print(E_keV)
