import time

ARRAY_SIZE = 10000
n = 100

def exec_program():
    # Record the start time
    start_time = time.time()

    # Run the program n times
    for k in range(n):
        # Perform static memory allocation
        static_array = [i + 1 for i in range(ARRAY_SIZE)]

    # Record the end time
    end_time = time.time()

    # Calculate the elapsed time in seconds
    elapsed_time = end_time - start_time

    print(f"{elapsed_time:.2e}")


if __name__ == "__main__":
    exec_program()
