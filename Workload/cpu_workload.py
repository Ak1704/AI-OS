import argparse
import time


def cpu_work(duration):

    start = time.time()

    while time.time() - start < duration:

        # CPU-intensive calculation
        x = 0

        for i in range(1_000_000):
            x += i * i


def main():

    parser = argparse.ArgumentParser()

    parser.add_argument(
        "--duration",
        type=int,
        default=30,
    )

    args = parser.parse_args()

    print(
        f"CPU workload started "
        f"for {args.duration} seconds"
    )

    cpu_work(args.duration)

    print("CPU workload finished")


if __name__ == "__main__":
    main()