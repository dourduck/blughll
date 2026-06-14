#ifndef CR_H
#define CR_H

#include <assert.h>
#include <stdbool.h>

/* vvv [ Command Configuration ] vvv */

#define MAX_COMMANDS 256

typedef enum {
  CMD_ENTITY_DESTROY,
  CMD_SCENE_LOAD,
} CmdKind;

typedef struct {
  int entityID;
} Command_EntityDestroy;

typedef struct {
  int sceneID;
} Command_SceneLoad;

typedef struct {
  CmdKind kind;
  union {
    Command_EntityDestroy entityDestroy;
    Command_SceneLoad sceneLoad;
  } data;
} Command;

/* ^^^ [ Command Configuration ] ^^^ */

#define MASK (MAX_COMMANDS - 1)
static_assert((MAX_COMMANDS & MASK) == 0,
              "MAX_COMMANDS should be a power of two.");

typedef struct {
  Command buffer[MAX_COMMANDS];
  int head;
  int tail;
} Ring;

void RingInitialize(Ring *r);

int RingLen(const Ring *r);
bool RingEmpty(const Ring *r);
bool RingFull(const Ring *r);

bool RingCommandPush(Ring *r, Command command);
bool RingCommandPop(Ring *r, Command *out);

void RingFlush(Ring *r, void (*execute)(Command, void *), void *ctx);

#endif
// /* vim:set ts=3 sw=2 sts=2 et: */
