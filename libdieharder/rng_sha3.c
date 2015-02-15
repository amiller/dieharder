/*
 * A SHA3 prng
 */

#include <dieharder/libdieharder.h>
#include <stdint.h>
#include "sha3.h"

static unsigned long int sha3_get (void *vstate);
static double sha3_get_double (void *vstate);
static void sha3_set (void *vstate, unsigned long int s);

typedef struct
  {
    uint32_t seed;
    uint32_t counter;
  }
sha3_state_t;

static unsigned long int
sha3_get (void *vstate)
{
  sha3_state_t *state = (sha3_state_t *) vstate;

  // input is 8 bytes in total, seed and counter
  uint8_t *input = (uint8_t *) state;
  // output is 64 bytes in total (sha3-512)
  uint8_t output[64];
  keccak(input, 8, output, 64);

  // Copy the first 4 bytes of the output as the random value
  uint32_t x = *((uint32_t *) output);

  // Update the state by incrementing the counter
  state->counter++; // This has a 32-bit cycle length
  //printf("[sha3] get:%u %u %u\n", state->seed, state->counter, x);

  return x;
}

static double
sha3_get_double (void *vstate)
{
  return (double) (sha3_get(vstate)) / (double) (UINT_MAX);
}

static void
sha3_set (void *vstate, unsigned long int s)
{
  sha3_state_t *state = (sha3_state_t *) vstate;
  state->seed = s;
  state->counter = 0;
  //printf("[sha3] set:%p %u\n", (void *) state, state->seed);
  return;
}

static const gsl_rng_type sha3_type =
{"sha3",			/* name */
 0,			        /* RAND_MAX */
 UINT_MAX,			/* RAND_MIN */
 sizeof (sha3_state_t),
 &sha3_set,
 &sha3_get,
 &sha3_get_double};

const gsl_rng_type *gsl_rng_sha3 = &sha3_type;
