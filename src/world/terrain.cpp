/* 
 * Triplane Classic - a side-scrolling dogfighting game.
 * Copyright (C) 1996,1997,2009  Dodekaedron Software Creations Oy
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * tjt@users.sourceforge.net
 */

/* Terrain drawing code */

#include "world/constants.h"
#include "menus/tripmenu.h"
#include "triplane.h"
#include "world/plane.h"

//\\ Infantry

extern int infan_x[MAX_INFANTRY];
extern int infan_y[MAX_INFANTRY];
extern int infan_direction[MAX_INFANTRY];
extern int infan_last_shot[MAX_INFANTRY];
extern int infan_state[MAX_INFANTRY];
extern int infan_country[MAX_INFANTRY];
extern int infan_frame[MAX_INFANTRY];
extern int infan_x_speed[MAX_INFANTRY];
extern int infan_stop[8];

extern int current_mode;

extern int dirty_area;
extern int isa_counter;
extern int dirty_x[100];
extern int dirty_y[100];
extern int dirty_yl[100];
extern int dirty_xl[100];
extern unsigned char *dirty_marker;

extern int itgun_shot_x[MAX_ITGUN_SHOTS];
extern int itgun_shot_y[MAX_ITGUN_SHOTS];

extern int player_x[16], player_y[16];
extern int player_angle[16];
extern int player_ammo[16], player_bombs[16], player_gas[16];
extern int player_upsidedown[16];
extern int player_rolling[16];
extern int player_spinning[16];
extern int player_x_speed[16], player_y_speed[16];
extern int player_last_shot[16];
extern int player_endurance[16];
extern int player_points[16];

extern int player_x_8[16];
extern int player_y_8[16];
extern int number_of_planes[16];

void terrain_to_screen(void);
void solo_terrain_to_screen(void);
void kangas_terrain_to_screen(int leftx);
void tboxi(int x1, int y1, int x2, int y2, unsigned char vari);


void vesa_terrain_to_screen(void);
void vesa_icons(int l);
void vesa_solo_icons(int l);


void tboxi(int x1, int y1, int x2, int y2, unsigned char vari) {
    if (x2 < 5 || y2 < 8 || x1 > 315 || y1 > 178)
        return;

    if (x2 > 315)
        x2 = 315;

    if (y2 > 178)
        y2 = 178;

    if (x1 < 5)
        x1 = 5;

    if (y1 < 8)
        y1 = 8;

    draw_line(x1, y1, x2, y1, vari);
    draw_line(x1, y1, x1, y2, vari);
    draw_line(x2, y2, x2, y1, vari);
    draw_line(x2, y2, x1, y2, vari);
}

/*
 * Draws terrain with offset shifted by (world_offset_x, world_offset_y) and clipped to (tx1,ty1,tx2,ty2)
 */
static void draw_terrain(int world_offset_x, int world_offset_y, int tx1, int ty1, int tx2, int ty2) {
    maisema->blit(world_offset_x, world_offset_y, tx1, ty1, tx2, ty2);
}

/*
 * Draws all objects with offset shifted by (world_offset_x, world_offset_y) and clipped to (tx1,ty1,tx2,ty2)
 */
static void draw_objects(int world_offset_x, int world_offset_y, int tx1, int ty1, int tx2, int ty2) {
    int l2;
    for (l2 = 0; l2 < MAX_STRUCTURES; l2++) {
        if ((structures[l2][struct_state[l2]] != NULL) && (!leveldata.struct_hit[l2]))
            structures[l2][struct_state[l2]]->blit((leveldata.struct_x[l2]) + world_offset_x,
                                                    (leveldata.struct_y[l2]) + world_offset_y, tx1, ty1,
                                                    tx2, ty2);

    }

    for (l2 = 0; l2 < 4; l2++) {
        if (!hangar_x[l2])
            continue;
        ovi[hangar_door_frame[l2]]->blit(hangar_x[l2] + 27 + world_offset_x,
                                            hangar_y[l2] + 3 + world_offset_y, tx1, ty1, tx2, ty2);

    }

    if (config.flags)
        for (l2 = 0; l2 < MAX_FLAGS; l2++) {
            if (flags_x[l2]) {

                flags[flags_owner[l2]][flags_frame[l2]]->blit(flags_x[l2] + world_offset_x,
                                                                flags_y[l2] + world_offset_y, tx1, ty1,
                                                                tx2, ty2);

            }
        }


    for (l2 = 0; l2 < MAX_AA_GUNS; l2++) {
        if (!kkbase_x[l2])
            continue;

        if (kkbase_status[l2] != 2) {
            kkbase[kkbase_type[l2]][kkbase_status[l2]][kkbase_frame[l2]]->blit(kkbase_x[l2] + world_offset_x,
                                                                                kkbase_y[l2] + world_offset_y,
                                                                                tx1, ty1, tx2, ty2);
        } else {
            kkbase[kkbase_type[l2]][kkbase_status[l2]][kkbase_frame[l2] >> 1]->blit(kkbase_x[l2] + world_offset_x,
                                                                                    kkbase_y[l2] + world_offset_y,
                                                                                    tx1, ty1, tx2, ty2);
        }
    }

    for (l2 = 0; l2 < MAX_INFANTRY; l2++) {
        if (!infan_x[l2])
            continue;

        switch (infan_state[l2]) {
        case 0:
            if (infan_frame[l2] < 12)
                infantry_walking[infan_country[l2]][infan_direction[l2]][infan_frame[l2]]->blit(infan_x[l2] + world_offset_x,
                                                                                                infan_y[l2] + world_offset_y,
                                                                                                tx1, ty1, tx2, ty2);
            else {
                if (infan_frame[l2] == 12)
                    infantry_dropping[infan_country[l2]][infan_direction[l2]]->blit(infan_x[l2] + world_offset_x,
                                                                                    infan_y[l2] + world_offset_y,
                                                                                    tx1, ty1, tx2, ty2);
                else
                    infantry_after_drop[infan_country[l2]][infan_direction[l2]]->blit(infan_x[l2] + world_offset_x,
                                                                                        infan_y[l2] + world_offset_y,
                                                                                        tx1, ty1, tx2, ty2);
            }
            break;

        case 1:

            infantry_aiming[infan_country[l2]][infan_direction[l2]][infan_frame[l2]]->blit(infan_x[l2] + world_offset_x,
                                                                                            infan_y[l2] + world_offset_y,
                                                                                            tx1, ty1, tx2, ty2);
            break;

        case 2:

            infantry_shooting[infan_country[l2]][infan_direction[l2]][infan_frame[l2]]->blit(infan_x[l2] + world_offset_x,
                                                                                                infan_y[l2] + world_offset_y,
                                                                                                tx1, ty1, tx2, ty2);
            break;

        case 3:

            infantry_dying[infan_country[l2]][infan_direction[l2]][infan_frame[l2] >> 1]->blit(infan_x[l2] + world_offset_x,
                                                                                                infan_y[l2] + world_offset_y,
                                                                                                tx1, ty1, tx2, ty2);
            break;

        case 4:

            infantry_wavedeath[infan_country[l2]][infan_direction[l2]][infan_frame[l2] >> 1]->blit(infan_x[l2] + world_offset_x,
                                                                                                    infan_y[l2] + world_offset_y,
                                                                                                    tx1, ty1, tx2, ty2);
            break;

        case 5:

            infantry_bdying[infan_country[l2]][infan_direction[l2]][infan_frame[l2] >> 1]->blit(infan_x[l2] + world_offset_x,
                                                                                                infan_y[l2] + world_offset_y,
                                                                                                tx1, ty1, tx2, ty2);
            break;

        }
    }

    ///
    for (l2 = 0; l2 < 4; l2++) {
        if (!mekan_x[l2])
            continue;

        switch (mekan_status[l2]) {
        case 1:
            mekan_running[mekan_frame[l2]][mekan_direction[l2]]->blit(mekan_x[l2] + world_offset_x,
                                                                        mekan_y[l2] + world_offset_y, tx1,
                                                                        ty1, tx2, ty2);
            break;

        case 2:
            mekan_pushing[0][mekan_frame[l2]][mekan_direction[l2]]->blit(mekan_x[l2] + world_offset_x,
                                                                            mekan_y[l2] + world_offset_y, tx1,
                                                                            ty1, tx2, ty2);
            break;

        case 3:
            mekan_pushing[1][mekan_frame[l2]][mekan_direction[l2]]->blit(mekan_x[l2] + world_offset_x,
                                                                            mekan_y[l2] + world_offset_y, tx1,
                                                                            ty1, tx2, ty2);
            break;


        }

    }

    for (l2 = 0; l2 < MAX_BOMBS; l2++)
        if (bomb_x[l2])
            bomb[(bomb_angle[l2] >> 8) / 6]->blit((bomb_x[l2] >> 8) - (4) + world_offset_x,
                                                    (bomb_y[l2] >> 8) - (4) + world_offset_y, tx1,
                                                    ty1, tx2, ty2);

    for (l2 = 0; l2 < 16; l2++)
        if (!in_closing[l2] && player_exists[l2] && !plane_coming[l2]) {
            if (l2 < 4) {
                if (hangarmenu_active[l2])
                    continue;

            }

            planes[l2][(player_angle[l2] >> 8) / 6][player_rolling[l2]][player_upsidedown[l2]]->blit((player_x_8[l2]) - 10 + world_offset_x,
                                                                                                        (player_y_8[l2]) - 10 + world_offset_y, tx1,
                                                                                                        ty1, tx2, ty2);
        } else {
            if (in_closing[l2] <= 12 && player_exists[l2] && !plane_coming[l2])
                plane_crash[(in_closing[l2] >> 1) - 1]->blit((player_x_8[l2]) - 10 + world_offset_x,
                                                                (player_y_8[l2]) - 10 + world_offset_y, tx1,
                                                                ty1, tx2, ty2);

        }

    if (config.shots_visible)
        for (l2 = 0; l2 < MAX_SHOTS; l2++) {
            if (shots_flying_x[l2])
                putpix((shots_flying_x[l2] >> 8) + world_offset_x,
                        (shots_flying_y[l2] >> 8) + world_offset_y, SHOTS_COLOR, tx1, ty1, tx2, ty2);

        }

    if (config.it_shots_visible)
        for (l2 = 0; l2 < MAX_ITGUN_SHOTS; l2++) {
            if (itgun_shot_x[l2])
                putpix((itgun_shot_x[l2] >> 8) + world_offset_x,
                        (itgun_shot_y[l2] >> 8) + world_offset_y, ITGUN_SHOT_COLOR, tx1, ty1,
                        tx2, ty2);

        }

    for (l2 = 0; l2 < MAX_FLYING_OBJECTS; l2++) {
        if (fobjects[l2].x) {
            switch (fobjects[l2].type) {
            case FOBJECTS_SMOKE:
                smoke[fobjects[l2].phase]->blit((fobjects[l2].x >> 8) - (fobjects[l2].width >> 1) + world_offset_x,
                                                (fobjects[l2].y >> 8) - (fobjects[l2].height >> 1) + world_offset_y,
                                                tx1, ty1, tx2, ty2);
                break;


            case FOBJECTS_SSMOKE:
                ssmoke[fobjects[l2].phase]->blit((fobjects[l2].x >> 8) - (fobjects[l2].width >> 1) + world_offset_x,
                                                    (fobjects[l2].y >> 8) - (fobjects[l2].height >> 1) + world_offset_y,
                                                    tx1, ty1, tx2, ty2);
                break;



            case FOBJECTS_RIFLE:
                rifle[fobjects[l2].phase]->blit((fobjects[l2].x >> 8) - (fobjects[l2].width >> 1) + world_offset_x,
                                                (fobjects[l2].y >> 8) - (fobjects[l2].height >> 1) + world_offset_y,
                                                tx1, ty1, tx2, ty2);
                break;


            case FOBJECTS_FLAME:
                if (config.flames)
                    flames[fobjects[l2].phase]->blit((fobjects[l2].x >> 8) - (fobjects[l2].width >> 1) + world_offset_x,
                                                        (fobjects[l2].y >> 8) - (fobjects[l2].height >> 1) + world_offset_y, tx1, ty1, tx2, ty2);
                break;


            case FOBJECTS_WAVE1:
                wave1[fobjects[l2].phase]->blit((fobjects[l2].x >> 8) - (fobjects[l2].width >> 1) + world_offset_x,
                                                (fobjects[l2].y >> 8) - (fobjects[l2].height >> 1) + world_offset_y,
                                                tx1, ty1, tx2, ty2);
                break;

            case FOBJECTS_WAVE2:
                wave2[fobjects[l2].phase]->blit((fobjects[l2].x >> 8) - (fobjects[l2].width >> 1) + world_offset_x,
                                                (fobjects[l2].y >> 8) - (fobjects[l2].height >> 1) + world_offset_y,
                                                tx1, ty1, tx2, ty2);
                break;

            case FOBJECTS_ITEXPLOSION:
                itexplosion[fobjects[l2].phase]->blit((fobjects[l2].x >> 8) - (fobjects[l2].width >> 1) + world_offset_x,
                                                        (fobjects[l2].y >> 8) - (fobjects[l2].height >> 1) + world_offset_y, 
                                                        tx1, ty1, tx2, ty2);
                break;

            case FOBJECTS_EXPLOX:
                if (fobjects[l2].phase < 0)
                    break;
                explox[fobjects[l2].subtype][fobjects[l2].phase]->blit((fobjects[l2].x >> 8) - (fobjects[l2].width >> 1) + world_offset_x,
                                                                        (fobjects[l2].y >> 8) - (fobjects[l2].height >> 1) + world_offset_y, 
                                                                        tx1, ty1, tx2, ty2);
                break;

            case FOBJECTS_PARTS:
                bites[fobjects[l2].phase]->blit((fobjects[l2].x >> 8) - (fobjects[l2].width >> 1) + world_offset_x,
                                                (fobjects[l2].y >> 8) - (fobjects[l2].height >> 1) + world_offset_y,
                                                tx1, ty1, tx2, ty2);
                break;

            }


        }

    }
}

static void draw_terrain_and_objects(int world_offset_x, int world_offset_y, int tx1, int ty1, int tx2, int ty2) {
    draw_terrain(world_offset_x, world_offset_y, tx1, ty1, tx2, ty2);
    draw_objects(world_offset_x, world_offset_y, tx1, ty1, tx2, ty2);
}

void terrain_to_screen(void) {
    int l, l2;
    int tempx, tempy;
    int temp;

    for (l = 0; l < 16; l++) {
        if (!player_exists[l])
            continue;

        if ((in_closing[l] > 0) && (in_closing[l] <= 86))
            in_closing[l] += 2;

    }

    for (l = 0; l < 4; l++) {
        if (!player_exists[l])
            continue;

        if (!hangarmenu_active[l]) {
            player_shown_x[l] = 80 - (player_x_speed[l] / 5500);
            player_shown_y[l] = 45 + (player_y_speed[l] / 10000);


            if (((player_x_8[l]) - player_shown_x[l] + 160) > NUMBER_OF_SCENES * 160)
                player_shown_x[l] -= NUMBER_OF_SCENES * 160 - ((player_x_8[l]) - player_shown_x[l] + 160);

            if (((player_y_8[l]) - player_shown_y[l] + 90) > 200)
                player_shown_y[l] -= 200 - ((player_y_8[l]) - player_shown_y[l] + 90);

            if (((player_x_8[l]) - player_shown_x[l]) < 0)
                player_shown_x[l] += ((player_x_8[l]) - player_shown_x[l]);

            if (((player_y_8[l]) - player_shown_y[l]) < 0)
                player_shown_y[l] += ((player_y_8[l]) - player_shown_y[l]);

            int screen_x1 = x1_raja[l];
            int screen_y1 = y1_raja[l] + in_closing[l];
            int screen_x2 = x2_raja[l];
            int screen_y2 = y2_raja[l];
            int world_offset_x = player_shown_x[l] - (player_x_8[l]) + x_muutos[l];
            int world_offset_y = player_shown_y[l] - (player_y_8[l]) + y_muutos[l];

            draw_terrain_and_objects(world_offset_x, world_offset_y, screen_x1, screen_y1, screen_x2, screen_y2);   

            // draw radar markers
            for (int l2 = 0; l2 < 16; l2++) {
                if (!in_closing[l2] && player_exists[l2] && !plane_coming[l2]) {
                    if (l2 < 4) {
                        if (hangarmenu_active[l2])
                            continue;
                    }

                    tempx = player_x_8[l2] + player_shown_x[l] - player_x_8[l];
                    tempy = player_y_8[l2] + player_shown_y[l] - player_y_8[l];

                    if (!computer_active[l] && (tempx > 156 || tempx < 0 || tempy > 88 || tempy < 0)) {
                        temp = (abs(tempx + tempy)) >> 8;

                        if (temp >= 8)
                            temp = 7;

                        tempx = tempx >> 5;
                        tempy = tempy >> 2;

                        radar[player_tsides[l2]][temp]->blit(screen_x1, y_muutos[l] + 44 + tempy, screen_x1, screen_y1, screen_x2, screen_y2);
                        radar[player_tsides[l2]][temp]->blit(x_muutos[l] + 78 + tempx, screen_y1, screen_x1, screen_y1, screen_x2, screen_y2);
                    }
                }
            }
        }

        boards[l]->blit(2 + x_muutos[l], 90 + (l / 2) * 98);

        for (l2 = 5; l2 >= 0; l2--) {
            if ((player_bombs[l] - 1) >= l2)
                break;

            bomb_icon->blit(59 + x_muutos[l] + l2 * 6, 91 + (l / 2) * 98);

        }

        for (l2 = 7; l2 >= 0; l2--) {
            if (((player_ammo[l] >> 4) - 1) >= l2)
                break;

            big_ammo_icon->blit(103 + x_muutos[l] + l2 * 4, 91 + (l / 2) * 98);
        }

        for (l2 = 15; l2 >= 0; l2--) {
            if ((player_ammo[l] - ((player_ammo[l] >> 4) << 4) - 1) >= l2)
                break;

            small_ammo_icon->blit(103 + x_muutos[l] + l2 * 2, 97 + (l / 2) * 98);
        }

        for (l2 = 7; l2 >= 0; l2--) {
            if (((player_gas[l]) >> 8) == l2)
                break;

            gas_icon->blit(3 + x_muutos[l] + l2 * 3, 92 + (l / 2) * 98);
        }

        gas_icon->blit(3 + x_muutos[l] + (player_gas[l] >> 8) * 3, 92 + (l / 2) * 98 - ((player_gas[l] - ((player_gas[l] >> 8) << 8))) / 32, 0,
                       92 + (l / 2) * 98, 319, 99 + (l / 2) * 98);

        if (computer_active[l]) {
            closed->blit(2 + x_muutos[l], 2 + y_muutos[l] + in_closing[l] - 88, x1_raja[l], y1_raja[l], x2_raja[l], y2_raja[l]);
        } else {
            if (in_closing[l]) {

                hangarmenu->blit(x1_raja[l], y1_raja[l] + in_closing[l] - 88, x1_raja[l], y1_raja[l], x2_raja[l], y2_raja[l]);
                flags[l][7]->blit(x1_raja[l] + 14, y1_raja[l] + in_closing[l] - 88 + 67, x1_raja[l], y1_raja[l], x2_raja[l], y2_raja[l]);


                switch (hangarmenu_position[l]) {
                case 0:
                    hangaractive->blit(x1_raja[l] + 35, y1_raja[l] + 74 - (46 * hangarmenu_bombs[l]) / plane_bombs[l] + in_closing[l] - 88, x1_raja[l],
                                       y1_raja[l], x2_raja[l], y2_raja[l]);
                    hangarinactive->blit(x1_raja[l] + 69, y1_raja[l] + 74 - (46 * hangarmenu_ammo[l]) / plane_ammo[l] + in_closing[l] - 88, x1_raja[l],
                                         y1_raja[l], x2_raja[l], y2_raja[l]);
                    hangarinactive->blit(x1_raja[l] + 103, y1_raja[l] + 74 - (46 * hangarmenu_gas[l]) / plane_gas[l] + in_closing[l] - 88, x1_raja[l],
                                         y1_raja[l], x2_raja[l], y2_raja[l]);
                    break;

                case 1:
                    hangarinactive->blit(x1_raja[l] + 35, y1_raja[l] + 74 - (46 * hangarmenu_bombs[l]) / plane_bombs[l] + in_closing[l] - 88, x1_raja[l],
                                         y1_raja[l], x2_raja[l], y2_raja[l]);
                    hangaractive->blit(x1_raja[l] + 69, y1_raja[l] + 74 - (46 * hangarmenu_ammo[l]) / plane_ammo[l] + in_closing[l] - 88, x1_raja[l],
                                       y1_raja[l], x2_raja[l], y2_raja[l]);
                    hangarinactive->blit(x1_raja[l] + 103, y1_raja[l] + 74 - (46 * hangarmenu_gas[l]) / plane_gas[l] + in_closing[l] - 88, x1_raja[l],
                                         y1_raja[l], x2_raja[l], y2_raja[l]);
                    break;

                case 2:
                    hangarinactive->blit(x1_raja[l] + 35, y1_raja[l] + 74 - (46 * hangarmenu_bombs[l]) / plane_bombs[l] + in_closing[l] - 88, x1_raja[l],
                                         y1_raja[l], x2_raja[l], y2_raja[l]);
                    hangarinactive->blit(x1_raja[l] + 69, y1_raja[l] + 74 - (46 * hangarmenu_ammo[l]) / plane_ammo[l] + in_closing[l] - 88, x1_raja[l],
                                         y1_raja[l], x2_raja[l], y2_raja[l]);
                    hangaractive->blit(x1_raja[l] + 103, y1_raja[l] + 74 - (46 * hangarmenu_gas[l]) / plane_gas[l] + in_closing[l] - 88, x1_raja[l], y1_raja[l],
                                       x2_raja[l], y2_raja[l]);
                    break;

                }

            } else if (hangarmenu_active[l]) {

                hangarmenu->blit(x1_raja[l], y1_raja[l], x1_raja[l], y1_raja[l], x2_raja[l], y2_raja[l]);
                flags[l][7]->blit(x1_raja[l] + 14, y1_raja[l] + 67);

                switch (hangarmenu_position[l]) {
                case 0:
                    hangaractive->blit(x1_raja[l] + 35, y1_raja[l] + 74 - (46 * hangarmenu_bombs[l]) / plane_bombs[l]);
                    hangarinactive->blit(x1_raja[l] + 69, y1_raja[l] + 74 - (46 * hangarmenu_ammo[l]) / plane_ammo[l]);
                    hangarinactive->blit(x1_raja[l] + 103, y1_raja[l] + 74 - (46 * hangarmenu_gas[l]) / plane_gas[l]);
                    break;

                case 1:
                    hangarinactive->blit(x1_raja[l] + 35, y1_raja[l] + 74 - (46 * hangarmenu_bombs[l]) / plane_bombs[l]);
                    hangaractive->blit(x1_raja[l] + 69, y1_raja[l] + 74 - (46 * hangarmenu_ammo[l]) / plane_ammo[l]);
                    hangarinactive->blit(x1_raja[l] + 103, y1_raja[l] + 74 - (46 * hangarmenu_gas[l]) / plane_gas[l]);
                    break;

                case 2:
                    hangarinactive->blit(x1_raja[l] + 35, y1_raja[l] + 74 - (46 * hangarmenu_bombs[l]) / plane_bombs[l]);
                    hangarinactive->blit(x1_raja[l] + 69, y1_raja[l] + 74 - (46 * hangarmenu_ammo[l]) / plane_ammo[l]);
                    hangaractive->blit(x1_raja[l] + 103, y1_raja[l] + 74 - (46 * hangarmenu_gas[l]) / plane_gas[l]);
                    break;

                }

            }
        }


		int points = get_player_points(l);
        fontti->printf(144 + x_muutos[l], 93 + (l / 2) * 98, "%3d", abs(points));
        if (points < 0)
            fontti->printf(144 + x_muutos[l], 93 + (l / 2) * 98, "-");

    }

    for (l = 0; l < 16; l++)
        if (in_closing[l] >= 88) {
            init_player(l, 1);
        }


}


void solo_terrain_to_screen(void) {
    int l, l2;
    int temp;

    for (l = 0; l < 16; l++) {
        if (!player_exists[l])
            continue;

        if ((in_closing[l] > 0) && (in_closing[l] <= 86))
            in_closing[l] += 2;
    }

    l = solo_mode;

    player_shown_x[l] = 160;
    player_shown_y[l] = player_y[l] >> 8;

    if (((player_x_8[l]) - player_shown_x[l] + 320) > NUMBER_OF_SCENES * 160)
        player_shown_x[l] -= NUMBER_OF_SCENES * 160 - ((player_x_8[l]) - player_shown_x[l] + 320);

    if (((player_x_8[l]) - player_shown_x[l]) < 0)
        player_shown_x[l] += ((player_x_8[l]) - player_shown_x[l]);

    // Sky and terrain behind UI
    draw_terrain(player_shown_x[l] - (player_x_8[l]), 0, 0, 0, screen_width_less, screen_height_less);

    boards[l]->blit(0, 0);
    board2->blit(158, 0);

    for (l2 = 0; l2 <= number_of_planes[solo_country]; l2++)
        picons[solo_country]->blit(181 + l2 * 10, 1);

    if (controls_power[solo_country])
        pwon->blit(163, 2);
    else
        pwoff->blit(163, 6);

    if (++status_frames > STATUS_SPEED) {
        status_frames = 0;
        if (status_state)
            status_state = 0;
        else
            status_state = 1;

    }

    if (solo_failed) {
        status_icons[0][status_state]->blit(285, 1);

    } else {
        if (solo_success) {
            status_icons[1][status_state]->blit(285, 1);
        }
    }

    for (l2 = 5; l2 >= 0; l2--) {
        if ((player_bombs[l] - 1) >= l2)
            break;

        bomb_icon->blit(57 + l2 * 6, 1);

    }

    for (l2 = 7; l2 >= 0; l2--) {
        if (((player_ammo[l] >> 4) - 1) >= l2)
            break;

        big_ammo_icon->blit(101 + l2 * 4, 1);
    }

    for (l2 = 15; l2 >= 0; l2--) {
        if ((player_ammo[l] - ((player_ammo[l] >> 4) << 4) - 1) >= l2)
            break;

        small_ammo_icon->blit(101 + l2 * 2, 7);
    }

    for (l2 = 7; l2 >= 0; l2--) {
        if (((player_gas[l]) >> 8) == l2)
            break;

        gas_icon->blit(1 + l2 * 3, 2);
    }

    gas_icon->blit(1 + (player_gas[l] >> 8) * 3, 2 - (((player_gas[l] - ((player_gas[l] >> 8) << 8))) >> 5), 0, 2, 319, 9);

    fontti->printf(142, 3, "%3d", abs(player_points[l]));
    if (player_points[l] < 0)
        fontti->printf(142, 3, "-");

    // Draw objects (in front of UI)
    draw_objects(player_shown_x[l] - (player_x_8[l]), 0, 0, 0, screen_width_less, screen_height_less);
    
    if (hangarmenu_active[l]) {

        hangarmenu->blit(0, 15);
        flags[solo_country][7]->blit(14, 15 + 67);

        if (!hangarmenu_max_bombs[solo_country])
            hruks->blit(31, 8 + 15);

        if (!hangarmenu_max_ammo[solo_country])
            hruks->blit(65, 8 + 15);

        switch (hangarmenu_position[l]) {
        case 0:
            hangaractive->blit(35, 15 + 74 - (46 * hangarmenu_bombs[l]) / plane_bombs[l]);
            hangarinactive->blit(69, 15 + 74 - (46 * hangarmenu_ammo[l]) / plane_ammo[l]);
            hangarinactive->blit(103, 15 + 74 - (46 * hangarmenu_gas[l]) / plane_gas[l]);
            break;

        case 1:
            hangarinactive->blit(35, 15 + 74 - (46 * hangarmenu_bombs[l]) / plane_bombs[l]);
            hangaractive->blit(69, 15 + 74 - (46 * hangarmenu_ammo[l]) / plane_ammo[l]);
            hangarinactive->blit(103, 15 + 74 - (46 * hangarmenu_gas[l]) / plane_gas[l]);
            break;

        case 2:
            hangarinactive->blit(35, 15 + 74 - (46 * hangarmenu_bombs[l]) / plane_bombs[l]);
            hangarinactive->blit(69, 15 + 74 - (46 * hangarmenu_ammo[l]) / plane_ammo[l]);
            hangaractive->blit(103, 15 + 74 - (46 * hangarmenu_gas[l]) / plane_gas[l]);
            break;

        }
    }

    // draw radar markers
    for (l2 = 0; l2 < 16; l2++)
        if (!in_closing[l2] && player_exists[l2] && !plane_coming[l2]) {
            if (l2 < 4) {
                if (hangarmenu_active[l2])
                    continue;

            }
            temp = abs(player_x_8[l] - player_x_8[l2]);

            if (temp > 160) {
                temp = temp >> 8;

                if (temp >= 8)
                    temp = 7;

                if ((player_x_8[l] - player_x_8[l2]) > 0)
                    radar[player_tsides[l2]][temp]->blit(0, player_y_8[l2] - 1);
                else
                    radar[player_tsides[l2]][temp]->blit(317, player_y_8[l2] - 1);
            }
        }

    for (l = 0; l < 16; l++)
        if (in_closing[l] >= 88) {
            //in_closing[l]=0;  
            init_player(l, 1);
        }
#ifdef COMPUDEBUG
    for (l = 0; l < 4; l++) {
        frost->printf(5, 10 + 12 * l, "Plane %d: Mission:%d Phase:%d Target:%d Up:%d Down:%d Power:%d Speed:%d Angle:%d Dst:%d A:%d", l,
                      current_mission[l], mission_phase[l], mission_target[l], controls_up[l], controls_down[l], controls_power[l], player_speed[l],
                      player_angle[l] >> 8, distances[l], angles[l]);


    }
#endif

}

void kangas_terrain_to_screen(int left_x) {
    int l2, l, l3;
    static int color_frame = 0;

    if (color_frame++ >= 3)
        color_frame = 0;

    do_flags();

    maisema->blit(left_x, -PROJECTOR_Y_ADJUSTMENT, PROJECTOR_X1, PROJECTOR_Y1, PROJECTOR_X2, PROJECTOR_Y2);


    for (l2 = 0; l2 < MAX_STRUCTURES; l2++) {

        if ((structures[l2][struct_state[l2]] != NULL) && (!leveldata.struct_hit[l2])) {
            structures[l2][0]->blit((leveldata.struct_x[l2]) + left_x, leveldata.struct_y[l2] - PROJECTOR_Y_ADJUSTMENT, PROJECTOR_X1, PROJECTOR_Y1,
                                    PROJECTOR_X2, PROJECTOR_Y2);


        }
    }



    for (l2 = 0; l2 < MAX_FLAGS; l2++) {
        if (flags_x[l2]) {

            flags[flags_owner[l2]][flags_frame[l2]]->blit(flags_x[l2] + left_x, flags_y[l2] - PROJECTOR_Y_ADJUSTMENT, PROJECTOR_X1, PROJECTOR_Y1, PROJECTOR_X2,
                                                          PROJECTOR_Y2);

        }
    }


    for (l2 = 0; l2 < MAX_AA_GUNS; l2++) {
        if (!kkbase_x[l2])
            continue;

        if (kkbase_status[l2] != 2)
            kkbase[kkbase_type[l2]][kkbase_status[l2]][kkbase_frame[l2]]->blit(kkbase_x[l2] + left_x, kkbase_y[l2] - PROJECTOR_Y_ADJUSTMENT, PROJECTOR_X1,
                                                                               PROJECTOR_Y1, PROJECTOR_X2, PROJECTOR_Y2);
        else
            kkbase[kkbase_type[l2]][kkbase_status[l2]][kkbase_frame[l2] >> 1]->blit(kkbase_x[l2] + left_x, kkbase_y[l2] - PROJECTOR_Y_ADJUSTMENT, PROJECTOR_X1,
                                                                                    PROJECTOR_Y1, PROJECTOR_X2, PROJECTOR_Y2);


    }

    for (l2 = 0; l2 < MAX_AA_GUNS; l2++) {
        if (!kkbase_x[l2])
            continue;


        if (kkbase_mission[l2]) {
            l3 = 2;

            for (l = color_frame; l < 3; l++) {
                tboxi((kkbase_x[l2]) + left_x - l3, kkbase_y[l2] - PROJECTOR_Y_ADJUSTMENT - l3, (kkbase_x[l2]) + left_x + 25 + l3,
                      kkbase_y[l2] - PROJECTOR_Y_ADJUSTMENT + 21 + l3, (kkbase_mission[l2] - 1) * 7 + 240 + l);
                l3++;
            }

            for (l = 0; l < color_frame; l++) {
                tboxi((kkbase_x[l2]) + left_x - l3, kkbase_y[l2] - PROJECTOR_Y_ADJUSTMENT - l3, (kkbase_x[l2]) + left_x + 25 + l3,
                      kkbase_y[l2] - PROJECTOR_Y_ADJUSTMENT + 21 + l3, (kkbase_mission[l2] - 1) * 7 + 240 + l);

                l3++;
            }
        }

    }

    for (l2 = 0; l2 < MAX_STRUCTURES; l2++) {

        if ((structures[l2][struct_state[l2]] != NULL) && (!leveldata.struct_hit[l2])) {

            if (leveldata.struct_type[l2]) {
                l3 = 2;

                for (l = color_frame; l < 3; l++) {
                    tboxi((leveldata.struct_x[l2]) + left_x - l3, leveldata.struct_y[l2] - PROJECTOR_Y_ADJUSTMENT - l3,
                          (leveldata.struct_x[l2]) + left_x + struct_width[l2] + l3 - 1,
                          leveldata.struct_y[l2] - PROJECTOR_Y_ADJUSTMENT + struct_heigth[l2] + l3, (leveldata.struct_type[l2] - 1) * 7 + 240 + l);
                    l3++;
                }

                for (l = 0; l < color_frame; l++) {
                    tboxi((leveldata.struct_x[l2]) + left_x - l3, leveldata.struct_y[l2] - PROJECTOR_Y_ADJUSTMENT - l3,
                          (leveldata.struct_x[l2]) + left_x + struct_width[l2] + l3 - 1,
                          leveldata.struct_y[l2] - PROJECTOR_Y_ADJUSTMENT + struct_heigth[l2] + l3, (leveldata.struct_type[l2] - 1) * 7 + 240 + l);
                    l3++;
                }
            }
        }
    }


}

void vesa_terrain_to_screen(void) {
    int l;

    // Draw world
    if (split_num == 0)
    {
        draw_terrain_and_objects(0, -4, 0, 0, screen_width_less, screen_height_less);
    }
    else if (split_num == 1)
    {
        draw_terrain_and_objects(-screen_width, 192, 0, 0, screen_width_less, screen_height_less);
        draw_terrain_and_objects(0, -4, 0, 0, screen_width_less, screen_height_less);
    }
    else
    {
        draw_terrain_and_objects(-2 * screen_width, 388, 0, 0, screen_width_less, screen_height_less);
        draw_terrain_and_objects(-screen_width, 192, 0, 0, screen_width_less, screen_height_less);
        draw_terrain_and_objects(0, -4, 0, 0, screen_width_less, screen_height_less);
    }

    // Update player hangar timer
    for (l = 0; l < 16; l++) {
        if (!player_exists[l])
            continue;

        if ((in_closing[l] > 0) && (in_closing[l] <= 86))
            in_closing[l] += 2;
    }

    // Draw hangar menu
    for (l = 0; l < 4; l++) {
        if (hangarmenu_active[l]) {
            hangarmenu->blit(200 * l + 20, 0);
            flags[l][7]->blit(200 * l + 20 + 14, 67);

            switch (hangarmenu_position[l]) {
            case 0:
                hangaractive->blit(200 * l + 20 + 35, 74 - (46 * hangarmenu_bombs[l]) / plane_bombs[l]);
                hangarinactive->blit(200 * l + 20 + 69, 74 - (46 * hangarmenu_ammo[l]) / plane_ammo[l]);
                hangarinactive->blit(200 * l + 20 + 103, 74 - (46 * hangarmenu_gas[l]) / plane_gas[l]);
                break;

            case 1:
                hangarinactive->blit(200 * l + 20 + 35, 74 - (46 * hangarmenu_bombs[l]) / plane_bombs[l]);
                hangaractive->blit(200 * l + 20 + 69, 74 - (46 * hangarmenu_ammo[l]) / plane_ammo[l]);
                hangarinactive->blit(200 * l + 20 + 103, 74 - (46 * hangarmenu_gas[l]) / plane_gas[l]);
                break;

            case 2:
                hangarinactive->blit(200 * l + 20 + 35, 74 - (46 * hangarmenu_bombs[l]) / plane_bombs[l]);
                hangarinactive->blit(200 * l + 20 + 69, 74 - (46 * hangarmenu_ammo[l]) / plane_ammo[l]);
                hangaractive->blit(200 * l + 20 + 103, 74 - (46 * hangarmenu_gas[l]) / plane_gas[l]);
                break;

            }

        }
    }

	//Icons
	if (playing_solo)
		vesa_solo_icons(solo_mode);
	else
		for (l = 0; l < 4; l++)	{
			vesa_icons(l);
		}

    for (l = 0; l < 16; l++)
        if (in_closing[l] >= 88) {
            //in_closing[l]=0;  
            init_player(l, 1);
        }
#ifdef COMPUDEBUG
    for (l = 0; l < 4; l++) {
        frost->printf(50, 20 + 12 * l, "Plane %d: Mission:%d Phase:%d Target:%d Up:%d Down:%d Power:%d Speed:%d Angle:%d Dst:%d A:%d", l,
                      current_mission[l], mission_phase[l], mission_target[l], controls_up[l], controls_down[l], controls_power[l], player_speed[l],
                      player_angle[l] >> 8, distances[l], angles[l]);


    }
#endif
}

void vesa_icons(int l) {
	int l2;
	boards[l]->blit(l * 160, screen_height - 12, 0, 0, screen_width_less, screen_height_less);
	for (l2 = 5; l2 >= 0; l2--) {
		if ((player_bombs[l] - 1) >= l2)
			break;

		bomb_icon->blit(l * 160 + 57 + l2 * 6, screen_height - 11, 0, 0, screen_width_less, screen_height_less);

	}

	for (l2 = 7; l2 >= 0; l2--) {
		if (((player_ammo[l] >> 4) - 1) >= l2)
			break;

		big_ammo_icon->blit(l * 160 + 101 + l2 * 4, screen_height - 11, 0, 0, screen_width_less, screen_height_less);
	}

	for (l2 = 15; l2 >= 0; l2--) {
		if ((player_ammo[l] - ((player_ammo[l] >> 4) << 4) - 1) >= l2)
			break;

		small_ammo_icon->blit(l * 160 + 101 + l2 * 2, screen_height - 5, 0, 0, screen_width_less, screen_height_less);
	}

	for (l2 = 7; l2 >= 0; l2--) {
		if (((player_gas[l]) >> 8) == l2)
			break;

		gas_icon->blit(l * 160 + 1 + l2 * 3, screen_height - 10, 0, 0, screen_width_less, screen_height_less);
	}

	gas_icon->blit(l * 160 + 1 + (player_gas[l] >> 8) * 3, screen_height - 10 - ((player_gas[l] - ((player_gas[l] >> 8) << 8))) / 32, 0, screen_height - 10, screen_width_less, screen_height_less);
	
	int points = get_player_points(l);
	fontti->printf(l * 160 + 142, screen_height - 9, "%3d", abs(points));
	if (points < 0)
		fontti->printf(l * 160 + 142, screen_height - 9, "-");

	
}

void vesa_solo_icons(int l) {
	int l2;

	boards[l]->blit(0, screen_height - 12, 0, 0, screen_width_less, screen_height_less);
	board2->blit(158, screen_height - 12, 0, 0, screen_width_less, screen_height_less);

	for (l2 = 5; l2 >= 0; l2--) {
		if ((player_bombs[l] - 1) >= l2)
			break;

		bomb_icon->blit(57 + l2 * 6, screen_height - 11, 0, 0, screen_width_less, screen_height_less);

	}

	for (l2 = 7; l2 >= 0; l2--) {
		if (((player_ammo[l] >> 4) - 1) >= l2)
			break;

		big_ammo_icon->blit(101 + l2 * 4, screen_height - 11, 0, 0, screen_width_less, screen_height_less);
	}

	for (l2 = 15; l2 >= 0; l2--) {
		if ((player_ammo[l] - ((player_ammo[l] >> 4) << 4) - 1) >= l2)
			break;

		small_ammo_icon->blit(101 + l2 * 2, screen_height - 5, 0, 0, screen_width_less, screen_height_less);
	}

	for (l2 = 7; l2 >= 0; l2--) {
		if (((player_gas[l]) >> 8) == l2)
			break;

		gas_icon->blit(1 + l2 * 3, screen_height - 10, 0, 0, screen_width_less, screen_height_less);
	}

	gas_icon->blit(1 + (player_gas[l] >> 8) * 3, screen_height - 10 - ((player_gas[l] - ((player_gas[l] >> 8) << 8))) / 32, 0, screen_height - 10, screen_width_less, screen_height_less);

	fontti->printf(142, screen_height - 9, "%3d", abs(player_points[l]));
	if (player_points[l] < 0)
		fontti->printf(142, screen_height - 9, "-");
		
	for (l2 = 0; l2 <= number_of_planes[solo_country]; l2++)
		picons[solo_country]->blit(181 + l2 * 10, screen_height - 11);

	if (controls_power[solo_country])
		pwon->blit(163, screen_height - 10);
	else
		pwoff->blit(163, screen_height - 6);

	if (++status_frames > STATUS_SPEED) {
		status_frames = 0;
		if (status_state)
			status_state = 0;
		else
			status_state = 1;

	}

	if (solo_failed) {
		status_icons[0][status_state]->blit(285, screen_height - 11);

	}
	else {
		if (solo_success) {
			status_icons[1][status_state]->blit(285, screen_height - 11);
		}
	}
}


