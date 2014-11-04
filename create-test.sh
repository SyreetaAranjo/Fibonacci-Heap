SIZE=1000000
FILE="/tmp/insert_extract_rank_${SIZE}"

echo -n "" > $FILE

echo "B ${SIZE}" >> $FILE;
for (( i=$SIZE-1; i >= 0; i-- )) 
do
	echo "I ${i}" >> $FILE;
done

echo "X" >> $FILE;

for (( i=$SIZE-2; i >= 0; i-- )) 
do
	echo "R ${i}" >> $FILE;
done

echo "E" >> $FILE;
