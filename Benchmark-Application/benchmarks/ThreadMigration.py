import threading
import time
from datetime import datetime

iterations = 100
NUMBER_OF_THREADS = 10000


def thread_function(id, iterations):
    time.sleep(iterations / 1000.0)


def exec_program():
    # Record the start time
    start_time = time.time()

    # Declare an array of thread objects
    threads = []

    # Start the threads
    for i in range(NUMBER_OF_THREADS):
        t = threading.Thread(target=thread_function, args=(i, iterations))
        threads.append(t)
        t.start()

    # Join the threads
    for t in threads:
        t.join()

    # Calculate the elapsed time in seconds
    end_time = time.time()
    elapsed_time = end_time - start_time

    print(f"{elapsed_time:.2e}")


if __name__ == "__main__":
    exec_program()
