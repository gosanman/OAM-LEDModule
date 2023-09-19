#include <LEDHelper.h>

/*

void rgbToHsv(byte r, byte g, byte b, byte hsvColor[]){
    byte min = min(min(r,g),b);
    byte max = max(max(r,g),b);
    byte diff = max - min;
    hsvColor[2] = max;
    if (hsvColor[2] == 0){
        hsvColor[0] = 0;
        hsvColor[1] = 0;
    }else{
        hsvColor[1] = 255 * (word)diff / hsvColor[2];
        if (hsvColor[1] == 0){
            hsvColor[0] = 0;
        }else{
            if (r == max)
                hsvColor[0] = 0 + ((g - b) * 43) / diff;
            else if (g == max)
                hsvColor[0] = 85 + ((b - r) * 43) / diff;
            else
                hsvColor[0] = 171 + ((r - g) * 43) / diff;
        }
    }
}

void hsvToRgb(byte h, byte s, byte v, byte rgbColor[]){
    if(s){
        byte i = h / 43;
        byte f = (h - (i * 43)) * 6; 
        byte p = (v * (255 - s)) >> 8;
        byte q = (v * (255 - ((s * f) >> 8))) >> 8;
        byte t = (v * (255 - ((s * (255 - f)) >> 8))) >> 8;
        switch (i){
            case 0:
                rgbColor[0] = v; 
                rgbColor[1] = t; 
                rgbColor[2] = p;
                break;
            case 1:
                rgbColor[0] = q; 
                rgbColor[1] = v; 
                rgbColor[2] = p;
                break;
            case 2:
                rgbColor[0] = p; 
                rgbColor[1] = v; 
                rgbColor[2] = t;
                break;
            case 3:
                rgbColor[0] = p; 
                rgbColor[1] = q; 
                rgbColor[2] = v;
                break;
            case 4:
                rgbColor[0] = t; 
                rgbColor[1] = p; 
                rgbColor[2] = v;
                break;
            default:
                rgbColor[0] = v; 
                rgbColor[1] = p; 
                rgbColor[2] = q;
                break;
        }
    }else{
        rgbColor[0] = v;
        rgbColor[1] = v;
        rgbColor[2] = v;
    }
}

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


void setAllHsv(byte h, byte s, byte v){
    dbg_print(F("setAllHsv H: %d, S: %d, V: %d"), h, s, v);
    currentTask = TASK_IDLE; //TASK_IDLE
    byte newRGB[3];
    hsvToRgb(h, s, v, newRGB);
 
    for(int i = 0; i < numberLeds; i++){
        neopixels->setPixelColor(i, newRGB[R], newRGB[G], newRGB[B]);
        pixelsShow = true;
    }
}


*/