# Plateformes & Domaines Spécialisés

## 1. Langages & Paradigmes

### Paradigmes
- **Impératif/procédural** : C, Pascal, statements séquentiels
- **Orienté objet** : encapsulation, héritage, polymorphisme, SOLID
- **Fonctionnel** : immutabilité, fonctions pures, higher-order, monades
- **Logique** : Prolog, unification, backtracking
- **Data-oriented** : cache-friendly, SoA, ECS
- **Réactif** : streams, observables, backpressure
- **Métaprogrammation** : macros (Rust, Lisp), templates (C++), codegen

### Compétences trans-langages
- Type systems : static/dynamic, strong/weak, inference, generics
- Ownership : Rust borrow checker, lifetimes, RAII
- Memory management : GC (mark-sweep, generational), ARC, manual, arenas
- ABI/FFI : calling conventions, marshalling, bindings
- Compilation : lexing → parsing → AST → IR → optimization → codegen → linking

## 2. Frontend & UX

### Fondamentaux web
- HTML5 sémantique, accessibility (ARIA, WCAG)
- CSS : Flexbox, Grid, animations, specificity
- Performance : CLS, LCP, FID, Core Web Vitals

### JavaScript/TypeScript
- Event loop : call stack, task queue, microtasks
- Closures, prototypes, `this` binding
- Async : callbacks, promises, async/await
- TypeScript : types, generics, discriminated unions, type guards

### Frameworks
- React : components, hooks, reconciliation, fiber
- Vue : reactivity, composition API, virtual DOM
- Svelte : compile-time reactivity, no virtual DOM
- State management : Redux, Zustand, Pinia, signals

### Rendering
- CSR, SSR, SSG, ISR, hybrid
- Hydration, partial hydration, islands
- Streaming SSR, React Server Components

### Web security
- CSP : Content-Security-Policy
- CORS : Cross-Origin Resource Sharing
- Sandboxing : iframes, Web Workers

### UX Engineering
- Design systems, component libraries
- Responsive design, mobile-first
- i18n/l10n : ICU, pluralization, RTL
- Typography, accessibility testing

## 3. Mobile & Desktop

### Android
- Activity/Fragment lifecycle
- Threading : Handler, Looper, Coroutines
- Jetpack : ViewModel, LiveData, Room, Compose
- Performance : StrictMode, systrace, baseline profiles

### iOS
- UIKit vs SwiftUI
- ARC, memory management
- Concurrency : GCD, async/await
- Instruments : Time Profiler, Allocations, Leaks

### Desktop
- Win32 : message loop, HWND, COM
- Cocoa : AppKit, NSApplication, delegates
- GTK/Qt : cross-platform, signal-slot
- Packaging : installers, code signing, notarization

### Cross-platform
- Flutter : Dart, widget tree, Skia
- React Native : bridge, JSI, new architecture
- Tauri : Rust backend, webview frontend
- Electron : Chromium + Node, trade-offs

## 4. Embarqué & Temps Réel

### Microcontrôleurs
- Architecture : ARM Cortex-M, RISC-V, AVR
- Memory : Flash, SRAM, EEPROM, linker scripts
- Périphériques : GPIO, timers, PWM, ADC/DAC
- Interrupts : NVIC, priorities, ISR design
- DMA : peripheral-to-memory, circular buffers

### RTOS
- Concepts : tasks, scheduling, preemption
- Priorities, priority inversion, priority inheritance
- Synchronisation : semaphores, mutexes, queues
- FreeRTOS, Zephyr, RTEMS
- ISR-safe vs task-level APIs

### Protocoles
- I2C : master/slave, addressing, clock stretching
- SPI : full-duplex, modes, chip select
- UART : baud rate, parity, flow control
- CAN : automotive, arbitration, error handling
- USB : endpoints, descriptors, classes
- BLE : GATT, advertising, connections
- Wi-Fi : stations, access points, mesh

### Drivers & HAL
- Hardware Abstraction Layer patterns
- Device trees (Linux embedded)
- Memory-mapped registers
- Interrupt handling, debouncing

### Fiabilité
- Watchdog timers, brownout detection
- ECC memory, CRC validation
- EMI/ESD considerations
- Power management : sleep states, DVFS
- OTA updates, dual partitions, secure boot

## 5. DevOps & SRE

### Linux internals
- Kernel tuning : sysctl, ulimits
- cgroups, namespaces (containers foundation)
- systemd, init systems
- Networking : iptables/nftables, tc, netns

### Containers
- Docker/OCI : images, layers, multi-stage builds
- Security : rootless, capabilities, seccomp
- Registry : push, pull, signing
- Runtime : containerd, cri-o

### Kubernetes
- Architecture : control plane, nodes, kubelet
- Workloads : Pods, Deployments, StatefulSets, DaemonSets
- Services : ClusterIP, NodePort, LoadBalancer, Ingress
- Config : ConfigMaps, Secrets, environment
- Storage : PV, PVC, StorageClasses
- Networking : CNI, network policies
- Operators : CRDs, controllers

### Infrastructure as Code
- Terraform : HCL, providers, state, modules
- Pulumi : general-purpose languages
- Ansible : playbooks, inventory, idempotence
- GitOps : ArgoCD, Flux

### CI/CD
- Pipelines : build, test, deploy stages
- Artifacts : versioning, provenance, attestation
- Environments : dev, staging, prod
- Deployment strategies : rolling, blue-green, canary

### Observability
- Metrics : Prometheus, StatsD, time series
- Logs : structured logging, aggregation, ELK/Loki
- Traces : OpenTelemetry, Jaeger, Zipkin
- Three pillars correlation

### SRE practices
- SLO/SLI/SLA : definitions, error budgets
- Incident response : on-call, runbooks, escalation
- Post-mortems : blameless, action items
- Capacity planning, load testing
- Chaos engineering : fault injection

## 6. Bases de Données

### Relationnel
- Normalisation : 1NF → BCNF
- ACID : Atomicity, Consistency, Isolation, Durability
- Isolation levels : read uncommitted → serializable
- Anomalies : dirty read, non-repeatable, phantom
- Locks : shared, exclusive, deadlock detection

### Indexing & performance
- B-Tree indexes : range queries
- Hash indexes : equality
- GIN/GIST : full-text, geometric
- Covering indexes, index-only scans
- Query planner : EXPLAIN, statistics, hints

### SQL avancé
- Window functions : ROW_NUMBER, RANK, LAG/LEAD
- CTEs : WITH clause, recursive
- Aggregations, grouping sets
- Lateral joins, JSONB operations

### NoSQL
- Document : MongoDB, CouchDB
- Key-value : Redis, DynamoDB
- Wide-column : Cassandra, HBase
- Graph : Neo4j, dgraph
- Time-series : InfluxDB, TimescaleDB

### Data streaming
- Kafka : topics, partitions, consumer groups
- Exactly-once semantics (realistically)
- Event sourcing storage

### Caching
- Redis : data structures, Lua scripts, clustering
- Cache patterns : aside, through, behind
- Invalidation strategies (the hard problem)

## 7. Systèmes Distribués

### Théorèmes fondamentaux
- CAP : Consistency, Availability, Partition tolerance
- PACELC : when Partitioned → trade-off, Else → trade-off
- FLP impossibility

### Consensus
- Paxos : proposers, acceptors, learners
- Raft : leader election, log replication
- Byzantine fault tolerance (PBFT)

### Patterns
- Idempotency : idempotency keys, deduplication
- Retries : exponential backoff, jitter
- Timeouts : connect, read, overall
- Circuit breaker : closed, open, half-open
- Sagas : choreography vs orchestration
- Outbox pattern : transactional messaging

### Clocks & ordering
- NTP : drift, stratum
- Logical clocks : Lamport, vector clocks
- Causality, happens-before

### Partitioning
- Horizontal sharding, consistent hashing
- Split brain, quorum systems
- Leader election

### Multi-region
- Active-active, active-passive
- Geo-replication, conflict resolution
- Disaster recovery : RTO, RPO

## 8. Compilateurs & VM

### Frontend
- Lexing : tokenization, regular expressions
- Parsing : recursive descent, Pratt, LR
- AST construction, semantic analysis
- Type checking, inference

### Intermediate representation
- SSA form : single static assignment
- Control flow graph, dominators
- LLVM IR concepts

### Optimizations
- Constant folding, propagation
- Dead code elimination
- Inlining, loop optimizations (LICM, unrolling)
- Escape analysis, scalar replacement

### Backend
- Instruction selection, register allocation
- Scheduling
- Linking : static, dynamic, LTO

### Runtimes
- GC algorithms : mark-sweep, copying, generational
- JIT compilation : tracing, method-based
- Bytecode interpreters
- ABI stability, symbol versioning

## 9. AI/ML Engineering

### Pipeline data
- Collection, cleaning, validation
- Feature engineering, feature stores
- Train/val/test splits, data leakage

### Modeling
- Training : loss functions, optimizers
- Regularization, hyperparameter tuning
- Evaluation : metrics, calibration

### Serving
- Quantization : INT8, FP16
- Batching, dynamic batching
- GPU vs CPU inference
- Model versioning, A/B testing

### RAG & Vector DB
- Embeddings, chunking strategies
- Vector stores : Pinecone, Weaviate, pgvector
- Retrieval evaluation, reranking

### MLOps
- Experiment tracking : MLflow, W&B
- Model registry, deployment pipelines
- Monitoring : drift detection, performance
- Governance : lineage, explainability