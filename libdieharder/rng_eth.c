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
eth_get (void *vstate)
{
  eth_state_t *state = (eth_state_t *) vstate;
  unsigned long long int y = state->x; // at least 64 bits
  //printf("get1:%u\n", y);
  y = (y * state->x) % P1;
  //printf("get2:%u\n", y);
  y = (y * state->x) % P1;
  //printf("get3:%u\n", y);
  y = CLAMP(2,y,P1-2);
  state->x = (unsigned long int) y;
  //printf("get:%u\n", y);
  return y;
}

static double
eth_get_double (void *vstate)
{
  return (double) (eth_get(vstate)-2) / (double) (P1-4);
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
 (P1-2),			 /* RAND_MAX */
 2,				/* RAND_MIN */
 sizeof (eth_state_t),
 &eth_set,
 &eth_get,
 &eth_get_double};

const gsl_rng_type *gsl_rng_eth = &eth_type;
