**Beast2 Architecture Documentation Overview**  
  
**Purpose**  
  
This document provides a high-level overview of the Beast2 architecture documents.  
  
Beast2 is designed as a **local generative AI powertool** that enables users to:  
	•	run generative models locally  
	•	compose reusable generators  
	•	build large media libraries  
	•	explore latent spaces  
	•	automate workflows using a DSL  
  
The architecture is divided into several subsystems. Each document focuses on a specific layer of the system.  
  
⸻  
  
**Architecture Layers**  
  
The Beast2 architecture can be understood in five major layers:  
	1.	**DSL / Generator Design**  
	2.	**Execution Engine**  
	3.	**Runtime & Compute**  
	4.	**Data Systems**  
	5.	**Exploration Tools**  
  
The documents in the repository correspond to these layers.  
  
⸻  
  
**1. Beast2 Prompt DSL Specification**  
  
**Purpose**  
  
Defines the **Beast2 generator DSL**, the language used to describe generators and workflows.  
  
The DSL is optimized for:  
	•	human readability  
	•	easy editing by local LLMs  
	•	minimal punctuation  
	•	deterministic parsing  
  
⸻  
  
**Key Concepts**  
  
The DSL introduces structured components such as:  
  
Sections  
Snippets  
Tags  
Workflow blocks  
  
Prompts are constructed by combining snippets grouped into reusable sections.  
  
Example concepts covered in the document:  
	•	prompt composition  
	•	section concatenation rules  
	•	snippet definitions  
	•	tagging system  
	•	generator metadata  
  
⸻  
  
**Why It Exists**  
  
Most AI pipelines require complex tools to edit workflows.  
  
The Beast2 DSL allows workflows to be:  
	•	written by humans  
	•	edited by local LLMs  
	•	version controlled  
	•	shared easily  
  
⸻  
  
**2. Beast2 Backend Architecture**  
  
**Purpose**  
  
Defines the **core runtime architecture** of Beast2.  
  
This document describes the overall structure of the system and how the main components interact.  
  
⸻  
  
**Key Responsibilities**  
  
The backend coordinates:  
	•	generator execution  
	•	GPU resource management  
	•	model runtime systems  
	•	media storage  
	•	database operations  
  
⸻  
  
**Major Components**  
  
The backend includes:  
  
Generator execution engine  
GPU scheduler  
Model runtime layer  
Tensor memory system  
Media library  
Database system  
  
⸻  
  
**Why It Exists**  
  
This document establishes the **core system structure** and how all major subsystems interact.  
  
⸻  
  
**3. Beast2 Backend Architecture (Expanded)**  
  
**Purpose**  
  
Provides a deeper explanation of the backend architecture.  
  
It expands on:  
	•	runtime responsibilities  
	•	system boundaries  
	•	workflow lifecycle  
	•	performance considerations  
  
⸻  
  
**Topics Covered**  
  
Generator lifecycle  
job scheduling  
logging  
runtime modularity  
resource management  
  
⸻  
  
**Why It Exists**  
  
The expanded version provides implementation-level guidance beyond the initial overview.  
  
⸻  
  
**4. Beast2 Generator Execution Engine**  
  
**Purpose**  
  
Defines the system responsible for **executing Beast2 generators**.  
  
Generators are the core automation primitive of Beast2.  
  
They define workflows such as:  
	•	generating media  
	•	running prompts  
	•	invoking models  
	•	processing results  
  
⸻  
  
**Key Responsibilities**  
  
The execution engine:  
	•	parses generator specifications  
	•	resolves variables  
	•	schedules tasks  
	•	executes pipeline steps  
	•	records outputs  
  
⸻  
  
**Workflow Example**  
  
Typical generator execution:  
	1.	Load generator specification  
	2.	Resolve prompt sections  
	3.	Prepare model inputs  
	4.	Execute generation steps  
	5.	Store outputs  
	6.	log results  
  
⸻  
  
**Why It Exists**  
  
This component turns the DSL into **actual executable workflows**.  
  
Without it, generators would only be configuration files.  
  
⸻  
  
**5. Beast2 GPU Scheduler Architecture**  
  
**Purpose**  
  
Defines how Beast2 schedules compute workloads on the GPU.  
  
Generative models are extremely GPU-intensive, so careful scheduling is required.  
  
⸻  
  
**Responsibilities**  
  
The GPU scheduler manages:  
	•	job queues  
	•	GPU memory usage  
	•	model loading  
	•	background generation  
	•	preview rendering  
  
⸻  
  
**Example Workloads**  
  
The scheduler may handle multiple simultaneous workloads:  
	•	background video generation  
	•	gallery preview decoding  
	•	latent explorer rendering  
	•	model loading  
  
⸻  
  
**Why It Exists**  
  
Without scheduling, competing workloads would:  
	•	exhaust VRAM  
	•	freeze the UI  
	•	reduce generation performance  
  
The scheduler ensures stable performance.  
  
⸻  
  
**6. Beast2 Model Runtime Layer Architecture**  
  
**Purpose**  
  
Defines how Beast2 executes machine learning models.  
  
The runtime layer provides a unified interface for different model types.  
  
⸻  
  
**Supported Model Types**  
  
The runtime supports:  
  
image diffusion models  
video generation models  
language models  
  
⸻  
  
**Responsibilities**  
  
The runtime handles:  
	•	model loading  
	•	inference execution  
	•	tensor input/output handling  
	•	precision modes  
	•	runtime selection  
  
⸻  
  
**Why It Exists**  
  
Different models require different execution engines.  
  
This layer hides those differences and provides a consistent API.  
  
⸻  
  
**7. Beast2 Latent Space Explorer Architecture**  
  
**Purpose**  
  
Defines the system used for **interactive latent space exploration**.  
  
This feature allows users to explore variations of generated media by manipulating latent vectors.  
  
⸻  
  
**Core Feature**  
  
The primary interface is a **2D latent explorer**.  
  
Users place latents at the corners of a grid and interpolate between them.  
  
Example concept:  
  
```
latent A       latent B

latent C       latent D

```
  
Dragging inside the grid interpolates between these latent vectors.  
  
⸻  
  
**Capabilities**  
  
The explorer supports:  
	•	real-time decoding  
	•	motion vector extraction  
	•	latent tagging  
	•	animation path generation  
  
⸻  
  
**Why It Exists**  
  
Latent exploration allows creators to generate variations rapidly without rerunning full generation pipelines.  
  
⸻  
  
**8. Beast2 Media Library & Database Architecture**  
  
**Purpose**  
  
Defines how Beast2 stores and organizes generated media.  
  
The system must scale to extremely large local libraries.  
  
⸻  
  
**Design Targets**  
  
Typical workstation libraries may contain:  
	•	hundreds of thousands of media files  
	•	tens of thousands of tags  
	•	10TB+ storage  
  
⸻  
  
**Responsibilities**  
  
The media system manages:  
	•	file storage  
	•	metadata indexing  
	•	tag management  
	•	latent storage  
	•	generator history  
  
⸻  
  
**Database**  
  
Metadata is stored in a local database while media files remain on disk.  
  
The system enables fast queries such as:  
	•	search by tags  
	•	search by generator  
	•	search by prompt  
  
⸻  
  
**Additional Capabilities**  
  
The database system also supports structured text databases used by generators, including:  
	•	local encyclopedias  
	•	belief databases  
	•	LLM knowledge sources  
  
⸻  
  
**System Interaction Overview**  
  
All architecture components work together during a generation workflow.  
  
Example pipeline:  
  
```
User launches generator
        ↓
Generator Execution Engine
        ↓
GPU Scheduler
        ↓
Model Runtime Layer
        ↓
Tensor Memory System
        ↓
Generated Media
        ↓
Media Library + Database

```
  
This pipeline enables Beast2 to function as a **complete local generative AI workstation**.  
  
⸻  
  
**Summary**  
  
The Beast2 architecture documents define a modular system designed for powerful local AI generation.  
  
Core ideas include:  
  
Generators as reusable tools  
Local execution without cloud dependencies  
Large local media libraries  
Latent exploration workflows  
Efficient GPU scheduling  
  
Together, these components enable Beast2 to function as a **general-purpose generative AI powertool** capable of running complex workflows entirely offline.  
  
⸻  
  
