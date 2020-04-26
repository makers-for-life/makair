MakAir — System Architecture
=====

**This design aims to explain how MakAir has been designed and to design new features thanks to a consistent architecture sketch.**

A system architecture model provides this two main goals:
- a functional analysis to explain what funtion each part has to perform
- an architecture schema to explain how each parts of MakAir interacts with the others

By providing a visual system view of Makair, it become easier to ramp up on the project as contributors can have an overview of the system without going to deeply in implementation considerations (like electronics, software, mechanic). A diagram can be easier to read than long document. For certification and validation, it can also be helpful as it provides a proof of consistency between Hardware, Software and Specifications.

# How to read this model?

This architecture design has been created with the Open Source project: [Capella](https://www.eclipse.org/capella). This project used a simple methodolody named [Arcadia](https://www.eclipse.org/capella/arcadia.html) to guide users accross each steps. In a nutshell:
- Operational: how users (patient, doctors, external system) interacts with together
- System: the Airmake as a black box, with a the function it is suppose to perform and interface with others systems
- Logical: the Airmake as a white box, with its internal subcomponents but without implementation details
- Physic: allocation of logical functions and components to the hardware and software

There is a full traceability from a step to the other one, to track how an initial requirement has been satisfied in a subpart of MakAir.

Is it too complex for me? For information, this project has been started by a 9 year old child. So, after 5 min, you can understand it.

# Authors

- Marceau Juliot (9 yo)
- Léonie Juliot (13 yo)
- [Etienne Juliot](https://github.com/ejuliot)

Special thanks to [Frederic Madiot](https://github.com/fmadiot) for this idea of contribution.

It is a draft document. Any contribution is welcomed.

## How to contribute?

Download Capella 1.4 at https://www.eclipse.org/capella/download.html then import this project.
Use Github to propose an update.

# Operational Analysis

Who are the actors?
<p>
  <img alt="actors" src="./export/[OEBD] Operational Entities.jpg" height="240">
</p>

Which activities do they perform?
<p>
  <img alt="actors" src="./export/[OAB] Operational Entities.jpg" height="240">
</p>

## Comments

- Does the maintenace Technician has really a role? If yes, what he can really peform with MakAir?


# System Analysis

Which main functions does MakAir perform? And does a function need other functions to work (provided by MakAir or an another system provided by the hospital)?
<p>
  <img alt="actors" src="./export/[SAB] Structure.jpg" height="240">
</p>

This analysis has been performed thanks to [these requirements](https://github.com/makers-for-life/makair/blob/master/docs/Notes/English/Medical%20Requirements%20(English).md).

# Logical Architecture

How the internal of MakAir is organized, in term of subcomponents? What are the functions performed by each subcomponent?
<p>
  <img alt="actors" src="./export/[LAB] Structure.jpg" height="240">
</p>

What is the functional dataflow of the pneumatic circuit?
<p>
  <img alt="actors" src="./export/[LDFB] Pneumatic Circuit Scheme Logical Function.jpg" height="240">
</p>

## Comments

- It should be useful to add functions relative to software algorithms.

# Physical Architecture

What is the physical breakdown of MakAir, to identify each physical part?
<p>
  <img alt="actors" src="./export/[PCBD] Structure.jpg" height="240">
</p>

How each physical part are deployed in the container and how are they connected together?
<p>
  <img alt="actors" src="./export/[PAB] Structure.jpg" height="240">
</p>

## Comments

- It should be useful to add implementation choice relative to software (deployment units)
- It should be useful to create diagrams which show how logical components and their functions are deployed on physical components.