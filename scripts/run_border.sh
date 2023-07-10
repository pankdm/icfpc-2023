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

# # == Iter 5 == 
# make compile && ./main.solver -mode run -solver border  -nthreads 4 -first_problem 61  -last_problem 61 -layers 12 &
# make compile && ./main.solver -mode run -solver border  -nthreads 4 -first_problem 72  -last_problem 72 -layers 11 &
# make compile && ./main.solver -mode run -solver border  -nthreads 4 -first_problem 86  -last_problem 86 -layers 10 &
# wait
#

# == Iter 6 ==
# make compile && ./main.solver -mode run -solver border  -nthreads 4 -first_problem 86  -last_problem 86 -layers 13 &


# Test
# make compile && ./main.solver -mode run -solver border  -nthreads 4 -first_problem 56  -last_problem 87 -layers 7

# for x in `jot - 1 5`; do
#   make compile && ./main.solver -mode run -solver border  -nthreads 4 -first_problem 56  -last_problem 60 -layers 5 -xoffset ${x}

#   make compile && ./main.solver -mode run -solver border  -nthreads 4 -first_problem 61  -last_problem 61 -layers 9 -xoffset ${x} &
#   make compile && ./main.solver -mode run -solver border  -nthreads 4 -first_problem 62  -last_problem 62 -layers 7 -xoffset ${x} &
#   make compile && ./main.solver -mode run -solver border  -nthreads 4 -first_problem 63  -last_problem 63 -layers 7 -xoffset ${x} &
#   make compile && ./main.solver -mode run -solver border  -nthreads 4 -first_problem 64  -last_problem 64 -layers 8 -xoffset ${x} & 
#   wait 

#   make compile && ./main.solver -mode run -solver border  -nthreads 4 -first_problem 65  -last_problem 65 -layers 5 -xoffset ${x} &
#   make compile && ./main.solver -mode run -solver border  -nthreads 4 -first_problem 66  -last_problem 66 -layers 6 -xoffset ${x} &
#   make compile && ./main.solver -mode run -solver border  -nthreads 4 -first_problem 72  -last_problem 72 -layers 10 -xoffset ${x} &
#   make compile && ./main.solver -mode run -solver border  -nthreads 4 -first_problem 86  -last_problem 86 -layers 10 -xoffset ${x} &
#   wait

#   make compile && ./main.solver -mode run -solver border  -nthreads 4 -first_problem 67  -last_problem 71 -layers 5 -xoffset ${x}
#   make compile && ./main.solver -mode run -solver border  -nthreads 4 -first_problem 73  -last_problem 85 -layers 5 -xoffset ${x}
# done


# Iter 2
for y in `jot - 1 5`; do
  make compile && ./main.solver -mode run -solver border  -nthreads 4 -first_problem 58  -last_problem 58 -layers 5 -yoffset ${y} &
  make compile && ./main.solver -mode run -solver border  -nthreads 4 -first_problem 61  -last_problem 61 -layers 9 -yoffset ${y} &
  make compile && ./main.solver -mode run -solver border  -nthreads 4 -first_problem 62  -last_problem 62 -layers 7 -yoffset ${y} &
  make compile && ./main.solver -mode run -solver border  -nthreads 4 -first_problem 63  -last_problem 63 -layers 7 -yoffset ${y} &
  make compile && ./main.solver -mode run -solver border  -nthreads 4 -first_problem 64  -last_problem 64 -layers 8 -yoffset ${y} & 
  wait 

  make compile && ./main.solver -mode run -solver border  -nthreads 4 -first_problem 65  -last_problem 65 -layers 5 -yoffset ${y} &
  make compile && ./main.solver -mode run -solver border  -nthreads 4 -first_problem 66  -last_problem 66 -layers 6 -yoffset ${y} &
  make compile && ./main.solver -mode run -solver border  -nthreads 4 -first_problem 72  -last_problem 72 -layers 10 -yoffset ${y} &
  make compile && ./main.solver -mode run -solver border  -nthreads 4 -first_problem 86  -last_problem 86 -layers 10 -yoffset ${y} &
  wait

  make compile && ./main.solver -mode run -solver border  -nthreads 4 -first_problem 67  -last_problem 71 -layers 5 -yoffset ${y} &
  make compile && ./main.solver -mode run -solver border  -nthreads 4 -first_problem 73  -last_problem 85 -layers 5 -yoffset ${y}
  wait
done
