#include <stdbool.h>

#include "game.h"
#include "raylib.h"

#include "cr.h"
#include "render.h"

typedef enum {
  TEXTURE_NONE = 0,
  TEXTURE_SLIME_IDLE = 1,
  TEXTURE_SLIME_HAPPY = 2,
  TEXTURE_SLIME_SAD = 3,
} TextureID;

static void execute(Command command, void *context_ptr) {
  GameCtx *gameCtx = (GameCtx *)context_ptr;

  switch (command.kind) {
  case CMD_WATER_UPDATE:
    if (gameCtx->water > -(command.data.updateWater.amount)) {
      gameCtx->water += command.data.updateWater.amount;
    } else {
      gameCtx->water = 0;
    }
    break;
  case CMD_RADS_UPDATE:
    if (gameCtx->rads > -(command.data.updateRads.amount)) {
      gameCtx->rads += command.data.updateRads.amount;
    } else {
      gameCtx->rads = 0;
    }
    break;
  case CMD_BOREDOM_UPDATE:
    if (gameCtx->boredom < (100 - command.data.updateBoredom.amount)) {
      gameCtx->boredom += command.data.updateBoredom.amount;
    } else {
      gameCtx->boredom = 100;
    }
    break;
  case CMD_UPDATE_TEXTURE:
    switch (command.data.updateTexture.textureID) {
    case TEXTURE_SLIME_IDLE:
      gameCtx->currentSlimeTexture = gameCtx->slimeTexture_idle;
      break;
    case TEXTURE_SLIME_HAPPY:
      gameCtx->currentSlimeTexture = gameCtx->slimeTexture_happy;
      break;
    case TEXTURE_SLIME_SAD:
      gameCtx->currentSlimeTexture = gameCtx->slimeTexture_sad;
      break;
    }
    break;
  default:
    break;
  }
}

void EntityVelocitySet(World *world, EntityID entityID, float dx, float dy) {
  world->dx[entityID] = dx;
  world->dy[entityID] = dy;
}

void EntityVelocityApply(World *world, EntityID entityID, float dt) {
  world->x[entityID] += (world->dx[entityID] * dt);
  world->y[entityID] += (world->dy[entityID] * dt);
}

World World_Create() { return (World){.nextID = 1}; }

EntityID WorldEntity_Create(World *world) {
  if (world->nextID <= MAX_ENTITIES) {
    EntityID entityID = world->nextID++;
    world->active[entityID] = true;
    return entityID;
  } else {
    return NIL;
  }
}

void WorldEntity_TraitsAdd(World *world, EntityID entityID, TraitFlag flag) {
  world->traits[entityID] |= flag;
}

void WorldEntity_TraitsRemove(World *world, EntityID entityID, TraitFlag flag) {
  world->traits[entityID] &= ~(flag);
}

void WorldEntity_TraitsClear(World *world, EntityID entityID) {
  world->traits[entityID] = TRAITS_NIL;
}

bool WorldEntity_HasTrait(World *world, EntityID entityID, TraitFlag flag) {
  return (world->traits[entityID] & flag) == flag;
}

void WorldEntity_Remove(World *world, EntityID entityID) {
  world->active[entityID] = false;
  world->x[entityID] = NIL;
  world->y[entityID] = NIL;
  world->dx[entityID] = NIL;
  world->dy[entityID] = NIL;
  world->traits[entityID] = TRAITS_NIL;
}

void WorldEntity_SetPosition(World *world, EntityID entityID, float x,
                             float y) {
  world->x[entityID] = x;
  world->y[entityID] = y;
}

void WorldEntity_SetVelocity(World *world, EntityID entityID, float dx,
                             float dy) {
  world->dx[entityID] = dx;
  world->dy[entityID] = dy;
}

void InputPull(Input *input, Camera2D camera) {
  Vector2 mouseWorldPosition = GetScreenToWorld2D(GetMousePosition(), camera);
  input->mouseWorldPosition.x = mouseWorldPosition.x;
  input->mouseWorldPosition.y = mouseWorldPosition.y;

  input->keyPressed_F = IsKeyPressed(KEY_F);
  input->keyPressed_R = IsKeyPressed(KEY_R);
  input->mouseLeftPressed = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
  input->keyPressed_GRAVE = IsKeyPressed(KEY_GRAVE);

  // int x = (int)(floor(mouseWorldPosition.x / cellSize)) * cellSize;
  // int y = (int)(floor(mouseWorldPosition.y / cellSize)) * cellSize;
  // input->mouseWorldPositionQuantized.x = x;
  // input->mouseWorldPositionQuantized.y = y;
}

void GameUpdate(Ring *ring, Input *input, float dt) {
  if (input->keyPressed_F) {
    ToggleFullscreen();
  } else if (input->keyPressed_GRAVE) {
    // gameContext->console = !gameContext->console;
  } else if (IsKeyPressed(KEY_ONE)) {
    Command command = {.kind = CMD_UPDATE_TEXTURE,
                       .data.updateTexture = {TEXTURE_SLIME_IDLE}};
    RingCommandPush(ring, command);
  } else if (IsKeyPressed(KEY_TWO)) {
    Command command = {.kind = CMD_UPDATE_TEXTURE,
                       .data.updateTexture = {TEXTURE_SLIME_HAPPY}};
    RingCommandPush(ring, command);
  } else if (IsKeyPressed(KEY_THREE)) {
    Command command = {.kind = CMD_UPDATE_TEXTURE,
                       .data.updateTexture = {TEXTURE_SLIME_SAD}};
    RingCommandPush(ring, command);
  }
}

void Tick(Ring *ring) {
  Command updateRads = {
      .kind = CMD_RADS_UPDATE,
      .data = {-3},
  };

  RingCommandPush(ring, updateRads);

  Command updateWater = {
      .kind = CMD_WATER_UPDATE,
      .data = {-10},
  };

  RingCommandPush(ring, updateWater);

  Command updateBoredom = {
      .kind = CMD_BOREDOM_UPDATE,
      .data = {5},
  };

  RingCommandPush(ring, updateBoredom);
}

void GameRun(GameConfig *config) {
  Ring ring;
  RingInitialize(&ring);

  World world = World_Create();

  GameCtx gameContext = {
      .world = &world,
      .water = 100,
      .rads = 100,
      .boredom = 0,
  };

  SetConfigFlags(FLAG_FULLSCREEN_MODE);

  int screenWidth = config->screenWidth;
  int screenHeight = config->screenHeight;
  const char *title = config->gameTitle;

  InitWindow(screenWidth, screenHeight, title);
  SetExitKey(KEY_Q);

  int width = GetRenderWidth();
  int height = GetRenderHeight();
  Vector2 offset = {(int)(width / 2), (int)(height / 2)};

  Camera2D camera = {.offset = offset, .zoom = 1.0};
  SetTargetFPS(60);
  Input input;

  gameContext.slimeTexture_idle = LoadTexture("./art/blug_idle.png");
  gameContext.slimeTexture_happy = LoadTexture("./art/blug_happy.png");
  gameContext.slimeTexture_sad = LoadTexture("./art/blug_sad.png");

  gameContext.currentSlimeTexture = gameContext.slimeTexture_idle;

  gameContext.source = (Rectangle){
      .x = 0,
      .y = 0,
      .width = gameContext.currentSlimeTexture.width,
      .height = gameContext.currentSlimeTexture.height,
  };

  gameContext.destination = (Rectangle){
      .x = -gameContext.currentSlimeTexture.width * 4,
      .y = -gameContext.currentSlimeTexture.height * 4,
      .width = gameContext.source.width * 8,
      .height = gameContext.source.height * 8,
  };

  gameContext.rotation = 0;
  gameContext.origin = (Vector2){0, 0};
  gameContext.tint = WHITE;

  SetTextureFilter(gameContext.currentSlimeTexture, TEXTURE_FILTER_POINT);

  float timer;
  timer = 0;
  float tickRate;
  tickRate = 1;

  while (!WindowShouldClose()) {
    InputPull(&input, camera);

    float dt = GetFrameTime();

    timer += dt;
    if (timer >= tickRate) {
      Tick(&ring);
      timer = 0;
    }

    GameUpdate(&ring, &input, dt);
    RingFlush(&ring, execute, &gameContext);

    RenderUpdate(&gameContext, &input, camera);
  }

  UnloadTexture(gameContext.slimeTexture_idle);
  UnloadTexture(gameContext.slimeTexture_happy);
  UnloadTexture(gameContext.slimeTexture_sad);

  CloseWindow();
}

// /* vim:set ts=3 sw=2 sts=2 et: */
