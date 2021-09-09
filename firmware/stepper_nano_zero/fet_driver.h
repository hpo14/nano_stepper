/**********************************************************************
    Copyright (C) 2018  MisfitTech LLC,  All rights reserved.

    MisfitTech uses a dual license model that allows the software to be used under
    a standard GPL open source license, or a commercial license.  The standard GPL
    license  requires that all software statically linked with MisfitTec Code is
    also distributed under the same GPL V2 license terms.  Details of both license
    options follow:

    - Open source licensing -
    MisfitTech is a free download and may be used, modified, evaluated and
    distributed without charge provided the user adheres to version two of the GNU
    General Public License (GPL) and does not remove the copyright notice or this
    text.  The GPL V2 text is available on the gnu.org web site

    - Commercial licensing -
    Businesses and individuals that for commercial or other reasons cannot comply
    with the terms of the GPL V2 license must obtain a low cost commercial license
    before incorporating MisfitTech code into proprietary software for distribution in
    any form.  Commercial licenses can be purchased from www.misfittech.net
    and do not require any source files to be changed.


    This code is distributed in the hope that it will be useful.  You cannot
    use MisfitTech's code unless you agree that you use the software 'as is'.
    MisfitTech's code is provided WITHOUT ANY WARRANTY; without even the implied
    warranties of NON-INFRINGEMENT, MERCHANTABILITY or FITNESS FOR A PARTICULAR
    PURPOSE. MisfitTech LLC disclaims all conditions and terms, be they
    implied, expressed, or statutory.


    Written by Trampas Stern for MisfitTech.

    Misfit Tech invests time and resources providing this open source code,
    please support MisfitTech and open-source hardware by purchasing
    products from MisfitTech, www.misifittech.net!
 *********************************************************************/

#ifndef FET_DRIVER_H_
#define FET_DRIVER_H_

#include <Arduino.h>
#include "board.h"
#include "angle.h"
#include "sine.h"

#ifdef NEMA_23_10A_HW
#define FET_DRIVER_NUM_MICROSTEPS (SINE_STEPS / 4) // number of steps to use for microstepping, default is 256
#define FET_DRIVER_NUM_ZERO_AVG   (100)

#define FET_ADC_TO_MA(x) (((x)*2537) / 1000)
#define FET_MA_TO_ADC(x) (((x)*1000) / 2537)
// prvent someone for making a mistake with the code
#if ((FET_DRIVER_NUM_MICROSTEPS * 4) != SINE_STEPS)
#error "SINE_STEPS must be 4x of Micro steps for the move function"
#endif

/*
 *  When it comes to the stepper driver if we use angles
 *  we will always have a rounding error. For example
 *  a 0-65536(360) angle for 1.8 degree step is 327.68 so
 *  if you increment 200 of these as 327 you have a 13.6 error
 *  after one rotation.
 *  If you use floating point the effect is the same but takes longer.
 *
 *  The only error-less accumulation system is to use native units, ie full
 *  steps and microsteps.
 *
 */

class FetDriver
{
    static FetDriver *ptrInstance;

private:
    uint32_t lastStepMicros; // time in microseconds that last step happened

    int32_t PWM_Table_B[512];
    int32_t PWM_Table_A[512];

    bool forwardRotation = true;
    volatile bool enabled = true;

    volatile int32_t adc;

    volatile int32_t coilB_value = 0;
    volatile int32_t coilB_Zero = -1;
    volatile int32_t coilB_SetPoint = 100;
    volatile int32_t coilB_error = 0;

    volatile int32_t coilA_value = 0;
    volatile int32_t coilA_Zero = -1;
    volatile int32_t coilA_SetPoint = 200;
    volatile int32_t coilA_error = 0;
    void ctrl_update(uint16_t channel, uint16_t value);
    void measureCoilB_zero(void);
    void measureCoilA_zero(void);
    void CalTableB(int32_t maxMA);
    void CalTableA(int32_t maxMA);
    int coilA_PWM(int32_t value);
    void coilB_PWM(int32_t value);
    int32_t getCoilB_mA(void);
    int32_t getCoilA_mA(void);

public:
    static void ADC_Callback(uint16_t channel, uint16_t value);
    void begin(void);

    // moves motor where the modulo of A4954_NUM_MICROSTEPS is a full step.
    int32_t move(int32_t stepAngle, uint32_t mA);

    uint32_t microsSinceStep(void) { return micros() - lastStepMicros; };
    void setRotationDirection(bool forward) { forwardRotation = forward; };

    void enable(bool enable) { enabled = enable; };
    void limitCurrent(uint8_t x) { return; };
};

#endif // #ifdef NEMA_23_10A_HW
#endif /* FET_DRIVER_H_ */
