# Neural Memory Architectures and Associative Memory Systems for AI
## Deep Research Report - Architectural Patterns for Brain-Like Memory

Date: 2026-04-09
Goal: Architectural patterns for a brain-like memory system that discovers connections autonomously, deployable with small (few million param) models on MSSQL data.

---

## 1. HOPFIELD NETWORKS AND MODERN VARIANTS (DENSE ASSOCIATIVE MEMORIES)

### Classical Hopfield Networks (1982)
- Binary neurons with symmetric weights, energy-based dynamics
- Pattern storage via Hebbian learning
- Capacity: ~0.138N patterns for N neurons
- Retrieval: pattern completion from partial/noisy input via energy minimization

### Dense Associative Memories (Krotov & Hopfield, 2016)
- Key Paper: arXiv:1606.01164, NIPS 2016
- Higher-order polynomial interactions replace quadratic energy
- Exponential storage capacity in dimension of feature space
- Duality between dense associative memory and feedforward deep networks

### Modern Hopfield Networks (Ramsauer et al., 2020)
- Key Paper: arXiv:2008.02217
- Continuous states with update rule equivalent to transformer attention
- Three energy minima: global fixed point, metastable states, single pattern storage
- Hopfield layers as plug-in replacements for attention
- GitHub: github.com/ml-jku/hopfield-layers

### Universal Hopfield Networks (Millidge et al., 2022)
- Key Paper: arXiv:2202.04557
- General framework: similarity -> separation -> projection
- Unifies classical HNs, SDMs, modern continuous Hopfield networks
- Euclidean/Manhattan distance outperforms dot-product similarity

### Key for Brain-Like Memory:
- Modern Hopfield nets = transformers attention (mathematical equivalence)
- Exponential storage capacity enables large-scale systems
- Energy-based framework for principled connection discovery

---

## 2. MEMORY-AUGMENTED NEURAL NETWORKS (MANN, NTM, DNC)

### Neural Turing Machines (Graves et al., 2014)
- Key Paper: arXiv:1410.5401 (seminal, ~5000+ citations)
- External memory via attentional processes
- Differentiable end-to-end
- Operations: read, write, content-based/location-based addressing

### Differentiable Neural Computer (Graves et al., 2016)
- Extension with temporal link matrix
- Sequential memory access and order preservation
- Graph traversal, family tree reasoning

### TARDIS (Gulcehre et al., 2017)
- Key Paper: arXiv:1701.08718
- Wormhole connections to past hidden states
- Reduces vanishing gradient for very long sequences

### Sparse Access Memory (SAM) (Rae et al., 2016)
- Key Paper: arXiv:1610.09027
- 1000x faster, 3000x less memory than non-sparse models
- Asymptotic lower bounds in space/time complexity
- Scales to 100,000+ time steps

### Distributed Associative Memory (Park et al., 2021)
- Key Paper: arXiv:2007.10637
- Multiple smaller memory blocks instead of single large memory
- Memory Refreshing Loss: reproduces inputs from stored memories
- Inspired by brain rehearsal process

---

## 3. VECTOR SYMBOLIC ARCHITECTURES (VSA) / HYPERDIMENSIONAL COMPUTING

### Core Concepts
- High-dimensional vectors (10,000+ dimensions) represent concepts
- Binding (element-wise multiplication): encodes associations
- Bundling (element-wise addition): creates superpositions
- Permutation (rotation): encodes sequences
- Holographic representation: every part contains whole information

### Systems
1. Holographic Reduced Representations (HRR) - Plate 1995
2. Binary Spatter Codes (BSC) - Kanerva 1996
3. MAP (Multiply-Add-Permute) - Gayler 2004
4. HD Computing for Edge AI - Rahimi et al.

### Self-Organizing Properties
- Binding creates new composite representations
- Similarity-based retrieval (cosine similarity)
- Recursive composition enables hierarchical concepts
- Natural connection discovery through algebraic operations

---

## 4. EPISODIC vs SEMANTIC vs PROCEDURAL MEMORY

### Episodic Memory (What happened)
- Memory of specific events with context
- AI: NTM/DNC with temporal links, experience replay
- Architecture: FIFO buffer + context indexing

### Semantic Memory (What is known)
- General factual knowledge
- AI: Knowledge graphs, learned prototypes, distributed representations
- Architecture: Prototype extraction from episodic consolidation

### Procedural Memory (How to do)
- Skills and habits
- AI: Trained neural network weights, policy networks
- Architecture: Compiled as network parameters

### Complementary Learning Systems (McClelland et al., 1995)
- Hippocampus: fast episodic learning (sparse, pattern-separated)
- Neocortex: slow statistical integration (distributed)
- Replay: hippocampus teaches neocortex during consolidation

---

## 5. AUTONOMOUS CONNECTION DISCOVERY

### Approaches:
A. Energy-Based: Energy minimization discovers implicit connections
B. Hyperdimensional Binding: Random bindings create novel compositions
C. Attention-Based: Self-attention discovers internal relationships
D. Graph-Based: Community detection + link prediction
E. Contrastive Learning: Related memories projected nearby

### Implementation Strategy:
1. Encode MSSQL records to high-dimensional vectors
2. Store in modern Hopfield layer
3. Discover via periodic self-attention sweeps + energy minimization
4. Consolidate strong connections into semantic graph
5. Retrieve via partial cues (associative completion)

---

## 6. SPARSE DISTRIBUTED MEMORY (SDM) - Kanerva 1988

### Core Concepts
- Binary address space of 2^N locations
- Hard locations: random points in address space
- Reading: sum contents of locations within radius R of query
- Writing: add pattern to locations within radius R
- Capacity: many more patterns than hard locations
- Graceful degradation with noise

### Modern Extensions
- Special case of Universal Hopfield Networks
- Similar to transformer attention with locality-sensitive hashing
- K-winner Modern Hopfield Network (2023)

---

## 7. RECOMMENDED ARCHITECTURE

### For Small Models (1-3M params) on MSSQL Data:



### Key Implementation References:
- Hopfield layers: github.com/ml-jku/hopfield-layers
- DNC reference: github.com/deepmind/dnc
- VSA library: github.com/hyperdimensional-computing

---

## 8. KEY ARXIV PAPERS

1. Neural Turing Machines - arXiv:1410.5401
2. Dense Associative Memory - arXiv:1606.01164
3. Hopfield Networks is All You Need - arXiv:2008.02217
4. Large Associative Memory Problem - arXiv:2008.06996
5. Universal Hopfield Networks - arXiv:2202.04557
6. TARDIS (Wormhole Connections) - arXiv:1701.08718
7. Sparse Access Memory - arXiv:1610.09027
8. Distributed Associative Memory - arXiv:2007.10637
9. DNC with Memory Demon - arXiv:2211.02987
10. Memory Association Networks - arXiv:2111.02353
11. Hopfield-Fenchel-Young Networks - arXiv:2411.08590
12. MDL Hopfield Networks - arXiv:2311.06518
13. Feature-Label Memory Network - arXiv:1710.07110
14. Encoding-based Memory Modules - arXiv:2001.11771
15. Review on Neural Turing Machine - arXiv:1904.05061

---

*Research from arXiv API queries and domain knowledge. Semantic Scholar was rate-limited during collection.*
