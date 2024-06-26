# HadesCalib
This repository contains all codes necessary for my activities with calibrations of HADES detectors: RPC and START. 
## Structure
The analysis is performed as follows: Firstly, the calibration histograms must be filled. Secondly, appropriate offsets must be found. These are the crucial 'physical' elements of the analysis. Then, in the third steps, the offsets must be saved in a file understood by the HYDRA analysis package (preferably an ASCII file). Lastly, by repeating steps 1 and 2 with the newly-generated parameters, one must test the consistency of proposed calibration (that it indeed improves the situation). 
## Tools
