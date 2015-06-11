#include <allegro.h>
#include <iostream>
#include "entities.h"
#include "sprite.h"

#define WINDOW_W 640
#define WINDOW_H 480
#define TANK_W 40
#define TANK_H 40
#define TANKS_SIZE 2
#define TILE_W 40
#define TILE_H 40
#define BULLET_W 10
#define BULLET_H 10
#define WHITE makecol(255, 255, 255)
#define BLACK makecol(0, 0, 0)
#define RED makecol(255, 0, 0)
#define BLUE makecol(0, 0, 255)
#define GREEN makecol(0, 255, 0)
#define TICKTIME 16
#define SOLID_SIZE 6

int start();
void stop();
void game_loop();
void render();
void render_tanks();
void render_solids();
void render_bullets();
void render_hud();
void tick();
void tick_tanks();
void tick_time();
void tick_bullets();
void tick_solids();
void check_collisions();
bool intersects_solid(int x, int y, int width, int height);
void check_keys();
void tank_move(int p, DIR dir);
void init_players();
void init_bullets();
void init_background();
void init_bitmaps();
void init_sounds();
void init_solids();
void fire_bullet(int p);
void reset();

int MAP[16*10] =
{
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
};

Solid solids[SOLID_SIZE];

long long millitime = 0;
long int lasttick = millitime;
int tps = 0;
int ticks_this_second = 0;
int last_check = millitime;

BITMAP* buffer;
BITMAP* tiles;
BITMAP* tank_sprs;
BITMAP* bg;

Tank tanks[TANKS_SIZE] = {Tank(true, 50, 130), Tank(true, 550, 390)};
Bullet bullets[2] = {Bullet(false, 1, tanks[0].getx(), tanks[0].gety()),
                        Bullet(false, 1, tanks[1].getx(), tanks[1].gety())};

int bg_viewx = 0, bg_viewy = 0;
int bg_tilepos = 0;
bool running;

SAMPLE* snd_shoot;
SAMPLE* snd_hit;
SAMPLE* snd_die;

int main()
{
    return start();
}
END_OF_MAIN()

int start()
{
    allegro_init();
    if(install_keyboard())
    {
        allegro_message("Error initilizing keyboard!\n");
        return 1;
    }
    if(install_sound(DIGI_AUTODETECT, MIDI_NONE, ""))
    {
        allegro_message("Error initilizing sound system.\n");
        return 1;
    }
    if(install_timer())
    {
        allegro_message("Error initilizing timing system.\n");
        return 1;
    }

    install_int(tick_time, 1);
    set_color_depth(32);

    if(set_gfx_mode(GFX_AUTODETECT_WINDOWED, WINDOW_W, WINDOW_H, 0, 0))
    {
        allegro_message("Error initilizing 32-bit color depth.\n");
        return 1;
    }

    init_solids();
    init_players();
    init_bitmaps();
    init_sounds();

    running = true;
    game_loop();
    return 0;
}

void init_sounds()
{
    snd_shoot = load_wav("sound//shoot.wav");
    snd_hit = load_wav("sound//hit.wav");
    snd_die = load_wav("sound//die.wav");
}

void init_bitmaps()
{
    tank_sprs = load_bitmap("img//tanks.bmp", NULL);
    buffer = create_bitmap(WINDOW_W, WINDOW_H);
    bg = create_bitmap(WINDOW_W, WINDOW_H);
    clear_bitmap(buffer);
    clear_bitmap(bg);
    init_background();
}

void init_background()
{
    tiles = load_bitmap("img//tilesx40.bmp", NULL);
    BITMAP* header = load_bitmap("img//header.bmp", NULL);

    for(int y = 80; y < bg->h; y+=40)
    {
        for(int x = 0; x < bg->w; x+=40)
        {
            blit_frame(tiles, bg, x, y, 0, 0, 40, 40, 2, MAP[bg_tilepos++]);
        }
    }
    blit(header, bg, 0, 0, 0, 0, header->w, header->h);
}

void init_players()
{
    tanks[0].setxspeed(1);
    tanks[0].setyspeed(1);
    tanks[1].setxspeed(1);
    tanks[1].setyspeed(1);
    tanks[0].setdir(LEFT);
    tanks[1].setdir(RIGHT);
}

void init_solids()
{
    solids[0] = Solid(NONE, 0, 80, 640, 40, false);
    solids[1] = Solid(NONE, 0, 120, 40, 360, false);
    solids[2] = Solid(NONE, 600, 120, 40, 360, false);
    solids[3] = Solid(NONE, 0, 440, 640, 40, false);
    solids[4] = Solid(STONE, 160, 200, 40, 160, true);
    solids[5] = Solid(STONE, 440, 200, 40, 160, true);
}

void stop()
{
    destroy_bitmap(buffer);
    destroy_bitmap(bg);
    destroy_bitmap(tiles);
    destroy_bitmap(tank_sprs);
    allegro_exit();
}

void reset()
{
    if(tanks[0].isalive() && tanks[1].isalive())
        return;
    tanks[0].setx(50);
    tanks[0].sety(130);
    tanks[1].setx(550);
    tanks[1].sety(390);
    for(int i = 0; i < TANKS_SIZE; i++)
    {
        tanks[i].setalive(true);
        tanks[i].sethealth(3);
    }
}

void game_loop()
{
    while(running)
    {
        if(millitime - lasttick >= TICKTIME)
        {
            tick();
            ticks_this_second++;
            lasttick = millitime;
        }

        if(millitime - last_check >= 1000)
        {
            tps = ticks_this_second;
            ticks_this_second = 0;
            last_check = millitime;
        }
        render();
    }
    stop();
}

void render()
{
    clear_bitmap(buffer);
    blit(bg, buffer, bg_viewx, bg_viewy, 0, 0, bg->w, bg->h);

    render_tanks();
    render_solids();
    render_bullets();
    render_hud();

    acquire_screen();
    blit(buffer, screen, 0, 0, 0, 0, WINDOW_W, WINDOW_H);
    release_screen();
}

void render_solids()
{
    for(int p = 0; p < SOLID_SIZE; p++)
    {
        if(!solids[p].isvisible())
            continue;
        switch(solids[p].getmat())
        {
            case DIRT:
                stretch_blit(get_frame(tiles, 0, 0, TILE_W, TILE_H, 2, 0),
                     buffer, 0, 0, TILE_W, TILE_H, solids[p].getx(), solids[p].gety(),
                     solids[p].getwidth(), solids[p].getheight());
                break;
            case STONE:
                stretch_blit(get_frame(tiles, 0, 0, TILE_W, TILE_H, 2, 1),
                     buffer, 0, 0, TILE_W, TILE_H, solids[p].getx(), solids[p].gety(),
                     solids[p].getwidth(), solids[p].getheight());;
                break;
            case NONE:
                rectfill(buffer, solids[p].getx(), solids[p].gety(),
                         solids[p].getx()+solids[p].getwidth(),
                         solids[p].gety()+solids[p].getheight(), RED);
                break;
        }
    }
}

void render_hud()
{
    textprintf_ex(buffer, font, 15, 50, WHITE, -1, "Health: %d", tanks[0].gethealth());
    textprintf_ex(buffer, font, 380, 50, WHITE, -1, "Health: %d", tanks[1].gethealth());
    if(!tanks[0].isalive() && !tanks[1].isalive())
        textprintf_centre_ex(buffer, font, 320, 140, WHITE, -1, "DRAW! (backspace to reset)");
    else if(tanks[0].isalive() && !tanks[1].isalive())
        textprintf_centre_ex(buffer, font, 320, 140, RED, -1, "Player 1 wins! (backspace to reset)");
    else if(!tanks[0].isalive() && tanks[1].isalive())
        textprintf_centre_ex(buffer, font, 320, 140, GREEN, -1, "Player 2 wins! (backspace to reset)");
}

void render_bullets()
{
    for(int i = 0; i < TANKS_SIZE; i++)
    {
        if(bullets[i].isalive())
            rectfill(buffer, bullets[i].getx(), bullets[i].gety(),
                    bullets[i].getx() + BULLET_W, bullets[i].gety() + BULLET_H, WHITE);
    }
}

void render_tanks()
{
    for(int i = 0; i < 2; i++)
    {
        if(!tanks[i].isalive())
            continue;
        switch(tanks[i].getdir())
        {
            case LEFT:
                rotate_sprite(buffer, get_frame(tank_sprs, 0, 0, 40, 40, 2, i),
                          tanks[i].getx(), tanks[i].gety(),
                          itofix(64));
            break;

            case RIGHT:
                rotate_sprite(buffer, get_frame(tank_sprs, 0, 0, 40, 40, 2, i),
                          tanks[i].getx(), tanks[i].gety(),
                          itofix(192));
                break;

            case UP:
                draw_sprite(buffer, get_frame(tank_sprs, 0, 0, 40, 40, 2, i),
                  tanks[i].getx(), tanks[i].gety());
                  break;

            case DOWN:
                rotate_sprite(buffer, get_frame(tank_sprs, 0, 0, 40, 40, 2, i),
                          tanks[i].getx(), tanks[i].gety(),
                          itofix(128));
                break;
        }

    }

}

void tick()
{
    check_keys();
    check_collisions();
    tick_solids();
    tick_tanks();
    tick_bullets();
}

void check_collisions()
{
    //Bullet0 & Tank1
    if(bullets[0].isalive() && tanks[1].isalive() && contains(tanks[1].getx(), tanks[1].gety(),
                                                              TANK_W, TANK_H,
                                                              bullets[0].getx(), bullets[0].gety(),
                                                              BULLET_W, BULLET_H))
    {
        bullets[0].setalive(false);
        tanks[1].damage(bullets[0].getdam());
        if(tanks[1].gethealth() <= 0)
            play_sample(snd_die, 255, 128, 1000, 0);
        else
            play_sample(snd_hit, 255, 128, 1000, 0);
    }

    //Bullet1 & Tank0
    if(bullets[1].isalive() && tanks[0].isalive() && contains(tanks[0].getx(), tanks[0].gety(),
                                                              TANK_W, TANK_H,
                                                              bullets[1].getx(), bullets[1].gety(),
                                                              BULLET_W, BULLET_H))
    {
        bullets[1].setalive(false);
        tanks[0].damage(bullets[1].getdam());
        if(tanks[0].gethealth() <= 0)
            play_sample(snd_die, 255, 128, 1000, 0);
        else
            play_sample(snd_hit, 255, 128, 1000, 0);
    }

    //Bullets & solids
    for(int i = 0; i < TANKS_SIZE; i++)
    {
        if(intersects_solid(bullets[i].getx(), tanks[i].gety(),
                            BULLET_W, BULLET_H))
        {
            bullets[i].setalive(false);
        }
    }

    //Tanks & Solids
    for(int i = 0; i < TANKS_SIZE; i++)
    {
        //Right
        if(intersects_solid(tanks[i].getx() + tanks[i].getxspeed(), tanks[i].gety(),
                            TANK_W, TANK_H))
        {
            if(tanks[i].getxvel() > 0)
                tanks[i].setxvel(0);
        }
        //Left
        if(intersects_solid(tanks[i].getx() - tanks[i].getxspeed(), tanks[i].gety(),
                            TANK_W, TANK_H))
        {
            if(tanks[i].getxvel() < 0)
                tanks[i].setxvel(0);
        }
        //Above
        if(intersects_solid(tanks[i].getx(), tanks[i].gety() - tanks[i].getyspeed(),
                            TANK_W, TANK_H))
        {
            if(tanks[i].getyvel() < 0)
                tanks[i].setyvel(0);
        }
        //Below
        if(intersects_solid(tanks[i].getx(), tanks[i].gety() + tanks[i].getyspeed(),
                            TANK_W, TANK_H))
        {
            if(tanks[i].getyvel() > 0)
                tanks[i].setyvel(0);
        }
    }
}

bool intersects_solid(int x, int y, int width, int height)
{
    for(int i = 0; i < SOLID_SIZE; i++)
    {
        if(contains(solids[i].getx(), solids[i].gety(),
                    solids[i].getwidth(), solids[i].getheight(),
                    x, y, width, height)) return true;
    }
    return false;
}

void tick_solids()
{
    for(int i = 0; i < SOLID_SIZE; i++)
    {
        solids[i].tick();
    }
}

void tick_tanks()
{
    for(int i = 0; i < TANKS_SIZE; i++)
    {
        tanks[i].tick();
    }
}

void tank_move(int p, DIR dir)
{
    if(!tanks[p].isalive())
        return;
    switch(dir)
    {
        case RIGHT:
            tanks[p].setxvel(tanks[p].getxspeed());
            tanks[p].setyvel(0);
            break;
        case LEFT:
            tanks[p].setxvel(-tanks[p].getxspeed());
            tanks[p].setyvel(0);
            break;
        case UP:
            tanks[p].setyvel(-tanks[p].getyspeed());
            tanks[p].setxvel(0);
            break;
        case DOWN:
            tanks[p].setyvel(tanks[p].getyspeed());
            tanks[p].setxvel(0);
            break;
    }
}

void tick_bullets()
{
    for(int i = 0; i < TANKS_SIZE; i++)
    {
        bullets[i].tick();
        if(!bullets[i].isalive())
        {
            switch(tanks[i].getdir())
            {
                case UP:
                    bullets[i].setx(tanks[i].getx() + BULLET_H + (BULLET_H / 2));
                    bullets[i].sety(tanks[i].gety() - BULLET_H);
                    break;
                case DOWN:
                    bullets[i].setx(tanks[i].getx() + 15);
                    bullets[i].sety(tanks[i].gety() + TANK_H);
                    break;
                case LEFT:
                    bullets[i].setx(tanks[i].getx() + TANK_W);
                    bullets[i].sety(tanks[i].gety() + BULLET_H + (BULLET_H / 2));
                    break;
                case RIGHT:
                    bullets[i].setx(tanks[i].getx() - BULLET_W);
                    bullets[i].sety(tanks[i].gety() + BULLET_H + (BULLET_H / 2));
                    break;
            }

        }
    }
}

void fire_bullet(int i)
{
        if(!tanks[i].isalive())
            return;
        if(bullets[i].isalive())
            return;
        if(millitime - tanks[i].getlastshot() < tanks[i].getreloadtime())
            return;

        switch(tanks[i].getdir())
        {
            case LEFT:
                bullets[i].setxvel(4);
                bullets[i].setyvel(0);
                break;
            case RIGHT:
                bullets[i].setxvel(-4);
                bullets[i].setyvel(0);
                break;
            case UP:
                bullets[i].setyvel(-4);
                bullets[i].setxvel(0);
                break;
            case DOWN:
                bullets[i].setyvel(4);
                bullets[i].setxvel(0);
                break;

        }
        play_sample(snd_shoot, 255, 128, 1000, 0);
        tanks[i].setlastshot(millitime);
        bullets[i].setalive(true);
}

void check_keys()
{
    if(key[KEY_ESC])
        running = false;

    //Tank 0 movement
    if(key[KEY_D])
        tank_move(0, RIGHT);
    else if(key[KEY_A])
        tank_move(0, LEFT);
    else
        tanks[0].setxvel(0);
    if(key[KEY_W])
        tank_move(0, UP);
    else if(key[KEY_S])
        tank_move(0, DOWN);
    else
        tanks[0].setyvel(0);

    //Tank 1 movement
    if(key[KEY_RIGHT])
        tank_move(1, RIGHT);
    else if(key[KEY_LEFT])
        tank_move(1, LEFT);
    else
        tanks[1].setxvel(0);
    if(key[KEY_UP])
        tank_move(1, UP);
    else if(key[KEY_DOWN])
        tank_move(1, DOWN);
    else
        tanks[1].setyvel(0);

    if(key[KEY_SPACE])
        fire_bullet(0);
    if(key[KEY_ENTER] || key[KEY_ENTER_PAD])
        fire_bullet(1);
    if(key[KEY_BACKSPACE])
        reset();
}

void tick_time(){millitime++;}
