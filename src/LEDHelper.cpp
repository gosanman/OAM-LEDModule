#include "LEDHelper.h"

/*

word getLogValue(byte index, float gamma, byte startValue, word maxValue, word steps){
    if (index >0){
        word result = round(pow((float)index / (float)(steps - 1.0), gamma) * (float)(maxValue - startValue) + startValue);
        if(result > maxValue)
            return maxValue;
        else
            return result;
    }else{
        return 0;
    }
}

void setDimmingCurves(){
    word maxSteps = 256;
    for(word i = 0; i < maxSteps; i++){
        curveR[i] = getLogValue(i, gammaCorrection, firstOnValue, maxR, maxSteps);
        curveG[i] = getLogValue(i, gammaCorrection, firstOnValue, maxG, maxSteps);
        curveB[i] = getLogValue(i, gammaCorrection, firstOnValue, maxB, maxSteps);
        curveW[i] = getLogValue(i, gammaCorrection, firstOnValue, maxW, maxSteps);
    }
}



*/