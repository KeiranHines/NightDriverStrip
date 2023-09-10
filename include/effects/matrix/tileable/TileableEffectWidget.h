//+--------------------------------------------------------------------------
//
// File:        TileableWidgetEffect.h
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
//   Defines the base class for effect that are managed by the
//   PatternTilableManager effect class.
//
// History:     Sep-10-2023         KeiranHines      Creatation of POC Design
//
//---------------------------------------------------------------------------

#ifndef TileableEffectWidget_H
#define TileableEffectWidget_H
#include "gfxbase.h"
#include <memory>
#include <stdlib.h>
#include <vector>

class TileableEffectWidget
{
  protected:
    size_t _cLEDs = 0;
    std::vector<std::shared_ptr<GFXBase>> _GFX;
    int _x;
    int _y;
    int _w;
    int _h;

  public:
    // Constructor using the widgets default size and scale.
    TileableEffectWidget(int x, int y)
    {
        _x = x;
        _y = y;
        _w = MATRIX_WIDTH;
        _h = MATRIX_HEIGHT;
    }

    // Constructor using the widgets default size with a new scale.
    TileableEffectWidget(int x, int y, int scale)
    {
        _x = x;
        _y = y;
        _w = MATRIX_WIDTH * scale;
        _h = MATRIX_HEIGHT * scale;
    }

    // Constructor using a custom size.
    TileableEffectWidget(int x, int y, int w, int h)
    {
        _x = x;
        _y = y;
        _w = w;
        _h = h;
    }

    virtual void Start()
    {
    }
    virtual void Draw()
    {
    }
    virtual ~TileableEffectWidget()
    {
    }
    std::shared_ptr<GFXBase> g(size_t channel = 0) const
    {
        return _GFX[channel];
    }
    virtual bool Init(std::vector<std::shared_ptr<GFXBase>> &gfx)
    {
        _GFX = gfx; // There are up to 8 channel in play per effect and when we
                    //   start up, we are given copies to their graphics interfaces
                    //   so that we can call them directly later from other calls
        _cLEDs = _GFX[0]->GetLEDCount();

        return true;
    }
};

#endif