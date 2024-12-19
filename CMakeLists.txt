cmake_minimum_required(VERSION 3.10)
project(MyProject)

# Add subdirectories
add_subdirectory(queue_circular_buffer)

# Custom target to build and run all in tmux
add_custom_target(run
  COMMAND ./QueueCircularBuffer
#   COMMAND tmux new-session ' ./QueueCircularBuffer'; split-window -h 'echo "Build and run complete"'
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/queue_circular_buffer
  COMMENT "Building and running all targets in tmux"
)