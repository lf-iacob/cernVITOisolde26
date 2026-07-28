#Passo a 10 minuti soltanto

def dimezza_file(percorso_file):
    with open(percorso_file, "r", encoding="utf-8") as file:
        righe = file.readlines()

    meta = len(righe) // 2

    # 3. Sovrascrive il file mantenendo solo la prima metà
    with open(percorso_file, "w", encoding="utf-8") as file:
        file.writelines(righe[:meta])

    print(
        f"Operazione completata! Il file originale aveva {len(righe)} righe, ora ne ha {meta}."
    )

dimezza_file("s1_200_10min_125_Sr90.txt")
