# Analysis

This component contains the analysis pipeline used to process and interpret GPU telemetry data across different workloads.

## Purpose

The goal of this analysis is not to build a production-ready classifier, but to explore whether hardware-level signals preserve information about how computation unfolds over time.

In particular, it examines whether different workloads — such as LLM training, inference, CNN training, and transformer-based masked language modeling — produce distinguishable patterns in telemetry.

## What is included

- Data loading and preprocessing for telemetry CSV files  
- Signal alignment and resampling  
- Visualization of raw telemetry across workloads  
- Comparative plots across signals (power, utilization, temperature, clocks, PCIe)  
- Frequency-domain analysis using power spectral density (PSD)  
- Aggregation across windows to produce stable signal representations  

## Structure of the analysis

The analysis proceeds in three stages:

### 1. Raw signal inspection

Telemetry signals are first visualized in the time domain to provide an intuitive understanding of how workloads behave.

These plots highlight:
- Differences in temporal variation  
- Stability vs. burstiness  
- Synchronization across signals  

This stage is qualitative and exploratory.

### 2. Multi-signal comparison

Multiple telemetry channels are analyzed jointly.

Rather than focusing on a single signal (e.g. power), the analysis considers:
- GPU utilization  
- Power consumption  
- Temperature  
- Clock behavior  
- PCIe data transfer  

This provides a more complete view of how computation interacts with the system.

### 3. Frequency-domain characterization

To move beyond visual inspection, signals are transformed into the frequency domain.

Each telemetry trace is:
- Divided into time windows  
- Converted into a power spectral density (PSD)  
- Averaged across windows  

This produces a more stable representation of signal structure, allowing comparisons across workloads.

## Design principles

- Preserve temporal structure rather than aggregate metrics  
- Avoid assumptions about workload internals  
- Use simple, interpretable transformations  
- Keep analysis modular and reproducible  

## Limitations

This analysis is exploratory and makes several simplifying assumptions:

- Limited number of runs per workload  
- Single hardware environment (A100)  
- Focus on steady-state regions only  
- No adversarial or noisy conditions  

As a result, the outputs should be interpreted as illustrative rather than definitive.

## Relation to the project

This analysis supports the central question of the project:

> Can external observers infer how compute is being used from hardware signals alone?

It provides initial evidence that telemetry may retain information about workload structure, motivating further investigation.
