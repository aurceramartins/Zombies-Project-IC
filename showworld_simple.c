/*
 * This file is part of "2º Projeto de Introdução à Computação 2017/2018"
 * (2oPIC1718).
 *
 * 2oPIC1718 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * 2oPIC1718 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with 2oPIC1718. If not, see <http://www.gnu.org/licenses/>.
 * */

/**
 * @file
 * Implementation of the interface defined in showworld.h which displays an
 * ASCII-based visualization of the current state of the simulation world.
 *
 * @note Students should change or replace this file with a new implementation
 * making use of graphical or game library.
 *
 * @author Alejandro Urcera, Andre Cosme, Frederico Placido.
 * @date 2018
 * @copyright [GNU General Public License version 3 (GPLv3)](http://www.gnu.org/licenses/gpl.html)
 * */
#include "raylib.h"

#include "showworld.h"
#include <stdio.h>
#include <stdlib.h>

#define screenWidth 800
#define screenHeight 800

/* The implementation of `SHOWWORLD` type used in this simple text-based world
 * visualization code. In this simple case, we only need to keep track of the
 * world dimensions and of the function pointer which knows how to read an
 * agent from the world data structure.
 *
 * For a more complex implementation, for example based on the g2 library,
 * it would also be necessary to keep the g2 device.
 * */
struct showworld {
    unsigned int xdim;
    unsigned int ydim;
    get_agent_info_at aginfo_func;
};

/* Create a new display/visualization object for the simulation world.
 *
 * This function obeys the `showworld_new()` prototype defined in
 * `showworld.h`. */
SHOWWORLD *showworld_new(
        unsigned int xdim,
        unsigned int ydim,
        get_agent_info_at aginfo_func) {


    InitWindow(screenWidth, screenHeight, "Proyecto Zombies");

    SHOWWORLD *sw = NULL;
    sw = malloc(sizeof (SHOWWORLD));
    sw->xdim = xdim;
    sw->ydim = ydim;
    sw->aginfo_func = aginfo_func;
    return sw;


}

/* Destroy a display/visualization object for the simulation world.
 *
 * This function obeys the `showworld_destroy()` prototype defined in
 * `showworld.h`. */
void showworld_destroy(SHOWWORLD *sw) {
    free(sw);
    CloseWindow();
}

/* Update the simulation world display/visualization.
 *
 * This function obeys the `showworld_update()` prototype defined in
 * `showworld.h`. */
void showworld_update(SHOWWORLD *sw, void *w) {
    Texture2D back = LoadTexture("background.png"); // Texture loading
    Texture2D galifaca = LoadTexture("faca3.png"); // Texture loading
    Texture2D hum = LoadTexture("gajo1.png"); // Texture loading

    BeginDrawing();
    ClearBackground(RAYWHITE);

    printf("\n");

    /* Cycle through all the rows */
    for (unsigned int y = 0; y < sw->ydim; ++y) {

        /* Cycle through all the columns for the current row */
        for (unsigned int x = 0; x < sw->xdim; ++x) {
            DrawTexture(back, 1500, 1500, WHITE);

            /* Get state of the world (in bit packed fashion) using the user
               supplied function. */
            unsigned int item = sw->aginfo_func(w, x, y);

            /* Extract the agent type (2 bits). */
            AGENT_TYPE ag_type = item & 0x3;
            /* Extract whether the agent is playable (1 bit). */
            unsigned char playable = (item >> 2) & 0x1;
            /* Extract the agent ID (16 bits). */
            unsigned short ag_id = (item >> 3) & 0xFFFF;

            /* Determine the agent type. */
            switch (ag_type) {

                    /* If no agent is present at (x,y) just print a dot. */
                case None:
                    DrawRectangleLines(x * (GetScreenWidth() / sw->xdim) + 10, y * (GetScreenHeight() / sw->ydim) + 5, 30, 30, RAYWHITE);
                    break;

                    /* If human agent present at (x,y) print 'h' or 'H'. */
                case Human:
                    if (playable) {
                        /* Uppercase 'H' for player-controlled human agent. */
                        //DrawRectangleLines(x * (GetScreenWidth() / sw->xdim) + 10, y * (GetScreenHeight() / sw->ydim) + 5, 25, 25, YELLOW);
                        DrawTexture(hum, x * (GetScreenWidth() / sw->xdim) + 10, y * (GetScreenHeight() / sw->ydim) + 5, WHITE);
                    } else {
                        /* Lowercase 'h' for AI-controlled human agent. */
                        //DrawRectangle(x * 40, y * 40, 25, 25, SKYBLUE);
                        //  DrawRectangleLines(x * (GetScreenWidth() / sw->xdim) + 10, y * (GetScreenHeight() / sw->ydim) + 5, 25, 25, GOLD);
                        DrawTexture(hum, x * (GetScreenWidth() / sw->xdim) + 10, y * (GetScreenHeight() / sw->ydim) + 5, WHITE);
                    }
                    /* Print the agent ID in front of the 'h'/'H'. */
                    // printf("%02X ", ag_id);
                    // Draw a color-filled rectangle
                    // DrawText("", 190, 200, 20, LIGHTGRAY);
                    //DrawText(FormatText("%02d", ag_id), x * (GetScreenWidth() / sw->xdim) + 15, y * (GetScreenHeight() / sw->ydim) + 10, 17, GOLD);
                    break;

                    /* If zombie agent present at (x,y) print 'z' or 'Z'. */
                case Zombie:
                    if (playable) {
                        /* Uppercase 'Z' for player-controlled zombie agent. */
                        // DrawRectangleLines(x * (GetScreenWidth() / sw->xdim) + 10, y * (GetScreenHeight() / sw->ydim) + 5, 25, 25, LIME);


                        DrawTexture(galifaca, x * (GetScreenWidth() / sw->xdim) + 10, y * (GetScreenHeight() / sw->ydim) + 5, WHITE);

                    } else {
                        /* Lowercase 'z' for AI-controlled zombie agent. */
                        // DrawRectangleLines(x * (GetScreenWidth() / sw->xdim) + 10, y * (GetScreenHeight() / sw->ydim) + 5, 25, 25, DARKGREEN);


                        DrawTexture(galifaca, x * (GetScreenWidth() / sw->xdim) + 10, y * (GetScreenHeight() / sw->ydim) + 5, WHITE);


                    }
                    /* Print the agent ID in front of the 'h'/'H'. */
                    // DrawText(FormatText("%02d", ag_id), x * (GetScreenWidth() / sw->xdim) + 15, y * (GetScreenHeight() / sw->ydim) + 10, 17, GREEN);
                    break;

                    /* Print '?' if unknown type detected. This should *never*
                       happen. */
                default:
                    DrawRectangle(x * (GetScreenWidth() / sw->xdim) + 10, y * (GetScreenHeight() / sw->ydim) + 5, 25, 25, MAGENTA);

            }
        }


    }
    EndDrawing();

}

