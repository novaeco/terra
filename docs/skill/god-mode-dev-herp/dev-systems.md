# Science des Systèmes

## 1. Architecture Ordinateur

### CPU & Pipeline
- Fetch → Decode → Execute → Memory → Writeback
- Hazards : data (RAW/WAR/WAW), control, structural
- Branch prediction : static, dynamic, BTB, speculative execution
- Out-of-order execution, register renaming, reorder buffer
- Superscalar, VLIW, SMT (hyperthreading)

### Hiérarchie mémoire
- Registres → L1 (split I/D) → L2 → L3 → RAM → SSD → HDD
- Cache : lines (64B typique), associativité (direct/n-way/full)
- Politiques : LRU, pseudo-LRU, random
- Write policies : write-through, write-back, write-allocate
- Cohérence : MESI, MOESI protocols
- TLB : translation lookaside buffer, page walk

### NUMA & mémoire
- Non-Uniform Memory Access : local vs remote
- Memory bandwidth, interleaving
- False sharing : cache line contention
- Prefetching : hardware, software hints

### SIMD & vectorisation
- x86 : SSE (128-bit), AVX (256-bit), AVX-512
- ARM : NEON, SVE
- Intrinsics, auto-vectorisation, alignment
- Gather/scatter, masking, shuffles

### Bus & I/O
- PCIe : lanes, generations, DMA
- Memory-mapped I/O vs port I/O
- Interrupts : edge/level, MSI/MSI-X
- DMA : scatter-gather, IOMMU

## 2. Systèmes d'Exploitation

### Processus & threads
- Process : address space, file descriptors, credentials
- Thread : shared memory, propre stack/registers
- Context switch : coût, TLB flush
- Process states : running, ready, blocked, zombie

### Scheduling
- Preemptive vs cooperative
- Algorithms : FIFO, SJF, priority, round-robin, CFS
- Real-time : rate monotonic, EDF
- Multi-core : load balancing, affinity, work stealing

### Gestion mémoire
- Virtual memory : pages (4KB), page tables, multi-level
- Paging : demand paging, page faults, thrashing
- mmap : file-backed, anonymous, shared/private
- Allocateurs : malloc implémentations (ptmalloc, jemalloc, tcmalloc)
- Fragmentation : interne, externe, compaction

### Syscalls & I/O
- Interface kernel/userspace, mode switch
- File descriptors, open/read/write/close
- Multiplexing : select, poll, epoll (Linux), kqueue (BSD), IOCP (Windows)
- Signaux : SIGTERM, SIGKILL, SIGCHLD, handlers
- Pipes, FIFOs, Unix domain sockets

### Filesystems
- Concepts : inodes, directories, links (hard/soft)
- Journaling : write-ahead logging, metadata/data
- Filesystems : ext4, XFS, NTFS, APFS, ZFS, Btrfs
- VFS layer, FUSE
- I/O scheduling : CFQ, deadline, noop, BFQ

### Primitives kernel
- Mutex : sleeping lock, ownership
- Spinlock : busy-wait, interrupt-safe
- Semaphore : counting, binary
- Futex : fast userspace mutex
- RWLock : shared readers, exclusive writer
- Condition variables, barriers

## 3. Concurrence & Parallélisme

### Modèles mémoire
- Sequential consistency (idéal, coûteux)
- Total Store Order (x86)
- Relaxed (ARM, POWER)
- C/C++ memory model : seq_cst, acquire, release, relaxed
- Java Memory Model, happens-before

### Atomics & ordering
- Atomic operations : load, store, CAS, fetch-add
- Memory barriers : acquire, release, full fence
- ABA problem : tagged pointers, hazard pointers, epoch

### Problèmes classiques
- Deadlock : mutual exclusion, hold-and-wait, no preemption, circular wait
- Livelock : repeated failed attempts
- Starvation : unfair scheduling
- Priority inversion : priority inheritance, priority ceiling

### Patterns de concurrence
- Thread pool : fixed, cached, work-stealing
- Producer-consumer : bounded buffer, backpressure
- Actor model : message passing, no shared state
- CSP : channels, communicating sequential processes
- Fork-join : divide-and-conquer parallelism

### Lock-free & wait-free
- Lock-free : au moins un thread progresse
- Wait-free : tous les threads progressent
- Michael-Scott queue, Treiber stack
- DCAS, MCAS techniques
- RCU : read-copy-update, grace periods

### Outils
- Race detection : ThreadSanitizer (TSAN)
- Formal verification : TLA+, model checking
- Stress testing, fuzzing

## 4. Réseaux

### Stack TCP/IP
- Physical → Data Link → Network → Transport → Application
- Ethernet : MAC, frames, MTU (1500)
- IP : v4/v6, fragmentation, TTL, routing
- ICMP : ping, traceroute, error messages
- ARP : IP to MAC resolution

### TCP
- 3-way handshake, 4-way teardown
- Sequence numbers, acknowledgments
- Flow control : sliding window, receive window
- Congestion control : slow start, AIMD, cubic, BBR
- MSS, Nagle's algorithm, delayed ACK
- Retransmission : RTO, fast retransmit
- TIME_WAIT, connection reuse

### UDP
- Connectionless, best-effort
- Use cases : DNS, gaming, streaming, QUIC
- Checksums optionnels (v4), obligatoires (v6)

### DNS
- Hierarchy : root, TLD, authoritative
- Record types : A, AAAA, CNAME, MX, TXT, SRV
- Resolution : recursive, iterative
- Caching, TTL, negative caching

### TLS/SSL
- Handshake : ClientHello, ServerHello, certificates, key exchange
- Cipher suites : key exchange + bulk cipher + MAC
- Perfect forward secrecy : ECDHE
- Certificate chain, CA, OCSP, CRL
- TLS 1.3 : 1-RTT, 0-RTT
- mTLS : mutual authentication

### HTTP
- HTTP/1.1 : persistent connections, pipelining, chunked
- HTTP/2 : multiplexing, header compression (HPACK), server push
- HTTP/3 : QUIC (UDP-based), 0-RTT
- Methods : GET, POST, PUT, DELETE, PATCH, HEAD, OPTIONS
- Headers : caching, auth, content negotiation
- Status codes : 2xx, 3xx, 4xx, 5xx

### Protocoles applicatifs
- WebSocket : full-duplex, upgrade from HTTP
- SSE : server-sent events, unidirectional
- gRPC : HTTP/2, Protocol Buffers, streaming

### Load balancing
- L4 : TCP/UDP, NAT, DSR
- L7 : HTTP, content-based routing
- Algorithms : round-robin, least connections, weighted, consistent hashing
- Health checks, circuit breakers

### Observabilité réseau
- tcpdump, Wireshark, pcap
- Distributed tracing : correlation IDs
- Network metrics : latency, throughput, packet loss