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
    _valueCurrent = 0;
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

void HWChannel::task() {
    _currentMillis = millis();
    switch (_currentTask) {
        case DIM_STOP:
            handleDimStop();
            break;
        case DIM_ON:
            handleDimOn();
            break;
        case DIM_OFF:
            handleDimOff();
            break;
        case DIM_SOFTON:
            handleDimSoftOn();
            break;
        case DIM_SOFTOFF:
            handleDimSoftOff();
            break;
        case DIM_UP:
            handleDimUp();
            break;
        case DIM_DOWN:
            handleDimDown();
            break;
        case DIM_SET:
            handleDimSet();
            break;
        case DIM_VALUE:
            handleDimValue();
            break;
        case DIM_IDLE:
        default:
            break;
    }
}

void HWChannel::handleDimStop() {
    _busy = false;
    _updateAvailable = true;
    _updateCounter = 0;
    _currentTask = DIM_IDLE;
}

void HWChannel::handleDimOn() {
    if (_valueCurrent != _valueMax) {
        _valueCurrent = _valueMax;
        setChannelValue(_valueCurrent);
        _currentTask = DIM_STOP;
    }
}

void HWChannel::handleDimOff() {
    if (_valueCurrent != _valueMin) {
        _valueCurrent = _valueMin;
        setChannelValue(_valueCurrent);
        _currentTask = DIM_STOP;
    }
}

void HWChannel::handleDimSoftOn() {
    if (_valueCurrent < _valueMax) {
        if (!_busy) {
            _delayAbsolute = (word)(_durationAbsolute / (_valueMax - _valueCurrent));
        }
        updateValue(1, _delayAbsolute);
    } else {
        _currentTask = DIM_STOP;
    }
}

void HWChannel::handleDimSoftOff() {
    if (_valueCurrent > _valueMin) {
        if (!_busy) {
            _delayAbsolute = (word)(_durationAbsolute / (_valueCurrent - _valueMin));
        }
        updateValue(-1, _delayAbsolute);
    } else {
        _currentTask = DIM_STOP;
    }
}

void HWChannel::handleDimUp() {
    if (_valueCurrent < _valueMax) {
        if (!_busy) {
            _delayRelative = (word)(_durationRelative / (_valueMax - _valueCurrent));
        }
        updateValue(1, _delayRelative);
    } else {
        _currentTask = DIM_STOP;
    }
}

void HWChannel::handleDimDown() {
    if (_valueCurrent > _valueMin) {
        if (!_busy) {
            _delayRelative = (word)(_durationRelative / (_valueCurrent - _valueMin));
        }
        updateValue(-1, _delayRelative);
    } else {
        _currentTask = DIM_STOP;
    }
}

void HWChannel::handleDimSet() {
    if (_valueCurrent < _valueNew) {
        if (!_busy) {
            _delayAbsolute = (word)(_durationAbsolute / (_valueNew - _valueCurrent));
        }
        updateValue(1, _delayAbsolute);
    } else if (_valueCurrent > _valueNew) {
        if (!_busy) {
            _delayAbsolute = (word)(_durationAbsolute / (_valueCurrent - _valueNew));
        }
        updateValue(-1, _delayAbsolute);
    } else {
        _currentTask = DIM_STOP;
    }
}

void HWChannel::handleDimValue() {
    if (_valueCurrent != _valueSetNew) {
        _valueCurrent = _valueSetNew;
        setChannelValue(_valueCurrent);
        _currentTask = DIM_STOP;
    }
}

void HWChannel::updateValue(int8_t step, uint32_t delay) {
    if (_currentMillis - _lastTaskExecution >= delay) {
        _valueCurrent += step;
        _busy = true;
        _updateCounter++;
        if (_updateCounter >= _updateInterval) {
            _updateAvailable = true;
            _updateCounter = 0;
        }
        setChannelValue(_valueCurrent);
        _lastTaskExecution = millis();
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
    if (valueNew > _valueMax || valueNew < _valueMin) {
        return; // Ungültiger Wert, keine Aktion
    }
    _valueNew = valueNew;
    _currentTask = DIM_SET;
}

void HWChannel::taskSetValue(byte valueNew) {
    if (valueNew > _valueMax || valueNew < _valueMin) {
        return; // Ungültiger Wert, keine Aktion
    }
    _valueSetNew = valueNew;
    _currentTask = DIM_VALUE;
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