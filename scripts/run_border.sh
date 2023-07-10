# make compile && ./main.solver -mode run -solver border  -nthreads 4 -first_problem 1  -last_problem 11 -layers 3 -offset 3
# make compile && ./main.solver -mode run -solver border  -nthreads 4 -first_problem 1  -last_problem 11 -layers 3 -offset 4
# make compile && ./main.solver -mode run -solver border  -nthreads 4 -first_problem 46  -last_problem 55 -layers 3 -offset 1
# make compile && ./main.solver -mode run -solver border  -nthreads 4 -first_problem 46  -last_problem 55 -layers 3 -offset 2

# make compile && ./main.solver -mode run -solver border  -nthreads 4 -first_problem 28  -last_problem 55 -layers 3 -offset 1
# make compile && ./main.solver -mode run -solver border  -nthreads 4 -first_problem 28  -last_problem 55 -layers 3 -offset 2
# make compile && ./main.solver -mode run -solver border  -nthreads 4 -first_problem 28  -last_problem 55 -layers 3 -offset 3
# make compile && ./main.solver -mode run -solver border  -nthreads 4 -first_problem 28  -last_problem 55 -layers 3 -offset 4
# make compile && ./main.solver -mode run -solver border  -nthreads 4 -first_problem 28  -last_problem 55 -layers 3 -offset 5


# make compile && ./main.solver -mode run -solver border  -nthreads 4 -first_problem 90  -last_problem 90 -layers 18 -offset 2
# make compile && ./main.solver -mode run -solver border  -nthreads 4 -first_problem 90  -last_problem 90 -layers 18 -offset 3
# make compile && ./main.solver -mode run -solver border  -nthreads 4 -first_problem 90  -last_problem 90 -layers 18 -offset 4


# make compile && ./main.solver -mode run -solver border  -nthreads 4 -first_problem 86  -last_problem 86 -layers 5 -offset 1
# make compile && ./main.solver -mode run -solver border  -nthreads 4 -first_problem 86  -last_problem 86 -layers 5 -offset 2
# make compile && ./main.solver -mode run -solver border  -nthreads 4 -first_problem 86  -last_problem 86 -layers 5 -offset 3
# make compile && ./main.solver -mode run -solver border  -nthreads 4 -first_problem 86  -last_problem 86 -layers 5 -offset 4
# make compile && ./main.solver -mode run -solver border  -nthreads 4 -first_problem 86  -last_problem 86 -layers 5 -offset 5


# make compile && ./main.solver -mode run -solver border  -nthreads 4 -first_problem 86  -last_problem 86 -layers 9 -offset 1
# make compile && ./main.solver -mode run -solver border  -nthreads 4 -first_problem 86  -last_problem 86 -layers 12 -offset 1


# for i in `jot - 1 5`
#   make compile && ./main.solver -mode run -solver border  -nthreads 4 -first_problem 58  -last_problem 58 -layers 4 -offset ${i}


# make compile && ./main.solver -mode run -solver border  -nthreads 4 -first_problem 56  -last_problem 87 -layers 5

# Iter 3
# make compile && ./main.solver -mode run -solver border  -nthreads 4 -first_problem 61  -last_problem 61 -layers 6
# make compile && ./main.solver -mode run -solver border  -nthreads 4 -first_problem 62  -last_problem 62 -layers 6
# make compile && ./main.solver -mode run -solver border  -nthreads 4 -first_problem 64  -last_problem 64 -layers 6
# make compile && ./main.solver -mode run -solver border  -nthreads 4 -first_problem 69  -last_problem 73 -layers 6
# make compile && ./main.solver -mode run -solver border  -nthreads 4 -first_problem 86  -last_problem 86 -layers 6


# Iter 4
# make compile && ./main.solver -mode run -solver border  -nthreads 4 -first_problem 61  -last_problem 61 -layers 9 &
# make compile && ./main.solver -mode run -solver border  -nthreads 4 -first_problem 62  -last_problem 62 -layers 7 &
# make compile && ./main.solver -mode run -solver border  -nthreads 4 -first_problem 64  -last_problem 64 -layers 8 &
# make compile && ./main.solver -mode run -solver border  -nthreads 4 -first_problem 69  -last_problem 69 -layers 7 &
# wait 


# make compile && ./main.solver -mode run -solver border  -nthreads 4 -first_problem 70  -last_problem 70 -layers 10 &
# make compile && ./main.solver -mode run -solver border  -nthreads 4 -first_problem 72  -last_problem 72 -layers 10 &
# make compile && ./main.solver -mode run -solver border  -nthreads 4 -first_problem 86  -last_problem 86 -layers 7 &
# wait

# == Iter 5 == 
make compile && ./main.solver -mode run -solver border  -nthreads 4 -first_problem 61  -last_problem 61 -layers 12 &
make compile && ./main.solver -mode run -solver border  -nthreads 4 -first_problem 72  -last_problem 72 -layers 11 &
make compile && ./main.solver -mode run -solver border  -nthreads 4 -first_problem 86  -last_problem 86 -layers 10 &
wait
