#include "HwChannel.h"

HWChannel::HWChannel(uint8_t iId)
{
    mId = iId;
}

HWChannel::~HWChannel() {}

void HWChannel::setup(uint8_t hwchannel, uint8_t curve, uint16_t durationabsolute, uint16_t durationrelative)
{
    _hwchannel = hwchannel;
    _curve = curve;

    // initial parameter for dimmer control
    _currentTask = DIM_IDLE;
    _updateAvailable = false;
    _valueMin = 0;
    _valueMax = 255;
    _valueNew = 0;
    _updateCounter = 0;
    _busy = false;
    _durationAbsolute = durationabsolute;
    _durationRelative = durationrelative;
    _lastTaskExecution = 0;

    // calculate the update interval
    calculateUpdateInterval();
}

void HWChannel::setChannelValue(byte value)
{
    LEDModule::_instance->setHwChannelValue(_hwchannel, value, _curve);
}

void HWChannel::task()
{
    _currentMillis = millis();
    switch (_currentTask)
    {
    // stop all activities
    case DIM_STOP:
        _busy = false;
        _updateAvailable = true;
        _updateCounter = 0;
        _currentTask = DIM_IDLE;
        // send channel information to ko

        break;
    // turn on immediately
    case DIM_ON:
        if (_valueCurrent != _valueMax)
        {
            _valueCurrent = _valueMax;
            setChannelValue(_valueCurrent);
            _currentTask = DIM_STOP;
        }
        break;
    // turn off immediately
    case DIM_OFF:
        if (_valueCurrent != _valueMin)
        {
            _valueCurrent = _valueMin;
            setChannelValue(_valueCurrent);
            _currentTask = DIM_STOP;
        }
        break;
    // smooth turn on
    case DIM_SOFTON:
        if (_valueCurrent < _valueMax)
        {
            if (!_busy)
            {
                _delayAbsolute = (word)(_durationAbsolute / (_valueMax - _valueCurrent));
            }
            if (_currentMillis - _lastTaskExecution >= _delayAbsolute)
            {
                _valueCurrent++;
                _busy = true;
                setChannelValue(_valueCurrent);
                _lastTaskExecution = millis();
            }
        }
        else
        {
            _currentTask = DIM_STOP;
        }
        break;
    // smooth turn off
    case DIM_SOFTOFF:
        if (_valueCurrent > _valueMin)
        {
            if (!_busy)
            {
                _delayAbsolute = (word)(_durationAbsolute / (_valueCurrent - _valueMin));
            }
            if (_currentMillis - _lastTaskExecution >= _delayAbsolute)
            {
                _valueCurrent--;
                _busy = true;
                setChannelValue(_valueCurrent);
                _lastTaskExecution = millis();
            }
        }
        else
        {
            _currentTask = DIM_STOP;
        }
        break;
        //increase value
        case DIM_UP:
            if(_valueCurrent < _valueMax){
                if(!_busy){
                    _delayRelative = (word)(_durationRelative / (_valueMax - _valueCurrent));
                }
                if(_currentMillis - _lastTaskExecution >= _delayRelative){
                    _valueCurrent++;
                    _busy = true;
                    _updateCounter++;
                    if(_updateCounter >= _updateInterval){
                        _updateAvailable  = true;
                        _updateCounter = 0;
                    }
                    setChannelValue(_valueCurrent);
                    _lastTaskExecution = millis();
                }
            }else{
                _currentTask = DIM_STOP;
            }
            break;
        //decrease value
        case DIM_DOWN:
            if(_valueCurrent > _valueMin){
                if(!_busy){
                    _delayRelative = (word)(_durationRelative / (_valueCurrent - _valueMin));
                }
                if(_currentMillis - _lastTaskExecution >= _delayRelative){
                    _valueCurrent--;
                    _busy = true;
                    _updateCounter++;
                    if(_updateCounter >= _updateInterval){
                        _updateAvailable  = true;
                        _updateCounter = 0;
                    }
                    setChannelValue(_valueCurrent);
                    _lastTaskExecution = millis();
                }
            }else{
                _currentTask = DIM_STOP;
            }
            break;
    // set value
    case DIM_SET:
        if (_valueCurrent < _valueNew)
        {
            if (!_busy)
            {
                _delayAbsolute = (word)(_durationAbsolute / (_valueNew - _valueCurrent));
            }
            if (_currentMillis - _lastTaskExecution >= _delayAbsolute)
            {
                _valueCurrent++;
                _busy = true;
                setChannelValue(_valueCurrent);
                _lastTaskExecution = millis();
            }
        }
        else if (_valueCurrent > _valueNew)
        {
            if (!_busy)
            {
                _delayAbsolute = (word)(_durationAbsolute / (_valueCurrent - _valueNew));
            }
            if (_currentMillis - _lastTaskExecution >= _delayAbsolute)
            {
                _valueCurrent--;
                _busy = true;
                setChannelValue(_valueCurrent);
                _lastTaskExecution = millis();
            }
        }
        else
        {
            _currentTask = DIM_STOP;
        }
        break;
    case DIM_IDLE:
    default:
        break;
    }
}

void HWChannel::calculateUpdateInterval() {
    _updateInterval = (uint8_t)((_valueMax - _valueMin) / 10) + 1;
}

void HWChannel::taskStop() {
    _currentTask = DIM_STOP;
}

void HWChannel::taskOn() {
    _currentTask = DIM_ON;
}

void HWChannel::taskOff() {
    _currentTask = DIM_OFF;
}

void HWChannel::taskSoftOn() {
    _currentTask = DIM_SOFTON;
}

void HWChannel::taskSoftOff() {
    _currentTask = DIM_SOFTOFF;
}

void HWChannel::taskDimUp(){
    _currentTask = DIM_UP;
}

void HWChannel::taskDimDown(){
    _currentTask = DIM_DOWN;
}

void HWChannel::taskNewValue(byte valueNew) {
    _valueNew = valueNew;
    _currentTask = DIM_SET;
}

bool HWChannel::isBusy() {
    return _busy;
}

byte HWChannel::getCurrentValue() {
    return _valueCurrent;
}

bool HWChannel::updateAvailable() {
    return _updateAvailable;
}

void HWChannel::resetUpdateFlag() {
    _updateAvailable = false;
}