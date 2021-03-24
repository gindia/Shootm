#include "common.h"
#include "structs.h"

static void init_player(void);
static void init_starfield(void);

static void logic(void);
static void do_player(void);
static void do_fighters(void);
static void do_bullets(void);
static void do_enemies(void);

static void do_background(void);
static void do_explosions(void);
static void do_starfield(void);
static void do_debris(void);


static void clip_player(void);

static void fire_bullet(void);
static void fire_enemy_bullet(Entity *e);

static void spwan_enemies(void);
static int  bullet_hit(Entity *bullet);

static void draw(void);
static void draw_player(void);
static void draw_fighters(void);
static void draw_bullets(void);

static void draw_background(void);
static void draw_starfields(void);
static void draw_debris(void);
static void draw_explosions(void);

static void reset_stage(void);
static void add_explosion(int x, int y, int num);
static void add_debris(Entity *e);

void init_stage(void)
{
    //app.delegate.logic = logic;
    //app.delegate.logic = draw;

    memset(&stage, 0, sizeof(Stage));
    stage.fighterTail   = &stage.fighterHead;
    stage.bulletTail    = &stage.bulletHead;
    stage.explosionTail = &stage.explosionHead;
    stage.debrisTail    = &stage.debrisHead;

    player_texture       = load_texture("../assets/player.png");
    bullet_texture       = load_texture("../assets/playerBullet.png");

    enemy_texture        = load_texture("../assets/enemy.png");
    enemy_bullet_texture = load_texture("../assets/alienBullet.png");

    background           = load_texture("../assets/background.png");
    explosion_texture    = load_texture("../assets/explosion.png");

    reset_stage();
}

static void reset_stage(void)
{
    Entity *e;
    Explosion *ex;
    Debris *d;

    while (stage.fighterHead.next)
    {
        e = stage.fighterHead.next;
        stage.fighterHead.next = e->next;
        free(e);
    }

    while (stage.bulletHead.next)
    {
        e = stage.bulletHead.next;
        stage.bulletHead.next = e->next;
        free(e);
    }

    while (stage.explosionHead.next)
    {
        ex = stage.explosionHead.next;
        stage.explosionHead.next = ex->next;
        free(ex);
    }

    while (stage.debrisHead.next)
    {
        d = stage.debrisHead.next;
        stage.debrisHead.next = d->next;
        free(d);
    }

    memset(&stage, 0, sizeof(Stage));
    stage.fighterTail = &stage.fighterHead;
    stage.bulletTail  = &stage.bulletHead;
    stage.explosionTail = &stage.explosionHead;
    stage.debrisTail    = &stage.debrisHead;

    init_player();
    init_starfield();

    enemy_spawn_timer = 0;

    stage_reset_timer = FPS * 3;
}

static void init_player(void)
{
    player =  malloc(sizeof(Entity));
    memset(player, 0, sizeof(Entity));

    stage.fighterTail->next = player;
    stage.fighterTail = player;

    player->x       = 100;
    player->y       = 100;
    player->health  = 1;
    player->texture = player_texture;
    SDL_QueryTexture(player->texture, NULL, NULL, &player->w, &player->h);

    player->side = SIDE_PLAYER;
}

static void init_starfield(void)
{
    int i;
    for (i = 0 ; i < MAX_STARS ; i++)
    {
        stars[i].x = rand() % SCREEN_WIDTH;
        stars[i].y = rand() % SCREEN_HEIGHT;
        stars[i].speed = 1 + rand() % 8;
    }
}

static void logic(void)
{
    if (player == NULL && --stage_reset_timer <= 0)
    {
        reset_stage();
    }

    do_background();
    do_starfield();

    do_player();

    do_enemies();

    do_fighters();

    do_bullets();

    spwan_enemies();

    clip_player();

    do_explosions();
    do_debris();
}

static void do_player(void)
{
    if (player == NULL)
        return;

    player->dx = player->dy = 0;

    if (player->reload > 0)
    {
        player->reload--;
    }
    if (controller.up)
    {
        player->dy  = -PLAYER_SPEED;
    }
    if (controller.down)
    {
        player->dy  = PLAYER_SPEED;
    }
    if (controller.left)
    {
        player->dx  = -PLAYER_SPEED;
    }
    if (controller.right)
    {
        player->dx  = PLAYER_SPEED;
    }
    if (controller.fire && player->reload == 0)
    {
        fire_bullet();
    }
    player->x += player->dx;
    player->y += player->dy;
}

static void do_background(void)
{
    if (--backgroundX < -SCREEN_WIDTH)
    {
        backgroundX = 0;
    }
}

static void do_starfield(void)
{
    int i;
     for (i = 0 ; i < MAX_STARS ; i++)
     {
         stars[i].x -= stars[i].speed;
         if (stars[i].x < 0)
         {
             stars[i].x = SCREEN_WIDTH + stars[i].x;
         }
     }
}

static void do_explosions(void)
{
    Explosion *e, *prev;

    prev = &stage.explosionHead;

    for (e = stage.explosionHead.next; e != NULL ; e = e->next)
    {
        e->x += e->dx;
        e->y += e->dy;

        if (--e->a <= 0)
        {
            if (e == stage.explosionTail)
            {
                stage.explosionTail = prev;
            }

            prev->next = e->next;
            free(e);
            e = prev;
        }
        prev = e;
    }
}

static void do_debris(void)
{
    Debris *d, *prev;

    prev = &stage.debrisHead;

    for (d = stage.debrisHead.next ; d != NULL ; d = d->next)
    {
        d->x += d->dx;
        d->y += d->dy;

        d->dy += 0.5;

        if (--d->life <= 0)
        {
            if (d == stage.debrisTail)
            {
                stage.debrisTail = prev;
            }

            prev->next = d->next;
            free(d);
            d = prev;
        }

        prev = d;
    }
}

static void add_explosion(int x, int y, int num)
{
    Explosion *e;
    int i;

    for (i = 0 ; i < num ; i++)
    {
        e = malloc(sizeof(Explosion));
        memset(e, 0, sizeof(Explosion));
        stage.explosionTail->next = e;
        stage.explosionTail = e;

        e->x = x + (rand() % 32) - (rand() % 32);
        e->y = y + (rand() % 32) - (rand() % 32);
        e->dx = (rand() % 10) - (rand() % 10);
        e->dy = (rand() % 10) - (rand() % 10);

        e->dx /= 10;
        e->dy /= 10;

        switch (rand() % 4)
        {
            case 0: e->r = 255; break;
            case 1: e->r = 255; e->g = 128; break;
            case 2: e->r = 255; e->g = 255; break;
            default:
                    e->r = 255;
                    e->g = 255;
                    e->b = 255;
                    break;
        }
        e->a = rand() % FPS * 3;
    }
}

static void add_debris(Entity *e)
{
    Debris *d;
    int x, y, w, h;

    w = e->w / 2;
    h = e->h / 2;

    for (y = 0 ; y <= h ; y += h)
    {
        for (x = 0 ; x <= w ; x += w)
        {
            d = malloc(sizeof(Debris));
            memset(d, 0, sizeof(Debris));
            stage.debrisTail->next = d;
            stage.debrisTail = d;

            d->x = e->x + e->w / 2;
			d->y = e->y + e->h / 2;
			d->dx = (rand() % 5) - (rand() % 5);
			d->dy = -(5 + (rand() % 12));
			d->life = FPS * 2;
			d->texture = e->texture;

			d->rect.x = x;
			d->rect.y = y;
			d->rect.w = w;
			d->rect.h = h;
        }
    }
}

static void clip_player(void)
{
    if (player == NULL)
        return;
    if (player->x < 0)
    {
        player->x = 0;
    }
    if (player->x > SCREEN_WIDTH/2)
    {
        player->x = SCREEN_WIDTH/2;
    }
    if (player->y < 0)
    {
        player->y = 0;
    }
    if (player->y > SCREEN_HEIGHT - player->h)
    {
        player->y = SCREEN_HEIGHT - player->h;
    }
}

static void fire_bullet(void)
{
    Entity *bullet;

    bullet = (Entity*) malloc(sizeof(Entity));
    memset(bullet, 0, sizeof(Entity));

    stage.bulletTail->next = bullet;
    stage.bulletTail = bullet;

    bullet->x       = player->x;
    bullet->y       = player->y;
    bullet->dx      = BULLET_ACCELERATION;
    bullet->health  = 1;
    bullet->side    = player->side;
    bullet->texture = bullet_texture;

    SDL_QueryTexture(bullet->texture, NULL, NULL, &bullet->w, &bullet->h);

    bullet->y += (player->h / 2) - (bullet->h / 2);

    player->reload = PLAYER_RELOAD;

}

static void do_fighters(void)
{
    Entity *e, *prev;

    prev = &stage.fighterHead;

    for (e = stage.fighterHead.next ; e != NULL ; e = e->next)
    {
        e->x += e->dx;
        e->y += e->dy;

        if ((e != player) && (e->x < -(e->w)))
        {
            e->health = 0;
        }
        if (e->health == 0)
        {
            if (e == player)
            {
                player = NULL;
            }

            if (e == stage.fighterTail)
            {
                stage.fighterTail = prev;
            }

            prev->next = e->next;
            free(e);
            e = prev;
        }
        prev = e;
    }
}

static void do_enemies(void)
{
    Entity *e;

    if (player == NULL)
        return;

    for (e = stage.fighterHead.next ; e != NULL ; e = e->next)
    {
        if (e != player && player != NULL && --e->reload <= 0)
        {
            fire_enemy_bullet(e);
        }
    }
}

static void fire_enemy_bullet(Entity *e)
{
    Entity *bullet;
    bullet = malloc(sizeof(Entity));
    memset(bullet, 0, sizeof(Entity));
    stage.bulletTail->next = bullet;
    stage.bulletTail = bullet;

    bullet->x = e->x;
    bullet->y = e->y;
    bullet->health = 1;
    bullet->texture = enemy_bullet_texture;
    bullet->side = SIDE_ENEMY;
    SDL_QueryTexture(bullet->texture, NULL, NULL, &bullet->w, &bullet->h);

    bullet->x += (e->w / 2) - (bullet->w / 2);
    bullet->y += (e->h / 2) - (bullet->h / 2);

    calc_slope(player->x + (player->w / 2), player->y + (player->h/2), e->x, e->y, &bullet->dx, &bullet->dy);

    bullet->dx *= ENEMY_BULLET_SPEED;
    bullet->dy *= ENEMY_BULLET_SPEED;


    e->reload = (rand() % FPS * 2);
}

static void do_bullets(void)
{
    Entity *b, *prev;
    int coll = 0;

    prev = &stage.bulletHead;

    for (b = stage.bulletHead.next ; b != NULL ; b = b->next)
    {
        b->x += b->dx;
        b->y += b->dy;
        coll = bullet_hit(b);

        if(coll || b->x < -b->w || b->y < -b->h || b->x > SCREEN_WIDTH || b->y > SCREEN_HEIGHT)
        {
            if(b == stage.bulletTail)
            {
                stage.bulletTail = prev;
            }

            prev->next = b->next;
            free(b);

            b = prev;
        }
        prev = b;
    }
}

static int bullet_hit(Entity *b)
{
    Entity *e;
    int bullet_coll, difrent_sides;

    for (e = stage.fighterHead.next ; e != NULL ; e = e->next)
    {
        bullet_coll   = collision(b->x, b->y, b->w, b->h, e->x, e->y, e->w, e->h);
        difrent_sides = (e->side != b->side);

        if (difrent_sides && bullet_coll)
        {
            b->health = 0;
            e->health = 0;
            return 1;
        }
    }
    return 0;
}

static void spwan_enemies(void)
{
    Entity *enemy;

    enemy_spawn_timer--;

    if (enemy_spawn_timer <= 0 )
    {
        enemy = malloc(sizeof(Entity));
        memset(enemy, 0, sizeof(Entity));
        stage.fighterTail->next = enemy;
        stage.fighterTail = enemy;

        enemy->x       = SCREEN_WIDTH;
        enemy->y       = rand() % SCREEN_HEIGHT;
        enemy->health  = 1;
        enemy->side    = SIDE_ENEMY;
        enemy->texture = enemy_texture;
        enemy->reload  = FPS * (1 + (rand() % 3));

        SDL_QueryTexture(enemy->texture, NULL, NULL, &enemy->w, &enemy->h);

        enemy->dx = -(2 + (rand() % 4));

        enemy_spawn_timer = 30 + (rand() % 60);
    }
}

static void draw(void)
{
    if (player == NULL)
        return;

    draw_background();
    draw_starfields();

    draw_player();
    draw_fighters();
    draw_bullets();

    draw_debris();
    draw_explosions();
}

static void draw_background(void)
{
    SDL_Rect dest;
    int x;
    for (x = backgroundX ; x < SCREEN_WIDTH ; x += SCREEN_WIDTH)
    {
        dest.x = x;
        dest.y = 0;
        dest.w = SCREEN_WIDTH;
        dest.h = SCREEN_HEIGHT;

        SDL_RenderCopy(app.renderer, background, NULL, &dest);
    }
}

static void draw_starfields(void)
{
    int i, c;

    for (i = 0 ; i < MAX_STARS ; i++)
    {
        c = 32 * stars[i].speed;

        SDL_SetRenderDrawColor(app.renderer, c, c, c, 255);
        SDL_RenderDrawLine(app.renderer, stars[i].x, stars[i].y, stars[i].x + 3, stars[i].y);
    }

}

static void draw_debris(void)
{
    Debris *d;

    for (d = stage.debrisHead.next ; d != NULL ; d = d->next)
    {
        blit_rect(d->texture, &d->rect, d->x, d->y);
    }
}

static void draw_explosions(void)
{
    Explosion *e;
    SDL_SetRenderDrawBlendMode(app.renderer, SDL_BLENDMODE_ADD);
    SDL_SetTextureBlendMode(explosion_texture, SDL_BLENDMODE_ADD);

    for (e = stage.explosionHead.next ; e != NULL ; e = e->next)
    {
        SDL_SetTextureColorMod(explosion_texture, e->r, e->g, e->b);
		SDL_SetTextureAlphaMod(explosion_texture, e->a);

		blit(explosion_texture, e->x, e->y);
    }

    SDL_SetRenderDrawBlendMode(app.renderer, SDL_BLENDMODE_NONE);
}

static void draw_player(void)
{
    blit(player->texture, player->x, player->y);
}

static void draw_fighters(void)
{
    Entity *e;

    for (e = stage.fighterHead.next ; e != NULL ; e = e->next)
    {
        blit(e->texture, e->x, e->y);
    }
}
static void draw_bullets(void)
{
    Entity *bullet;
    for (bullet = stage.bulletHead.next ; bullet != NULL ; bullet = bullet->next)
    {
        blit(bullet->texture, bullet->x, bullet->y);
    }
}
