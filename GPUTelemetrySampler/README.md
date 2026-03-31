# GPUTelemetrySampler

To explore whether computation leaves a detectable footprint at the hardware level, it is necessary to observe what GPUs are doing over time.

Modern GPUs expose signals such as power consumption, utilization, temperature, clock frequencies, and data transfer activity. These signals reflect the physical behavior of the device as it executes workloads.

However, most tools are designed for coarse monitoring, not for capturing the temporal structure of computation.

This component provides a lightweight telemetry sampler designed to collect these signals during controlled experiments.

## What this is

This is not a general-purpose monitoring tool.

It is a research instrument, designed to make visible aspects of computation that are usually hidden.

In this project, it is used to examine whether different workloads — such as LLM training, inference, and other models — produce distinguishable patterns in hardware-level signals.

## Design goals

- Capture multiple hardware signals simultaneously  
- Preserve the temporal structure of computation  
- Minimize interference with the workload  
- Produce clean, analysis-ready outputs  

The focus is not only on *how much* compute is used, but on *how it unfolds over time*.

## Implementation

This sampler is based on [gpowerSAMPLER](https://github.com/hpc-ulisboa/gpowerSAMPLER). The original tool provides a simple interface for collecting GPU power data using NVIDIA's NVML API.

For this project, the implementation was extended to support:

- Multi-signal telemetry (power, utilization, clocks, PCIe, temperature)
- High-frequency sampling with improved timing control
- Monotonic timestamps for consistent alignment
- CPU affinity and priority control
- Structured output (per-signal CSV files)
- Metadata logging for reproducibility

These changes allow the sampler to capture richer information about how workloads interact with hardware.

## How to use

The sampler runs alongside a target workload, collecting telemetry while it executes.

### Build

make

### Run

./GPUTelemetrySampler -s 10 -a <command> [args...]

Example:

./GPUTelemetrySampler -s 10 -a python train.py

This will start the sampler, execute the command, and record telemetry until the process finishes.

### Output

The sampler generates one file per signal, for example:

- telemetry_power_mw_data.csv
- telemetry_gpu_util_data.csv
- telemetry_temperature_data.csv
- telemetry_sm_clock_data.csv
- telemetry_pcie_tx_data.csv

Each file contains timestamped measurements, allowing reconstruction of how the workload evolves over time.


## Attribution
This work builds upon [gpowerSAMPLER](https://github.com/hpc-ulisboa/gpowerSAMPLER). All credit for the original implementation goes to its authors.
