#!/bin/bash

# Path to your client executable
CLIENT_EXEC="./client"  # Replace with the actual path if different

# Run the client 10 times
for i in {1..10}
do
    echo "Running client iteration $i"
    $CLIENT_EXEC
    # sleep 1  # Optional: Add a delay of 1 second between runs (adjust as needed)
done