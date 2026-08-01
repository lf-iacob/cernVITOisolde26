#Convert from .h5 to .txt file
import h5py
import numpy as np

file_h5_input = "s1_200_10k_125_1_Sr90.h5"
file_txt_output = "s1_200_10k_125_1_Sr90.txt"

def estrai_dati_cartella_A(nome, oggetto, file_txt):
    if isinstance(oggetto, h5py.Dataset):
        parti_percorso = nome.split('/')
        
        if parti_percorso[0] == 'A' and 'Abscissa' not in parti_percorso:
            dati = oggetto[:]
            try:
                if dati.ndim <= 2:
                    np.savetxt(file_txt, dati, fmt='%s')
                else:
                    file_txt.write(np.array2string(dati, threshold=np.inf))
                    file_txt.write("\n")
            except Exception as e:
                file_txt.write(f"{dati}\n")

print("Converting file .h5: taking waveform data from directory /A...")

with h5py.File(file_h5_input, 'r') as h5_file, open(file_txt_output, 'w', encoding='utf-8') as txt_file:
    h5_file.visititems(lambda nome, obj: estrai_dati_cartella_A(nome, obj, txt_file))

print(f"Extraction completed - Data saved in '{file_txt_output}'.")


