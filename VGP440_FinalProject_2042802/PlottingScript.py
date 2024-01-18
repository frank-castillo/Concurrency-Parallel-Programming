# -*- coding: utf-8 -*-
"""
Created on Fri Mar 10 06:23:17 2023

@author: juanf
"""

import numpy as np
import matplotlib.pyplot as plt

# Load the FFT magnitude spectrum data from the exported file
fft_data = np.loadtxt('Magnitude_result.txt')

# Compute the corresponding frequency values based on the sampling rate and number of data points
sample_rate = 44100  # example sampling rate (replace with your actual value)
num_samples = len(fft_data)
freqs = np.arange(num_samples) * sample_rate / num_samples

# Plot the FFT magnitude spectrum
plt.plot(freqs[:num_samples//2], fft_data[:num_samples//2])
plt.xlabel('Frequency (Hz)')
plt.ylabel('Magnitude')
plt.savefig('FFT_Plot.png')
plt.show()