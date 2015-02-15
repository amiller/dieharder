/*
 * Inspired by BLUM BLUM SHUB
 * See https://github.com/ethereum/wiki/wiki/Ethash
 */

#include <dieharder/libdieharder.h>

#define MIN(a,b) ((a < b) ? (a) : (b))
#define MAX(a,b) ((a > b) ? (a) : (b))
#define CLAMP(lo,x,hi) (MAX(MIN((x),(hi)), (lo)))


static unsigned long int eth_get (void *vstate);
static double eth_get_double (void *vstate);
static void eth_set (void *vstate, unsigned long int s);

#define P1 (4294967087)
#define P2 (4294963787)

typedef struct
  {
    unsigned long int x;
  }
eth_state_t;

static unsigned long int
_eth_get (void *vstate)
{
  eth_state_t *state = (eth_state_t *) vstate;
  unsigned long long int y = state->x; // at least 64 bits
  y = (y * state->x) % P1;
  y = (y * state->x) % P1;
  y = CLAMP(2,y,P1-2);
  state->x = (unsigned long int) y;
  return y;
}

// A wrapper, that runs the original eth_get (in the range [2,P1-2] and
// projects it (using rejection sampling) to the range [0,2**31)
static unsigned long int
eth_get (void *vstate)
{
  while (1) {
    unsigned long int c = _eth_get(vstate) - 2;
    if (c > (1L << 31)-1) continue;
    return c;
  }
}

static double
eth_get_double (void *vstate)
{
  return (double) (eth_get(vstate)) / (double) ((1L<<31)-1);
}

static void
eth_set (void *vstate, unsigned long int s)
{
  eth_state_t *state = (eth_state_t *) vstate;
  state->x = CLAMP(2,s,P1-2);
  printf("set:%p %u\n", state, state->x);
  return;
}

static const gsl_rng_type eth_type =
{"eth",			/* name */
 (1L<<31)-1,
 0,
 sizeof (eth_state_t),
 &eth_set,
 &eth_get,
 &eth_get_double};

const gsl_rng_type *gsl_rng_eth = &eth_type;
