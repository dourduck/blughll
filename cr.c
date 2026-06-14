#include <assert.h>
#include <stdbool.h>
#include <string.h>

#include "cr.h"

void RingFlush(Ring *r, void (*execute)(Command, void *), void *ctx); 

void RingInitialize(Ring *r)
{ memset(r, 0, sizeof(*r)); }

int RingLen(const Ring *r) { return (r->head - r->tail) & MASK; }
bool RingEmpty(const Ring *r) { return r->head == r->tail; }
bool RingFull(const Ring *r) { return RingLen(r) == MAX_COMMANDS; }


bool RingCommandPush(Ring *r, Command command) {
  if (RingFull(r)) {
    return false;
  }

  // needs to wrap around max_commands
  r->buffer[r->head & MASK] = command;
  /* push increments head */
  r->head = (r->head + 1) & MASK;
  return true;
}

bool RingCommandPop(Ring *r, Command *out) {
  if (RingEmpty(r)) {
    return false;
  }

  /* Pop increments tail */
  /* Writes oldest command to "out" Command */

  *out = r->buffer[(r->tail & MASK)];
  r->tail = (r->tail + 1) & MASK;

  return true;
}

void RingFlush(Ring *r, void (*execute)(Command, void *), void *ctx) {
  /* [SUMMARY]
   * while the ring buffer is not empty
   * pop(write) oldest command to "command" variable
   * run command with a given state context "ctx"
   * repeat until ring buffer is empty
   * */
  Command command;
  while (RingCommandPop(r, &command)) {
    execute(command, ctx);
  }
}

// /* vim:set ts=3 sw=2 sts=2 et: */
