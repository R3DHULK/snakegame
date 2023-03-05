use strict;
use warnings;

# Set up the game board
my $board_size = 20;
my @board = map { [ (' ') x $board_size ] } 1..$board_size;

# Set up the snake
my @snake = (
  { row => int($board_size/2), col => int($board_size/2) },
  { row => int($board_size/2), col => int($board_size/2)-1 },
);
my $snake_direction = 'right';

# Set up the initial food location
my $food_row = int(rand($board_size));
my $food_col = int(rand($board_size));

# Game loop
while (1) {
  # Clear the screen
  system("clear");

  # Draw the game board
  print " ";
  print "_" x $board_size;
  print "\n";
  foreach my $row (@board) {
    print "|";
    print join("", @$row);
    print "|\n";
  }
  print " ";
  print "-" x $board_size;
  print "\n";

  # Move the snake
  my $head = $snake[0];
  my $new_head;
  if ($snake_direction eq 'up') {
    $new_head = { row => $head->{row}-1, col => $head->{col} };
  } elsif ($snake_direction eq 'down') {
    $new_head = { row => $head->{row}+1, col => $head->{col} };
  } elsif ($snake_direction eq 'left') {
    $new_head = { row => $head->{row}, col => $head->{col}-1 };
  } elsif ($snake_direction eq 'right') {
    $new_head = { row => $head->{row}, col => $head->{col}+1 };
  }
  unshift(@snake, $new_head);

  # Check for collision with walls
  if ($new_head->{row} < 0 || $new_head->{row} >= $board_size ||
      $new_head->{col} < 0 || $new_head->{col} >= $board_size) {
    print "Game over!\n";
    last;
  }

  # Check for collision with snake body
  my %snake_body = map { $_->{row} . ',' . $_->{col} => 1 } @snake;
  if (exists $snake_body{$new_head->{row} . ',' . $new_head->{col}}) {
    print "Game over!\n";
    last;
  }

  # Check for collision with food
  if ($new_head->{row} == $food_row && $new_head->{col} == $food_col) {
    $food_row = int(rand($board_size));
    $food_col = int(rand($board_size));
  } else {
    pop(@snake);
  }

  # Update the board with the snake and food
  foreach my $row (@board) {
    @$row = map { ' ' } @$row;
  }
  foreach my $seg (@snake) {
    $board[$seg->{row}][$seg->{col}] = 'O';
  }
  $board[$food_row][$food_col] = 'X';

  # Read input from the user
  my $key;
  if (eval { require Term::ReadKey }) {
    Term::ReadKey::ReadMode('cbreak');
    $key = Term::ReadKey::ReadKey(0);
    Term::ReadKey::ReadMode('normal');
  } else {
    $key = <STDIN>;
  }

  # Update the snake direction based on input
  if ($key eq 'w' && $snake_direction ne 'down') {
    $snake_direction = 'up';
  } elsif ($key eq 's' && $snake_direction ne 'up') {
    $snake_direction = 'down';
  } elsif ($key eq 'a' && $snake_direction ne 'right') {
    $snake_direction = 'left';
  } elsif ($key eq 'd' && $snake_direction ne 'left') {
    $snake_direction = 'right';
  }

	select(undef, undef, undef, 0.1);
}
 

