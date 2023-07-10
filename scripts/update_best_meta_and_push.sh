
pushd solvers/cpp
make compile && ./main.solver -mode eval_json -solution loks_best
popd
python scripts/solution_previews.py && gca -m "new previews + latest score" && gm && gp
