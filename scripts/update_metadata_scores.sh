pushd solvers/cpp
make compile && ./main.solver -mode eval_json -solution loks_best
make compile && ./main.solver -mode eval_json -solution dm_border2
make compile && ./main.solver -mode eval_json -solution loks_rgreedy1
make compile && ./main.solver -mode eval_json -solution loks_border1
popd
