compile:
	gcc -pthread dinner_problem.c -o dinner_problem.out
run: compile
	./dinner_problem.out