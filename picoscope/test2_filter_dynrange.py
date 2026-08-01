# Write new h5 file by writing manually the wfs beyond the dynamic range

import h5py

# Wfs to be deleted
to_be_delete = {"Waveform 01245", "Waveform 02727"}

with h5py.File("original_file.h5", "r") as f_in:
    with h5py.File("new_file.h5", "w") as f_out:
        group_A_out = f_out.create_group("A")
        group_A_in = f_in["A"]

        for name_wave in group_A_in:
            if name_wave not in to_be_deleted:
                f_in.copy(f"A/{name_wave}", group_A_out)

