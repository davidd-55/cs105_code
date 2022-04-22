#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

// Definition for the club struct including the
// number of CS and math majors present
typedef struct club {
  int comp_count;
  int math_count;
  int capacity;

  // TODO(Task 1): add synchronization primitives for accessing club data
  //               and ensuring the club is always exclusively CS or math majors

  // TODO(Task 2): add a lock for preventing starvation
} club_t;

// ----------------------------------------------------------------
// Global Variables
// ----------------------------------------------------------------

// Simulation parameters
const int NUM_COMP = 5;
const int NUM_MATH = 5;

// TODO(Task 3): change the capacity to something less than NUM_COMP + NUM_MATH
const int CLUB_CAPACITY = NUM_COMP + NUM_MATH;
club_t daclub229;

// ----------------------------------------------------------------
// Club Utilities
// ----------------------------------------------------------------

// Check the status of the club
void sanitycheck(club_t *club) {

  // Check for invalid mixing
  if (club->comp_count > 0 && club->math_count > 0) {
    printf("Sync error: bad social mixup! CS Majors=%d, Math Majors=%d\n",
           club->comp_count, club->math_count);
    exit(1);
  }

  // Check for over capacity
  if (club->comp_count > club->capacity || club->math_count > club->capacity) {
    printf("Sync error: too many patrons! CS Majors=%d, Math Majors=%d\n",
           club->comp_count, club->math_count);
    exit(1);
  }

  // Check for missing patrons
  if (club->comp_count < 0 || club->math_count < 0) {
    printf("Sync error: lost track of people! CS Majors=%d, Math Majors=%d",
           club->comp_count, club->math_count);
    exit(1);
  }
}

// Stay in the club for 1 to 4 seconds
void hangout() { sleep(rand() % 4 + 1); }

// ----------------------------------------------------------------
// Club Initialization
// ----------------------------------------------------------------

void club_init(club_t *club, int capacity) {
  club->comp_count = 0;
  club->math_count = 0;
  club->capacity = capacity;

  // TODO(Task 1): initialize lock and condition variables

  // TODO(Task 2): initialize a lock for starvation
}

// ----------------------------------------------------------------
// CS Major Behaviors
// ----------------------------------------------------------------

void comp_enter(club_t *club) {
  // TODO(Task 1): use lock and condition variables
  // TODO(Task 2): use a lock to prevent starvation
  // TODO(Task 3): modify your condition check to prevent going over capacity

  club->comp_count++;

  sanitycheck(club);
}

void comp_exit(club_t *club) {
  // TODO(Task 1): use lock and condition variables

  club->comp_count--;

  sanitycheck(club);
}

void *comp_thread(void *vargp) {
  // Cast the void * argument to an int
  int id = *((int *)vargp);

  while (1) {

    printf("CS Major #%02d arrives (%2d/%d CS and %2d/%d Math Majors in the "
           "club)\n",
           id, daclub229.comp_count, daclub229.capacity, daclub229.math_count,
           daclub229.capacity);

    comp_enter(&daclub229);

    printf("CS Major #%02d enters  (%2d/%d CS and %2d/%d Math Majors in the "
           "club)\n",
           id, daclub229.comp_count, daclub229.capacity, daclub229.math_count,
           daclub229.capacity);

    hangout();

    comp_exit(&daclub229);

    printf("CS Major #%02d leaves  (%2d/%d CS and %2d/%d Math Majors in the "
           "club)\n",
           id, daclub229.comp_count, daclub229.capacity, daclub229.math_count,
           daclub229.capacity);
  }
}

// ----------------------------------------------------------------
// Math Major Behaviors
// ----------------------------------------------------------------

void math_enter(club_t *club) {
  // TODO(Task 1): use lock and condition variables
  // TODO(Task 2): use a lock to prevent starvation
  // TODO(Task 3): modify your condition check to prevent going over capacity

  club->math_count++;

  sanitycheck(club);
}

void math_exit(club_t *club) {
  // TODO(Task 1): use lock and condition variables

  club->math_count--;

  sanitycheck(club);
}

void *math_thread(void *vargp) {
  // Cast the void * argument to an int
  int id = *((int *)vargp);

  while (1) {

    printf("Math Major #%02d arrives (%2d/%d CS and %2d/%d Math Majors in the "
           "club)\n",
           id, daclub229.comp_count, daclub229.capacity, daclub229.math_count,
           daclub229.capacity);

    math_enter(&daclub229);

    printf("Math Major #%02d enters  (%2d/%d CS and %2d/%d Math Majors in the "
           "club)\n",
           id, daclub229.comp_count, daclub229.capacity, daclub229.math_count,
           daclub229.capacity);

    hangout();

    math_exit(&daclub229);

    printf("Math Major #%02d leaves  (%2d/%d CS and %2d/%d Math Majors in the "
           "club)\n",
           id, daclub229.comp_count, daclub229.capacity, daclub229.math_count,
           daclub229.capacity);
  }
}

// ----------------------------------------------------------------
// Simulation Starup
// ----------------------------------------------------------------

int main(int argc, char **argv) {
  int comp_ids[NUM_COMP];
  int math_ids[NUM_MATH];
  pthread_t thread_id;

  club_init(&daclub229, CLUB_CAPACITY);

  // Create CS major threads
  for (int i = 0; i < NUM_COMP; i++) {
    comp_ids[i] = i;
    pthread_create(&thread_id, NULL, comp_thread, &(comp_ids[i]));
  }

  // Create math major threads
  for (int i = 0; i < NUM_MATH; i++) {
    math_ids[i] = i;
    pthread_create(&thread_id, NULL, math_thread, &(math_ids[i]));
  }

  // Wait for unending threads to prevent main thread exiting
  pthread_join(thread_id, NULL);
}

