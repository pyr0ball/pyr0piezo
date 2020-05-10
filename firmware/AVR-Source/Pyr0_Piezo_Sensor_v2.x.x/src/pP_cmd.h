#ifndef PP_CMD_H
#define PP_CMD_H

void updateGainFactor(int value);
void updateVFol(int value);
void updateVComp(int value);
void updateLoopDuration(int value);
void updateTrigDuration(int value);
void updateHysteresis(int value);
void updateLogic(int value);
void updatePzDet(int value);
void updateVccSwitch(int value);
void updateConstant(long value);
void adjustConstant(int value);
void updateDebug(int value);

#endif // PP_CMD_H
