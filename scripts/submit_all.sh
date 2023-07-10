tag=$1
PATH=localhost:8000/solutions/${tag}
echo "Submitting @ ${PATH}"
/usr/bin/curl -X POST ${PATH}
