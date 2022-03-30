// Standard headers
#include <stdio.h>

// Internal headers
#include "direction.h"
#include "position.h"
#include "spy.h"

// Main header
#include "defender.h"

// Macros
#define UNUSED(x) (void)(x) // Auxiliary to avoid error of unused parameter

// Additional headers to support random numbers generation
#include <time.h>
#include <stdlib.h>

/*----------------------------------------------------------------------------*/
/*                              PUBLIC FUNCTIONS                              */
/*----------------------------------------------------------------------------*/

// Returns 0 or 1 randomly
int generate_random_num_0_to_1() {
  srand(time(NULL));
  return rand() % 2;
}

// Returns 0, 1 or 2 randomly
int generate_random_num_0_to_2() {
  srand(time(NULL));
  return rand() % 3;
}

bool is_moving_up(direction_t dir) {
  return dir.i < 0;
}

bool is_moving_left(direction_t dir) {
  return dir.j < 0;
}

direction_t move_vertically(direction_t previous_direction, int max_up) {
  direction_t move_up[2] = { DIR_UP, DIR_UP_LEFT };
  direction_t move_down[2] = { DIR_DOWN, DIR_DOWN_RIGHT };
  direction_t move_stay_down[2] = { DIR_DOWN, DIR_STAY };

  if (max_up == 1) return move_stay_down[generate_random_num_0_to_1()];
  if (is_moving_up(previous_direction) && max_up > 1) return move_up[generate_random_num_0_to_1()];
  return move_down[generate_random_num_0_to_1()];
}

direction_t execute_defender_strategy(
    position_t defender_position, Spy attacker_spy) {
  static position_t previous_position = INVALID_POSITION;
  static direction_t best_direction = DIR_UP;

  static int moved_left = false;

  // Maximum number of possible movements up before hitting the border
  int max_up = defender_position.i - 1;
  direction_t left_mov[3] = { DIR_LEFT, DIR_UP_LEFT, DIR_DOWN_LEFT };

  // Check if the defender was unable to move in the last turn
  if (equal_positions(defender_position, previous_position)) {
    if (!moved_left && get_spy_number_uses(attacker_spy) == 0) {
      // If possible, SPY the attacker's position to decide which vertical direction to go
      position_t attacker_pos = get_spy_position(attacker_spy);
      if (attacker_pos.i >= defender_position.i) best_direction = (direction_t) DIR_DOWN_LEFT;
      else best_direction = (direction_t) DIR_UP_LEFT;
    } else if (!moved_left) {
      // If not possible to SPY, keep in the same vertical direction
      best_direction = move_vertically(best_direction, max_up);
    } else {
      // If the defender has already moved left, but couldn't move in the last turn, then change vertical direction
      if (is_moving_up(best_direction)) return (direction_t) DIR_DOWN;
      else return (direction_t) DIR_UP;
    }
  } else {
    if (!moved_left && is_moving_left(best_direction)) {
      // If the defender could move left (for the first time) in the last turn, choose a vertical direction
      moved_left = true;

      if (get_spy_number_uses(attacker_spy) == 0) {
        // If possible, SPY the attacker's position to decide which vertical direction to go
        position_t attacker_pos = get_spy_position(attacker_spy);
        if (attacker_pos.i >= defender_position.i) best_direction = (direction_t) DIR_DOWN;
        else best_direction = (direction_t) DIR_UP;
      } else {
        // If not possible to SPY, keep going in the same vertical direction (unless the defender is near the border)
        if (is_moving_up(best_direction) && max_up > 1) best_direction = (direction_t) DIR_UP;
        else best_direction = (direction_t) DIR_DOWN;
      }
    } else if (!moved_left) {
      // If the defender hasn't moved left yet, try to move left
      best_direction = left_mov[generate_random_num_0_to_2()];
    } else {
      /* If the defender has already moved left some turns before,
         then move in the same vertical direction (unless the defender is near the border) */
      best_direction = move_vertically(best_direction, max_up);
    }
  }

  previous_position = defender_position;
  return best_direction;
}

/*----------------------------------------------------------------------------*/
