# Lub Gen CSV Database Prototype

This repository contains a small C++17 prototype for loading CSV files into memory and running simple equality queries. It is a starting point for a future machine-learning-friendly data engine.

## Features

- Parse CSV files with quoted fields and configurable delimiters
- Store table data in an in-memory structure with column lookups
- Run equality filters via a light `QueryEngine`
- Convert query results into numeric vectors for ML preprocessing
- Command-line interface for quick inspection and filtering

## Building

```bash
cmake -S . -B build
cmake --build build
```

This produces two targets:

- `liblubgen.a` static library
- `lubgen_db` executable demonstrating CSV loading and querying

## Usage

```bash
./build/lubgen_db data.csv [--where column=value] [--head n]
```

- `--where column=value` filters rows where the specified column matches the value
- `--head n` controls how many rows are shown when no filter is provided (default 5)

Example:

```bash
./build/lubgen_db examples/iris.csv --where species=setosa --head 10
```

The program prints a short summary to stderr and writes matching rows back out as CSV on stdout.

## Extending

- Add more predicates (range comparisons, pattern matching, joins)
- Implement projection to select only specific columns
- Provide column type inference for downstream numerical work
- Persist indexes to speed up repeated queries on large datasets

