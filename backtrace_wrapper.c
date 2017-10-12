#include <stdio.h>
#include <string.h>
#include <signal.h>
#include "backtrace.h"
#include "backtrace-supported.h"

struct bt_ctx {
  struct backtrace_state *state;
  //int   error;
  int   oft;
  int   buf_len;
  char  *output;
};

static void error_callback(void *data, const char *msg, int errnum)
{
  struct bt_ctx *ctx = data;
  ctx->oft += snprintf(ctx->output+ctx->oft, ctx->buf_len - ctx->oft, "ERROR: %s (%d)", msg, errnum);
  //ctx->error = 1;
}

static void syminfo_callback (void *data, uintptr_t pc, const char *symname, uintptr_t symval, uintptr_t symsize)
{
  struct bt_ctx *ctx = data;
  if (symname) {
    ctx->oft += snprintf(ctx->output+ctx->oft, ctx->buf_len - ctx->oft, "%016lx %32s ??:0\n", (unsigned long)pc, symname);
  } else {
    ctx->oft += snprintf(ctx->output+ctx->oft, ctx->buf_len - ctx->oft, "%016lx %32s ??:0\n", (unsigned long)pc, "??");
  }
}

static int full_callback(void *data, uintptr_t pc, const char *filename, int lineno, const char *function)
{
  struct bt_ctx *ctx = data;
  if (function) {
    ctx->oft += snprintf(ctx->output+ctx->oft, ctx->buf_len - ctx->oft, "%016lx %32s %s:%d\n",
        (unsigned long)pc, function, filename?filename:"??", lineno);
  } else {
    backtrace_syminfo (ctx->state, pc, syminfo_callback, error_callback, data);
  }
  return 0;
}

static int simple_callback(void *data, uintptr_t pc)
{
  struct bt_ctx *ctx = data;
  backtrace_pcinfo(ctx->state, pc, full_callback, error_callback, data);
  return 0;
}

//static inline int bt(struct backtrace_state *state, struct bt_ctx *ctx)
static int bt(struct backtrace_state *state, struct bt_ctx *ctx)
{
  //backtrace_print(state, 0, stdout); // too simple
  backtrace_simple(state, 0, simple_callback, error_callback, ctx);
}

int get_stack_str(char *stack_str, int *len)
{
  //struct backtrace_state *state = backtrace_create_state(NULL, BACKTRACE_SUPPORTS_THREADS, error_callback, NULL);
  struct backtrace_state *state = backtrace_create_state(NULL, BACKTRACE_SUPPORTS_THREADS, NULL, NULL);
  struct bt_ctx ctx = {state, 0, *len, stack_str};
  bt(state, &ctx);
  *len = ctx.oft;
}
