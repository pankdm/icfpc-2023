tag=$1
task_id=$2
PATH=localhost:8000/solutions/${tag}/${task_id}
echo "Submitting @ ${PATH}"
/usr/bin/curl -X POST ${PATH}
