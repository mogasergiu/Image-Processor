# Image-Processor
A program that reads ".bmp" images and applies filters to them, such as: Black and White, Min/Max Pooling, Clustering, No Crop and Convolutional Layers.

In order to properly run the program and obtain the filters, special input must be provided as information in a text file named input.txt with the following information:
 - name of binary file representing the ".bmp" image
 - name of the text file containing the filter matrix for the Convolutional Layers filter. It may have the following format:<br />
N <br />
VAL_11 VAL_12 ... VAL_1N<br />
VAL_21 VAL_22 ... VAL_2N<br />
........................<br />
VAL_N1 VAL_N2 ... VAL_NN<br />
where N is the size of the filter and VAL_ij is the double float value of the filter<br />
 - name of the text file containing the filter information for the Min/Max Pooling Filter. It may have the following format:<br />
m/M N<br />
where the first character must be either 'm' (for min) or "M" (for max), followed by a whitespace and N, where N is the size of the filter.<br />
- name of the text file containing the threshold value required for the Clustering Filter<br />

Example of an input.tx:<br />
test.bmp
filter.txt
pooling.txt
cluster.txt

Examples of output images are given in the "output/" directory.
