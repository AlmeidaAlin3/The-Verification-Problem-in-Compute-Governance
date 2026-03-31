# GPUTelemetrySampler

To explore whether computation leaves a detectable footprint at the hardware level, it is necessary to observe what GPUs are doing over time.

Modern GPUs expose a range of telemetry signals, such as power consumption, utilization, temperature, clock frequencies, and data transfer activity. These signals reflect the physical behavior of the device as it executes workloads.

However, accessing these signals in a consistent and high-frequency way is not entirely straightforward. Existing tools are often designed for coarse monitoring, rather than for capturing the fine-grained temporal structure of computation.

This component of the project provides a lightweight telemetry sampler designed to collect such signals during controlled workload execution.

## Design goals

The sampler is designed with the following goals in mind:

* Capture multiple hardware signals simultaneously
* Preserve the temporal structure of computation
* Minimize interference with the workload being measured
* Produce clean, analysis-ready outputs

In particular, the focus is not only on *how much* compute is used, but on *how it unfolds over time*.

## Implementation

This sampler is based on the following project:

gpowerSAMPLER
https://github.com/hpc-ulisboa/gpowerSAMPLER

The original tool provides a interface for collecting GPU power data using NVIDIA's NVML API.

For the purposes of this project, the implementation was substantially extended to support:

* Multi-signal telemetry (power, utilization, clocks, PCIe, temperature)
* High-frequency sampling with improved timing control
* Monotonic timestamping for consistent temporal alignment
* CPU affinity and priority control for more stable sampling
* Structured output (per-signal CSV files)
* Metadata logging for reproducibility

These modifications allow the sampler to capture richer information about how workloads interact with hardware, which is central to the broader question explored in this project.

## What this is (and is not)

This tool is not intended as a general-purpose monitoring solution.

It is a research-oriented instrument, designed to make visible aspects of computation that are usually hidden behind higher-level abstractions.

In particular, it is used to examine whether different workloads — such as LLM training, inference, and other models — produce distinguishable patterns in hardware-level signals.

## Attribution

This work builds upon gpowerSAMPLER.
All credit for the original implementation goes to its authors.
