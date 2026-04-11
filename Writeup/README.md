# Writeup

This component contains the full writeup of the project:

**The Verification Problem in Compute Governance**

## Purpose

The writeup presents the motivation, approach, and findings of the project in a form intended for a broader audience, including readers interested in AI governance and technical alignment.

It is not a formal paper or benchmark, but a structured argument supported by illustrative empirical results.

## Overview

The writeup explores a central question:

> If large-scale AI training is difficult to observe directly, can it be inferred from external signals?

It focuses on the possibility of using GPU telemetry — power, utilization, and related signals — as a non-intrusive source of evidence about how compute is being used.

## Structure

The document is organized as follows:

- **The problem**  
  Why compute governance requires verification, and why this is difficult in practice  

- **Why this matters**  
  Implications for enforceability of AI governance proposals  

- **The idea**  
  Hardware telemetry as a potential source of external, non-intrusive signals  

- **What telemetry might reveal**  
  Why different workloads may leave different temporal signatures  

- **Exploring hardware signal patterns**  
  Qualitative comparisons across workloads  

- **Frequency-domain analysis**  
  Moving from raw signals to spectral representations  

- **What this does (and does not) show**  
  Interpreting results without overclaiming  

- **Limitations and open questions**  
  Key uncertainties and directions for future work  

## Positioning

This work should be understood as:

- An initial exploration of a possible approach  
- A demonstration that telemetry can exhibit structured differences  
- A step toward making the verification problem more concrete  

It is **not**:

- A reliable detection method  
- A deployable monitoring system  
- Evidence of robustness across real-world conditions  

## Intended audience

The writeup is designed to be accessible to:

- AI governance researchers  
- Technical AI safety practitioners  
- Readers familiar with machine learning systems  

It assumes basic familiarity with GPU-based computation, but does not require deep expertise.

## Relation to the repository

The writeup is supported by:

- **GPUTelemetrySampler/** for data collection  
- **Analysis/** for processing and visualization  

Together, these components provide both the conceptual framing and the empirical basis for the arguments presented.
