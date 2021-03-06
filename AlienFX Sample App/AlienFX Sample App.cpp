#include "stdafx.h"
#include <windows.h>
#include "LFX2.h"
#include <iostream>
#include <iomanip>

using namespace std;

int main(int argc, char *argv[]) {
	LFX2INITIALIZE initFunction;
	LFX2RELEASE releaseFunction;
	LFX2RESET resetFunction;
	LFX2UPDATE updateFunction;
	LFX2LIGHT lightFunction;
	LFX2ACTIONCOLOR lightActionColorFunction;
	LFX2SETLIGHTACTIONCOLOR setlightActionColor;
	LFX2SETLIGHTACTIONCOLOREX setlightActionColorEx;
	LFX2ACTIONCOLOREX lightActionColorExFunction;
	LFX2SETTIMING setTiming;
	LFX2GETNUMDEVICES getNumDevicesFunction;
	LFX2GETDEVDESC getDeviceDescriptionFunction;
	LFX2GETNUMLIGHTS getNumLightsFunction;
	LFX2SETLIGHTCOL setLightColorFunction;
	LFX2GETLIGHTCOL getLightColorFunction;
	LFX2GETLIGHTDESC getLightDescriptionFunction;

	HINSTANCE hLibrary = LoadLibrary(_T(LFX_DLL_NAME));
	if (hLibrary) {
		initFunction = (LFX2INITIALIZE) GetProcAddress(hLibrary, LFX_DLL_INITIALIZE);
		releaseFunction = (LFX2RELEASE) GetProcAddress(hLibrary, LFX_DLL_RELEASE);
		resetFunction = (LFX2RESET) GetProcAddress(hLibrary, LFX_DLL_RESET);
		updateFunction = (LFX2UPDATE) GetProcAddress(hLibrary, LFX_DLL_UPDATE);
		lightFunction = (LFX2LIGHT) GetProcAddress(hLibrary, LFX_DLL_LIGHT);
		lightActionColorFunction = (LFX2ACTIONCOLOR) GetProcAddress(hLibrary, LFX_DLL_ACTIONCOLOR);
		lightActionColorFunction = (LFX2ACTIONCOLOR) GetProcAddress(hLibrary, LFX_DLL_ACTIONCOLOR);
		lightActionColorExFunction = (LFX2ACTIONCOLOREX) GetProcAddress(hLibrary, LFX_DLL_ACTIONCOLOREX);
		setlightActionColor = (LFX2SETLIGHTACTIONCOLOR) GetProcAddress(hLibrary, LFX_DLL_SETLIGHTACTIONCOLOR);
		setlightActionColorEx = (LFX2SETLIGHTACTIONCOLOREX) GetProcAddress(hLibrary, LFX_DLL_SETLIGHTACTIONCOLOREX);
		getNumDevicesFunction = (LFX2GETNUMDEVICES) GetProcAddress(hLibrary, LFX_DLL_GETNUMDEVICES);
		getDeviceDescriptionFunction = (LFX2GETDEVDESC) GetProcAddress(hLibrary, LFX_DLL_GETDEVDESC);
		getNumLightsFunction = (LFX2GETNUMLIGHTS) GetProcAddress(hLibrary, LFX_DLL_GETNUMLIGHTS);
		setLightColorFunction = (LFX2SETLIGHTCOL) GetProcAddress(hLibrary, LFX_DLL_SETLIGHTCOL);
		getLightColorFunction = (LFX2GETLIGHTCOL) GetProcAddress(hLibrary, LFX_DLL_GETLIGHTCOL);
		getLightDescriptionFunction = (LFX2GETLIGHTDESC) GetProcAddress(hLibrary, LFX_DLL_GETLIGHTDESC);
		setTiming = (LFX2SETTIMING) GetProcAddress(hLibrary, LFX_DLL_SETTIMING);
		LFX_RESULT result = initFunction();
		if (result == LFX_SUCCESS) {
			result = resetFunction();

			unsigned int numDevs = 0;
			result = getNumDevicesFunction(&numDevs);

			//Get number of devices (usually its only 1) and loop through number of lights each device has
			for (unsigned int devIndex = 0; devIndex < numDevs; devIndex++) {
				unsigned int numLights = 0;
				result = getNumLightsFunction(devIndex, &numLights);

				//Add the number of colors you want for each LED, I am taking 2 for example
				LFX_COLOR color1, color2;

				color1.red = 255;
				color1.green = 0;
				color1.blue = 170;
				color1.brightness = 255;

				color2.red = 155;
				color2.green = 120;
				color2.blue = 255;
				color2.brightness = 255;

				//Example which sets 2 colors across all LED's for each odd and even index
				for (unsigned int lightIndex = 0; lightIndex < numLights; lightIndex++)
					setLightColorFunction(devIndex, lightIndex, (lightIndex % 2 == 0) ? &color1 : &color2);

				//For more advanced method to change color effects such as Pulse (Blinking) or Morph (transition of one color to another)
				//setlightActionColor(devIndex, lightIndex, LFX_ACTION_PULSE, &color1); //Blinking Effect
				//setlightActionColorEx(devIndex, lightIndex, LFX_ACTION_MORPH, &color1,&color2);	//Transition of color1 to color2
			}

			//This code is not needed to change LED colors. It only displays LED index and their respective name for your reference.
			for (unsigned int devIndex = 0; devIndex < numDevs; devIndex++) {
				unsigned char devType = 0;
				unsigned char* devTypePtr = &devType;

				unsigned int descSize = 255;
				char* description = new char[descSize];
				result = getDeviceDescriptionFunction(devIndex, description, descSize, &devType);
				std::cout << "Description: " << description << std::endl;
				delete[]description;

				description = new char[descSize];
				LFX_COLOR color;

				unsigned int numLights = 0;
				result = getNumLightsFunction(devIndex, &numLights);

				for (unsigned int lightIndex = 0; lightIndex < numLights; lightIndex++) {
					result = getLightDescriptionFunction(devIndex, lightIndex, description, descSize);
					if (result != LFX_SUCCESS)
						continue;

					result = getLightColorFunction(devIndex, lightIndex, &color);
					if (result != LFX_SUCCESS)
						continue;

					std::cout << "\tLight: " << lightIndex << "\tDescription: " << description << "\tColor: " <<
						(short) color.brightness << ", " << (short) color.red << ", " <<
						(short) color.green << ", " << (short) color.blue << std::endl;
				}

				delete[]description;
			}

			result = updateFunction();
			std::cout << "Done.\r\n\r\nPress any key to finish ..." << std::endl;
			_gettch();

			//Use this code to release resources such as when program exits and set AlienFX to default state
			result = releaseFunction();

		} else {
			switch (result) {
			case LFX_ERROR_NODEVS:
				std::cout << "There is not AlienFX device available." << std::endl;
				break;
			default:
				std::cout << "There was an error initializing the AlienFX device." << std::endl;
				break;
			}
		}

		FreeLibrary(hLibrary);
	} else
		std::cout << "Failed to load LightFX.dll." << std::endl;

	return 0;
}

