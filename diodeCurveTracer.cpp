/*
Electric Sheep Labs
2/14/19
Diode Curve Tracer

The power supply channel of the Analog Discovery 2 (AD2) is used to ramp the voltage across the diode with a triangle wave, and an input channel is used to measure the voltage across a series resistor of known value to get the current through the diode. These data are output to a file, which can then be plotted to give the IV curve for the diode.
*/

#include "sample.h"
#include <stdio.h>

int main(int carg, char **szarg){
    HDWF hdwf;
    STS sts;
    char szError[512] = {0};
    int cSamplesMin, cSamplesMax;
    double* rgdSamples;
    FILE *fp;
    int fOn;

    printf("Open automatically the first available device\n");
    if(!FDwfDeviceOpen(-1, &hdwf)) {
        FDwfGetLastErrorMsg(szError);
        printf("Device open failed\n\t%s", szError);
        return 0;
    } 
   
    //setup acquisition channel
    // enable first channel 
    FDwfAnalogInChannelEnableSet(hdwf, 0, true);
    FDwfAnalogInChannelOffsetSet(hdwf, 0, 0);// set 0V offset
    FDwfAnalogInChannelRangeSet(hdwf, 0, 10);// set 5V pk2pk input range
    FDwfAnalogInConfigure(hdwf, 0, false);// start signal generation
    FDwfAnalogInFrequencySet(hdwf, 16384.0);//sample rate equal to double buffer size...for one half period acquisition
    // get the maximum buffer size
    FDwfAnalogInBufferSizeInfo(hdwf, &cSamplesMin, &cSamplesMax);
    FDwfAnalogInBufferSizeSet(hdwf, cSamplesMax);

    rgdSamples = new double[32];
    printf("sample min: %i sample max: %i \n", cSamplesMin, cSamplesMax);
    // configure trigger
    //FDwfAnalogInTriggerSourceSet(hdwf, trigsrcDetectorAnalogIn);
    FDwfAnalogInTriggerSourceSet(hdwf, trigsrcNone);
    FDwfAnalogInTriggerAutoTimeoutSet(hdwf, 10.0);
    FDwfAnalogInTriggerChannelSet(hdwf, 0); 

    // wait at least 2 seconds with Analog Discovery for the offset to stabilize, before the first reading after device open or offset/range change
    Wait(2);
    fp = fopen("/home/nick/Desktop/test.txt", "w+");

 	    for(int i=0;i<=50;i++){
	    double DCbias =(float)i/10.0;
	    FDwfAnalogIOChannelNodeSet(hdwf, 0, 1, DCbias);//power supply voltage
	    Wait(1);

      	    /////////////start acquisition////////////////
	    FDwfAnalogInConfigure(hdwf, 0, true);
  	    FDwfAnalogOutConfigure(hdwf, 0, true);
	    
	    printf("Waiting for triggered or auto acquisition\n");
	    do{
		FDwfAnalogInStatus(hdwf, true, &sts);
	    }while(sts != stsDone);
	    
	    // get the samples from input channel
	    FDwfAnalogInStatusData(hdwf, 0, rgdSamples, 32); //use 32 samples over which to average
	    FDwfAnalogInConfigure(hdwf, 0, false);
  	    FDwfAnalogOutConfigure(hdwf, 0, false);
	    //////////////end acquisition/////////////////

		   double average = 0.0;
		    for(int j=0;j<32;j++){
			average+=rgdSamples[j]; //sun the 32 datapoints, which will then be divided and this average used as the final value...
			}
			fprintf(fp, "%f %f\n", average/32.0, (DCbias-average/32.0)/323.0);//print voltage across and current through diode
			
	   }
   	fclose(fp);

    // close the device
    FDwfDeviceCloseAll();
}
