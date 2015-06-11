#ifndef ALLEGRO_H
#define ALLEGRO_H
#endif // ALLEGRO_H

void blit_frame(BITMAP* source, BITMAP* dest, int destx, int desty,
                        int startx, int starty,
                        int width, int height,
                        int columns, int frame)
{
    int x = startx + (frame % width) * width;
    int y = starty + (frame / height) * height;
    masked_blit(source, dest, x, y, destx, desty, width, height);
}

BITMAP* get_frame(BITMAP* source, int startx, int starty,
                        int width, int height,
                        int columns, int frame)
{
    BITMAP* temp = create_bitmap(width, height);

    int x = startx + (frame % width) * width;
    int y = starty + (frame / height) * height;

    blit(source, temp, x, y, 0, 0, width, height);
    return temp;
}
