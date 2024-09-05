import threading
import time
from datetime import datetime

NUMBER_THREADS = 10000


def thread_function(result_list):
    pass


def exec_program():
    # Record the start time
    start_time = time.time()

    threads = []
    results = [0] * NUMBER_THREADS

    for i in range(NUMBER_THREADS):
        t = threading.Thread(target=thread_function, args=(results,))
        threads.append(t)
        t.start()

    for t in threads:
        t.join()

    # Calculate the elapsed time in seconds
    end_time = time.time()
    elapsed_time = end_time - start_time

    print(f"{elapsed_time:.2e}")


if __name__ == "__main__":
    exec_program()
