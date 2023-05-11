ThreadPoolTask (Task 2)
Noamya Shani and Eitan Shenkolevski

To run the program run make, and then write:
./coder [key] -[d/e]
and refer to it a file containing text or perform pipe.
(key is int. flag -d is decrypting and -e is encrypting)

for example:
./coder 2 -d < examole_input.txt > output
or:
echo "Hello" | ./coder 5 -e

If the terminal fails to recognize the dynamic library libCodec.so, Run the following command:
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/