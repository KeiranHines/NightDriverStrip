//+--------------------------------------------------------------------------
//
// File:        TileableDigitalClock.h
//
// NightDriverStrip - (c) 2023 Plummer's Software LLC.  All Rights Reserved.
//
// This file is part of the NightDriver software project.
//
//    NightDriver is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    NightDriver is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with Nightdriver.  It is normally found in copying.txt
//    If not, see <https://www.gnu.org/licenses/>.
//
// Description:
//
//   Defines a proof of concept effect for rendering a digital clock anywhere
//   on the given LED matrix. This clock is based off the work from the
//   Pong clock effect
//
// History:     Sep-10-2023         KeiranHines      Creatation of POC Design
//
//---------------------------------------------------------------------------

/*
 *
 * RGB Pong Clock - Andrew Holmes @pongclock
 * https://github.com/pkourany/RGBPongClock
 *
 * Inspired by, and shamelessly derived from
 *     Nick's LED Projects
 * https://123led.wordpress.com/about/
 *
 * Modified for SmartMatrix by Jason Coon
 *
 * Aurora: https://github.com/pixelmatix/aurora
 * Copyright (c) 2014 Jason Coon
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "effects/matrix/tileable/TileableEffectWidget.h"
#include "systemcontainer.h"

#ifndef TileableDigitalClock_H
#define TileableDigitalClock_H

const int default_w = 26;
const int default_h = 8;

class TileableDigitalClock : public TileableEffectWidget
{
  private:
    uint8_t mins;
    uint8_t hours;

  public:
    TileableDigitalClock(int x, int y) : TileableEffectWidget(x, y)
    {
        _w = default_w;
        _h = default_h;
    }

    TileableDigitalClock(int x, int y, int scale) : TileableEffectWidget(x, y, scale)
    {
        // Scaling not supported by font library. 
        _w = default_w;
        _h = default_h;
    }

    TileableDigitalClock(int x, int y, int w, int h) : TileableEffectWidget(x, y, w, h)
    {

        // Scaling not supported by font library.
        _w = default_w;
        _h = default_h;
    }

    void Start() override
    {
        time_t ttime = time(0);
        tm *local_time = localtime(&ttime);

        bool ampm = !g_ptrSystem->DeviceConfig().Use24HourClock();

        // update score / time
        mins = local_time->tm_min;
        hours = local_time->tm_hour;
        if (hours > 12 && ampm)
            hours -= 12;
        else if (hours == 0 && ampm)
            hours = 12;
    }

    void Draw() override
    {
        const time_t ttime = time(0);
        const tm *local_time = localtime(&ttime);

        // if time no longer matches, update time.
        if ((local_time->tm_min != mins))
        {
            bool ampm = !g_ptrSystem->DeviceConfig().Use24HourClock();

            mins = local_time->tm_min;
            hours = local_time->tm_hour;
            if (hours > 12 && ampm)
                hours -= 12;
            else if (hours == 0 && ampm)
                hours = 12;
        }

        // draw hh:mm separator colon that blinks once per second
        const int center_w = _x + (_w / 2);
        const int center_h = _y + (_h / 2);
        if (local_time->tm_sec % 2 == 0)
        {
            g()->setPixel(center_w - 1, center_h, RED16);
            g()->setPixel(center_w - 1, center_h + 2, RED16);
        }

        LEDMatrixGFX::backgroundLayer.setFont(gohufont11b);

        // The compiler warns that with a nul terminator, 4 bytes could be needed, which is true
        // but we're only writing 3 bytes, but I'll waste the byte to avoid the warning.
        char buffer[4];

        auto clockColor = rgb24(255, 255, 255);
        sprintf(buffer, "%2d", hours);
        // TODO Font Size respecting _w and _h
        LEDMatrixGFX::backgroundLayer.drawString(_x, _y, clockColor, buffer);

        sprintf(buffer, "%02d", mins);
        LEDMatrixGFX::backgroundLayer.drawString(center_w + 1, _y, clockColor, buffer);
    }
};

#endif