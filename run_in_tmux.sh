#!/bin/bash

# Start a new tmux session named 'sorting_session'
tmux new-session -s sorting_session -d

# Split the window into two panes, with the bottom pane taking 20% of the height
tmux split-window -v -p 20

# Send the first command to the top pane (80% height)
tmux send-keys -t sorting_session:0.0 './build/queue_circular_buffer/QueueCircularBuffer' C-m

# Send the second command to the bottom pane (20% height)
tmux send-keys -t sorting_session:0.1 './build/sorting_algo_cmp/SortingAlgoCompare' C-m

# Attach to the session so you can see the panes
tmux attach-session -t sorting_session