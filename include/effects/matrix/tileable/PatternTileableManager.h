//+--------------------------------------------------------------------------
//
// File:        PatternTilableManager.h
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
//   Effect code for managing multiple effect layers. This Class would be the
//   main entry-point for LEDStrip effects with each layer being a subclass
//   of TileableLEDStripEffect.
//
// History:     Sep-10-2023         KeiranHines      Creatation of POC Design
//
//---------------------------------------------------------------------------

#ifndef PatternTileableManager_H
#define PatternTileableManager_H

#include "TileableEffectWidget.h"
#include "systemcontainer.h"
#include "types.h"
#include "WidgetDependencies.h"


class PatternTileableManager : public LEDStripEffect
{
  private:
    std::vector<std::shared_ptr<TileableEffectWidget>> layers;

  public:
    PatternTileableManager() : LEDStripEffect(EFFECT_MATRIX_TILEABLE, "Tileable")
    {
    }

    PatternTileableManager(const JsonObjectConst &jsonObject) : LEDStripEffect(jsonObject)
    {
    }

    virtual size_t DesiredFramesPerSecond() const override
    {
        return 35;
    }

    bool RequiresDoubleBuffering() const override
    {
        return true;
    }

    void Start() override
    {
        // TODO add ability to load config from filesystem.
        const std::shared_ptr<TileableEffectWidget> pong = make_shared_psram<TileablePong>(0, 0, 32, 32);
        const std::shared_ptr<TileableEffectWidget> pong2 = make_shared_psram<TileablePong>(32, 0, 32, 32);
        layers.push_back(pong);
        layers.push_back(pong2);
        const std::shared_ptr<TileableEffectWidget> clock = make_shared_psram<TileableDigitalClock>(20, 0);
        layers.push_back(clock);
        
        for (unsigned i = 0; i < layers.size(); i++)
        {
            const std::shared_ptr<TileableEffectWidget> layer = layers[i];
            layer->Init(_GFX);
            layer->Start();
        }
    }

    void Draw() override
    {
        g()->Clear();
        for (unsigned i = 0; i < layers.size(); i++)
        {
            const std::shared_ptr<TileableEffectWidget> layer = layers[i];
            layer->Draw();
        }
    }
};

#endif