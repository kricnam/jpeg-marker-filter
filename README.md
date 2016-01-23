# jpeg-marker-filter
filter away no standard jfif markers from a jpeg file, in some case, could make some private modified jpeg pic to be displayed by standard tools.

#compile

in the source directory, type:

    g++ -o filter *.cpp
  
#usage

after compile, type the following to get result

    ./filter sample.jpg > filtered-sample.jpg

## prepare material

To extract some no standard jpeg data from base64 encoded string in a TXT file, and show it on screen, 
a command line solution such as the following:

    sed -n 's/ \{1,\}/,/g;1p' result.txt | cut -d ',' -f 9 | base64 -d | ./a.out - | display
