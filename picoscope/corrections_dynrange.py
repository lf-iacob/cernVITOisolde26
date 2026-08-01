# DISCARD WAVEFORMS BEYOND THE DYNAMIC RANGE

import h5py
import numpy as np
from tqdm import tqdm  # Optional library to display a progress bar

input_filename = "s2grease_150_10min_125_1_Sr90.h5"
output_filename = "s4_150_10min_125_1_Sr90.h5"
group_name = "A"

# List to store the names of discarded waveforms (useful for logging/verification)
discarded_waveforms = []
valid_waveforms_count = 0

print("\nAnalyzing and filtering waveforms...")

with h5py.File(input_filename, "r") as infile, h5py.File(output_filename, "w") as outfile:
    if group_name in infile:
        in_group = infile[group_name]
        out_group = outfile.create_group(group_name)
    else:
        raise KeyError(f"Group '{group_name}' was not found in the HDF5 file.")

    # Get total count BEFORE closing the file context
    waveform_keys = list(in_group.keys())
    total_waveforms = len(waveform_keys)

    # Iterate over all datasets inside group 'A'
    for wf_name in tqdm(waveform_keys):
        # Read dataset into NumPy array
        data = in_group[wf_name][:]

        # Check if waveform contains 'inf' or 'NaN'
        has_inf_or_nan = np.isinf(data).any() or np.isnan(data).any()

        if has_inf_or_nan:
            discarded_waveforms.append(wf_name)
        else:
            # Copy dataset and metadata to output file
            out_dataset = out_group.create_dataset(wf_name, data=data)
            for attr_name, attr_value in in_group[wf_name].attrs.items():
                out_dataset.attrs[attr_name] = attr_value
            
            valid_waveforms_count += 1

# Summary print statements now safely run outside the 'with' block
print("\n--- FILTERING RESULTS ---")
print(f"Total waveforms analyzed: {total_waveforms}")
print(f"Valid waveforms saved:    {valid_waveforms_count}")
print(f"Discarded waveforms:      {len(discarded_waveforms)}")

with open("discarded_wf_150.txt", "w") as f:
    for wf in discarded_waveforms:
        f.write(f"{wf}\n")

print("\nList of discarded waveforms saved to 'discarded_wf_150.txt'.\n")
