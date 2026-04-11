# The Verification Problem in Compute Governance

**Can you detect LLM training just from GPU signals?**  
Implications for AI safety and governance



## TL;DR

Many proposals for governing advanced AI rely on monitoring large-scale training, but in practice it may be difficult to verify what GPUs are doing without intrusive access to proprietary software or data.
This project examines whether hardware-level telemetry preserves information about how computation is organized over time. Initial comparisons suggest that different workloads — including LLM training, LLM inference, CNN training (ResNet50), and masked language modeling (BERT) — can produce distinguishable temporal patterns in these signals.
These results do not solve the verification problem, but they suggest that non-intrusive hardware signals may offer a promising source of evidence about how compute is being used, without requiring access to sensitive model internals.



## Repository structure  
This repository is organized into three main components:

- **Analysis/**  
  Notebooks and scripts used to process, clean, and visualize telemetry data.

- **Writeup/**  
  Longer-form explanation of the project, including methodology, results, and discussion.

- **GPUTelemetrySampler/**  
  A C-based tool for collecting GPU telemetry (power, utilization, clocks, PCIe throughput).  
  This is used to capture raw hardware signals during different workloads.
