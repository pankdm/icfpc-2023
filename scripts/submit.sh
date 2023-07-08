task_id=$1
PATH=localhost:8000/solutions/${USER}/${task_id}
echo "Submitting @ ${PATH}"
/usr/bin/curl -X POST ${PATH}
