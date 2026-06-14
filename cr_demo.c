#include "cr.h"
#include "raylib.h"

/*
 * NOTE:
 * Look into how "static" and "const" keywords work in C language context
 * */

typedef struct {
  int score;
  int some_global_data;
} GameCtx;

static void execute(Command command, void *context_ptr) {
  GameCtx *gameCtx = (GameCtx *)context_ptr;

  switch (command.kind) {
  case CMD_ENTITY_DESTROY:
    int entityID = command.data.entityDestroy.entityID;
    /* [Destroy Entity here] */
    break;
  case CMD_SCENE_LOAD:
    int sceneID = command.data.sceneLoad.sceneID;
    /* [Load Scene here] */
    break;
  default:
    break;
  }
}

int main(void) {
  SetConfigFlags(FLAG_FULLSCREEN_MODE);

  int width = 1080;
  int height = 720;
  char *title = "game";

  InitWindow(width, height, title);

  Ring ring;
  RingInitialize(&ring);
  GameCtx gameCtx;

  while (!WindowShouldClose()) {

    RingFlush(&ring, execute, &gameCtx);

    BeginDrawing();
    ClearBackground(SKYBLUE);

    EndDrawing();
  }

  CloseWindow();
  return 0;
}

// /* vim:set ts=3 sw=2 sts=2 et: */
