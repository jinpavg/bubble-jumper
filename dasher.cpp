#include "raylib.h"

struct AnimData
{
    int framecount;
    Rectangle rec;
    Vector2 pos;
    int frame;
    float updateTime;
    float runningTime;
};

bool IsOnGround(AnimData data, int windowHeight)
{
    return data.pos.y >= windowHeight - data.rec.height;
}

AnimData UpdateAnimData (AnimData data, float deltaTime) 
{
    // update running time
    data.runningTime += deltaTime;
    if (data.runningTime >= data.updateTime)
    {
        data.runningTime = 0.0;
        // update animation frame
        data.rec.x = data.frame * data.rec.width;
        data.frame++;
        if (data.frame > (data.framecount - 1))
        {
            data.frame = 0;
        }
    }
    return data;
}

int main()
{
    // window dims
    int windowDimensions[2];
    windowDimensions[0] = 512;
    windowDimensions[1] = 380;

    // initialize window
    InitWindow(windowDimensions[0], windowDimensions[1], "dash!");

    // acceleration due to gravity (pixels/s)/s
    const int gravity{1'000};

    // Nebula variables
    Texture2D nebula = LoadTexture("textures/spikey.png");

    const int sizeOfNebulae{6};
    AnimData nebulae[sizeOfNebulae]{};

    for (int i = 0; i < sizeOfNebulae; i++)
    {
        nebulae[i].framecount = 3;
        nebulae[i].rec.x = 0.0;
        nebulae[i].rec.y = 0.0;
        nebulae[i].rec.width = nebula.width/nebulae[i].framecount;
        nebulae[i].rec.height = nebula.height;
        nebulae[i].pos.y = windowDimensions[1] - nebula.height;
        nebulae[i].frame = 0;
        nebulae[i].updateTime = 1.0/12.0;
        nebulae[i].runningTime = 0.0;

        nebulae[i].pos.x = windowDimensions[0] + (i * 300);
    }

    float finishLine{ nebulae[sizeOfNebulae - 1].pos.x };
    
    // nebula velocity
    int nebVel{-200};

    // scarfy variables
    Texture2D scarfy = LoadTexture("textures/avg-buddy.png");
    AnimData scarfyData{
        6,
        {0.0, 0.0, scarfy.width/scarfyData.framecount, scarfy.height },
        {windowDimensions[0]/2 - scarfyData.rec.width/2, windowDimensions[1] - scarfyData.rec.height},
        0,
        1.0/12.0,
        0.0
    };

    // is the rectangle in the air?
    bool isInAir{};
    // jump velocity (pixels/second)
    const int jumpVel{-600};

    int velocity{0};

    Texture2D background = LoadTexture("textures/background-avg.png");
    Texture2D midground = LoadTexture("textures/midground-aadavg.png");
    Texture2D foreground = LoadTexture("textures/foreground-avg.png");
    float bgX{};
    float mgX{};
    float fgX{};

    bool collision{};

    SetTargetFPS(60);
    while (!WindowShouldClose())
    {
        // delta time (time since last frame)
        const float dT{ GetFrameTime() };

        // start drawing
        BeginDrawing();
        ClearBackground(WHITE);

        // background scroll
        bgX -= 20 * dT;
        if (bgX <= -background.width*2)
        {
            bgX = 0.0;
        }

        // draw the background
        Vector2 bg1Pos{bgX, 0.0};
        DrawTextureEx(background, bg1Pos, 0.0, 2.0, WHITE);
        Vector2 bg2Pos{bgX + background.width*2, 0.0};
        DrawTextureEx(background, bg2Pos, 0.0, 2.0, WHITE);

        // midground scroll
        mgX -= 40 * dT;
        if (mgX <= -midground.width*2)
        {
            mgX = 0.0;
        }

        // draw the midground
        Vector2 mg1Pos{mgX, 0.0};
        DrawTextureEx(midground, mg1Pos, 0.0, 2.0, WHITE);
        Vector2 mg2Pos{mgX + midground.width*2, 0.0};
        DrawTextureEx(midground, mg2Pos, 0.0, 2.0, WHITE);

        // foreground scroll
        fgX -= 80 * dT;
        if (fgX <= -foreground.width*2)
        {
            fgX = 0.0;
        }

        // draw the foreground
        Vector2 fg1Pos{fgX, 0.0};
        DrawTextureEx(foreground, fg1Pos, 0.0, 2.0, WHITE);
        Vector2 fg2Pos{fgX + foreground.width*2, 0.0};
        DrawTextureEx(foreground, fg2Pos, 0.0, 2.0, WHITE);

        // ground check for scarfy
        if(IsOnGround(scarfyData, windowDimensions[1]))
        {
          // we are on ground
          velocity = 0;
          isInAir = false;
        }
        else
        {
            // we are in the air
            velocity += gravity * dT;
            isInAir = true;
        }
        
        // scarfy jump
        if (IsKeyPressed(KEY_SPACE) && !isInAir)
        {
            velocity += jumpVel;
        }
        
        for (int i = 0; i < sizeOfNebulae; i++)
        {
            // update nebula position
            nebulae[i].pos.x += nebVel * dT;
            
            // update nebula animation frame
            nebulae[i] = UpdateAnimData(nebulae[i], dT); 
        }

        
        for (AnimData nebula : nebulae)
        {
            float padding{50};
            Rectangle nebRec{
                nebula.pos.x + padding,
                nebula.pos.y + padding,
                nebula.rec.width - (2 * padding),
                nebula.rec.height - (2 * padding),
            };
            Rectangle scarfyRec{
                scarfyData.pos.x,
                scarfyData.pos.y,
                scarfyData.rec.width,
                scarfyData.rec.height,
            };

            if (CheckCollisionRecs(nebRec, scarfyRec))
            {
                collision = true;
            }

        }


        // update finish line position
        finishLine += nebVel * dT;

        // update scarfy position
        scarfyData.pos.y += velocity * dT;

        // update scarfy animation frame
        if (!isInAir)
        {
            scarfyData = UpdateAnimData(scarfyData, dT);
        }

        if (collision)
        {
            // lose the game
            DrawText("Game Over :(", windowDimensions[0]/4, windowDimensions[1]/2, 30, WHITE);
        }
        else if(scarfyData.pos.x >= finishLine)
        {
            DrawText("You Win!", windowDimensions[0]/4, windowDimensions[1]/2, 30, WHITE);
        }
        else
        {
            for (int i = 0; i < sizeOfNebulae; i++)
            {
                // draw nebulae
                DrawTextureRec(nebula, nebulae[i].rec, nebulae[i].pos, WHITE); 
            }

            // draw scarfy
            DrawTextureRec(scarfy, scarfyData.rec, scarfyData.pos, WHITE);
        }
        
        EndDrawing();
    }
    UnloadTexture(scarfy); // can i make an array for textures to load/unload and handle this with for loops?
    UnloadTexture(nebula);
    UnloadTexture(background);
    UnloadTexture(midground);
    UnloadTexture(foreground);

    CloseWindow();
}