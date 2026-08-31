
import argparse
import subprocess
import time
import uuid
from dataclasses import dataclass
from typing import Optional


@dataclass
class Workload:
    job_id: str
    name: str
    workload_type: str
    priority: int
    duration: int
    process: Optional[subprocess.Popen] = None
    submitted_at: float = 0.0


class WorkloadManager:

    def __init__(self):
        self.workloads = {}

    def submit(
        self,
        name: str,
        workload_type: str,
        priority: int,
        duration: int,
    ):

        job_id = str(uuid.uuid4())[:8]

        workload = Workload(
            job_id=job_id,
            name=name,
            workload_type=workload_type,
            priority=priority,
            duration=duration,
            submitted_at=time.time(),
        )

        self.workloads[job_id] = workload

        print()
        print("Workload submitted")
        print("------------------")
        print(f"Job ID:      {job_id}")
        print(f"Name:        {name}")
        print(f"Type:        {workload_type}")
        print(f"Priority:    {priority}")
        print(f"Duration:    {duration}s")
        print()

        return job_id

    def start(self, job_id):

        if job_id not in self.workloads:
            print(f"Unknown job: {job_id}")
            return

        workload = self.workloads[job_id]

        command = [
            "python3",
            "Workload/cpu_workload.py",
            "--duration",
            str(workload.duration),
        ]

        process = subprocess.Popen(command)

        workload.process = process

        print(
            f"Started {workload.name} "
            f"(PID={process.pid}, Job={job_id})"
        )

    def status(self):

        print()
        print(
            f"{'JOB ID':<10}"
            f"{'NAME':<20}"
            f"{'TYPE':<15}"
            f"{'PRIORITY':<10}"
            f"{'STATUS':<12}"
        )

        print("-" * 67)

        for workload in self.workloads.values():

            if workload.process is None:
                status = "WAITING"

            elif workload.process.poll() is None:
                status = "RUNNING"

            else:
                status = "FINISHED"

            print(
                f"{workload.job_id:<10}"
                f"{workload.name:<20}"
                f"{workload.workload_type:<15}"
                f"{workload.priority:<10}"
                f"{status:<12}"
            )


def main():

    parser = argparse.ArgumentParser(
        description="AI OS Workload Manager"
    )

    subparsers = parser.add_subparsers(dest="command")

    submit_parser = subparsers.add_parser("submit")

    submit_parser.add_argument("--name", required=True)

    submit_parser.add_argument(
        "--type",
        required=True,
        choices=[
            "training",
            "inference",
            "data",
            "background",
        ],
    )

    submit_parser.add_argument(
        "--priority",
        type=int,
        default=5,
    )

    submit_parser.add_argument(
        "--duration",
        type=int,
        default=30,
    )

    subparsers.add_parser("status")

    args = parser.parse_args()

    manager = WorkloadManager()

    if args.command == "submit":

        job_id = manager.submit(
            name=args.name,
            workload_type=args.type,
            priority=args.priority,
            duration=args.duration,
        )

        manager.start(job_id)

    elif args.command == "status":

        manager.status()

    else:
        parser.print_help()


if __name__ == "__main__":
    main()