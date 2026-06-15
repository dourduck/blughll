#include "render.h"
#include "game.h"
#include "raylib.h"
#include <stdio.h>

/* vvv DEBUG CONSLE vvv */
void Draw_DebugConsole(Input *input) {
  Rectangle r = {.x = 0, .y = 0, .width = GetRenderWidth(), .height = 300};
  DrawRectangleRec(r, Fade(BLACK, 0.6));

  int x = (input->mouseWorldPosition.x);
  int y = (input->mouseWorldPosition.y);

  DrawText(TextFormat("Mouse World Position Quantized: (%d, %d)", x, y), 10, 10,
           24, RAYWHITE);
}
/* ^^^ DEBUG CONSLE ^^^ */

void RenderUpdate(GameCtx *gameContext, Input *input, Camera2D camera) {
  BeginDrawing();
  ClearBackground(DARKGRAY);

  BeginMode2D(camera);

  int fontSize = 80;
  int text_x = -camera.offset.x + (int)(fontSize * 0.2);
  int text_y = -camera.offset.y;

  char buffer_text[32];
  snprintf(buffer_text, sizeof(buffer_text), "WATER: %d",
                             gameContext->water);
  DrawText(buffer_text, text_x, text_y, fontSize, SKYBLUE);
  text_y += (int)(fontSize * 1.5);

  snprintf(buffer_text, sizeof(buffer_text), "RADS: %d", gameContext->rads);
  DrawText(buffer_text, text_x, text_y, fontSize, LIME);
  text_y += (int)(fontSize * 1.5);

  snprintf(buffer_text, sizeof(buffer_text), "BOREDOM: %d",
                               gameContext->boredom);
  DrawText(buffer_text, text_x, text_y, fontSize, MAROON);

  EndMode2D();

  if (gameContext->console) {
    Draw_DebugConsole(input);
  }

  EndDrawing();
}

// /* vim:set ts=3 sw=2 sts=2 et: */
