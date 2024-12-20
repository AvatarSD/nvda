cmake_minimum_required(VERSION 3.10)
project(MyProject)

# Subprojects
add_subdirectory(queue_circular_buffer)
add_subdirectory(sorting_algo_cmp)

# Run in tmux
add_custom_target(run
  COMMAND ./QueueCircularBuffer
#   COMMAND tmux new-session ' ./QueueCircularBuffer'; split-window -h 'echo "Build and run complete"'
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/queue_circular_buffer
  COMMENT "Building and running all targets in tmux"
)

add_custom_target(run_sort
  COMMAND ./SortingAlgoCompare
#   COMMAND tmux new-session ' ./QueueCircularBuffer'; split-window -h 'echo "Build and run complete"'
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/sorting_algo_cmp
  COMMENT "Building and running all targets in tmux"
)