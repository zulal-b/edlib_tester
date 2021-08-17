# edlib_tester
Tester scripts for edlib (https://github.com/Martinsos/edlib) <br />
edlib source files must be included. <br />
### Usage:
undef_flag: 1 for directly passing, 0 for processing the strings which have character 'N' <br />
#### Single-Thread
```
make
./edlib_run $read_file_input $ref_file_input $undef_flag $read_length $error_threshold
```
#### Multi-Threaded
```make mt```
##### Single input file:
file_input format per line: read \t reference_segment <br />
```./edlib_run_mt $read_file_input "single" $undef_flag $read_length $error_threshold $num_threads```
##### Separate read and reference segment files:
```./edlib_run_mt $read_file_input $ref_file_input $undef_flag $read_length $error_threshold $num_threads```



