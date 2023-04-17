#!/bin/bash
make build

count=10
pids=()
for (( i=0; i<$count; i++ ))
do
	./main test &
	pids+=($!)
done

sleep 5m

completedTasks=0
for pid in ${pids[@]}
do
	kill -SIGINT $pid
	if [[ $? -eq 0 ]]
	then
		((completedTasks++))
	fi
done

echo "Expected tasks: " $count >> result.txt
echo "Actual tasks: " $completedTasks >> result.txt
cat stat >> result.txt
rm stat