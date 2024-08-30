# HadesCalib

This repository contains all codes necessary for my activities with calibrations of HADES detectors: RPC and START.

## Structure

Below the rough workflow of the calibration is described.

1. Calibration histograms.

In this step, one must use raw experimental data to obtain the calibration histograms. They will be stuff like channel-wise time differences, time-over-threshold (ToT) distributions and many, many more. You use these histograms to deduce corrections needed to improve the data quality. These corrections will be usually based on some simple theoretical expectations, e.g. time difference must be set to 0. 

2. Actually finding the correction values

After one has filled the calibration histograms, it is usually visible to the naked eye what sort of correction is needed. It is usually not a great idea to do the correction by manually finding the offsets and hardcoding them (to be more specific: it is absolutely possible, just time-consuming and a little... hmmm... not honorable?). And so, one must develop some sort of an algorithm for actually scanning the calibration histograms and reading-out the necessary offsets. On one hand, the analysis must be quite flexible, so that it can find offsets even if they are 'weird' (the whole reason why calibrating is needed, is that these distributions on their own are NOT all the same). On the other hand, the analysis must be rigid, so that the fitter actually does what we want (e.g. it doesn't fit some super-wide gaussian to the background area). And so the hardest and most interesting part of the calibration lies here: preparing a rigid-but-flexible algorithm that will find the offsets for you. 

Additional note: the number of START channels in the Feb24 Au+Au data was 16+16+8. Reading out 40 offets, writing them on a piece of paper and transferring that to a .txt file is time consuming, but simple and predictable. Optimizing an algorithm, on the other hand, can go into hours or days. And if you only need a few small corrections, then the difference is even more significant. This is why sometimes, when you are time-pressured and only need a quick, rough calibration, hand-reading the offsets may not be a terrible idea. An optimized algorithm can be developed a little later and used to correct small differences. This is all fine for the 40-channel START. There is absolutely no discussion about the 4 * 6 * 192 channels necessary for the RPC calibration... 

3. Writing the correction values to an ASCII file

HYDRA can use an ASCII (.txt) file to load calibration parameters. This is the easiest way of doing early-stage calibrations, as it is very interactive and intuitive. The procedure of writing the file is a little awkward since you must use the correct HYDRA structure, but it is a pure formality. The only really tough part of it is that you must absolutely keep track of these parameter files. You must be certain that when updating the ASCII file, you are in fact updating the same file, that was used to generate the studied results. Otherwise you will make quite a mess. One idea is to always add the date of generation to every ASCII file. This is what I use and it is more or less okay. It is also good to have some sidenote/readme when you explain which parameter version is which. Of course if you're familiar with Git, then you can also track all your calibration params in a git repository and then the version-tracking will be done for you. 

4. Verification

After you have a new ASCII file, you must still confirm that it actually does what it's supposed to do. You do this simply by re-running steps 1-2 of this instruction and demonstrating that you need no further offsets (for this only step 1 is enough) or that your algorithm found all offsets to be equal to 0.0 (for this you must repeat also step 2). One important thing: always check visually the distributions after calibration! The fact that your offsets are all 0.0 doesn't necessarily prove that the calibration is right -- it only demonstrates that you are self-consistent. You may be self-consistent while doing something stupid. Bear in mind that you will usually not get the calibration right at the first attempt. It is natural to approach this iteratively: first a rough calibration, then optimize the algorithm and refine the calibration, then maybe some individual manual fixes for particularily problematic channels.

5. Answer lots of questions

During/after your calibration you will be asked lots of questions about the quality, performance, calibration etc. of the detectors. Everytime I had to add a histogram to the analysis in order to answer a question, I just left it in the code for good. This way, you only have to send the code once per iteration of calibration and you always have some backup slides to answer weird questions. In consequence, the analysis is more resource-consuming. 

## Tools

Tools provided in this repository will be:

 - FillRpcAndStartHistosWithTracking [task class inheriting from HReconstructor; fills all histograms necessary for further calibrations including those requiring tracking]
 - FillRpcAndStartHistosNoTracks [task class inheriting from HReconstructor; fills a part of the calibration histograms, that doesn't require tracking (START module vs. module and RPC charge)]
 - generateStart2CalPar.C [macro getting new offsets based on filled histos and generating a new ASCII file]
 - generateRpcCalPar.C [macro getting new offsets for charge and time calibration of RPC and generating a new ASCII file]
 - generateStart2CalRunPar.C [macro analyzing time walk histograms and the run-by-run parameter file]
 - getTimingMatrix.C [macro analyzing calibration status run-by-run (in reality: batch job by job, timing granularity depends on nFilesPerJob)]

and some plotting tools. 

Currently, however, this is not the case. All classes are found in myClasses.h and all method are in myIncludes. Other macros are currently more dispersed than I would expect, but you can more or less figure out what the code does based on its name.

## Filling histos

The philosophy of my code is to _ALWAYS_ fill _ALL_ histos necessary for _ALL_ calibration activities. This is slightly wasteful, as more memory and operating time is used, than necessary. The benefit, however, is a significant reduction of the number of batchfarm jobs that must be sent (you can send just one analysis for all calibration activities). Keep in mind that this is the final status of the analysis after the feb24 run. All histograms that were in anyway useful were added to the task. This way, any reasonable request from the calibration team should be already included in the analysis. But, then again, they will always come up with something new that you never analysed...

In general I recommend only using the `FillRpcAndStartHistosWithTracking` task. The tracking, of course, takes a lot of time. But, once again, the entire analysis is cleaner if only this task is sent. Including tracking only increases the capabilities of the code, there is no loss involved (except for the loss of time, of course). This way, every calibration analysis you send will have the same structure and full capabilities. 


