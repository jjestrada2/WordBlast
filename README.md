# Word Tally in War and Peace using Threads📚 🐰

## Description🤗

This project reads the text of "War and Peace" and counts the occurrences of each word that is 6 or more characters long. The program utilizes multiple threads to process different chunks of the file concurrently, tallying the results to find the ten most frequently occurring words of 6 or more characters.

## Getting Started🙌

### Prerequisites🐢

- **Linux Environment**
- **GCC Compiler**
- **Pthreads Library**

### Files Provided🦸‍♂️

- `WarAndPeace.txt`: The text file of "War and Peace."

### Program Parameters🏋️

The program should be executed with two command line parameters:
1. `FileName`: The name of the file to read (e.g., `WarAndPeace.txt`).
2. `ThreadCount`: The number of threads to spawn for processing the file.
   
**Sample Output**🎮
```
gcc -o bierman_robert_HW4_main bierman_robert_HW4_main.o -g -I. -l pthread
./bierman_robert_HW4_main WarAndPeace.txt 4


Word Frequency Count on WarAndPeace.txt with 4 threads
Printing top 10 words 6 characters or more.
Number 1 is Pierre with a count of 1963
Number 2 is Prince with a count of 1928
Number 3 is Natásha with a count of 1212
Number 4 is Andrew with a count of 1143
Number 5 is himself with a count of 1020
Number 6 is princess with a count of 916
Number 7 is French with a count of 881
Number 8 is before with a count of 833
Number 9 is Rostóv with a count of 776
Number 10 is thought with a count of 767
Total Time was 1.938852743 seconds
```

## License📜

This project is licensed under the MIT License - see the [LICENSE](https://choosealicense.com/licenses/mit/) file for details.

## Credits🔥

- [Juan Estrada](https://github.com/jjestrada2) - Developer of this project.

## Contact🦻

For support or inquiries, please contact [Juan Estrada](mailto:juan5801331@gmail.com).

