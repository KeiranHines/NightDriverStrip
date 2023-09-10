//+--------------------------------------------------------------------------
//
// File:        TileablePong.h
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
//
// Description:
//
//   Effect code ported from PatternPongClock, moved clock into its own
//   widget.
//   TODO: Add scaling to pong.
//
// History:     Sep-10-2023         KeiranHines      PatternPongClock
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

#ifndef TileablePong_H
#define TileablePong_H

#include "effects/matrix/tileable/TileableEffectWidget.h"
#include "systemcontainer.h"

#define SPEEDUP 1.15
#define MAXSPEED 4.0f
#define MAX_TURNS 100
int color = 0;
class TileablePong : public TileableEffectWidget
{
  private:
    // Pong left bat x pos (this is where the ball collision occurs, the bat is drawn 1 behind these coords)
    rgb24 bat_color = rgb24(255, 255, 255);
    u16_t ball_color = WHITE16;
    int bat1_x;
    int bat2_x;
    int bat_height;
    float ballpos_x, ballpos_y;
    uint8_t erase_x = 10; // holds ball old pos so we can erase it, set to blank area of screen initially.
    uint8_t erase_y = 10;
    float ballvel_x, ballvel_y;
    int bat1_y = 5; // bat starting y positions
    int bat2_y = 5;
    int bat1_target_y = 5; // bat targets for bats to move to
    int bat2_target_y = 5;
    uint8_t bat1_update = 1; // flags - set to update bat position
    uint8_t bat2_update = 1;
    uint8_t restart = 1;
    uint8_t hits = 0;   // number of times the ball has been hit this round, initially 0;
    uint8_t end_at = 0; // number of hits the round should end at.
  public:
    TileablePong(int x, int y) : TileableEffectWidget(x, y)
    {
    }
    TileablePong(int x, int y, int w, int h) : TileableEffectWidget(x, y, w, h)
    {
      if (color == 0) {
        color = 1;
      } else {
        bat_color = rgb24(189, 23, 11);
        ball_color = CYAN16;
      }
    }
    void Start() override
    {
        bat1_x = _x + 1;
        bat2_x = (_x + _w) - 4;
        bat_height = (_h / 4);
    }

    void Draw() override
    {
        const uint8_t center = _x + (_w / 2);

        // draw pitch centre line
        for (uint16_t i = 0; i < _h; i += 2)
            g()->setPixel(center, i, 0x6666);

        // if restart flag is 1, set up a new game
        if (restart)
        {
            // set ball start pos
            ballpos_x = center;
            ballpos_y = random(4, _h - 4);
            ballvel_x = 0;

            // pick random ball direction
            if (random(0, 2) > 0)
            {
                ballvel_x = 1;
            }
            else
            {
                ballvel_x = -1;
            }

            if (random(0, 2) > 0)
            {
                ballvel_y = 0.5;
            }
            else
            {
                ballvel_y = -0.5;
            }

            // reset game restart flag
            restart = 0;
            hits = 0;
            end_at = random(0, MAX_TURNS);
        }

        // AI - we run 2 sets of 'AI' for each bat to work out where to go to hit the ball back
        // very basic AI...
        //  For each bat, First just tell the bat to move to the height of the ball when we get to a random location.
        // for bat1

        if (ballpos_x == random(center + 2, _x + _w))
        {
            bat1_target_y = ballpos_y;
        }
        // for bat2
        if (ballpos_x == random(4, center))
        {
            bat2_target_y = ballpos_y;
        }

        // Define the "Thinking zone" around center.  Player will set their targets based on what they see here.

        constexpr float LOOKAHEAD = 1.0;
        float leftEdge = center - MAXSPEED * LOOKAHEAD;
        float rightEdge = center + MAXSPEED * LOOKAHEAD;

        // If ball going leftwards towards BAT1,

        if (ballvel_x < 0 && ballpos_x > leftEdge && ballpos_x < rightEdge)
        {

            uint8_t end_ball_y = pong_get_ball_endpoint(ballpos_x, ballpos_y, ballvel_x, ballvel_y);

            // if we have hit the ball enough to end the round,
            // then the bat needs to miss the ball when it gets to end_ball_y
            if (hits >= end_at)
            {
                if (end_ball_y > _h / 2)
                {
                    bat1_target_y = random(0, 3);
                }
                else
                {
                    bat1_target_y = 8 + random(0, 3);
                }
            }
            // if the miss flag isn't set,  set bat target to ball end point with some randomness, so it's not always
            // hitting top of bat
            else
            {
                bat1_target_y = end_ball_y - random(0, bat_height);
                if (bat1_target_y < 0)
                    bat1_target_y = 0;
                if (bat1_target_y > _h - bat_height)
                    bat1_target_y = _h - bat_height;
            }
        }

        // right bat AI
        // if positive velocity then predict for right bat - first just match ball height
        // when the ball is closer to the right bat, run the ball maths to find out where it will land

        if (ballvel_x > 0 && ballpos_x > leftEdge && ballpos_x < rightEdge)
        {
            uint8_t end_ball_y = pong_get_ball_endpoint(ballpos_x, ballpos_y, ballvel_x, ballvel_y);

            // if we have hit the ball enough to end the round.
            if (hits >= end_at)
            {
                // if ball end point above 8 then move bat down, else move it up- so either way it misses
                if (end_ball_y > _h / 2)
                {
                    bat2_target_y = random(0, 3);
                }
                else
                {
                    bat2_target_y = _h / 2 + random(0, 3);
                }
            }
            else
            {
                // set bat target to ball end point with some randomness
                bat2_target_y = end_ball_y - random(0, bat_height);
                if (bat2_target_y < 0)
                    bat2_target_y = 0;
                if (bat2_target_y > _h - bat_height)
                    bat2_target_y = _h - bat_height;
            }
        }

        // move bat 1 towards target
        // if bat y greater than target y move down until hit 0 (don't go any further or bat will move off screen)
        if (bat1_y > bat1_target_y && bat1_y > 0)
        {
            bat1_y--;
            bat1_update = 1;
        }

        // if bat y less than target y move up until hit 10 (as bat is 6)
        if (bat1_y < bat1_target_y && bat1_y < _h - bat_height)
        {
            bat1_y++;
            bat1_update = 1;
        }

        // draw bat 1
        //       if (bat1_update) {
        LEDMatrixGFX::backgroundLayer.fillRectangle(bat1_x - 1, bat1_y, bat1_x, bat1_y + bat_height, bat_color);
        //      }

        // move bat 2 towards target (don't go any further or bat will move off screen)
        // if bat y greater than target y move down until hit 0
        if (bat2_y > bat2_target_y && bat2_y > 0)
        {
            bat2_y--;
            bat2_update = 1;
        }

        // if bat y less than target y move up until hit max of 10 (as bat is 6)
        if (bat2_y < bat2_target_y && bat2_y < _h - 6)
        {
            bat2_y++;
            bat2_update = 1;
        }

        LEDMatrixGFX::backgroundLayer.fillRectangle(bat2_x + 1, bat2_y, bat2_x + 2, bat2_y + bat_height, bat_color);

        // update the ball position using the velocity
        ballpos_x = ballpos_x + ballvel_x;
        ballpos_y = ballpos_y + ballvel_y;

        // check ball collision with top and bottom of screen and reverse the y velocity if either is hit
        if (ballpos_y <= 0)
        {
            ballpos_y *= -1;
            ballvel_y *= -1;
        }

        if (ballpos_y >= _h)
        {
            ballpos_y = 2 * _h - ballpos_y;
            ballvel_y *= -1;
        }

        // check for ball collision with bat1. check ballx is same as batx
        // and also check if bally lies within width of bat i.e. baty to baty + 6. We can use the exp if(a < b && b < c)
        if ((int)ballpos_x <= bat1_x + 1 && (bat1_y <= (int)ballpos_y && (int)ballpos_y <= bat1_y + bat_height - 1))
        {

            hits++;
            ballpos_x = bat1_x + 1;

            // random if bat flicks ball to return it - and therefor changes ball velocity
            if (!random(0, 3))
            { // not true = no flick - just straight rebound and no change to ball y vel
                ballvel_x = ballvel_x * -SPEEDUP;
                ballvel_x = std::max(ballvel_x, -MAXSPEED);
                ballvel_x = std::min(ballvel_x, MAXSPEED);
            }
            else
            {
                bat1_update = 1;
                uint8_t flick; // 0 = up, 1 = down.

                if (bat1_y > 1 || bat1_y < _h / 2)
                {
                    flick = random(0, 2); // pick a random dir to flick - up or down
                }

                // if bat 1 or 2 away from top only flick down
                if (bat1_y <= 1)
                {
                    flick = 0; // move bat down 1 or 2 pixels
                }
                // if bat 1 or 2 away from bottom only flick up
                if (bat1_y >= _h / 2)
                {
                    flick = 1; // move bat up 1 or 2 pixels
                }

                switch (flick)
                {
                // flick up
                case 0:
                    bat1_target_y = bat1_target_y + random(1, 3);
                    ballvel_x = ballvel_x * -1;
                    if (ballvel_y < 2)
                    {
                        ballvel_y = ballvel_y + 0.5;
                    }
                    break;

                    // flick down
                case 1:
                    bat1_target_y = bat1_target_y - random(1, 3);
                    ballvel_x = ballvel_x * -1;
                    if (ballvel_y > 0.5)
                    {
                        ballvel_y = ballvel_y - 0.5;
                    }
                    break;
                }
            }
        }

        // check for ball collision with bat2. check ballx is same as batx
        // and also check if bally lies within width of bat i.e. baty to baty + 6. We can use the exp if(a < b && b < c)
        if ((int)ballpos_x >= bat2_x && (bat2_y <= (int)ballpos_y && (int)ballpos_y <= bat2_y + bat_height - 1))
        {
            hits++;
            ballpos_x = bat2_x;

            // random if bat flicks ball to return it - and therefor changes ball velocity
            if (!random(0, 3))
            {
                ballvel_x =
                    ballvel_x * -SPEEDUP; // not true = no flick - just straight rebound and no change to ball y vel
                ballvel_x = std::max(ballvel_x, -MAXSPEED);
                ballvel_x = std::min(ballvel_x, MAXSPEED);
            }
            else
            {
                bat1_update = 1;
                uint8_t flick; // 0 = up, 1 = down.

                if (bat2_y > 1 || bat2_y < _h / 2)
                    flick = random(0, 2); // pick a random dir to flick - up or down

                // if bat 1 or 2 away from top only flick down

                if (bat2_y <= 1)
                    flick = 0; // move bat up 1 or 2 pixels

                // if bat 1 or 2 away from bottom only flick up

                if (bat2_y >= _h / 2)
                    flick = 1; // move bat down 1 or 2 pixels

                switch (flick)
                {
                // flick up
                case 0:
                    bat2_target_y = bat2_target_y + random(1, 3);
                    ballvel_x = ballvel_x * -1;
                    if (ballvel_y < 2)
                        ballvel_y = ballvel_y + random(1.0) + 0.5;
                    break;

                    // flick down
                case 1:
                    bat2_target_y = bat2_target_y - random(1, 3);
                    ballvel_x = ballvel_x * -1;
                    if (ballvel_y > 0.5)
                        ballvel_y = ballvel_y - random(1.0) - 0.5;
                    break;
                }
            }
        }

        // plot the ball on the screen
        uint8_t plot_x = (int)(ballpos_x + 0.5f);
        uint8_t plot_y = (int)(ballpos_y + 0.5f);

        if (g()->isValidPixel(plot_x, plot_y))
            g()->setPixel(plot_x, plot_y, ball_color);

        // check if a bat missed the ball. if it did, reset the game.
        if (ballpos_x < _x || ballpos_x > _x + _w)
        {
            restart = 1;
        }
    }

    float pong_get_ball_endpoint(float xpos, float ypos, float xspeed, float yspeed)
    {
        // In the following, the fabs() mirrors it over the bottom wall.  The fmod wraps it when it exceeds twice
        // the top wall.  If the ball ends up in the top half of the double height section, we reflect it back
        //
        // auto deltaX = (xspeed > 0) ? (bat2_x - xpos) : -(xpos - bat1_x);        // How far from ball to opponent bat
        // auto slope = yspeed / xspeed;                                           // Rise over run, ie: deltaY per X
        // float newY = fmod(fabs(ypos + deltaX * slope), (2 * _h));    // New Y, but wrappped every 2*height
        //
        // if (newY > _h)                                               // If in top half, reflect to bottom
        //    newY = 2 * _h - newY;
        // return newY;
        auto deltaX = (xspeed > 0) ? (bat2_x - xpos) : -(xpos - bat1_x); // How far from ball to opponent bat
        auto slope = yspeed / xspeed;                                    // Rise over run, ie: deltaY per X
        float newY = fmod(fabs(ypos + deltaX * slope), (2 * _h));        // New Y, but wrappped every 2*height
        if (newY > _h)                                                   // If in top half, reflect to bottom
            newY = 2 * _h - newY;
        return newY;
    }
};

#endif