#!/bin/bash

# diseasesFile $1
# diseasesFile $2

if [ $# -ne 5 ]
then
    echo "You must enter exactly 5 arguments for the script to run"
    exit 1
fi

input="countriesFile"

recordId=0

while IFS= read -r cntry
do
    mkdir -p $3/$cntry
    echo "TATAS"
    for (( j=0; j<$4; j++))
    do
        # generate random number and adjust to DD form if needed
        day=$((1 + RANDOM % 29))
        if [ "$day" -le 9 ]
        then
            day=0$day
        fi

        # generate random number and adjust to MM form if needed
        month=$((1 + RANDOM % 11))
        if [ "$month" -le 9 ]
        then
            month=0$month
        fi

        # generate random date in [1900. 2020]
        date=$day-$month-$((RANDOM % 120 + 1900))

        for (( i=0; i<$5; i++))
        do
            id=$recordId
            state="ENTER"
            random=$((RANDOM % 10))
            if [ "$random" -le 4 ]
            then
                state="EXIT"
            fi
            entry=$id" "$state" "$(head /dev/urandom | tr -dc A-Za-z | head -c 13)" "$(head /dev/urandom | tr -dc A-Za-z | head -c 13)" "$(shuf -n 1 $1)" "$((RANDOM % 120 ))
            echo $entry >> $3/$cntry/$date
            recordId=$(($recordId+1))
        done
    done
done < "$input"