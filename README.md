# The Verification Problem in Compute Governance

**Can you detect LLM training just from GPU signals?**  
Implications for AI safety and governance



## TL;DR

Many proposals for governing advanced AI rely on monitoring large-scale training, but in practice it may be difficult to verify what GPUs are doing from the outside.

This project explores whether hardware-level telemetry (e.g. power, utilization) contains information about how computation unfolds over time, not just how much compute is used.

Simple examples suggest that different workloads — such as LLM training, LLM inference, and non-transformer training — can produce qualitatively different temporal patterns.

While highly preliminary, this raises the possibility that partial, non-intrusive signals could support some forms of external verification, even without access to internal logs or models.



## Why this matters

If large-scale training cannot be externally verified, many governance approaches would need to rely on trust or self-reporting.

If even partial signals can be observed from hardware telemetry, this could expand the set of feasible approaches to monitoring and verification in AI systems.



## Repository structure  
This repository is organized into three main components:

- **Analysis/**  
  Notebooks and scripts used to process, clean, and visualize telemetry data.

- **Writeup/**  
  Longer-form explanation of the project, including methodology, results, and discussion.

- **GPUTelemetrySampler/**  
  A C-based tool for collecting GPU telemetry (power, utilization, clocks, PCIe throughput).  
  This is used to capture raw hardware signals during different workloads.
