// Standard headers
#include <stdio.h>

// Internal headers
#include "direction.h"
#include "position.h"
#include "spy.h"

// Main header
#include "attacker.h"

// Additional headers to support random numbers generation
#include <time.h>
#include <stdlib.h>

// Macros
#define UNUSED(x) (void)(x) // Auxiliary to avoid error of unused parameter

/*----------------------------------------------------------------------------*/
/*                              PUBLIC FUNCTIONS                              */
/*----------------------------------------------------------------------------*/

// Returns 0 or 1 randomly
int generate_rand_num_0_to_1() {
  int rand_num = rand();
  return rand() % 2;
}

// Returns 0, 1, 2, 3, 4, 5 or 6 randomly
int generate_rand_num_0_to_6() {
  int rand_num = rand();
  return rand() % 7;
}

bool is_atk_moving_up(direction_t dir) {
  return dir.i < 0;
}

direction_t move_atk_horizontally(bool is_up_main_dir) {
  direction_t up_mov[7] =   { DIR_RIGHT, DIR_RIGHT, DIR_RIGHT, DIR_STAY, DIR_UP_LEFT, DIR_UP_RIGHT, DIR_DOWN };
  direction_t down_mov[7] = { DIR_RIGHT, DIR_RIGHT, DIR_RIGHT, DIR_STAY, DIR_DOWN_LEFT, DIR_DOWN_RIGHT, DIR_UP };

  if (is_up_main_dir) return up_mov[generate_rand_num_0_to_6()];
  return down_mov[generate_rand_num_0_to_6()];
}

direction_t move_atk_vertically(bool is_up_main_dir) {
  direction_t move_up[2] = { DIR_UP, DIR_UP_RIGHT };
  direction_t move_down[2] = { DIR_DOWN, DIR_DOWN_RIGHT };

  if (is_up_main_dir) return move_up[generate_rand_num_0_to_1()];
  return move_down[generate_rand_num_0_to_1()];
}

direction_t execute_attacker_strategy(
    position_t attacker_position, Spy defender_spy) {
  static position_t previous_position = INVALID_POSITION;
  static direction_t best_direction = DIR_DOWN;

  direction_t vertical_mov[2] = { DIR_UP, DIR_DOWN };
  
  static int move_vertical = 0;
  static bool is_up_main_dir = false;
  if (move_vertical == 0) {
    srand((unsigned int)time(NULL));
  }

  if (move_vertical == 0) {
    // Choose a random vertical direction to start moving to the border
    best_direction = vertical_mov[generate_rand_num_0_to_1()];
    is_up_main_dir = is_atk_moving_up(best_direction);
    move_vertical++;
  } else {
    if (equal_positions(attacker_position, previous_position) && move_vertical == 1) {
      // The attacker has moved up/down and reached the border or an obstacle
      if (get_spy_number_uses(defender_spy) == 0) {
        // SPY the defender's position
        position_t defender_pos = get_spy_position(defender_spy);

        // If the attacker could lose by just moving to the right, change vertical direction
        if (abs(attacker_position.i - defender_pos.i) <= 3) {
          best_direction = is_up_main_dir ? (direction_t) DIR_DOWN : (direction_t) DIR_UP;
          is_up_main_dir = !is_up_main_dir;
        } else {
          // The attacker must now start moving horizontally
          move_vertical++;
          best_direction = move_atk_horizontally(is_up_main_dir);
        }
      } else {
        // The attacker must now start moving horizontally
        move_vertical++;
        best_direction = move_atk_horizontally(is_up_main_dir);
      }
    } else if (move_vertical == 1) {
      // The attacker can keep moving up/down
      best_direction = move_atk_vertically(is_up_main_dir);
    } else {
      // The attacker can keep moving horizontally
      best_direction = move_atk_horizontally(is_up_main_dir);
    }
  }

  previous_position = attacker_position;
  return best_direction;
}

/*----------------------------------------------------------------------------*/
