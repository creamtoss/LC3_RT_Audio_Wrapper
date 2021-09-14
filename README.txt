	    //---------------------------------------//
	   //        LC3_RT_Audio_Wrapper           //
	  //        by: Christoph Zimmer           //
	 //  e-mail: zimmerchristoph97@gmail.com  //
	//---------------------------------------//

This is a step by step guide for the integration of the LC3 into RT-Audio on Windows 
with Visual Studio 2019. In this integration method any pre-existing project of 
RT-Audio (for this example "duplex" will be used) is modified and extended by the 
floating-point source code of the LC3 and the LC3-Wrapper files. The download links 
for the mentioned code can be found be found below.



DOWNLOAD LINKS:

LC3-Wrapper:		https://github.com/creamtoss/LC3_RT_Audio_Wrapper

Visual Studio 2019:	https://visualstudio.microsoft.com/de/downloads/

RT-Audio:		https://github.com/thestk/rtaudio

LC3: 			https://www.iis.fraunhofer.de/de/ff/amm/kommunikation/lc3.html



STEP BY STEP GUIDE

1. Install Visual Studio (VS) 2019:
	Select the Desktop development with C++ during the installation process.

2. Unzip all the downloaded .zip files

3. Create sub folders for the RT-Audio extensions
	• ../rtaudio-master/lc3
	• ../rtaudio-master/wrapper

4. Add files to the subfolders
	• Move files all files and folders from ../LC3plusET SIsrcv1148220190725/src/floatingpoint to ../rtaudio−master/lc3
	• Move lc3wrapper.c and lc3wrapper.h from ../LC3RTAudioWrapper−main to ../rtaudio-master/wrapper
	• Move rtaudiolc3.cpp from ../LC3RTAudioWrapper−main to ../rtaudio-master/tests

5. Open the RT-Audio VS-Workspace
	open the rtaudio.dsw file from ../rtaudio−master/tests/Windows. Click "OK" if VS asks for an unidirectional upgrade 
	of the project. From now on rtaudio.sln can be opened.

6. Adapt RT-Audio to VS 19
	Right-Click on the duplex project in the rtaudio project folder withing VS. Select "Project Settings", "C/C++",
	"Codegeneration", "Activate function level linking" and select "Yes (/Gy)".

7. Test build the "duplex" project
	by right-clicking the "duplex" and select "build".

8. Run the test build
	by copying your ../rtaudio−masterold/tests/Windows path and opening the command-line interpreter. Inside of the interpreter, 
	type "cp [YOUR PATH]" and hit enter. Then type "duplex 2 48000" and hit enter. If your audio input is now mirrored to your 
	audio output your device is compatible with RT-Audio. If this is not the case, try enabling your device via the windows 
	settings, download the driver for your device or change your device.

9. Add the extensions to RT-Audio
	by right-clicking on the "duplex" project and selecting "add already existing element". Add:
	• All files within ../rtaudio−master/lc3 but NOT the files withing the../rtaudio−master/lc3/fft subfolder!
	• All files within ../rtaudio−master/wrapper
	• The rtaudiolc3.cpp file within ../rtaudio−master/tests

10. Edit further project settings
	by right-clicking the "duplex" project and selecting "project properties":
	• "C/C++", "General", "Additional include directories" and add the following directories:
		– ../../wrapper
		– ../../lc3
	•"C/C++", "Extended", "Deactivate specific warnings" and fill in: "4068;4996;%(DisableSpecificWarnings)"

11. Delete codecexe.c and duplex.c 
	to avoid multiple main() functions by right-clicking on the corresponding file in the "duplex" project and selecting "delete".

12. Build and run the duplex project 
	by right-clicking the "duplex" project and select "build"

13. Run the build
	by using the command line interpreter.
