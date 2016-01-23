# jpeg-marker-filter
filter away no standard jfif markers from a jpeg file, in some case, could make some private modified jpeg pic to be displayed by standard tools.

#compile

in the source directory, type:

    g++ -o filter *.cpp
  
#usage

after compile, type the following to get result

   ./filter sample.jpg > filtered-sample.jpg

