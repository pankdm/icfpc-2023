# Iter 1
# for x in `jot - 1 4`; do
#   echo ${x};
#   make compile && ./main.solver -mode run -solver border  -nthreads 2 -first_problem 1  -last_problem 54 -layers 3 -xoffset ${x} &
# done

# wait 

# CONFLICT (content): Merge conflict in solutions/loks_best/9.json
# Auto-merging solutions/loks_best/45.json
# CONFLICT (content): Merge conflict in solutions/loks_best/45.json
# Auto-merging solutions/loks_best/44.json
# CONFLICT (content): Merge conflict in solutions/loks_best/44.json
# Auto-merging solutions/loks_best/41.json
# CONFLICT (content): Merge conflict in solutions/loks_best/41.json
# Auto-merging solutions/loks_best/29.json
# CONFLICT (content): Merge conflict in solutions/loks_best/29.json
# Auto-merging solutions/loks_best/28.json
# CONFLICT (content): Merge conflict in solutions/loks_best/28.json

# Iter 1
for y in `jot - 1 4`; do
  echo ${y};
  make compile && ./main.solver -mode run -solver border  -nthreads 5 -first_problem 1  -last_problem 54 -layers 3 -yoffset ${y}
done

