import time
import numpy as np

ARRAY_SIZE = 10000
n = 100


def exec_program():
    # Record the start time
    start_time = time.time()

    # Run the program n times
    for k in range(n):
        # Perform dynamic memory allocation
        dynamic_array = np.arange(1, ARRAY_SIZE + 1, dtype=int)

        # Release the dynamically allocated memory (not required in Python, as it is managed by the garbage collector)
        # No equivalent of "delete[]" in Python

    # Record the end time
    end_time = time.time()

    # Calculate the elapsed time in seconds
    elapsed_time = end_time - start_time

    print(f"{elapsed_time:.2e}")


if __name__ == "__main__":
    exec_program()
