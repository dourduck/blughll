#include "game.h"
#include "raylib.h"

// #include "raylib.h"
#include "render.h"
// #include <math.h>

#include <stdbool.h>
// #include <string.h>

// tagged enum

// typedef int PositionX;
// typedef int PositionY;
// typedef float VelocityY;

// typedef struct {
//   Color color;
//   VelocityY dy;
//   int radius;
// } Puck;

// #define GRID_COLS 7
// #define GRID_ROWS 7
//
// typedef struct {
//   Rectangle cellRectangles[GRID_ROWS][GRID_COLS];
//   int cellSize;
//   int lineThickness;
//   Color color;
// } Grid;
//
// typedef struct {
//   const char *msg;
//   int width;
//   int height;
//   int paddingX;
//   int paddingY;
//   Color textColor;
//   Color backgroundColor;
//   int fontSize;
// } UIMessage;
//
// typedef enum { PUCK, GRID, UI_MESSAGE } EntityKind;
//
// typedef struct {
//   EntityKind kind;
//   PositionX x;
//   PositionY y;
//   bool active;
//
//   union {
//     Puck puck;
//     Grid grid;
//     UIMessage uiMessage;
//   };
// } Entity;
//
// #define MAX_ENTITIES 64
//
// typedef struct {
//   Entity slots[MAX_ENTITIES];
//   int count;
// } EntityPool;
//
// Entity *PoolSpawn(EntityPool *pool, EntityKind kind) {
//   for (int i = 0; i < MAX_ENTITIES; i++) {
//     if (!pool->slots[i].active) {
//       Entity *e = &pool->slots[i];
//       memset(e, 0, sizeof(Entity)); // not sure exactly how memset works
//       e->active = true;
//       e->kind = kind;
//       if (i >= pool->count) {
//         pool->count = i + 1;
//       }
//       return e;
//     }
//   }
//
//   return NULL;
// }
//
// void PoolKill(Entity *e) { e->active = false; }
//
// void DrawPuck(Puck puck, PositionX x, PositionY y) {
//   DrawCircle(x, y, puck.radius, puck.color);
// }
//
// void InitializeGrid(Grid *grid, PositionX posX, PositionY posY) {
//   for (int yi = 0; yi < GRID_ROWS; yi++) {
//     for (int xi = 0; xi < GRID_COLS; xi++) {
//       grid->cellRectangles[yi][xi] = (Rectangle){
//           .x = (xi * grid->cellSize) + posX,
//           .y = (yi * grid->cellSize) + posY,
//           .width = grid->cellSize,
//           .height = grid->cellSize,
//       };
//     }
//   }
// }
//
// void DrawDaGrid(Grid grid) {
//   for (int y = 0; y < GRID_ROWS; y++) {
//     for (int x = 0; x < GRID_COLS; x++) {
//       DrawRectangleLinesEx(grid.cellRectangles[y][x], grid.lineThickness,
//                            grid.color);
//     }
//   }
// }
//
// void DrawUIMessage(UIMessage uiMessage, PositionX posX, PositionY posY) {
//   int paddingX_half = uiMessage.paddingX / 2;
//   int paddingY_half = uiMessage.paddingY / 2;
//
//   DrawRectangle(posX, posY, uiMessage.width + paddingX_half,
//                 uiMessage.height + paddingY_half, uiMessage.backgroundColor);
//
//   DrawText(uiMessage.msg, posX, posY, uiMessage.fontSize,
//   uiMessage.textColor);
// }
//
// void PoolDraw(EntityPool *pool) {
//   for (int i = 0; i < pool->count; i++) {
//     Entity *e = &pool->slots[i];
//     if (!e->active) {
//       continue;
//     }
//     switch (e->kind) {
//     case PUCK:
//       DrawPuck(e->puck, e->x, e->y);
//       break;
//     case GRID:
//       DrawDaGrid(e->grid);
//       break;
//     case UI_MESSAGE:;
//       DrawUIMessage(e->uiMessage, e->x, e->y);
//       break;
//     }
//   }
// }

// Puck CheckForWin(World *world, int puckGridIndex, Puck puckValue) {
//   int deltas[4][2] = {
//       {-1, 0},  // West -> East
//       {-1, -1}, // North West -> South East
//       {0, -1},  // North -> South
//       {1, -1},  // North East -> South West
//   };
//
//   int stride = GAME_COLUMN;
//   int puckX = puckGridIndex % stride;
//   int puckY = puckGridIndex / stride;
//
//   for (int i = 0; i < 4; i++) {
//     int dx = deltas[i][0];
//     int dy = deltas[i][1];
//
//     for (int j = -3; j <= 0; j++) {
//       int xx = puckX + (dx * j);
//       int yy = puckY + (dy * j);
//
//       int matchCount = 0;
//
//       for (int k = 0; k < 4; k++) {
//         int nx = xx + ((dx)*k);
//         int ny = yy + ((dy)*k);
//
//         if (nx >= 0 && nx < stride && ny >= 0 && ny < GAME_ROW) {
//
//           int idx = ny * stride + nx;
//           int nPuck = world->grid[idx];
//
//           if (idx == puckGridIndex || nPuck == puckValue) {
//             matchCount++;
//           }
//         }
//       }
//
//       if (matchCount >= 4) {
//         return puckValue;
//       }
//     }
//   }
//
//   return PUCK_NIL;
// }

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

void GameUpdate(World *world, Input *input, float dt) {
  if (input->keyPressed_F) {
    ToggleFullscreen();
  }

  // else if (input->keyPressed_R) {
  //   for (int i = 0; i < world->nextID; i++) {
  //     WorldEntity_Remove(world, i);
  //     world->puckColumnIndex[i] = 0;
  //   }
  //
  //   world->nextID = 1;
  //   world->currentPuckIndex = 0;
  //   world->currentPuckTeam = PUCK_NIL;
  //
  //   for (int i = 0; i < 7; i++) {
  //     world->columnStopPosition[i] = 0;
  //   }
  //   world->puckFalling = false;
  //   world->console = false;
  //
  //   for (int i = 0; i < GAME_COLUMN * GAME_ROW; i++) {
  //     world->grid[i] = PUCK_NIL;
  //   }
  //
  //   world->winner = PUCK_NIL;
  //
  //   int stride = 7;
  //   int lineWidth = 4;
  //   int cellSize_half = cellSize / 2;
  //   int puckRadius = cellSize_half - (int)(lineWidth * 1.5);
  //
  //   // Cell Creation
  //   for (int i = 0; i < (stride * stride); i++) {
  //     EntityID entityID = WorldEntity_Create(world);
  //
  //     int x = (i % stride);
  //     int y = (i / stride);
  //
  //     WorldEntity_SetPosition(world, entityID, (x * cellSize), (y *
  //     cellSize));
  //
  //     Traits traits = TRAITS_CELL | TRAITS_POSITIONABLE;
  //     WorldEntity_TraitsAdd(world, entityID, traits);
  //   }
  // }

  // if (world->winner == PUCK_NIL) {
  //   int x = input->mouseWorldPositionQuantized.x;
  //   int y = input->mouseWorldPositionQuantized.y;
  //
  //   if (input->mouseLeftPressed && !world->puckFalling) {
  //     if (world->currentPuckTeam == PUCK_NIL) {
  //       world->currentPuckTeam = PUCK_BLUE;
  //     } else {
  //       world->currentPuckTeam =
  //           world->currentPuckTeam == PUCK_BLUE ? PUCK_RED : PUCK_BLUE;
  //     }
  //
  //     if (x >= 0 && x < (GAME_COLUMN * cellSize) && y >= 0 &&
  //         y < (GAME_ROW * cellSize)) {
  //       int column = x / cellSize;
  //       if (world->columnStopPosition[column] < GAME_ROW) {
  //         int entityID = CreatePuck(world, world->currentPuckTeam, x, 0,
  //                                   cellSize, (cellSize / 2));
  //         EntityVelocitySet(world, entityID, 0, GRAVITY);
  //
  //         world->puckColumnIndex[entityID] = column;
  //         world->puckFalling = true;
  //         world->currentPuckIndex = entityID;
  //       }
  //     }
  //   }

  //   if (world->puckFalling && world->currentPuckIndex != NIL) {
  //     EntityVelocityApply(world, world->currentPuckIndex, dt);
  //
  //     int column = world->puckColumnIndex[world->currentPuckIndex];
  //     int stopPosition =
  //         ((GAME_ROW - world->columnStopPosition[column]) * cellSize) -
  //         cellSize_half;
  //
  //     if (world->y[world->currentPuckIndex] >= stopPosition) {
  //       world->dy[world->currentPuckIndex] = 0;
  //       world->y[world->currentPuckIndex] = stopPosition;
  //       world->columnStopPosition[column]++;
  //
  //       int gridIdx = ((stopPosition) / cellSize) * GAME_ROW + column;
  //       world->grid[gridIdx] = world->currentPuckTeam;
  //
  //       world->winner = CheckForWin(world, gridIdx, world->currentPuckTeam);
  //
  //       world->puckFalling = false;
  //     }
  //   }
  // }
}

// EntityID CreatePuck(World *world, Puck puck, int posX, int posY, int
// cellSize,
//                     int cellSize_half) {
//   EntityID entityID = WorldEntity_Create(world);
//
//   WorldEntity_SetPosition(world, entityID, posX + cellSize_half,
//                           posY + cellSize_half);
//
//   WorldEntity_SetPuck(world, entityID, puck);
//
//   Traits traits = TRAITS_PUCK | TRAITS_POSITIONABLE | TRAITS_PHYSICS;
//   WorldEntity_TraitsAdd(world, entityID, traits);
//
//   return entityID;
// }

void GameRun(GameConfig *config) {
  World world = World_Create();

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

  // Cell Creation
  // for (int i = 0; i < (stride * stride); i++) {
  //   EntityID entityID = WorldEntity_Create(&world);
  //
  //   int x = (i % stride);
  //   int y = (i / stride);
  //
  //   WorldEntity_SetPosition(&world, entityID, (x * cellSize), (y *
  //   cellSize));
  //
  //   Traits traits = TRAITS_CELL | TRAITS_POSITIONABLE;
  //   WorldEntity_TraitsAdd(&world, entityID, traits);
  // }

  while (!WindowShouldClose()) {
    InputPull(&input, camera);

    if (input.keyPressed_GRAVE == true) {
      world.console = !world.console;
    }

    float dt = GetFrameTime();

    GameUpdate(&world, &input, dt);
    RenderUpdate(&world, &input, camera);
  }

  CloseWindow();
}

// /* vim:set ts=3 sw=2 sts=2 et: */
