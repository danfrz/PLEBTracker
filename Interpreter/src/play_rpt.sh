KEY='';
while [[ $KEY == "" ]]; do
    sh itp.sh "$@" ;
    read -n 1 -t 1 KEY
done
