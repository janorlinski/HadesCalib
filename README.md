# HadesCalib
This repository contains all codes necessary for my activities with calibrations of HADES detectors: RPC and START. 
## Structure
The analysis is performed as follows: Firstly, the calibration histograms must be filled. Secondly, appropriate offsets must be found. These are the crucial 'physical' elements of the analysis. Then, in the third steps, the offsets must be saved in a file understood by the HYDRA analysis package (preferably an ASCII file). Lastly, by repeating steps 1 and 2 with the newly-generated parameters, one must test the consistency of proposed calibration (that it indeed improves the situation). 
## Tools
Tools provided in this repository are:
 - FillRpcAndStartHistosWithTracking [task class inheriting from HReconstructor; fills all histograms necessary for further calibrations including those requiring tracking]
 - FillRpcAndStartHistosNoTracks [task class inheriting from HReconstructor; fills a part of the calibration histograms, that doesn't require tracking (START module vs. module and RPC charge)]
 - generateStart2CalPar.C [macro getting new offsets based on filled histos and generating a new ASCII file]
 - generateRpcCalPar.C [macro getting new offsets for charge and time calibration of RPC and generating a new ASCII file]
 - generateStart2CalRunPar.C [macro analyzing time walk histograms and the run-by-run parameter file]
 - getTimingMatrix.C [macro analyzing calibration status run-by-run (in reality: batch job by job, timing granularity depends on nFilesPerJob)]
## Filling histos
The philosophy of the code is to _ALWAYS_ fill _ALL_ histos necessary for _ALL_ calibration activities. This is slightly wasteful, as memory and operating time is somehow wasted. The benefit however is a significant reduction of the number of batchfarm jobs that must be sent. Keep in mind that this is the final status of the analysis after the feb24 run. All histograms that were in anyway useful were added to the task. This way, any reasonable request from the calibration team should be already included in the analysis. 

In general I recommend only using the `FillRpcAndStartHistosWithTracking` task. The tracking, of course, takes a lot of time. But, once again, the entire analysis is cleaner and lighter on the batchfarm if only this task is sent. Including tracking only increases the capabilities of the code, there is no loss involved. 
