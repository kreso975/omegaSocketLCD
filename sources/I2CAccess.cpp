#include "I2CAccess.h"

I2CAccess::I2CAccess() {
    lastResult = I2C_OK;
}

I2C_Result I2CAccess::getLastResult() {
    return lastResult;
}
