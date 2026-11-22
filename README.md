# Mark-and-Sweep Garbage Collector

A simple mark-and-sweep garbage collector implemented in C to explore automatic memory management, object reachability, and pointer manipulation.

## How It Works

The collector supports integer objects and pairs that reference other objects. A VM stack holds the roots, and a linked list tracks allocated objects.

1. **Mark:** Recursively visit objects reachable from the VM stack.
2. **Sweep:** Free unreachable objects and reset the marks on survivors.

Collection runs automatically when the allocation threshold is reached. The threshold is then adjusted based on the number of surviving objects.

## Project Structure

- `gc.h` — Object and VM definitions and function declarations.
- `gc.c` — Garbage collector implementation.
- `gc_tests.c` — Unit tests and test runner.

## Build and Run

Requires GCC or another C11-compatible compiler.

Clone the repository:

```bash
git clone https://github.com/FNaderi01/Garbage-Collector.git
cd Garbage-Collector
```

Compile the garbage collector and tests:

```bash
gcc -std=c11 -Wall -Wextra -pedantic -I. gc.c gc_tests.c -o gc_tests
```

Run the tests on Linux or macOS:

```bash
./gc_tests
```

On Windows (PowerShell):

```powershell
.\gc_tests.exe
```

A successful run ends with:

```text
All 13 tests passed!
```

## Testing

The 13 assertion-based tests cover stack operations, object creation, reachability, nested and shared references, cycles, null children, linked-list sweeping, automatic collection, and threshold adjustment.

## Reference

Bob Nystrom — [Baby's First Garbage Collector](https://journal.stuffwithstuff.com/2013/12/08/babys-first-garbage-collector/) (2013).
