#ifndef ALLEGRO_H
#define ALLEGRO_H
#endif // ALLEGRO_H

enum DIR {LEFT, RIGHT, UP, DOWN};
enum MATERIAL {DIRT, STONE, NONE};

bool contains_point(int x1, int y1, int width, int height,
                     int x2, int y2)
{
     return((x2 >= x1 && x2 <= x1+width) && (y2 >= y1 && y2 <= y1+height));
}

bool contains(int x1, int y1, int width1, int height1,
              int x2, int y2, int width2, int height2)
{
    if(contains_point(x1, y1, width1, height1, x2, y2))
        return true;
    if(contains_point(x1, y1, width1, height1, x2+width2, y2))
        return true;
    if(contains_point(x1, y1, width1, height1, x2, y2+height2))
        return true;
    if(contains_point(x1, y1, width1, height1, x2+width2, y2+height2))
     return true;

    if(contains_point(x1, y1, width1, height1, x2+(width2/2), y2))
        return true;
    if(contains_point(x1, y1, width1, height1, x2+(width2/2), y2+height2))
        return true;
    if(contains_point(x1, y1, width1, height1, x2, y2+(height2/2)))
       return true;
    if(contains_point(x1, y1, width1, height1, x2+height2, y2+(height2/2)))
        return true;

    return false;
}

class Entity
{
    protected:
        int x, y;
        int xvel, yvel;
        int width, height;
        int xspeed, yspeed;

    public:
        //Constructors
        Entity();
        Entity(int x, int y);
        Entity(int x, int y, int xvel , int yvel, int xspeed, int yspeed);

        //Getters
        int getx(){return x;}
        int gety(){return y;}
        int getxvel(){return xvel;}
        int getyvel(){return yvel;}
        int getxspeed(){return xspeed;}
        int getyspeed(){return yspeed;}

        //Setters
        void setx(int x){this->x = x;}
        void sety(int y){this->y = y;}
        void setxvel(int xvel){this->xvel = xvel;}
        void setyvel(int yvel){this->yvel = yvel;}
        void incxvel(int xvel){this->xvel += xvel;}
        void inxyvel(int yvel){this->yvel += yvel;}
        void setxspeed(int xspeed){this->xspeed = xspeed;}
        void setyspeed(int yspeed){this->yspeed = yspeed;}

        //Other
        virtual void tick();
};

Entity::Entity()
{
    x = y= 0;
    xvel = yvel = 0;
    xspeed = yspeed = 0;
}

Entity::Entity(int x, int y)
{
    this->x = x;
    this->y = y;
    xvel = yvel = 0;
    xspeed = yspeed = 0;
}

Entity::Entity(int x, int y, int xvel, int yvel, int xspeed, int yspeed)
{
    this->x = x;
    this->y = y;
    this->xvel = xvel;
    this->yvel = yvel;
    this->xspeed = xspeed;
    this->yspeed = yspeed;
}

void Entity::tick()
{
    x += xvel;
    y += yvel;
}

class Tank : public Entity
{
    private:
        int health;
        bool alive;
        DIR dir;
        long int lastshot;
        int reloadtime;

    public:
        Tank() : Entity(){}
        Tank(bool alive, int x, int y) : Entity(x, y)
        {
            this->alive = alive;
            health = 3;
            dir = UP;
            lastshot = 0;
            reloadtime = 1000;
        }

        Tank(bool alive, int x, int y,
             int xvel, int yvel,
             int xspeed, int yspeed) : Entity(x, y,
                                              xvel, yvel,
                                              xspeed, yspeed)
        {
            this->alive = alive;
            this->health = 3;
            dir = UP;
            lastshot = 0;
            reloadtime = 1000;
        }

        //Getters
        bool isalive(){return alive;}
        int gethealth(){return health;}
        DIR getdir(){return dir;}
        long int getlastshot(){return lastshot;}
        int getreloadtime(){return reloadtime;}

        //Setters
        void setalive(bool alive){this->alive = alive;}
        void sethealth(int health){this->health = health;}
        void damage(int dam){this->health -= dam;}
        void setdir(DIR dir){this->dir = dir;}
        void setlastshot(int lastshot){this->lastshot = lastshot;}
        void setreloadtime(int reloadtime){this->reloadtime = reloadtime;}

        //Other
        void tick();
};

void Tank::tick()
{
    if(health <= 0)
        alive = false;
    x+=xvel;
    y+=yvel;

    if(xvel > 0)
        dir = LEFT;
    else if(xvel < 0)
        dir = RIGHT;
    if(yvel > 0)
        dir = DOWN;
    else if(yvel < 0)
        dir = UP;
}

class Bullet : public Entity
{
    private:
        bool alive;
        int dam;

    public:
        Bullet() : Entity(){this->alive = false;}
        Bullet(bool alive, int x, int y) : Entity(x, y){this->alive = alive;}
        Bullet(bool alive, int dam, int x, int y) : Entity(x, y){this->alive = alive; this->dam = dam;}

        //Setters
        void setalive(bool alive){this->alive = alive;}
        void setdam(int dam){this->dam = dam;}

        //Getters
        bool isalive(){return alive;}
        int getdam(){return dam;}

        //Other
        void tick();
};

void Bullet::tick()
{
    if(alive)
    {
        x += xvel;
        y += yvel;
    }

    if(x >= 590 || x <= 40 || y >= 430 || y <= 130)
        alive = false;
}


class Solid : public Entity
{
    private:
        int width, height;
        bool visible;
        MATERIAL mat;

    public:
        Solid() : Entity()
        {
            width = 0;
            height = 0;
            visible = false;
            mat = NONE;
        }
        Solid(MATERIAL mat, int x, int y, int width, int height) : Entity(x, y)
        {
            this->width = width;
            this->height = height;
            this->visible = true;
            this->mat = mat;
        }
        Solid(MATERIAL mat, int x, int y, int width, int height, bool visible) : Entity(x, y)
        {
            this->width = width;
            this->height = height;
            this->visible = visible;
            this->mat = mat;
        }

        void setwidth(int width){this->width = width;}
        void setheight(int height){this->height = height;}
        void setvisible(bool visible){this->visible = visible;}
        void setmat(MATERIAL mat){this->mat = mat;}

        int getwidth(){return width;}
        int getheight(){return height;}
        bool isvisible(){return visible;}
        MATERIAL getmat(){return mat;}
};
