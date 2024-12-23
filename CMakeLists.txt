cmake_minimum_required(VERSION 3.10)
project(NVDA-Preview)

# Subprojects
add_subdirectory(queue_circular_buffer)
add_subdirectory(sorting_algo_cmp)

# Run in tmux
add_custom_target(run_queue
  COMMAND ./QueueCircularBuffer
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/queue_circular_buffer
  COMMENT "Building and running queueu with two threads"
)

add_custom_target(run_sort
  COMMAND ./SortingAlgoCompare
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/sorting_algo_cmp
  COMMENT "Building and running sorting comparision"
)

add_custom_target(run
  COMMAND ./run_in_tmux.sh
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/..
  COMMENT "Building and running all targets in tmux"
)