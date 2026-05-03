# AI Dreaming & Autonomous Background Memory Processing
## Deep Research Report

Date: 2026-04-11
Subject: Architectures for autonomous "dreaming" -- background memory consolidation, connection exploration, and insight generation for neural memory systems

---

## PART A: KEY FINDINGS FROM LITERATURE

### 1. Sleep-Inspired AI Memory Consolidation

#### 1.1 Wake-Sleep Consolidated Learning (WSCL) [arXiv:2401.08623]
- Most directly relevant paper to our goals
- Implements full wake/sleep cycle in neural networks
- **Wake phase**: Exposed to sensory input, adapts representations, stores episodic memories in short-term memory (hippocampus analog)
- **Sleep phase has two stages**:
  - **NREM**: Synaptic weight consolidation using replayed samples from short+long-term memory; synaptic plasticity mechanism strengthens important connections, weakens unimportant ones
  - **REM**: Exposed to previously-unseen realistic visual sensory experience; "dreaming" explores potential feature space, preparing synapses for future knowledge
- Demonstrated that dreaming enables positive forward transfer (preparing for tasks not yet seen)

#### 1.2 MyGO: Memory Yielding Generative Offline-consolidation [arXiv:2508.21296]
- Biological wake-sleep cycle inspired
- **Wake phase**: Rapidly learns new task, trains compact generative model (G-mem) to capture data distribution
- **Sleep phase**: Uses all learned G-mem models to generate pseudo-data ("dreams"), consolidates via knowledge distillation
- No raw data storage needed -- only compact generative models
- Proven on Split-MNIST and Split-AG News benchmarks

#### 1.3 Brain-Inspired Replay [Nature 2020, github.com/GMvandeVen/brain-inspired-replay]
- Published in Nature Communications
- Scales generative replay to natural images (CIFAR-100)
- Generative model produces "pseudo-experiences" mimicking hippocampal replay
- Key insight: Brain replays not just recent experiences but also synthetically generated novel experiences
- PyTorch implementation available with 251 stars

#### 1.4 SYNERgy [arXiv:2206.04016]
- Combines synaptic consolidation + dual memory experience replay
- **Semantic memory**: Accumulates and consolidates information across tasks
- **Episodic memory**: Stores specific experiences for replay
- **Synaptic consolidation**: Tracks parameter importance, anchors to consolidated parameters
- First method to combine dual memory + synaptic consolidation for general continual learning (no task boundaries needed)

#### 1.5 Deep Generative Replay [Shin et al., NeurIPS 2017]
- Seminal paper (1811.11682)
- Cooperative dual model: "generator" (synthesizes past data) + "solver" (performs tasks)
- Generator produces pseudo-samples from previous tasks during new learning
- Foundation for all generative replay approaches

### 2. Spreading Activation for Autonomous Exploration

#### 2.1 Core Algorithm
- Classic cognitive science technique for associative networks
- Activation spreads from source nodes through weighted edges
- Decay factor alpha at each step: activation(node) = sum(previous * weight * alpha)
- Used in: information retrieval, semantic networks, memory systems

#### 2.2 Query-Oriented Constrained Spreading Activation
- Novel improvement: only uses relations relevant to query content
- 18.9% better than syntactic search, 43.8% better than classical constrained SA
- Key for our system: adapt SA to use semantic similarity to gate propagation

#### 2.3 IGMiRAG [2024]
- Hierarchical heterogeneous hypergraph for multi-granular knowledge
- Bidirectional diffusion algorithm navigating deductive paths
- "Intuition-guided" retrieval controlling mining depth and memory window
- Dual-focus retrieval with adaptive memory activation

### 3. Dream-Like Offline Processing

#### 3.1 Hippocampal Replay Mechanisms
- **Forward replay**: Reinforces trajectories (experience consolidation)
- **Reverse replay**: Accelerates learning from reinforcement (policy gradient)
- Key paper: "A Robotic Model of Hippocampal Reverse Replay for RL" -- demonstrates reverse replay accelerates learning while improving stability

#### 3.2 Mesoscopic Model of Hippocampal Replay [2023]
- Stochastic neural mass model derived from spiking neurons
- Captures variability in content, direction, and timing of replay events
- Metastability from finite-size fluctuations + local fatigue
- Functionally desirable variability in replay

#### 3.3 Wake-Sleep Algorithm (Hinton et al., 1995)
- Original: unsupervised learning with directed generative models
- **Wake**: Follow directed connections upward, adjust generative weights
- **Sleep**: Follow undirected connections downward, adjust recognition weights
- Modern reincarnation in VAEs, hierarchical models

### 4. Self-Supervised Background Learning in Knowledge Graphs

#### 4.1 Knowledge Graph Completion
- Link prediction using graph embeddings (TransE, RotatE, ComplEx)
- Self-supervised: learn from existing graph structure to predict missing links
- Directly applicable to discovering new connections between memories

#### 4.2 Graph Attention-Based Representations [arXiv:2208.08130]
- Attention-enhanced message-passing captures local semantics
- Path-based context captures semantic relationships between distant entities
- Can be used during idle time to strengthen/diminish connections

#### 4.3 KG-Enhanced LLMs (Roadmap)
- Three frameworks: KG-enhanced LLMs, LLM-augmented KGs, Synergized LLMs+KGs
- Most relevant: Using LLMs for KG completion, construction, and refinement
- During idle time: LLM generates hypotheses about missing connections

### 5. Memory Consolidation Algorithms

#### 5.1 Experience Replay
- Standard: store experiences in buffer, sample uniformly for training
- **Prioritized Experience Replay**: Replay more important/transitional experiences
- **Episodic Memory**: Store representative exemplars (iCaRL)
- Foundational for any "dreaming" system

#### 5.2 Gradient Episodic Memory (GEM)
- Stores subset of past examples
- Uses gradients from stored examples to constrain new learning
- Prevents catastrophic forgetting while enabling new learning
- A-GEM variant: average gradients for efficiency

#### 5.3 Elastic Weight Consolidation (EWC)
- Fisher information matrix identifies important weights
- Regularization prevents change of important weights
- Complementary to replay: protect consolidated knowledge

#### 5.4 Synaptic Intelligence (SI)
- Online version of EWC
- Tracks parameter importance during training
- More efficient than computing full Fisher matrix

---

## PART B: CONCRETE ALGORITHMS/APPROACHES

### Algorithm 1: Dreaming Cycle for Knowledge Graph Memory

```
DREAM_CYCLE(memory_system):
    Phase NREM: "Replay & Consolidate"
        1. SELECT sample of recent memories (weighted by recency)
        2. FOR each memory m in sample:
            a. SPREAD activation from m through weighted connections
            b. COLLECT activation pattern (top-K activated nodes)
            c. COMPARE activation cluster with stored prototypes
            d. IF cluster coheres with existing prototype:
                STRENGTHEN connections within cluster
                STRENGTHEN prototype representation
               ELSE IF cluster is novel:
                CREATE new prototype from cluster centroid
        3. DECAY connections not recently activated
    
    Phase REM: "Explore & Connect"
        1. SELECT memories with low connection density (isolated)
        2. FOR each isolated memory m:
            a. COMPUTE embedding similarity with random other memories
            b. IF similarity > threshold:
                CREATE tentative connection (low weight)
            c. SPREAD activation from m through tentative connections
            d. IF activation creates coherent pattern:
                STRENGTHEN tentative connection
               ELSE:
                DISCARD tentative connection
        3. LOOK for structural holes (bridges between clusters)
        4. IF bridge found:
            CREATE "insight" node connecting clusters
            TAG insight with connection metadata
```

### Algorithm 2: Generative Replay for Embedding Space

```
GENERATIVE_REPLAY_DREAM(memory_system):
    1. SELECT K random memory embeddings from storage
    2. GENERATE pseudo-embeddings by:
        a. Interpolation: blend pairs of real embeddings
           pseudo = alpha * emb_i + (1-alpha) * emb_j
        b. Perturbation: add small noise to real embeddings
           pseudo = emb_i + epsilon * N(0,1)
    3. FIND nearest neighbors of pseudo-embeddings in real memories
    4. IF pseudo-embedding bridges two clusters:
        CREATE tentative connection between neighbor memories
        EVALUATE connection strength by cluster coherence
    5. UPDATE embedding positions of consolidated memories:
        For each cluster, compute centroid, move memories slightly toward centroid
```

### Algorithm 3: Spreading Activation Exploration

```
SA_DREAM(memory_system):
    1. SELECT random seed node
    2. Initialize activation[node] = 1.0 for seed, 0.0 elsewhere
    3. For T iterations:
        new_activation = zeros()
        FOR each node n with activation > threshold:
            FOR each connected node c:
                new_activation[c] += activation[n] * weight[n,c] * alpha
        activation = normalize(new_activation)
    4. Top-K activated nodes form a "dream cluster"
    5. If dream cluster spans multiple existing clusters:
        CREATE bridging insight node
        CONNECT bridging node to top-activated nodes
    6. LOG dream content (activated memories, patterns found)
```

### Algorithm 4: Self-Supervised Link Prediction

```
LINK_PREDICTION_DREAM(memory_system):
    1. Sample positive edges (existing connections)
    2. Sample negative edges (non-existent connections)
    3. Train lightweight link predictor:
        Score(e1, e2) = sigmoid(MLP(concat(emb_e1, emb_e2, edge_features)))
    4. Use predictor to score potential new connections
    5. Top-scoring new connections above threshold:
        CREATE tentative connections with predicted weight
    6. Periodically retrain predictor on updated graph
```

---

## PART C: PROPOSED ARCHITECTURE FOR OUR SYSTEM

### System Specifications (given)
- SQLite storage
- 384-dim SentenceTransformer embeddings
- Knowledge graph with weighted connections
- Spreading activation (neural_think)
- Semantic search (neural_recall)
- 918 memories, 40K connections

### Proposed Dreaming Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                    DREAM CONTROLLER                         │
│  (Background thread, triggered on idle or scheduled)        │
│                                                             │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐      │
│  │ NREM Phase   │  │ REM Phase    │  │ Insight Phase│      │
│  │              │  │              │  │              │      │
│  │ - Replay     │  │ - Explore    │  │ - Synthesize │      │
│  │ - Strengthen │  │ - Connect    │  │ - Abstract   │      │
│  │ - Decay      │  │ - Bridge     │  │ - Record     │      │
│  └──────┬───────┘  └──────┬───────┘  └──────┬───────┘      │
│         │                 │                  │               │
│         └─────────────────┴──────────────────┘               │
│                           │                                  │
│                    ┌──────▼──────┐                           │
│                    │  DREAM LOG  │                           │
│                    │  (SQLite)   │                           │
│                    └─────────────┘                           │
└─────────────────────────────────────────────────────────────┘
                           │
         ┌─────────────────┼─────────────────┐
         │                 │                 │
   ┌─────▼─────┐   ┌──────▼──────┐   ┌─────▼──────┐
   │  MEMORY    │   │ CONNECTIONS │   │ EMBEDDINGS │
   │  TABLE     │   │   TABLE     │   │   CACHE    │
   │ (SQLite)   │   │  (SQLite)   │   │ (384-dim)  │
   └────────────┘   └─────────────┘   └────────────┘
```

### Schema Extensions for Dreaming

```sql
-- Dream session log
CREATE TABLE dream_sessions (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    started_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    completed_at TIMESTAMP,
    phase TEXT CHECK(phase IN ('nrem', 'rem', 'insight')),
    memories_processed INTEGER,
    connections_created INTEGER,
    connections_strengthened INTEGER,
    connections_pruned INTEGER,
    insights_generated INTEGER,
    metadata JSON
);

-- Generated insights
CREATE TABLE dream_insights (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    session_id INTEGER REFERENCES dream_sessions(id),
    type TEXT CHECK(type IN ('bridge', 'cluster', 'analogy', 'abstraction', 'correction')),
    source_memories JSON,  -- Array of memory IDs involved
    insight_text TEXT,      -- Human-readable description
    embedding BLOB,         -- 384-dim embedding of insight
    confidence REAL,        -- Connection strength / confidence
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    reviewed BOOLEAN DEFAULT FALSE,
    promoted BOOLEAN DEFAULT FALSE  -- Promoted to permanent memory
);

-- Connection history for tracking dream modifications
CREATE TABLE connection_history (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    memory_a_id INTEGER,
    memory_b_id INTEGER,
    action TEXT CHECK(action IN ('created', 'strengthened', 'weakened', 'pruned')),
    old_weight REAL,
    new_weight REAL,
    source TEXT CHECK(source IN ('nrem_replay', 'rem_explore', 'insight', 'user')),
    dream_session_id INTEGER,
    timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);
```

### Implementation: Dream Controller

```python
class DreamController:
    """
    Background dreaming process for neural memory system.
    Runs in separate thread, triggered on idle periods.
    """
    
    def __init__(self, memory_system, config=None):
        self.memory = memory_system
        self.config = config or {
            'nrem_sample_size': 50,      # Memories per NREM batch
            'rem_sample_size': 20,       # Isolated memories to explore
            'activation_decay': 0.7,     # SA decay per hop
            'activation_threshold': 0.1, # Minimum activation
            'max_hops': 5,               # SA depth
            'connection_threshold': 0.6, # Min similarity for new connections
            'strengthen_rate': 0.05,     # Connection weight increase
            'weaken_rate': 0.01,         # Connection weight decrease
            'prune_threshold': 0.05,     # Connections below this are pruned
            'insight_confidence': 0.7,   # Min confidence for insight creation
            'idle_trigger_seconds': 300, # 5 min idle before dreaming
            'max_dream_duration': 60,    # Max seconds per dream cycle
        }
        self.is_dreaming = False
        self.last_interaction = time.time()
    
    def should_dream(self):
        """Check if conditions are right for dreaming."""
        idle_time = time.time() - self.last_interaction
        return (idle_time > self.config['idle_trigger_seconds'] 
                and not self.is_dreaming
                and self.memory.has_pending_dreaming())
    
    def run_dream_cycle(self):
        """Execute a full dream cycle: NREM -> REM -> Insights."""
        self.is_dreaming = True
        session_id = self._start_session()
        
        try:
            # Phase 1: NREM - Replay and consolidate recent memories
            nrem_results = self._nrem_phase()
            
            # Phase 2: REM - Explore connections and bridge gaps
            rem_results = self._rem_phase()
            
            # Phase 3: Synthesize insights
            insight_results = self._insight_phase()
            
            self._complete_session(session_id, {
                'nrem': nrem_results,
                'rem': rem_results,
                'insights': insight_results
            })
            
            return self._format_dream_report(session_id)
            
        finally:
            self.is_dreaming = False
    
    def _nrem_phase(self):
        """
        NREM Sleep: Replay and strengthen important connections.
        
        1. Sample recent memories (weighted by recency)
        2. For each, run spreading activation
        3. Strengthen activated connections
        4. Weaken dormant connections
        5. Prune very weak connections
        """
        results = {
            'memories_processed': 0,
            'connections_strengthened': 0,
            'connections_weakened': 0,
            'connections_pruned': 0,
        }
        
        # Sample memories weighted by recency and importance
        memories = self.memory.sample_recent_memories(
            k=self.config['nrem_sample_size'],
            weight_by='recency_and_connection_count'
        )
        
        for mem in memories:
            # Run spreading activation from this memory
            activation = self.memory.spreading_activation(
                source_id=mem['id'],
                max_hops=self.config['max_hops'],
                decay=self.config['activation_decay']
            )
            
            # Strengthen activated connections
            for node_id, activation_level in activation.items():
                if activation_level > self.config['activation_threshold']:
                    connections = self.memory.get_connections_between(
                        mem['id'], node_id
                    )
                    for conn in connections:
                        new_weight = min(1.0, 
                            conn['weight'] + self.config['strengthen_rate'] * activation_level
                        )
                        self.memory.update_connection_weight(
                            conn['id'], new_weight, source='nrem_replay'
                        )
                        results['connections_strengthened'] += 1
            
            results['memories_processed'] += 1
        
        # Decay connections not recently activated
        self._decay_dormant_connections(results)
        
        return results
    
    def _rem_phase(self):
        """
        REM Sleep: Explore potential connections.
        
        1. Find memories with low connection density (isolated)
        2. For each, find semantically similar memories via embedding
        3. Create tentative connections
        4. Test via spreading activation for coherence
        """
        results = {
            'memories_explored': 0,
            'tentative_connections': 0,
            'connections_created': 0,
            'bridges_found': 0,
        }
        
        # Find isolated memories (few connections relative to importance)
        isolated = self.memory.get_isolated_memories(
            k=self.config['rem_sample_size']
        )
        
        for mem in isolated:
            # Find top-K nearest neighbors by embedding
            neighbors = self.memory.find_nearest_by_embedding(
                embedding=mem['embedding'],
                k=10,
                exclude_connected=True  # Don't suggest already-connected
            )
            
            for neighbor in neighbors:
                if neighbor['similarity'] >= self.config['connection_threshold']:
                    # Create tentative connection
                    self.memory.create_connection(
                        source_id=mem['id'],
                        target_id=neighbor['id'],
                        weight=neighbor['similarity'] * 0.3,  # Start low
                        connection_type='tentative',
                        source='rem_explore'
                    )
                    results['tentative_connections'] += 1
                    
                    # Test coherence via spreading activation
                    activation = self.memory.spreading_activation(
                        source_id=mem['id'],
                        max_hops=3,
                        decay=0.8
                    )
                    
                    if neighbor['id'] in activation:
                        # Connection creates coherent activation pattern
                        self.memory.promote_connection(
                            mem['id'], neighbor['id']
                        )
                        results['connections_created'] += 1
            
            results['memories_explored'] += 1
        
        return results
    
    def _insight_phase(self):
        """
        Insight Phase: Identify bridges and generate abstractions.
        
        1. Find community structure (clusters) in the graph
        2. Identify bridge nodes connecting clusters
        3. Generate abstraction nodes for strong cluster patterns
        4. Record insights for human review
        """
        results = {
            'insights_generated': 0,
            'bridges_identified': 0,
            'abstractions_created': 0,
        }
        
        # Find community structure
        communities = self.memory.detect_communities(method='louvain')
        
        # Identify bridges (nodes in multiple communities or connecting them)
        for i, comm_a in enumerate(communities):
            for j, comm_b in enumerate(communities[i+1:]):
                bridges = self.memory.find_bridges_between(comm_a, comm_b)
                for bridge in bridges:
                    results['bridges_identified'] += 1
                    self._create_insight(
                        type='bridge',
                        source_memories=bridge['nodes'],
                        description=f"Bridge between {comm_a['label']} and {comm_b['label']}",
                        confidence=bridge['strength']
                    )
                    results['insights_generated'] += 1
        
        # Generate abstractions for large, coherent clusters
        for comm in communities:
            if len(comm['members']) > 5 and comm['coherence'] > 0.7:
                abstraction = self._generate_abstraction(comm)
                if abstraction:
                    results['abstractions_created'] += 1
                    results['insights_generated'] += 1
        
        return results
    
    def _create_insight(self, type, source_memories, description, confidence):
        """Store a dream-generated insight for review."""
        if confidence < self.config['insight_confidence']:
            return
        
        # Generate embedding from source memory embeddings
        source_embeddings = [
            self.memory.get_embedding(mid) for mid in source_memories
        ]
        insight_embedding = np.mean(source_embeddings, axis=0)
        
        self.memory.db.execute("""
            INSERT INTO dream_insights 
            (type, source_memories, insight_text, embedding, confidence)
            VALUES (?, ?, ?, ?, ?)
        """, (
            type,
            json.dumps(source_memories),
            description,
            insight_embedding.tobytes(),
            confidence
        ))
    
    def _generate_abstraction(self, community):
        """
        Generate an abstraction node for a coherent cluster.
        Uses LLM to summarize common themes.
        """
        # Get memory texts for community members
        texts = [
            self.memory.get_memory(mid)['content'] 
            for mid in community['members'][:10]
        ]
        
        # Compute cluster centroid embedding
        embeddings = [
            self.memory.get_embedding(mid) 
            for mid in community['members']
        ]
        centroid = np.mean(embeddings, axis=0)
        
        self._create_insight(
            type='abstraction',
            source_memories=community['members'],
            description=f"Cluster: {community.get('label', 'unlabeled')} "
                       f"({len(community['members'])} memories)",
            confidence=community['coherence']
        )
    
    def _decay_dormant_connections(self, results):
        """Slowly decay connection weights for non-activated connections."""
        self.memory.db.execute("""
            UPDATE connections 
            SET weight = MAX(0, weight - ?)
            WHERE last_activated_at < datetime('now', '-7 days')
            AND source != 'user'
        """, (self.config['weaken_rate'],))
        
        # Prune very weak connections
        pruned = self.memory.db.execute("""
            DELETE FROM connections 
            WHERE weight < ? AND source != 'user'
        """, (self.config['prune_threshold'],)).rowcount
        
        results['connections_pruned'] = pruned
```

### Integration with Existing System

```
TRIGGERING DREAMING:
- After 5 minutes of no user interaction
- When memory count crosses threshold (e.g., every 100 new memories)
- On explicit API call: neural_dream()
- Scheduled (e.g., nightly batch)

DREAM OUTPUTS:
1. Strengthened connections (NREM)
2. New discovered connections (REM)  
3. Insight nodes (bridges, abstractions)
4. Pruned weak/dead connections
5. Dream report (JSON/Markdown)

INTEGRATION POINTS:
- neural_think() already does SA -- reuse for NREM replay
- neural_recall() does semantic search -- reuse for REM neighbor finding
- Connections table already has weights -- dream modifies them
- Add dream_insights table for generated insights
```

### Dream Report Format

```json
{
    "session_id": 42,
    "started_at": "2026-04-11T03:00:00Z",
    "completed_at": "2026-04-11T03:01:12Z",
    "duration_seconds": 72,
    "nrem_phase": {
        "memories_replayed": 50,
        "connections_strengthened": 142,
        "connections_weakened": 38,
        "connections_pruned": 7
    },
    "rem_phase": {
        "memories_explored": 20,
        "tentative_connections": 34,
        "connections_created": 12
    },
    "insights": [
        {
            "type": "bridge",
            "description": "Discovered connection between 'Python performance' cluster and 'C++ optimization' cluster via memory #452",
            "confidence": 0.82,
            "source_memories": [452, 189, 301]
        },
        {
            "type": "abstraction", 
            "description": "Abstract pattern: 'Memory architecture research' (12 memories)",
            "confidence": 0.75,
            "source_memories": [1, 2, 3, 5, 8, 13, 21, 34, 55, 89, 144, 233]
        }
    ],
    "summary": "Consolidated recent memories, discovered 12 new connections, found 2 insight bridges."
}
```

---

## PART D: KEY REFERENCES

### Academic Papers
1. **Wake-Sleep Consolidated Learning** - arXiv:2401.08623 (2024)
2. **MyGO: Memory Yielding Generative Offline-consolidation** - arXiv:2508.21296 (2025)
3. **SYNERgy: Synaptic Consolidation + Dual Memory Replay** - arXiv:2206.04016 (2022)
4. **Brain-Inspired Replay** - Nature Communications 2020, github.com/GMvandeVen/brain-inspired-replay
5. **Continual Learning with Deep Generative Replay** - NeurIPS 2017, arXiv:1705.08690
6. **Experience Replay for Continual Learning** - arXiv:1811.11682
7. **Carousel Memory for CL** - arXiv:2110.07276
8. **Energy-Based Models for CL** - arXiv:2011.12216
9. **Supervised Contrastive Replay** - arXiv:2103.13885
10. **Discovering Latent Info by Spreading Activation** - academic paper on constrained SA

### GitHub Implementations
1. **GMvandeVen/continual-learning** (1842 stars) - PyTorch CL methods (EWC, SI, LwF, DGR, A-GEM, iCaRL)
2. **GMvandeVen/brain-inspired-replay** (251 stars) - Brain-inspired generative replay
3. **kuc2477/pytorch-deep-generative-replay** (167 stars) - DGR implementation
4. **Fozikio/cortex-engine** - Full cognitive engine with NREM/REM dreaming, 57 MCP tools, SQLite backend
5. **EESIZ/clawdreamer** - "Give your AI agent the ability to dream. Nightly memory consolidation."
6. **cgbarlow/machine-dream_AG** - GRASP loop, dreaming consolidation, persistent memory, SQLite AgentDB
7. **VoidLight00/ai-dream** - Auto-dream memory consolidation for AI assistants
8. **adammounir/NullFlow** - Wake-sleep consolidation + flow matching

### Theoretical Foundations
- Complementary Learning Systems (McClelland et al., 1995)
- Wake-Sleep Algorithm (Hinton et al., 1995)
- Hippocampal replay during sleep (Karlsson & Frank, 2009)
- Sparse Distributed Memory (Kanerva, 1988)
- Modern Hopfield Networks (Ramsauer et al., 2020)

---

## SUMMARY OF RECOMMENDATIONS

For the given system (SQLite, 384-dim embeddings, knowledge graph, spreading activation):

1. **Start with NREM replay**: Reuse existing spreading activation to replay and consolidate connections. Low complexity, high impact.

2. **Add REM exploration**: Use embedding similarity to discover new connections for isolated memories. Medium complexity.

3. **Add insight generation**: Detect communities and bridges, generate abstraction nodes. Medium complexity.

4. **Implement connection decay**: Slowly weaken dormant connections, prune dead ones. Low complexity.

5. **Log dream sessions**: Add dream_sessions and dream_insights tables. Track what dreaming accomplishes.

6. **Schedule dreaming**: Trigger after idle periods or on schedule. Background thread that doesn't block user interactions.

7. **Consider generative replay**: If LLM available, use it to generate synthetic "pseudo-memories" during dreams that bridge existing clusters.

8. **Consider link prediction**: Train lightweight classifier to predict missing connections from existing graph structure.

The most impactful first step is implementing the NREM phase (replay + connection strengthening/weakening) since it requires minimal new infrastructure and reuses existing spreading activation.

---

---

## APPENDIX: EXISTING IMPLEMENTATIONS (REFERENCE CODE)

### clawdreamer (EESIZ/clawdreamer) - Python, LanceDB

Architecture:
```
Episodes (md files) → NREM (chunk, cluster, dedup, store) → REM (conflict detect, merge, decay) → Dream Log
```

**NREM Phase** (nrem.py):
- Load episode markdown files (date-named: YYYY-MM-DD.md)
- Chunk by markdown headers and paragraphs (semantic units)
- Embed all chunks via sentence transformer
- Greedy agglomerative clustering by cosine similarity (threshold ~0.82)
- For each cluster: LLM analyzes to extract key facts
- Deduplicate against existing memories (similarity check)
- Store summarized semantic memories in vector DB
- Optional: create reference docs for new procedures

**REM Phase** (rem.py):
- Find conflicts between NEW memories and ALL existing memories
- 3-tier classification: state_change / different_aspects / unrelated
- **state_change**: Merge into one memory with historical context
- **different_aspects**: Consolidate into comprehensive memory or split
- **unrelated**: Skip
- Apply importance decay: age_days^0.5 * decay_rate
- Soft-delete memories below importance threshold
- Archive processed episodes

Key config parameters:
- CLUSTER_SIMILARITY: 0.82 (cluster formation threshold)
- DEDUP_SIMILARITY: 0.92 (deduplication threshold)
- CONTRADICTION_SIMILARITY: 0.85 (conflict detection threshold)
- IMPORTANCE_DECAY_RATE: 0.02 per sqrt(day)
- SOFT_DELETE_THRESHOLD: 0.1

### cortex-engine (Fozikio/cortex-engine) - TypeScript, SQLite

57 cognitive tools over MCP. Key dreaming-related:
- `dream` - NREM compression + REM integration
- `wander` - Random associative exploration
- `evolve` - Self-modification of knowledge structure

NREM: cluster, refine, create
REM: connect, score, abstract

### machine-dream_AG (cgbarlow/machine-dream_AG) - Node.js, SQLite

5-phase dreaming pipeline:
1. Pattern extraction from experiences
2. Clustering (FastCluster, DeepCluster, LLMCluster)
3. Strategy synthesis
4. Anti-pattern identification
5. Learning unit creation

GRASP Loop: Generate → Review → Absorb → Synthesize → Persist

Uses AgentDB with ReasoningBank + ReflexionMemory tables.


---

## APPENDIX: EXISTING IMPLEMENTATIONS (REFERENCE CODE)

### clawdreamer (EESIZ/clawdreamer) - Python, LanceDB

Architecture: Episodes -> NREM (chunk, cluster, dedup, store) -> REM (conflict detect, merge, decay) -> Dream Log

**NREM Phase**:
- Load episode markdown files (date-named: YYYY-MM-DD.md)
- Chunk by markdown headers and paragraphs
- Embed all chunks via sentence transformer
- Greedy agglomerative clustering by cosine similarity (threshold ~0.82)
- For each cluster: LLM analyzes to extract key facts
- Deduplicate against existing memories
- Store summarized semantic memories in vector DB

**REM Phase**:
- Find conflicts between NEW and ALL existing memories
- 3-tier classification: state_change / different_aspects / unrelated
- state_change: Merge into one memory with historical context
- different_aspects: Consolidate into comprehensive memory
- Apply importance decay: age_days^0.5 * decay_rate
- Soft-delete memories below importance threshold
- Archive processed episodes

Key parameters: CLUSTER_SIMILARITY=0.82, DEDUP_SIMILARITY=0.92, IMPORTANCE_DECAY_RATE=0.02

### cortex-engine (Fozikio/cortex-engine) - TypeScript, SQLite

57 cognitive tools. NREM: cluster, refine, create. REM: connect, score, abstract.
Tools: dream, wander, evolve, observe, believe, query.

### machine-dream_AG - Node.js, SQLite

GRASP Loop: Generate -> Review -> Absorb -> Synthesize -> Persist
5-phase dreaming with FastCluster/DeepCluster/LLMCluster algorithms.


---

## APPENDIX: EXISTING IMPLEMENTATIONS (REFERENCE CODE)

### clawdreamer (EESIZ/clawdreamer) - Python, LanceDB

Architecture: Episodes -> NREM (chunk, cluster, dedup, store) -> REM (conflict detect, merge, decay) -> Dream Log

NREM Phase:
- Load episode markdown files (date-named: YYYY-MM-DD.md)
- Chunk by markdown headers and paragraphs
- Embed all chunks via sentence transformer
- Greedy agglomerative clustering by cosine similarity (threshold 0.82)
- For each cluster: LLM analyzes to extract key facts
- Deduplicate against existing memories
- Store summarized semantic memories in vector DB

REM Phase:
- Find conflicts between NEW and ALL existing memories
- 3-tier classification: state_change / different_aspects / unrelated
- state_change: Merge into one memory with historical context
- different_aspects: Consolidate into comprehensive memory
- Apply importance decay: age_days^0.5 * decay_rate
- Soft-delete memories below importance threshold
- Archive processed episodes

Key parameters: CLUSTER_SIMILARITY=0.82, DEDUP_SIMILARITY=0.92, IMPORTANCE_DECAY_RATE=0.02

### cortex-engine (Fozikio/cortex-engine) - TypeScript, SQLite

57 cognitive tools. NREM: cluster, refine, create. REM: connect, score, abstract.
Tools: dream, wander, evolve, observe, believe, query.

### machine-dream_AG - Node.js, SQLite

GRASP Loop: Generate -> Review -> Absorb -> Synthesize -> Persist
5-phase dreaming with FastCluster/DeepCluster/LLMCluster algorithms.
