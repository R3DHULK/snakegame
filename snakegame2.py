import random
import os
import time

# Define the game constants
BOARD_WIDTH = 20
BOARD_HEIGHT = 10
SNAKE_INITIAL_LENGTH = 3
SNAKE_INITIAL_X = BOARD_WIDTH // 2
SNAKE_INITIAL_Y = BOARD_HEIGHT // 2

# Define the game variables
snake = [(SNAKE_INITIAL_X, SNAKE_INITIAL_Y)]
food = (random.randint(0, BOARD_WIDTH - 1),
        random.randint(0, BOARD_HEIGHT - 1))
direction = (1, 0)
score = 0

# Define the function to display the game board


def display_board():
    os.system('clear')
    print("+" + "-" * BOARD_WIDTH + "+")
    for y in range(BOARD_HEIGHT):
        row = "|"
        for x in range(BOARD_WIDTH):
            if (x, y) in snake:
                row += "O"
            elif (x, y) == food:
                row += "*"
            else:
                row += " "
        row += "|"
        print(row)
    print("+" + "-" * BOARD_WIDTH + "+")
    print(f"Score: {score}")

# Define the function to update the game state


def update_game():
    global snake, food, direction, score
    # Move the snake
    head_x, head_y = snake[-1]
    dx, dy = direction
    new_head_x, new_head_y = head_x + dx, head_y + dy
    if (new_head_x, new_head_y) in snake:
        # The snake collided with itself, end the game
        return False
    if new_head_x < 0 or new_head_x >= BOARD_WIDTH or new_head_y < 0 or new_head_y >= BOARD_HEIGHT:
        # The snake collided with a wall, end the game
        return False
    snake.append((new_head_x, new_head_y))
    # Check if the snake ate the food
    if (new_head_x, new_head_y) == food:
        score += 1
        # Generate a new food location
        while food in snake:
            food = (random.randint(0, BOARD_WIDTH - 1),
                    random.randint(0, BOARD_HEIGHT - 1))
    else:
        snake.pop(0)
    return True

# Define the function to play the game


def play():
    global direction
    # Initialize the snake
    for i in range(SNAKE_INITIAL_LENGTH - 1):
        snake.insert(0, (SNAKE_INITIAL_X - i - 1, SNAKE_INITIAL_Y))
    # Start the game loop
    while True:
        display_board()
        # Get the user input
        key = input()
        if key == "w":
            direction = (0, -1)
        elif key == "a":
            direction = (-1, 0)
        elif key == "s":
            direction = (0, 1)
        elif key == "d":
            direction = (1, 0)
        elif key == "q":
            # Quit the game
            break
        if not update_game():
            # End the game
            break
        time.sleep(0.1)


if __name__ == "__main__":
    play()
