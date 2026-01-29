# Génie Logiciel & Engineering

## 1. Conception & Architecture

### Domain-Driven Design (DDD)
- Ubiquitous language : vocabulaire partagé métier/tech
- Bounded contexts : limites explicites, context mapping
- Aggregates : cohérence transactionnelle, racine
- Entities vs Value Objects
- Domain events, event storming
- Anti-corruption layer

### Architectures
- Clean Architecture : dependency rule, use cases, entities
- Hexagonal (Ports & Adapters) : core isolé, adapters
- Onion Architecture : couches concentriques
- CQRS : séparation read/write models
- Event Sourcing : état = somme des events, replay
- Microservices vs monolithe : trade-offs

### Patterns (GoF + autres)
**Création** : Factory, Abstract Factory, Builder, Singleton, Prototype
**Structure** : Adapter, Bridge, Composite, Decorator, Facade, Proxy
**Comportement** : Strategy, Observer, Command, State, Template Method, Visitor, Chain of Responsibility

### Anti-patterns à éviter
- God class, spaghetti code
- Golden hammer, premature optimization
- Cargo cult, copy-paste programming
- Distributed monolith

### API Design
- REST : resources, verbs, status codes, HATEOAS
- gRPC : Protocol Buffers, streaming, deadlines
- GraphQL : schema, queries, mutations, subscriptions
- Versioning : URL, header, content negotiation
- Backward/forward compatibility, deprecation

### Modularité
- High cohesion, low coupling
- Dependency inversion, injection
- Interface segregation
- Package principles : REP, CCP, CRP, ADP, SDP, SAP

## 2. Qualité & Tests

### Pyramide des tests
- Unit tests : isolés, rapides, nombreux
- Integration tests : composants ensemble
- E2E tests : système complet, lents, fragiles
- Contract tests : API boundaries

### Techniques avancées
- Property-based testing : QuickCheck, Hypothesis
- Mutation testing : PIT, Stryker
- Fuzzing : AFL, libFuzzer, coverage-guided
- Golden/snapshot tests : output comparison
- Chaos testing : fault injection

### Testability by design
- Dependency injection
- Ports & adapters pour mocking
- Pure functions, determinism
- Time/clock abstraction
- Feature flags

### CI/CD gating
- Pre-commit hooks, linting
- Coverage thresholds (meaningful, not vanity)
- Performance regression detection
- Security scanning (SAST, DAST, SCA)
- Canary deployments, blue-green

## 3. Debugging (niveau élite)

### Debuggers
- GDB/LLDB : breakpoints, watchpoints, reverse debugging
- WinDbg : Windows, kernel debugging
- IDE debuggers : conditional breaks, evaluate expressions

### Profilers
- CPU : perf (Linux), VTune, Instruments
- Memory : Valgrind, Heaptrack, dotMemory
- Sampling vs instrumentation trade-offs

### Tracing
- eBPF : kernel/userspace tracing, low overhead
- DTrace : Solaris/macOS/FreeBSD
- ETW : Windows Event Tracing
- Application tracing : OpenTelemetry, Jaeger

### Sanitizers
- ASan : address sanitizer (heap overflow, UAF)
- MSan : memory sanitizer (uninitialized reads)
- TSan : thread sanitizer (data races)
- UBSan : undefined behavior sanitizer

### Techniques
- Core dumps : génération, analyse
- Stack unwinding, symbolication
- Crash triage, bucketing
- Root cause analysis (RCA) : 5 whys, fishbone
- Minimal repro, bisection (git bisect)
- Feature flags pour isolation

## 4. Performance Engineering

### Méthodologie
- Profiling-driven : mesurer d'abord
- Identify bottleneck → optimize → measure again
- Amdahl's law : speedup limits
- Benchmarking : micro, macro, system

### Métriques
- Latency vs throughput
- Percentiles : p50, p90, p95, p99, p99.9
- Tail latency, coordinated omission
- Little's law : L = λW

### Optimisations CPU
- Cache locality : spatial, temporal
- Data layout : AoS vs SoA
- Branch prediction : likely/unlikely hints
- Vectorization : SIMD, autovectorization
- Inlining, loop unrolling

### Optimisations mémoire
- Allocation patterns : object pools, arenas
- Avoid fragmentation
- Memory-mapped files
- Copy-on-write, zero-copy

### Optimisations I/O
- Async I/O : io_uring, IOCP
- Batching : amortize syscall overhead
- Buffering strategies
- Connection pooling

### Capacity planning
- Load modeling, forecasting
- Queueing theory basics
- Stress testing, soak testing
- Auto-scaling triggers

## 5. Sécurité

### Threat Modeling
- STRIDE : Spoofing, Tampering, Repudiation, Info disclosure, DoS, Elevation
- Attack trees, data flow diagrams
- Risk assessment : likelihood × impact
- Trust boundaries

### Cryptographie appliquée
- Hash : SHA-256, SHA-3, BLAKE3
- MAC : HMAC, poly1305
- AEAD : AES-GCM, ChaCha20-Poly1305
- KDF : PBKDF2, Argon2, scrypt
- Asymmetric : RSA, ECDSA, Ed25519
- Key exchange : ECDH, X25519

### Authentication & Authorization
- OAuth2 flows : authorization code, PKCE, client credentials
- OIDC : ID tokens, userinfo
- JWT : structure, signing, validation, pitfalls (alg:none, weak secrets)
- Session management : secure cookies, token rotation
- RBAC vs ABAC vs ReBAC
- Least privilege principle

### Gestion des secrets
- KMS : AWS KMS, GCP KMS, Azure Key Vault
- Vault : HashiCorp Vault, sealed/unsealed
- Rotation automatique
- Environment variables vs secret managers
- Never commit secrets

### Vulnérabilités web (OWASP Top 10)
1. Broken Access Control
2. Cryptographic Failures
3. Injection (SQL, NoSQL, LDAP, OS)
4. Insecure Design
5. Security Misconfiguration
6. Vulnerable Components
7. Authentication Failures
8. Software & Data Integrity Failures
9. Logging & Monitoring Failures
10. SSRF

### Vulnérabilités code
- XSS : reflected, stored, DOM-based → escape, CSP
- CSRF : tokens, SameSite cookies
- Injection : parameterized queries, input validation
- Deserialization : avoid untrusted data
- Memory safety : UAF, buffer overflow, double free
- Integer overflow, format string

### Supply chain
- Dependencies : SCA tools, Dependabot
- SBOM : Software Bill of Materials
- Signatures : Sigstore, cosign
- Reproducible builds
- Typosquatting, dependency confusion

### Secure coding
- Input validation, output encoding
- Principle of least privilege
- Defense in depth
- Fail securely
- Security linting : semgrep, CodeQL