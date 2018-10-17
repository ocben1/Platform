#include "splashkit.h"

#include <vector>
using std::vector;

#define GRAVITY 0.2
#define SPEED   6

struct player_data
{
    rectangle   shape;
    vector_2d   velocity;
};

struct platformer_data
{
    player_data         player;
    vector<rectangle>   platforms;
    int                 over_idx;
};

rectangle collision_area(const player_data &player)
{
    return rectangle_from(
        player.shape.x,
        player.shape.y + player.shape.height,
        player.shape.width,
        10 + player.velocity.y * 2
    );
}

bool on_platform(const player_data &player, const rectangle &platform)
{
    return rectangles_intersect(collision_area(player), platform);
}

void update_player(player_data &player, int idx, const vector<rectangle> &platforms)
{
    player.velocity.y += GRAVITY;
    
    player.shape.x += player.velocity.x;

    if ( idx != -1 and on_platform(player, platforms[idx]) and player.velocity.y > 0 )
    {
        player.shape.y = platforms[idx].y - player.shape.height;
        player.velocity.y = 0;
    }
    else
    {
        player.shape.y += player.velocity.y;
    }
}

void handle_input(platformer_data &game)
{
    game.player.velocity.x = 0;

    if ( game.over_idx != -1 )
    {
        if ( key_typed(SPACE_KEY) )
        {
            game.player.velocity.y = -SPEED;
        }
    }

    if ( key_down(LEFT_KEY) ) game.player.velocity.x -= SPEED;
    if ( key_down(RIGHT_KEY) ) game.player.velocity.x += SPEED;
}

void draw_player(const player_data &player)
{
    fill_rectangle(COLOR_RED, player.shape);
    
    double line_x, line_y;

    line_x = player.shape.x + player.shape.width / 2;
    line_y = player.shape.y + player.shape.height / 2;

    draw_line(COLOR_BLACK, 
        line_x, 
        line_y, 
        line_x + player.velocity.x * 10,
        line_y + player.velocity.y * 10 );

    draw_rectangle(COLOR_BLACK, collision_area(player));
}

player_data new_player()
{
    player_data result;
    result.shape = rectangle_from(50, 50, 100, 100);
    result.velocity = vector_to(0,0);
    return result;
}

void detect_over_platform(platformer_data &game)
{
    for(int i = 0; i < game.platforms.size(); i++)
    {
        if ( on_platform(game.player, game.platforms[i]) )
        {
            game.over_idx = i;
            return;
        }
    }

    game.over_idx = -1;
}

int main()
{
    open_window("Platformer Test", 800, 600);

    platformer_data game;
    game.player = new_player();
    game.platforms.push_back(rectangle_from(0, 500, 400, 20));
    game.platforms.push_back(rectangle_from(500, 300, 400, 20));
    game.platforms.push_back(rectangle_from(450, 400, 40, 20));

    while( not quit_requested() )
    {
        process_events();

        detect_over_platform(game);

        handle_input(game);
        update_player(game.player, game.over_idx, game.platforms);

        clear_screen(COLOR_WHITE);
        for(int i = 0; i < game.platforms.size(); i++)
        {
            fill_rectangle(COLOR_GREEN, game.platforms[i]);
        }
        draw_player(game.player);
        refresh_screen(60);
    }
    return 0;
}