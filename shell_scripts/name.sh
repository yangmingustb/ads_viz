pdir=$1

for SRC in `find $pdir -depth`
do 
    DST=`dirname "${SRC}"`/`basename "${SRC}" | tr '[A-Z]' '[a-z]'`
   EXT=${DST##*.}
    if [ "${SRC}" != "${DST}" ] && [ "${EXT}" != "mk" ]
    then
    #[ "${EXT}" == "h" ] && echo -e "\e[1;31m $DST \e[0m" || echo -e "\e[1;36m $DST \e[0m"
    [ ! -e "${DST}" ] && mv -T "${SRC}" "${DST}" | echo -e "\e[1;36m $DST \e[0m" || echo -e "\e[1;31m $DST \e[0m"
    fi
done