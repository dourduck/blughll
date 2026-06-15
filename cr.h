#ifndef CR_H
#define CR_H

#include <assert.h>
#include <stdbool.h>

/* vvv [ Command Configuration ] vvv */

#define MAX_COMMANDS 256

/*
 * Increment/decrement character status type by some amount
 * Need to read character status and update visuals
 * Take input and update character status
 * Update character status per game tick
 */

typedef enum {
  CMD_WATER_UPDATE,
  CMD_RADS_UPDATE,
  CMD_BOREDOM_UPDATE,
} CmdKind;

typedef struct {
  int amount;
} Cmd_UpdateWater;

typedef struct {
  int amount;
} Cmd_UpdateRads;

typedef struct {
  int amount;
} Cmd_UpdateBoredom;

typedef struct {
  CmdKind kind;
  union {
    Cmd_UpdateWater updateWater;
    Cmd_UpdateRads updateRads;
    Cmd_UpdateBoredom updateBoredom;
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
