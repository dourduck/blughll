#include <stdbool.h>
#include <stddef.h>
// #include <string.h>

#include "game.h"
#include "raylib.h"

#include "cr.h"
#include "render.h"

/* vvv [UI Button] vvv */

typedef struct {
  int width;
  int height;
  int borderWidth;
  int fontSize;

  Color backgroundColor;
  Color foregroundColor;
  Color borderColor;
} ButtonConfig;

typedef struct {
  ButtonConfig config;

  Rectangle rect;
  const char *labelText;

  bool isPressed;
  bool isHovered;

} Button;

ButtonConfig CreateButtonConfig() {
  int width = 350;
  int height = 200;
  int font_size = 40;
  int border_width = 4;

  Color col_bground = DARKPURPLE;
  Color col_fground = LIGHTGRAY;
  Color col_border = RAYWHITE;

  return (ButtonConfig){
      .width = width,
      .height = height,
      .fontSize = font_size,
      .borderWidth = border_width,
      .backgroundColor = col_bground,
      .foregroundColor = col_fground,
      .borderColor = col_border,
  };
}

Button CreateButton(ButtonConfig config, int pos_x, int pos_y,
                    const char *label_text) {
  return (Button){.config = config,
                  .rect = (Rectangle){.x = pos_x,
                                      .y = pos_y,
                                      .width = config.width,
                                      .height = config.height},
                  .labelText = label_text,
                  .isHovered = false,
                  .isPressed = false};
}

bool ButtonIsHovered(Button *button, Input *input) {
  return CheckCollisionPointRec(input->mousePosition, button->rect);
}

bool ButtonIsPressed(Button *button, Input *input) {
  return (input->mouseLeftPressed && ButtonIsHovered(button, input));
}

typedef struct {
  Button btn_water;
  Button btn_rads;
  Button btn_boredom;
} ButtonGroup;

ButtonGroup CreateButtonGroup() {
  ButtonConfig config_wtr = CreateButtonConfig();
  ButtonConfig config_rad = CreateButtonConfig();
  ButtonConfig config_bor = CreateButtonConfig();

  int bx = GetScreenWidth() - (int)(config_wtr.width * 1.5);
  int by = (int)(config_wtr.height);
  int padding = config_wtr.borderWidth * 3;

  return (ButtonGroup){
      .btn_water = CreateButton(config_wtr, bx, by, "WATER"),
      .btn_rads = CreateButton(config_rad, bx,
                               by + (config_rad.height + padding), "RADS"),
      .btn_boredom = CreateButton(
          config_bor, bx, by + ((config_bor.height + padding) * 2), "BOREDOM"),
  };
}

void UIUpdate(Ring *ring, Input *input, ButtonGroup *btn_group) {
  if (ButtonIsPressed(&btn_group->btn_water, input)) {
    Command update_water = {
        .kind = CMD_WATER_UPDATE,
        .data = {20},
    };

    RingCommandPush(ring, update_water);

    Command update_tex = {.kind = CMD_UPDATE_TEXTURE};
    RingCommandPush(ring, update_tex);

  } else if (ButtonIsPressed(&btn_group->btn_rads, input)) {
    Command update_rads = {
        .kind = CMD_RADS_UPDATE,
        .data = {10},
    };

    RingCommandPush(ring, update_rads);

    Command update_tex = {.kind = CMD_UPDATE_TEXTURE};
    RingCommandPush(ring, update_tex);
  } else if (ButtonIsPressed(&btn_group->btn_boredom, input)) {
    Command update_boredom = {
        .kind = CMD_BOREDOM_UPDATE,
        .data = {-20},
    };

    RingCommandPush(ring, update_boredom);

    Command update_tex = {.kind = CMD_UPDATE_TEXTURE};
    RingCommandPush(ring, update_tex);
  }

  if (ButtonIsHovered(&btn_group->btn_water, input)) {
    btn_group->btn_water.config.borderColor = SKYBLUE;
  } else {
    btn_group->btn_water.config.borderColor = RAYWHITE;
  }

  if (ButtonIsHovered(&btn_group->btn_rads, input)) {
    btn_group->btn_rads.config.borderColor = LIME;
  } else {
    btn_group->btn_rads.config.borderColor = RAYWHITE;
  }

  if (ButtonIsHovered(&btn_group->btn_boredom, input)) {
    btn_group->btn_boredom.config.borderColor = MAGENTA;
  } else {
    btn_group->btn_boredom.config.borderColor = RAYWHITE;
  }
}

void DrawButton(Button *button) {
  // Background
  DrawRectangle(button->rect.x, button->rect.y, button->rect.width,
                button->rect.height, button->config.backgroundColor);
  // Outline
  DrawRectangleLinesEx(button->rect, button->config.borderWidth,
                       button->config.borderColor);

  // Text
  Vector2 text_size = MeasureTextEx(GetFontDefault(), button->labelText,
                                    button->config.fontSize, 0);

  int text_x =
      button->rect.x + (int)(button->config.width / 2) - (int)(text_size.x / 2);
  int text_y = button->rect.y + (int)(button->config.height / 2) -
               (int)(text_size.y / 2);

  DrawText(button->labelText, text_x, text_y, button->config.fontSize,
           button->config.foregroundColor);
}

void UIRender(ButtonGroup *btn_grp) {
  DrawButton(&btn_grp->btn_water);
  DrawButton(&btn_grp->btn_rads);
  DrawButton(&btn_grp->btn_boredom);
}

/* ^^^ [UI Button] ^^^ */

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
    if (gameCtx->boredom >= 70 || gameCtx->water <= 30 || gameCtx->rads <= 30) {
      gameCtx->currentSlimeTexture = gameCtx->slimeTexture_sad;
    } else if (gameCtx->boredom <= 20 && gameCtx->water >= 80 &&
               gameCtx->rads >= 80) {
      gameCtx->currentSlimeTexture = gameCtx->slimeTexture_happy;
    } else {
      gameCtx->currentSlimeTexture = gameCtx->slimeTexture_idle;
    }

    // switch (command.data.updateTexture.textureID) {
    // case TEXTURE_SLIME_IDLE:
    //   gameCtx->currentSlimeTexture = gameCtx->slimeTexture_idle;
    //   break;
    // case TEXTURE_SLIME_HAPPY:
    //   gameCtx->currentSlimeTexture = gameCtx->slimeTexture_happy;
    //   break;
    // case TEXTURE_SLIME_SAD:
    //   gameCtx->currentSlimeTexture = gameCtx->slimeTexture_sad;
    //   break;
    // }
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
  if (world->nextID <= ENTITY_MAX) {
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
  Vector2 mouse_position = GetMousePosition();

  input->mousePosition.x = mouse_position.x;
  input->mousePosition.y = mouse_position.y;

  Vector2 mouse_world_position = GetScreenToWorld2D(mouse_position, camera);
  input->mouseWorldPosition.x = mouse_world_position.x;
  input->mouseWorldPosition.y = mouse_world_position.y;

  input->keyPressed_F = IsKeyPressed(KEY_F);
  input->keyPressed_R = IsKeyPressed(KEY_R);
  input->mouseLeftPressed = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
  input->keyPressed_GRAVE = IsKeyPressed(KEY_GRAVE);

  // int x = (int)(floor(mouseWorldPosition.x / cellSize)) * cellSize;
  // int y = (int)(floor(mouseWorldPosition.y / cellSize)) * cellSize;
  // input->mouseWorldPositionQuantized.x = x;
  // input->mouseWorldPositionQuantized.y = y;
}

// TODO:
/*- Swap slime textures based on slime health status
 *  Limit status update commands (rads, water, boredom) per tick (~3 or so?)
 *  Play sound effects per status update command and when slime is sick
 *  Description text for slime status (Depleted, thirsty, bored, happy, sad,
 * etc)
 * */

void GameUpdate(Ring *ring, Input *input, float dt) {
  if (input->keyPressed_F) {
    ToggleFullscreen();
  } else if (input->keyPressed_GRAVE) {
    // gameContext->console = !gameContext->console;
  } else if (IsKeyPressed(KEY_ONE)) {
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

  Command command = {.kind = CMD_UPDATE_TEXTURE,
                     .data.updateTexture = {TEXTURE_SLIME_IDLE}};
  RingCommandPush(ring, command);
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

  StrWrite(TEXT_BORED, &gameContext.bored);
  StrWrite(TEXT_HUNGRY, &gameContext.hungry);
  StrWrite(TEXT_SAD, &gameContext.sad);

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
  tickRate = 5;

  Command command = {.kind = CMD_UPDATE_TEXTURE,
                     .data.updateTexture = {TEXTURE_SLIME_IDLE}};
  RingCommandPush(&ring, command);

  ButtonGroup btn_grp = CreateButtonGroup();

  while (!WindowShouldClose()) {
    InputPull(&input, camera);

    float dt = GetFrameTime();

    timer += dt;
    if (timer >= tickRate) {
      Tick(&ring);
      timer = 0;
    }

    UIUpdate(&ring, &input, &btn_grp);
    GameUpdate(&ring, &input, dt);
    RingFlush(&ring, execute, &gameContext);

    RenderUpdate(&gameContext, &input, camera);
    UIRender(&btn_grp);
  }

  UnloadTexture(gameContext.slimeTexture_idle);
  UnloadTexture(gameContext.slimeTexture_happy);
  UnloadTexture(gameContext.slimeTexture_sad);

  CloseWindow();
}

// /* vim:set ts=3 sw=2 sts=2 et: */
