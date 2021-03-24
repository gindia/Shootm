#include "common.h"
#include "structs.h"

static void init_player(void);

static void logic(void);
static void do_player(void);
static void do_fighters(void);
static void do_bullets(void);
static void do_enemies(void);

static void clip_player(void);

static void fire_bullet(void);
static void fire_enemy_bullet(Entity *e);

static void spwan_enemies(void);
static int  bullet_hit(Entity *bullet);

static void draw(void);
static void draw_player(void);
static void draw_fighters(void);
static void draw_bullets(void);

static void reset_stage(void);

void init_stage(void)
{
    //app.delegate.logic = logic;
    //app.delegate.logic = draw;

    memset(&stage, 0, sizeof(Stage));
    stage.fighterTail = &stage.fighterHead;
    stage.bulletTail  = &stage.bulletHead;

    player_texture       = load_texture("../assets/player.png");
    bullet_texture       = load_texture("../assets/playerBullet.png");

    enemy_texture        = load_texture("../assets/enemy.png");
    enemy_bullet_texture = load_texture("../assets/alienBullet.png");

    reset_stage();
}

static void reset_stage(void)
{
    Entity *e;
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

    memset(&stage, 0, sizeof(Stage));
    stage.fighterTail = &stage.fighterHead;
    stage.bulletTail  = &stage.bulletHead;

    init_player();

    enemy_spawn_timer = 0;

    stage_reset_timer = FPS * 2;
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

static void logic(void)
{
    if (player == NULL && --stage_reset_timer <= 0)
    {
        reset_stage();
    }

    do_player();

    do_enemies();

    do_fighters();

    do_bullets();

    spwan_enemies();

    clip_player();
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
    draw_player();
    draw_fighters();
    draw_bullets();
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
